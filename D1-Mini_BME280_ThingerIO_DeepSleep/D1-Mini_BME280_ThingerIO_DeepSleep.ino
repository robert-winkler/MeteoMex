#define THINGER_SERVER "xx.xxx.xxx.xx"
#define _DEBUG_ //to show the connection trace and detect any problem with credentials, TLS or WiFi
//in case of problems with TLS 
#define _DISABLE_TLS_

#include "Wire.h"
#include <BME280I2C.h>
#include <ThingerESP8266.h>

ThingerESP8266 thing("username", "device", "device-secret");

//https://github.com/finitespace/BME280/
BME280I2C bme;    // Default : forced mode, standby time = 1000 ms
// Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,


const int deepSleepSecs = 3600;  
unsigned long int actSleepTime = deepSleepSecs * 1000000; //microseconds  

void setup() {
 Serial.begin(115200); // serial port 115200 baud

  while(!Serial) {} // Wait
  Wire.begin();

  // Connecting to Thinger.io
  thing.add_wifi("SSID", "Wifi-password");
   thing["BME280"] >> [](pson& out){
   float temp(NAN), hum(NAN), pres(NAN);
   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);
   bme.read(pres, temp, hum, tempUnit, presUnit);
   out["pres"] = pres;
   out["temp"] = temp;
   out["hum"] = hum;
  };


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

 
 }

void loop() {

  thing.handle();
  thing.write_bucket("bucket-name", "sensor");

   float temp(NAN), hum(NAN), pres(NAN);

   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);

   bme.read(pres, temp, hum, tempUnit, presUnit);

   Serial.print("Temp: ");
   Serial.print(temp);
   Serial.print(" °"+ String(tempUnit == BME280::TempUnit_Celsius ? 'C' :'F'));
   Serial.print("\t\tHumidity: ");
   Serial.print(hum);
   Serial.print("% RH");
   Serial.print("\t\tPressure: ");
   Serial.print(pres);
   Serial.println(" Pa");

  Serial.println("......................"); 
  Serial.println("SIESTA TIME"); 
  delay(1000);
  ESP.deepSleep(actSleepTime, WAKE_RF_DEFAULT);// Deep Sleep  
  delay(1000);

  
}
