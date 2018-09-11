#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include <train.h>

static RF24 trainRf(CE, CS);
static const byte stationAddress[5] = {'s','t','a','t','n'};
static const byte trainAddress[5] = {'t','r','a','i','n'};
static const int GAP_SIZE = 20; // assume that 20cm
static const int DIST_TO_STATION = 200; // assume 200cm

static byte dataReceived[2] = {0, 0};
static double dataToSend[4];
static bool newData = false;

static void setupRf() {
  trainRf.begin();
  trainRf.openReadingPipe(1, stationAddress);
  trainRf.openWritingPipe(trainAddress);
  trainRf.startListening();
  dataToSend[3] = 0x34;
}

static void setupPins() {
  pinMode(HALL_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(EMER_BTN, INPUT);
  pinMode(RETS_BTN, INPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
}

void setup_train() {
  setupPins();
  setupRf();
  Serial.begin(9600);
}

bool getHallSensorSignal() {
  return digitalRead(HALL_PIN);
}

double calculateSpeed() {
  static byte count = 0;
  static int firstHitTime = 0;
  static int lastHitTime = 0;
  int hallSignal = getHallSensorSignal();

  if (hallSignal && !count) {
    count = 1;
    return -1;
  }

  if (!hallSignal && count == 1) {
    count = 2;
    firstHitTime = millis() / 1000;
    return -1;
  }

  if (hallSignal && count == 2) {
    count = 3;
    return -1;
  }

  if (!hallSignal && count == 3) {
    count = 0;
    lastHitTime = millis() / 1000;
    double speed = (double)GAP_SIZE / abs((lastHitTime - firstHitTime));
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

  // if (rslt)
  // {
  //   Serial.println("Acknowledge Received");
  // }
  // else
  // {
  //   Serial.println("Tx failed");
  // }
  // Serial.println();
}

static void getData()
{
  if (trainRf.available())
  {
    trainRf.read(&dataReceived, sizeof(dataReceived));
    if ((byte)dataReceived[1] != 0x34) {
      return;
    }
    newData = true;
  }
}

void alertDanger() {
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(SPEAKER_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(SPEAKER_PIN, LOW);
  delay(100);
}

static void handleReceivedData() {
  if (dataReceived) {
    alertDanger();
  }
}

static bool getEmergencySignal() {
  return digitalRead(EMER_BTN);
}

bool getResetSignal() {
  return digitalRead(RETS_BTN);
}

void loop_train() {
  double speed = calculateSpeed();
  // alertDanger();
  // Serial.println(getEmergencySignal());
  // Serial.println(getResetSignal());
  // Serial.println(digitalRead(HALL_PIN));
  // Serial.println();
  // delay(1000);
  // return;
  if (getEmergencySignal()) {
    dataToSend[0] = 0.0;
    dataToSend[1] = 0.0;
    dataToSend[2] = 1.0;
    sendInfoToStation();
  }
  if (getResetSignal()) {
    dataReceived[0] = 0;
    dataReceived[1] = 0;
    newData = false;
  }
  if (speed > 0) {
    double t = DIST_TO_STATION / speed;
    dataToSend[0] = speed;
    dataToSend[1] = t;
    dataToSend[2] = 0.0;
    sendInfoToStation();
    speed = -1;
  }
  getData();
  if (newData) {
    handleReceivedData();
  }
  delay(25);
}