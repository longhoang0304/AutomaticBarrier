#include <Servo.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include <Wire.h>
#include "station.h"

Servo servo;
RF24 stationRf(CE, CS);

static const byte stationAddress[5] = {'s', 't', 'a', 't', 'n'};
static const byte trainAddress[5] = {'t', 'r', 'a', 'i', 'n'};
static const byte dataToSend = 1;
static double dataReceived[2] = {0, 0};
static bool newData = false;
static bool isTrainComming = false;
static int countdownTimer = 0;
static int startTime = 0;
static double speed = 0;

// ===============================================================

void sendESP8266Data()
{
  Wire.write(data, len);
}

void handleESP8266Action(int numBytes)
{
  byte data[numBytes] = {0};
  byte i = 0;
  while (Wire.available())
  {
    data[i] = Wire.read();
    i++;
  }
}

void controlServo(int angle)
{
  servo.write(angle);
}

void setupI2C()
{
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(sendESP8266Data);
  Wire.onReceive(handleESP8266Action);
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
  setupI2C();
  setupServo();
  setupRf();
}

// ===============================================================

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

void handleReceivedData()
{
  newData = false;
  isTrainComming = true;
  speed = dataReceived[0];
  countdownTimer = dataReceived[1];
  startTime = millis();
  // reset
  dataReceived[0] = dataReceived[1] = 0.0;
}

void playAlert()
{
  for (i = 0; i < 255; i = i + 2)
  {
    analogWrite(SPEAKER, i);
    delay(10);
  }
  for (i = 255; i > 1; i = i - 2)
  {
    analogWrite(SPEAKER, i);
    delay(5);
  }
  for (i = 1; i <= 10; i++)
  {
    analogWrite(SPEAKER, 200);
    delay(100);
    analogWrite(SPEAKER, 25);
    delay(100);
  }
}

void stopAlert()
{
  analogWrite(SPEAKER, 0);
}

void printData()
{
}

void clearScreen()
{
}

void controlSystem()
{
  static int t = 0;
  t += abs(millis() - startTime);
  if (t >= 1000 * 60)
  {
    countdownTimer -= 1;
    t = 0;
  }
  printData();
  if (countdownTimer < 1)
  {
    controlServo(180);
    countdownTimer = 0;
    startTime = 0;
    isTrainComming = false;
    clearScreen();
    stopAlert();
  }
  else
  {
    playAlert();
  }
}

void loop_station()
{
  if (digitalRead(EMER_BTN))
  {
    alertTrain();
  }
  getData();
  if (newData)
  {
    handleReceivedData();
  }
  if (isTrainComming)
  {
    controlSystem();
  }
  delay(250);
}