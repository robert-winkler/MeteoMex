#define THINGER_SERVER "xx.xxx.xxx.xx"
#define _DEBUG_ //to show the connection trace and detect any problem with credentials, TLS or WiFi
//in case of problems with TLS 
//#define _DISABLE_TLS_

//TEMP
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <ThingerESP8266.h>

OneWire oneWire(2); //is D4
DallasTemperature sensors(&oneWire);

ThingerESP8266 thing("username", "device", "device-secret");


const int deepSleepSecs = 3600;  
unsigned long int actSleepTime = deepSleepSecs * 1000000; //microseconds  

void setup() {
 Serial.begin(115200); // serial port 115200 baud
  pinMode (17, INPUT); // pin A0 (analog) input relative moisture

 sensors.begin(); // listen to TEMP sensors
  
  // Connecting to Thinger.io
  thing.add_wifi("SSID", "password");
  thing["soil_T_Hum"] >> [](pson& out){
  int moistSensor = analogRead(17);
  sensors.requestTemperatures();
  float tempSensor = sensors.getTempCByIndex(0);
  out["soilmoisture"] = map(moistSensor,1024,560,0,100);
  out["temp"] = tempSensor;
  };
 
 }

void loop() {

  thing.handle();
  thing.write_bucket("bucketname", "sensordata");

  //SOIL MOISTURE
  int moistSensor = analogRead(17);
  Serial.print("\nsoil moisture sensor: "); 
  Serial.println(moistSensor); 
  Serial.print("soil moisture in %: "); 
  Serial.println(map(moistSensor,1024,560,0,100)); // adjust the values for min/max readings of sensor

  //TEMP
  sensors.requestTemperatures();
  float tempSensor = sensors.getTempCByIndex(0);
  Serial.print("temperature C: "); 
  Serial.println(tempSensor); 

  Serial.println("......................"); 
  Serial.println("SIESTA TIME"); 
  delay(1000);

  ESP.deepSleep(actSleepTime, WAKE_RF_DEFAULT);// Deep Sleep  
  delay(1000);

  
}
