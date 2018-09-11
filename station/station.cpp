#include <Servo.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include "station.h"

Servo servo;
RF24 stationRf(CE, CS);

static const byte stationAddress[5] = {'s','t','a','t','n'};
static  const byte trainAddress[5] = {'t','r','a','i','n'};

void controlServo(int angle) {
  servo.write(angle);
}

void setupRf() {
  servo.begin();
  servo.setDataRate(RF24_250KBPS);
  servo.openWritingPipe(stationAddress);
  servo.openReadingPipe(1, trainAddress);
}

void setup_station() {
  servo.attach(BAR_PIN);
  controlServo(0);
  setupRf();
}

void loop_station() {

}