# MeteoMex
This repository provides code examples for programming MeteoMex devices (<http://www.meteomex.com>) and connecting them with the Internet of Things (IoT).

# Microcontroller board
The MeteoMex devices are built on a **Wemos D1 mini** board (<https://www.wemos.cc/en/latest/d1/d1_mini.html>) which features a ESP8266 microcontroller @80 MHz, 4 Mb flash memory, 80 Kb RAM, 11 digital I/O pins and 1 analog I/O pin. The board comes with WiFi support, weights only 3 g and has very low energy consumption, which makes it an ideal platform for IoT applications.

# Circuit design files
The `Fritzing-PCB` directory contains the ciruit design files for the MeteoMex aeria and Meteomex terra Wemos shields. The files can be viewed and edited with Fritzing <https://fritzing.org/home/>).

# Programming with Arduino IDE
The Wemos/ESP8266 is compatible with the Arduino platform (<https://www.arduino.cc>) and therefore can be programmed over the USB port with the Arduino IDE (<https://www.arduino.cc/en/Main/Software>). For programming the Wemos, you need to install the ESP32 additional boards from http://arduino.esp8266.com/stable/package_esp8266com_index.json in the Arduino IDE and **choose the correct device: LOLIN/Wemos D1 R1.** Otherwise, you might "brick" it forever (happened to me..)! You also should be careful with the pinout of the Wemos D1 mini board (see <https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/>), if you modify the programs/ add functions. The GPIO pins on the Wemos D1 mini have different descriptions than the Arduino Pins. E.g. the Wemos D1 mini analog pin A0 corresponds in the programs to pin 17, and D1 to pin 5.

# Jumper J1 and Pin D3 during flashing
The J1 needs to be closed for waking up the microprocessor from DeepSleep via the RST pin. For reprogramming, the jumper must be removed. If transferring the compiled program to the Wemos D1 mini fails, you should try the reprogramming without the MeteoMex shield. If this still does not work, connect D3 with Ground and reset the Wemos. Now it should be in flashing mode.

# Thinger.io IoT platform
Several programs provided here are written for the IoT platform Thinger.io (<https://thinger.io>). You can sign up for free on Thinger.io and use it for a small project (up to 2 devices). If you are working on larger projects, you can set up your own Thinger.io IoT server (<https://docs.thinger.io/deployment>). 

# ThingsBoard IoT platform
Currently, I migrate all MeteoMex programs to the ThingsBoard IoT platform (<https://thingsboard.io>), because 1) it provides an excellent (and real) Open Source community edition, and 2) a professional version for large-scale projects. There are various options to set-up a own server (<https://thingsboard.io/docs/user-guide/install/installation-options/>). The data are stored in database. For up to 5,000 messages/ second, the Open Source database PostgreSQL (<https://www.postgresql.org>) is recommended. You can see a public dashbord (Weather station with VOC sensor) at: <http://www.meteomex.com:8080/dashboard/cbde9020-2f3e-11ea-a96e-c3dba8703f6d?publicId=e96f3c70-2f4d-11ea-a96e-c3dba8703f6d>

# Description of the programs

Arduino IDE program | MeteoMex | Sensors | IoT platform | Operation
--------------------|----------|---------|--------------|----------
autoconnect_ThingsBoard_aeria_VOCs | aeria | climate, VOCs | ThingsBoard | on USB, constantly connected to WiFi, data are pushed continuously via HTTP to ThingsBoard (~150 data points per minute!), configuration of WiFi from mobile phone using a web interphase <https://github.com/prampec/IotWebConf>.
D1-Mini_BME280_CCS811_ThingerIO | aeria | climate, VOCs | Thinger | on USB, constantly connected to WiFi, data are collected every 5 minutes by Thinger.
D1-Mini_BME280_ThingerIO_DeepSleep | aeria | climate | Thinger | on 3xAA batteries, wakes up every 1 hour, connects to WiFi, and pushes data to Thinger.
D1-Mini_DS18B20_soil_v1_2_ThingerIO_DeepSleep | terra | soil moisture and temperature |Thinger | on 3xAA batteries, wakes up every 1 hour, connects to WiFi, and pushes data to Thinger.
MeteoMex_USB_ThingsBoard_aeria_VOCs | aeria | climate, VOCs | ThingsBoard | on USB, constantly connected to WiFi, data are pushed every 10 minutes via HTTP to ThingsBoard.
MeteoMex_USB_ThingsBoard_serial_DS18B20_soil_v1_2 | terra | soil moisture and temperature | ThingsBoard | on USB, sends data every hour to ThingsBoard.
Prototype_USB_ThingsBoard_Waste_Water-Treatment | custom | SR04 ultrasonic and analog turbidity | ThingsBoard | on USB, measures tank filling level and turbidity every minute and pushes data to ThingsBoard. Data are also processed with a noise peak chopper (turbitidy) and Kalman-Filter and also sent to ThingsBoard.


# Forum/ Discussion group
You can post your ideas/ suggestions/ etc at <https://groups.google.com/forum/#!forum/meteomex>

# Contact
meteomex@kuturabi.com
