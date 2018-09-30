# AutomaticBarrier
Sample automatic barrier for study purpose

## File structure
### Gateway
Contains code for nodemcu
- gateway.h
- gateway.cpp
- gateway.ino
### Station
Contains code for train station
- station.h
- station.cpp
- station.ino
### Train
Contains code for train
- train.h
- train.cpp
- train.ino
### External Library
- RF24: For controlling RF module
- LiquidCrystal_I2C: For controlling LCD
- WiFiManager: Library help user configure Wi-Fi
- ArduinoJson: Library help parsing JSON

## Installation
### For Train and Station
1. Copy **LiquidCrystal_I2C**, **RF24** folder in **lib** to Arduino **libraries** folder
2. Create new folder called **AutomaticBarrier** in Arduino libraries folder
3. Copy **station.h** and **staion.cpp**, **train.h** and **train.cpp** to AutomaticBarrier folder.
4. Open **station.ino** to uploading code for station or **train.ino** to uploading code for train.

### For NodeMCU
1. Install [NodeMCU Driver](https://github.com/nodemcu/nodemcu-devkit/tree/master/Drivers)
2. Install [ESP8266 Board and Libraries](http://arduino.vn/bai-viet/1496-esp8266-ket-noi-internet-phan-1-cai-dat-esp8266-lam-mot-socket-client-ket-noi-toi)
3. Copy **WiFiManage**r, **ArduinoJson**, to Arduino libraries folder
4. Create new folder called **TrainGateway** in Arduino libraries
5. Copy **gateway.h** and **gateway.cpp** to TrainGateway
6. Open both file and remove the commented code
7. Open **gateway.ino** and start to upload code
