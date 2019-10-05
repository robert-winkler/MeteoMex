#define THINGER_SERVER "xx.xxx.xx.xx"
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


/******************************************************************************  
https://github.com/sparkfun/CCS811_Air_Quality_Breakout  
https://github.com/sparkfun/SparkFun_CCS811_Arduino_Library  
******************************************************************************/
#include "SparkFunCCS811.h"

//#define CCS811_ADDR 0x5B //Default I2C Address
#define CCS811_ADDR 0x5A //Alternate I2C Address

CCS811 VOCccs811(CCS811_ADDR);

void setup() {
 Serial.begin(115200); // serial port 115200 baud

  while(!Serial) {} // Wait
  Wire.begin();

  // Connecting to Thinger.io
  thing.add_wifi("SSID", "password");
   thing["BME280"] >> [](pson& out){
   float temp(NAN), hum(NAN), pres(NAN);
   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);
   bme.read(pres, temp, hum, tempUnit, presUnit);
   out["pres"] = pres;
   out["temp"] = temp;
   out["hum"] = hum;
  };


   thing["CCS811"] >> [](pson& out){
   float temp(NAN), hum(NAN), pres(NAN);
   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);
   bme.read(pres, temp, hum, tempUnit, presUnit);
   VOCccs811.setEnvironmentalData(hum, temp);
   VOCccs811.readAlgorithmResults();
   out["co2"] = VOCccs811.getCO2();
   out["tvoc"] = VOCccs811.getTVOC();
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

    //It is recommended to check return status on .begin(), but it is not
  //required.
  CCS811Core::status returnCode = VOCccs811.begin();
  if (returnCode != CCS811Core::SENSOR_SUCCESS)
  {
    Serial.println(".begin() returned with an error.");
    while (1); //Hang if there was a problem.
  }

 
 }

void loop() {

  thing.handle();

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

     //Check to see if CCS811 data is ready with .dataAvailable()
    if (VOCccs811.dataAvailable())
    {
  
      //This sends the humidity/ temperature data to the CCS811 for compensation
      VOCccs811.setEnvironmentalData(hum, temp);
      
      //If so, have the sensor read and calculate the results.
      //Get them later
      VOCccs811.readAlgorithmResults();
  
      Serial.print("eCO2: ");
      //Returns calculated CO2 reading
      Serial.print(VOCccs811.getCO2());
      Serial.print(" ppm \t\t");
      Serial.print(" tVOC:");
      //Returns calculated TVOC reading
      Serial.print(VOCccs811.getTVOC());
      Serial.print(" ppb \n");
      Serial.println();
    }
   
   delay(500);

  Serial.println("......................"); 

  
}
