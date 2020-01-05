/*
 * This program connects a MeteoMex aeria (http://www.meteomex.com) by WiFi and sends data to a ThingsBoard server
 * In the ThingsBoard strings you have to use the ThingsBoard server address and the access token of your device 
 * e.g. http://www.meteomex.com:8080/api/v1/SASras98FRsrsB98SAsf5/telemetry
 * the server address is www.meteomex.com:8080 and the access token is SASras98FRsrsB98SAsf5
 * 
 * Copyright (C) 2020 Robert Winkler <robert.winkler.mail@gmail.com>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  
 */

// Configure network

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "yourssid";
const char* password = "yourpassword";

// Configuration of sensors

#include "Wire.h"
#include <BME280I2C.h>

//https://github.com/finitespace/BME280/
BME280I2C bme;    // Default : forced mode, standby time = 1000 ms
// Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,

/******************************************************************************  
https://github.com/sparkfun/CCS811_Air_Quality_Breakout  
https://github.com/sparkfun/SparkFun_CCS811_Arduino_Library  
******************************************************************************/
#include "SparkFunCCS811.h"

//#define CCS811_ADDR 0x5B //Default I2C Address
#define CCS811_ADDR 0x5A //Alternate I2C Address

CCS811 VOCccs811(CCS811_ADDR);

void setup() 
{
  Serial.begin(115200);
  
  while(!Serial) {} // Wait
  Wire.begin();
  Serial.println();
  Serial.println("Starting up...");

  //start WiFi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
   delay(1000);
   Serial.println("Connecting..");
  }
  Serial.println(WiFi.localIP());

  //initialize BME280 sensor
  while(!bme.begin())
  {
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }

  switch(bme.chipModel())
  {
     case BME280::ChipModel_BME280:
       Serial.println("Found BME280 sensor! Success.");
       break;
     case BME280::ChipModel_BMP280:
       Serial.println("Found BMP280 sensor! No Humidity available.");
       break;
     default:
       Serial.println("Found UNKNOWN sensor! Error!");
  }

  //initialize CCS811 sensor
  //It is recommended to check return status on .begin(), but it is not
  //required.
  
  CCS811Core::status returnCode = VOCccs811.begin();
  if (returnCode != CCS811Core::SENSOR_SUCCESS)
  {
    Serial.println(".begin() returned with an error.");
    while (1); //Hang if there was a problem.
  }
 
}

void loop() 
{

  //To check the connection and posting of data from your terminal
  //curl -v -X POST -d "{\"temperature\":25}"  http://www.meteomex.com:8080/api/v1/SASras98FRsrsB98SAsf5/telemetry --header "Content-Type:application/json"

   HTTPClient http;    //Declare object of class HTTPClient
 
   http.begin("http://www.meteomex.com:8080/api/v1/SASras98FRsrsB98SAsf5/telemetry");      //Specify request destination
   http.addHeader("Content-Type", "application/json");  //Specify content-type header

  // Read BME280 data
 
   float temp(NAN), hum(NAN), pres(NAN);

   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);

   bme.read(pres, temp, hum, tempUnit, presUnit);

   //UNCOMMENT FOR SERIAL OUTPUT
   //Serial.print("Temp: ");
   //Serial.print(temp);
   //Serial.print(" °"+ String(tempUnit == BME280::TempUnit_Celsius ? 'C' :'F'));
   //Serial.print("\t\tHumidity: ");
   //Serial.print(hum);
   //Serial.print("% RH");
   //Serial.print("\t\tPressure: ");
   //Serial.print(pres);
   //Serial.println(" Pa");

   String PostTemp = "{\"temperature (C)\":" + String(temp) + "}"; //Build a POST string
   int httpCodeTemp = http.POST(PostTemp);   //Send the request

   String PostHum = "{\"humidity (rel %)\":" + String(hum) + "}"; //Build a POST string
   int httpCodeHum = http.POST(PostHum);   //Send the request
   
   String PostPres = "{\"pressure (Pa)\":" + String(pres) + "}"; //Build a POST string
   int httpCodePres = http.POST(PostPres);   //Send the request

    //Check to see if CCS811 data is ready with .dataAvailable()
    if (VOCccs811.dataAvailable())
    {
  
      //This sends the humidity/ temperature data to the CCS811 for compensation
      VOCccs811.setEnvironmentalData(hum, temp);
      
      //If so, have the sensor read and calculate the results.
      //Get them later
      VOCccs811.readAlgorithmResults();

      //UNCOMMENT FOR SERIAL OUTPUT
      //Serial.print("eCO2: "); //Returns calculated CO2 reading
      //Serial.print(VOCccs811.getCO2());
      //Serial.print(" ppm \t\t");
      //Serial.print(" tVOC:"); //Returns calculated TVOC reading
      //Serial.print(VOCccs811.getTVOC());
      //Serial.print(" ppb \n");
      //Serial.println();

      String PostCO2 = "{\"eCO2 (ppm)\":" + String(VOCccs811.getCO2()) + "}"; //Build a POST string
      int httpCodeCO2 = http.POST(PostCO2);   //Send the request

      String PostVOC = "{\"tVOCs (ppb)\":" + String(VOCccs811.getTVOC()) + "}"; //Build a POST string
      int httpCodeVOC = http.POST(PostVOC);   //Send the request
   
    }
    
    //UNCOMMENT FOR SERIAL OUTPUT
    //Serial.println("......................"); 

 // debugging code
 //  String payload = http.getString();                  //Get the response payload
 //  Serial.println(httpCode);   //Print HTTP return code
 //  Serial.println(payload);    //Print request response payload
 
   http.end();  //Close connection

   // save traffic. The delay is in milliseconds.
   delay(600000);
 
}
