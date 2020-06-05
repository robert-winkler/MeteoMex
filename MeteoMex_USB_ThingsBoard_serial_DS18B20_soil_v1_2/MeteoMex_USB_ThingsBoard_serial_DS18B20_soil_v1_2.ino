/*
 * This program drives a MeteoMex terra (http://www.meteomex.com); data are sent to a ThingsBoard server
 * 
 * Copyright (C) 2020 Robert Winkler <robert.winkler.mail@gmail.com>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. 
 */

// Configure network

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "your WiFi";
const char* password = "password";

//TEMP
#include <OneWire.h> 
#include <DallasTemperature.h>

OneWire oneWire(2); //is D4
DallasTemperature sensors(&oneWire);

void setup() {
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
 
  pinMode (17, INPUT); // pin A0 (analog) input relative moisture

  sensors.begin(); // listen to TEMP sensors

 }

void loop() {

    //To check the connection and posting of data from your terminal
  //curl -v -X POST -d "{\"humidity\":99}"  http://www.meteomex.com:8080/api/v1/DEVICETOKEN/telemetry --header "Content-Type:application/json"

   HTTPClient http;    //Declare object of class HTTPClient
 
   http.begin("http://www.meteomex.com:8080/api/v1/DEVICETOKEN/telemetry");      //Specify request destination
   http.addHeader("Content-Type", "application/json");  //Specify content-type header

  //SOIL MOISTURE
  int moistSensor = analogRead(17);
  Serial.print("\nsoil moisture sensor: "); 
  Serial.println(moistSensor); 
  Serial.print("soil moisture in %: "); 
  float moistPercent = map(moistSensor,1024,600,0,100);
  Serial.println(moistPercent); // adjust the values for min/max readings of sensor

  //TEMP
  sensors.requestTemperatures();
  float tempSensor = sensors.getTempCByIndex(0);
  Serial.print("temperature C: "); 
  Serial.println(tempSensor); 

  String PostHum = "{\"soil moisture (%)\":" + String(moistPercent) + "}"; //Build a POST string
  int httpCodeHum = http.POST(PostHum);   //Send the request

  String PostTemp = "{\"temperature (C)\":" + String(tempSensor) + "}"; //Build a POST string
  int httpCodeTemp = http.POST(PostTemp);   //Send the request
   
  http.end();  //Close connection

  // save traffic. The delay is in milliseconds.
  delay(3600*1000);
  
}
