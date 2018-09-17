#include <Servo.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <station.h>

Servo servo;
RF24 stationRf(CE, CS);
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void printData(int sig, int second = 0);

static const byte stationAddress[5] = {'s', 't', 'a', 't', 'n'};
static const byte trainAddress[5] = {'t', 'r', 'a', 'i', 'n'};
static const byte dataToSend = 1;
static double dataReceived[2] = {0, 0};
static bool newData = false;
static bool isTrainComming = false;
static int countdownTimer = -6;
static int startTime = 0;
static double speed = 0;

// ===============================================================

void setupLCD() {
  lcd.init();
  lcd.backlight();
  printData(0);
}

void sendESP8266Data()
{
  Wire.write(dataToSend);
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
  setupI2C();
  setupServo();
  setupRf();
  setupLCD();
  setupPins();
  Serial.begin(9600);
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

static void getData()
{
  if (stationRf.available())
  {
    stationRf.read(&dataReceived, sizeof(dataReceived));
    if(dataReceived[0] && dataReceived[1]) {
      newData = true;
      // Serial.println(dataReceived[0]);
      // Serial.println(dataReceived[1]);
      // Serial.println();
    }
  }
}

static void handleReceivedData()
{
  newData = false;
  isTrainComming = true;
  speed = dataReceived[0];
  countdownTimer = (int)dataReceived[1];
  startTime = millis();
  // reset
  dataReceived[0] = dataReceived[1] = 0.0;
}

void playAlert()
{
  digitalWrite(SPEAKER_PIN, HIGH);
  digitalWrite(RED_LED1, LOW);
  digitalWrite(RED_LED2, HIGH);
  delay(100);
  digitalWrite(SPEAKER_PIN, LOW);
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
  lcd.clear();
  if (!sig) {
    lcd.setCursor(0, 0);
    lcd.print("   THUONG  LO   ");
    lcd.setCursor(0, 1);
    lcd.print("    BINH  AN    ");
  }
  if(sig == 1) {
    char ld[16]={0};
    sprintf(ld,"THOI GIAN: %d", second);
    lcd.setCursor(0, 0);
    lcd.print("  TAU DANG TOI  ");
    lcd.setCursor(0, 1);
    lcd.print(ld);
  }
  if (sig == 2) {
    lcd.setCursor(0, 0);
    lcd.print("    CANH BAO    ");
    lcd.setCursor(0, 1);
    lcd.print("   NGUY  HIEM   ");
  }
}

void clearScreen()
{
  lcd.clear();
}

void controlSystem()
{
  static int t = 0;
  t += abs(millis() - startTime);
  if (t >= (1000))
  {
    countdownTimer -= 1;
    startTime = millis();
    t = 0;
  }
  if (countdownTimer > 0) {
    printData(1, countdownTimer);
    digitalWrite(YELLOW_LED, HIGH);
    return;
  }
  if (!countdownTimer) {
    printData(2);
    digitalWrite(YELLOW_LED, LOW);
    playAlert();
    return;
  }
  if (countdownTimer < -5)
  {
    controlServo(180);
    startTime = 0;
    isTrainComming = false;
    printData(0);
    stopAlert();
    return;
  }
  playAlert();
}

static bool getEmergencySignal() {
  return digitalRead(EMER_BTN);
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