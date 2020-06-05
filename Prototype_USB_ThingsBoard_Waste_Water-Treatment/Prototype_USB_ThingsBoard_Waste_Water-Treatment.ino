/*
 * This program calculates the fill level of a waste water tank using a SR04 ultrasonic sensor
 * and the analog signal of a turbidity sensor
 * 
 * Both signals are processed to remove noise and sent to a ThingsBoard server as raw and processed data.
 * 
 * Copyright (C) 2020 Robert Winkler <robert.winkler.mail@gmail.com>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. 
 */


#define echoPin D7 // Echo Pin
#define trigPin D6 // Trigger Pin

// Configure network

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Your WiFi";
const char* password = "WiFi password";

long duration, distance; // Duration used to calculate distance

// Circular buffer set-up
#include <CircularBuffer.h>

CircularBuffer<float,10> turbbuffer; // buffer capacity for turbidity is 10

// Kalman Filter library
#include <SimpleKalmanFilter.h>
SimpleKalmanFilter ultrasonicKalmanFilter(1, 1, 0.01);

void setup()
{
 Serial.begin(115200); // serial port 115200 baud
  
  while(!Serial) {} // Wait
  Serial.println();
  Serial.println("Starting up...");

  //start WiFi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
   delay(1000);
   Serial.println("Connecting..");
  }
  Serial.println(WiFi.localIP());
 
  pinMode (17, INPUT); // pin A0 (analog) input

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}
 
void loop()
{

  //To check the connection and posting of data from your terminal
  //curl -v -X POST -d "{\"humidity\":99}"  http://www.meteomex.com:8080/api/v1/DEVICETOKEN/telemetry --header "Content-Type:application/json"

   HTTPClient http;    //Declare object of class HTTPClient
 
   http.begin("http://www.meteomex.com:8080/api/v1/DEVICETOKEN/telemetry");      //Specify request destination
   http.addHeader("Content-Type", "application/json");  //Specify content-type header

    /* The following trigPin/echoPin cycle is used to determine the
    distance of the nearest object by bouncing soundwaves off of it. */
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    //minimal is 10, but 20 turned out to be more reliable
    delayMicroseconds(20);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    //Calculate the distance (in cm) based on the speed of sound.
    distance = duration/58.2;
    Serial.print("\n distance in cm: "); 
    Serial.println(distance);
  
    float fillpercent = 100 * (1-((distance-47.5)/120));

    // apply Kalman filter
    float fill_estimate = ultrasonicKalmanFilter.updateEstimate(fillpercent);
    
  Serial.print("\n filling in % measured: "); 
  Serial.println(fillpercent);

  Serial.print("\n filling in % Kalman Filter: "); 
  Serial.println(fill_estimate);

  //Delay before next reading.
  delay(30*1000);

  //Turbidity
  int turbSensor = analogRead(17);
  float turbpercent = map(turbSensor,1024,0,0,100);
  
  turbbuffer.push(turbpercent);

  float turbpercentmin = 100;
  using index_t = decltype(turbbuffer)::index_t;
  for (index_t i = 0; i < turbbuffer.size(); i++) {
    if (turbbuffer[i] < turbpercentmin) {
      turbpercentmin =  turbbuffer[i];}
  }

  Serial.print("\n turbidity sensor % measured: "); 
  Serial.println(turbpercent); 
  
  Serial.print("\n turbidity sensor % filtered: "); 
  Serial.println(turbpercentmin); 

  String PostLevelraw = "{\"filling % raw\":" + String(fillpercent) + "}"; //Build a POST string
  int httpCodeLeverawl = http.POST(PostLevelraw);   //Send the request
  
  String PostLevelKalman = "{\"filling % Kalman\":" + String(fill_estimate) + "}"; //Build a POST string
  int httpCodeLevelKalman = http.POST(PostLevelKalman);   //Send the request

  String PostTurbraw = "{\"turbidity % raw\":" + String(turbpercent) + "}"; //Build a POST string
  int httpCodeTurbraw = http.POST(PostTurbraw);   //Send the request

  String PostTurbFilter = "{\"turbidity % filtered\":" + String(turbpercentmin) + "}"; //Build a POST string
  int httpCodeTurbFilter = http.POST(PostTurbFilter);   //Send the request
   
  http.end();  //Close connection

  //Delay before next reading.
  delay(30*1000);
}
