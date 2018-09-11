#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include "train.h"

RF24 stationRf(CE, CS);

static const byte stationAddress[5] = {'s','t','a','t','n'};
static const byte trainAddress[5] = {'t','r','a','i','n'};
static const int GAP_SIZE = 500; // assume that 500m
static const int DIST_TO_STATION = 10000; // assum that 10km

static byte dataReceived = 0;
static double dataToSend[2];
static bool newData = false;

void setupRf() {
  servo.begin();
  servo.setDataRate(RF24_250KBPS);
  servo.openReadingPipe(1, stationAddress);
  servo.openWritingPipe(trainAddress);
}

void setup_train() {
  pinMode(HALL_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  setupRf();
}

double calculateSpeed() {
  static byte count = 0;
  static int firstHitTime = 0;
  static int lastHitTime = 0;

  if (digitalRead(HALL_PIN) && !count) {
    count = 1;
    firstHitTime = millis();
  }

  if (!digitalRead(HALL_PIN) && count == 1) {
    count = 2;
  }

  if (digitalRead(HALL_PIN) && count == 2) {
    count = 3;
    lastHitTime = millis();
  }

  if (!digitalRead(HALL_PIN) && count == 3) {
    count = 0;
    double speed = abs((lastHitTime - firstHitTime) / (double)GAP_SIZE);
    return speed;
  }
  return -1;
}

void sendInfoToStation()
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
  digitalWrite(LED_PIN, 1);
}

void loop_train() {
  double speed = calculateSpeed();
  if (speed > 0) {
    double t = DIST_TO_STATION / speed;
    dataToSend[0] = speed;
    dataToSend[1] = t;
    sendInfoToStation();
    speed = -1;
  }
  getData();
  if (newData) {
    handleReceivedData();
  }
  delay(50);
}