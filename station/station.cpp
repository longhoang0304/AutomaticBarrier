#include <Servo.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include "station.h"

Servo servo;
RF24 stationRf(CE, CS);

static const byte stationAddress[5] = {'s', 't', 'a', 't', 'n'};
static const byte trainAddress[5] = {'t', 'r', 'a', 'i', 'n'};
static const byte dataToSend = 1;
static double dataReceived[2];
static bool newData = false;

void controlServo(int angle)
{
  servo.write(angle);
}

void setupServo()
{
  servo.attach(BAR_PIN);
  controlServo(0);
}

void setupRf()
{
  stationRf.begin();
  stationRf.setDataRate(RF24_250KBPS);
  stationRf.openWritingPipe(stationAddress);
  stationRf.openReadingPipe(1, trainAddress);
  stationRf.setRetries(3, 5);
}

void setup_station()
{
  pinMode(EMER_BTN, INPUT);
  setupServo();
  setupRf();
}

void alertTrain()
{
  stationRf.stopListening();
  bool rslt;
  rslt = stationRf.write(&dataToSend, sizeof(dataToSend));
  stationRf.startListening();

  if (rslt)
  {
    Serial.println("Acknowledge Received");
  }
  else
  {
    Serial.println("Tx failed");
  }
  Serial.println();
}

void getData()
{
  if (stationRf.available())
  {
    stationRf.read(&dataReceived, sizeof(dataReceived));
    newData = true;
  }
}

void handleReceivedData() {
  newData = false;
  // control light here
}

void loop_station()
{
  if(digitalRead(EMER_BTN)) {
    alertTrain();
  }
  getData();
  if (newData) {
    handleReceivedData();
  }
  delay(1000);
}