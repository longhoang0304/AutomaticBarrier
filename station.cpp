#include <Servo.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <station.h>

#define DEFAULT_ANGLE 35
#define MAX_ANGLE 125
#define DEFAULT_DANGER_TIME 5

Servo servo;
RF24 stationRf(CE, CS);
LiquidCrystal_I2C lcd(0x3f, 16, 2);

void printData(int sig, int second = 0);

static const byte stationAddress[5] = {'s', 't', 'a', 't', 'n'};
static const byte trainAddress[5] = {'t', 'r', 'a', 'i', 'n'};
static const byte dataToSend[2] = {1, 0x34};
static double dataReceived[4] = {0, 0, 0, 0};
static bool newData = false;
static bool isTrainComming = false;
static int countdownTimer = -11;
static unsigned long startTime = 0;
static double speed = 0;
static short int servoAngle = DEFAULT_ANGLE;
static short int angleGap = DEFAULT_ANGLE / 5;

// ===============================================================

void setupLCD() {
  lcd.init();
  lcd.backlight();
  printData(0);
}

byte *convertValueToByteArray(uint16_t value) {
  if (value <= 1) value = 0;
  uint16_t v = value;
  const size_t len = sizeof(uint16_t);
  byte index = 0;
  byte * arr;
  arr = (byte *)calloc(len, sizeof(byte));
  while(v) {
    arr[index] = v & 0xff;
    v >>= 0x08;
    index++;
  }
  return arr;
}

void copyValueToByteArray(uint16_t value, byte *data, byte &i) {
  byte * arr = convertValueToByteArray(value);
  byte len = sizeof(uint16_t);
  byte j = 0;
  while(len) {
    data[i++] = arr[j++];
    len--;
  }
  free(arr);
}

void sendESP8266Data()
{
  const size_t len = sizeof(uint16_t) * 2;
  byte i = 0;
  byte data[len] = {0};
  // copy temperatur to array
  copyValueToByteArray((int)speed, data, i);
  // copy humidity to array
  copyValueToByteArray((int)((speed - (int)speed) * 1000), data, i);

  speed = 0;
  Wire.write(data, len);
}

void controlServo(int angle)
{
  servo.write(angle);
}

void setupI2C()
{
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(sendESP8266Data);
}

void setupServo()
{
  servo.attach(BAR_PIN);
  controlServo(DEFAULT_ANGLE);
}

static void setupPins() {
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED1, OUTPUT);
  pinMode(RED_LED2, OUTPUT);
  pinMode(EMER_BTN, INPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
}

static void setupRf()
{
  stationRf.begin();
  stationRf.openWritingPipe(stationAddress);
  stationRf.openReadingPipe(1, trainAddress);
  stationRf.setRetries(3, 5);
  stationRf.startListening();
}

void setup_station()
{
  setupServo();
  setupRf();
  setupLCD();
  setupPins();
  setupI2C();
  Serial.begin(9600);
}

// ===============================================================

void alertTrain()
{
  stationRf.stopListening();
  bool rslt;
  rslt = stationRf.write(&dataToSend, sizeof(dataToSend));
  stationRf.startListening();

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
  if (stationRf.available())
  {
    stationRf.read(&dataReceived, sizeof(dataReceived));
    if ((int)dataReceived[3] != 0x34) {
      return;
    }
    if(dataReceived[0] && dataReceived[1]) {
      newData = true;
      // Serial.println(dataReceived[0]);
      // Serial.println(dataReceived[1]);
      // Serial.println(dataReceived[3]);
      // Serial.println();
    }
  }
}

static void handleReceivedData()
{
  newData = false;
  isTrainComming = true;
  speed = dataReceived[0];
  countdownTimer = (int)dataReceived[1] + 1;
  if (countdownTimer < DEFAULT_DANGER_TIME) {
    angleGap = ((MAX_ANGLE - DEFAULT_ANGLE) / countdownTimer);
  } else {
    angleGap = ((MAX_ANGLE - DEFAULT_ANGLE) / DEFAULT_DANGER_TIME);
  }
  
  startTime = millis();
  // reset
  dataReceived[0] = dataReceived[1] = 0.0;
  dataReceived[2] = dataReceived[3] = 0.0;
  lcd.clear();
}

void playAlertSoundOnly()
{
  digitalWrite(SPEAKER_PIN, HIGH);
}

void playAlert()
{
  digitalWrite(RED_LED1, LOW);
  digitalWrite(RED_LED2, HIGH);
  delay(100);
  digitalWrite(RED_LED1, HIGH);
  digitalWrite(RED_LED2, LOW);
  delay(100);
}

void stopAlert()
{
  digitalWrite(SPEAKER_PIN, LOW);
  digitalWrite(RED_LED1, LOW);
  digitalWrite(RED_LED2, LOW);
  digitalWrite(YELLOW_LED, LOW);
}

void printData(int sig, int second = 0)
{
  // lcd.clear();
  if (!sig) {
    lcd.setCursor(0, 0);
    lcd.print("   THUONG  LO   ");
    lcd.setCursor(0, 1);
    lcd.print("    BINH  AN    ");
    return;
  }
  if(sig == 1) {
    char ld[16]={0};
    sprintf(ld,"THOI GIAN: %02d", second);
    lcd.setCursor(0, 0);
    lcd.print("  TAU DANG TOI  ");
    lcd.setCursor(0, 1);
    lcd.print(ld);
    return;
  }
  if (sig == 2) {
    lcd.setCursor(0, 0);
    lcd.print("    CANH BAO    ");
    lcd.setCursor(0, 1);
    lcd.print("   NGUY  HIEM   ");
    return;
  }
}

void clearScreen()
{
  lcd.clear();
}

void controlSystem()
{
  bool resetTime = false;
  static long t = 0;
  static unsigned long passedTime = 0;
  t += (unsigned long)abs(millis() - startTime);
  if (t >= (5000))
  {
    countdownTimer -= t / 5000;
    passedTime += t / 5000;
    startTime = millis();
    t -= (5000 * (t / 5000));
    if (t < 0) t = 0;
    resetTime = true;
  }
  if (passedTime < 3) {
    digitalWrite(YELLOW_LED, HIGH);
  } else {
    digitalWrite(YELLOW_LED, LOW);
    playAlert();
  }
  if (
    countdownTimer > DEFAULT_DANGER_TIME && countdownTimer <= DEFAULT_DANGER_TIME + 5
  ) {
    if(resetTime || !t) {
      servoAngle += angleGap;
      controlServo(servoAngle);
    }
  }
  if (countdownTimer > 0) {
    printData(1, countdownTimer);
    return;
  }
  if (!countdownTimer) {
    printData(2);
    return;
  }
  if (countdownTimer < -3)
  {
    startTime = 0;
    isTrainComming = false;
    printData(0);
    stopAlert();
    controlServo(DEFAULT_ANGLE);
    servoAngle = DEFAULT_ANGLE;
    passedTime = 0;
    return;
  }
  playAlert();
}

static bool getEmergencySignal() {
  return digitalRead(EMER_BTN);
}

void loop_station()
{
  // playAlert();
  // Serial.println(digitalRead(EMER_BTN));
  // Serial.println();
  // return;
  if (digitalRead(EMER_BTN))
  {
    alertTrain();
    // playAlert();
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
  delay(100);
}