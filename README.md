# MeteoMex
This repository provides code examples for programming MeteoMex devices (<http://www.meteomex.com>) and connecting them with the Internet of Things (IoT).

# Microcontroller board
The MeteoMex devices are built on a **Wemos D1 mini** board (<https://wiki.wemos.cc/products:d1:d1_mini>) which features a ESP8266 microcontroller @80 MHz, 4 Mb flash memory, 80 Kb RAM, 11 digital I/O pins and 1 analog I/O pin. The board comes with WiFi support, weights only 3 g and has very low energy consumption, which makes it an ideal platform for IoT applications.

# Programming with Arduino IDE
The Wemos/ESP8266 is compatible with the Arduino platform (<https://www.arduino.cc>) and therefore can be programmed over the USB port with the Arduino IDE (<https://www.arduino.cc/en/Main/Software>). For programming the Wemos, you need to install the ESP32 additional boards from https://dl.espressif.com/dl/package_esp32_index.json in the Arduino IDE and **choose the correct device: LOLIN/Wemos D1 R1.** Otherwise, you might "brick" it forever (happened to me..)! You also should be careful with the pinout of the Wemos D1 mini board (see <https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/>), if you modify the programs/ add functions. The GPIO pins on the Wemos D1 mini have different descriptions than the Arduino Pins. E.g. the Wemos D1 mini analog pin A0 corresponds in the programs to pin 17, and D1 to pin 5.

# Jumper J1 and Pin D3 during flashing
The J1 needs to be closed for waking up the microprocessor from DeepSleep via the RST pin. For reprogramming, the jumper must be removed. If transferring the compiled program to the Wemos D1 mini fails, you should try the reprogramming without the MeteoMex shield. If this still does not work, connect D3 with Ground and reset the Wemos. Now it should be in flashing mode.

# Thinger.io IoT platform
The programs provided here work for the Open Source IoT platform Thinger.io (<https://thinger.io>). You can sign up for free on Thinger.io and use it for a small project (up to 2 devices). If you are working on larger projects, you can set up your own Thinger.io IoT server (<https://docs.thinger.io/deployment>).

# Description of the programs

Arduino IDE program | MeteoMex | Sensors | Operation
--------------------|----------|---------|----------
D1-Mini_BME280_CCS811_ThingerIO | aeria | climate, VOCs | on USB, constantly connected to WiFi, data are collected every 5 minutes by Thinger
D1-Mini_BME280_ThingerIO_DeepSleep | aeria | climate | on 3xAA batteries, wakes up every 1 hour, connects to WiFi, and pushes data to Thinger
D1-Mini_DS18B20_soil_v1_2_ThingerIO_DeepSleep | tierra | soil moisture and temperature | on 3xAA batteries, wakes up every 1 hour, connects to WiFi, and pushes data to Thinger

# Forum/ Discussion group
You can post your ideas/ suggestions/ etc at <https://groups.google.com/forum/#!forum/meteomex>

# Contact
meteomex@kuturabi.com
