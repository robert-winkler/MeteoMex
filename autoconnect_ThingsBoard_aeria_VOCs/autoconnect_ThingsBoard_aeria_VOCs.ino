/**
 * IotWebConf02StatusAndReset.ino -- IotWebConf is an ESP8266/ESP32
 *   non blocking WiFi/AP web configuration library for Arduino.
 *   https://github.com/prampec/IotWebConf 
 *
 * Copyright (C) 2018 Balazs Kelemen <prampec+arduino@gmail.com>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

/**
 * Example: Status and Reset
 * Description:
 *   This example is very similar to the "minimal" example.
 *   But here we provide a status indicator LED, to get feedback
 *   of the thing state.
 *   Further more we set up a push button. If push button is detected
 *   to be pressed at boot time, the thing will use the initial
 *   password for accepting connections in Access Point mode. This
 *   is usefull e.g. when custom password was lost.
 *   (See previous examples for more details!)
 * 
 * Hardware setup for this example:
 *   - An LED is attached to LED_BUILTIN pin with setup On=LOW.
 *     This is hopefully already attached by default.
 *   - A push button is attached to pin D2, the other leg of the
 *     button should be attached to GND.
 */

#include <IotWebConf.h>
#include <ESP8266HTTPClient.h>

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "meteomex";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "smartthing";

// -- When CONFIG_PIN is pulled to ground on startup, the Thing will use the initial
//      password to build an AP. (E.g. in case of lost password)
#define CONFIG_PIN D2

// -- Status indicator pin.
//      First it will light up (kept LOW), on Wifi connection it will blink,
//      when connected to the Wifi it will turn off (kept HIGH).
#define STATUS_PIN LED_BUILTIN

DNSServer dnsServer;
WebServer server(80);

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword);

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

  // -- Initializing the configuration.
  iotWebConf.setStatusPin(STATUS_PIN);
  iotWebConf.setConfigPin(CONFIG_PIN);
  iotWebConf.init();

  // -- Set up required URL handlers on the web server.
  server.on("/", handleRoot);
  server.on("/config", []{ iotWebConf.handleConfig(); });
  server.onNotFound([](){ iotWebConf.handleNotFound(); });

  Serial.println("IoT WiFi configuration ready.");

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
  // -- doLoop should be called as frequently as possible.
  iotWebConf.doLoop();
  //To check the connection and posting of data from your terminal
  //curl -v -X POST -d "{\"temperature\":25}"  http://www.meteomex.com:8080/api/v1/AsWAvAfTgSRSDdNGdrpS/telemetry --header "Content-Type:application/json"

   HTTPClient http;    //Declare object of class HTTPClient
 
   http.begin("http://www.meteomex.com:8080/api/v1/AsWAvAfTgSRSDdNGdrpS/telemetry");      //Specify request destination
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
 
}

/**
 * Handle web requests to "/" path.
 */
void handleRoot()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>IotWebConf 02 Status and Reset</title></head><body>Welcome to your MeteoMex thing!<br>";
  s += "Go to <a href='config'>configure page</a> to change settings.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}
