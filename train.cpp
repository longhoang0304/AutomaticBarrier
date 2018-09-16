#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include <train.h>

static RF24 trainRf(CE, CS);
static const byte stationAddress[5] = {'s','t','a','t','n'};
static const byte trainAddress[5] = {'t','r','a','i','n'};
static const int GAP_SIZE = 500; // assume that 500m
static const int DIST_TO_STATION = 10000; // assum that 10km

static byte dataReceived = 0;
static double dataToSend[2];
static bool newData = false;

static void setupRf() {
  trainRf.begin();
  trainRf.setDataRate(RF24_250KBPS);
  trainRf.openReadingPipe(1, stationAddress);
  trainRf.openWritingPipe(trainAddress);
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
  trainRf.stopListening();
  bool rslt;
  rslt = trainRf.write(&dataToSend, sizeof(dataToSend));
  trainRf.startListening();

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

static void getData()
{
  if (trainRf.available())
  {
    trainRf.read(&dataReceived, sizeof(dataReceived));
    newData = true;
  }
}

static void handleReceivedData() {
  newData = false;
  if (dataReceived) digitalWrite(LED_PIN, 1);
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