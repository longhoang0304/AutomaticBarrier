#include <Servo.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <station.h>

#define DEFAULT_ANGLE 35
#define MAX_ANGLE 125
#define DEFAULT_DANGER_TIME 3
#define GAP_SIZE 30
#define DIST_TO_STATION 150

Servo servo;
// RF24 stationRf(CE, CS);
LiquidCrystal_I2C lcd(0x3f, 16, 2);

void printData(int sig, int second = 0);

static int countdownTimer = 0;
static double speed = 0;
static short int servoAngle = DEFAULT_ANGLE;
static short int angleGap = DEFAULT_ANGLE / DEFAULT_DANGER_TIME;
static long timer = 0;
static long totalTime = 0;
static long clocker = 0;
SYSTEM_SYNC_ACTION systemAction;

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
  // copy speed to array
  copyValueToByteArray((int)speed, data, i);
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
  // pinMode(ALERT_BTN, INPUT);
  pinMode(RESET_BTN, INPUT);

  pinMode(HALL_PIN1, INPUT);
  pinMode(HALL_PIN2, INPUT);
  pinMode(HALL_PIN3, INPUT);

  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(SPEAKER_PIN2, OUTPUT);
}

void setup_station()
{
  setupServo();
  // setupRf();
  setupLCD();
  setupPins();
  setupI2C();
  systemAction = WAIT_FIRST_HALL_ON;
  Serial.begin(9600);
}

// ===============================================================

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

static void calculateSpeed(short t, byte hit) {
  static short timePassed = 0;
  if (!hit) timePassed = t;
  if (hit) {
    timePassed = t - timePassed;
    speed = GAP_SIZE / (timePassed*1.0);
    countdownTimer = (speed / DIST_TO_STATION) * 1000;
  }
}

void warning() {
  if(clocker < 4)
    digitalWrite(YELLOW_LED, HIGH);
  else
    digitalWrite(YELLOW_LED, LOW);
  digitalWrite(SPEAKER_PIN, HIGH);
}

void danger() {
  digitalWrite(YELLOW_LED, LOW);
  if (clocker < 4)
    digitalWrite(RED_LED1, HIGH);
  else
    digitalWrite(RED_LED1, LOW);
}

void dangerTrain() {
  digitalWrite(SPEAKER_PIN2, HIGH);
  if (clocker < 4)
    digitalWrite(RED_LED2, HIGH);
  else
    digitalWrite(RED_LED2, LOW);
}

void resetTrainDanger() {
  digitalWrite(SPEAKER_PIN2, LOW);
  digitalWrite(RED_LED2, LOW);
}

void handleWarningTime() {
  totalTime += millis() - timer;
  if (totalTime >= 2000) {
    systemAction = STATION_DANGER;
    countdownTimer -= totalTime;
    totalTime = 0;
  }
  timer = millis();
}

void handleDangerTime() {
  static long localDangerTimer = millis();
  localDangerTimer += millis() - localDangerTimer;
  if (localDangerTimer >= 500) {
    servoAngle += angleGap;
    if (servoAngle <= MAX_ANGLE)
      controlServo(servoAngle);
    localDangerTimer = millis();
  }

  totalTime += millis() - timer;
  if (totalTime >= 4000) {
    systemAction = STATION_WAIT_FOR_TRAIN;
    countdownTimer -= totalTime;
    totalTime = 0;
  }
  timer = millis();
}

void handleWaitTime() {
  totalTime += millis() - timer;

  if (totalTime > countdownTimer) {
    systemAction = WAIT_THIRD_HALL_ON;
    countdownTimer = 0;
    totalTime = 0;
  }
  timer = millis();
}

void handlePutUpBarrierTimer() {
  static long localBarrierTimer = millis();
  localBarrierTimer += millis() - localBarrierTimer;
  if (localBarrierTimer >= 250) {
    servoAngle -= angleGap;
    if (servoAngle >= DEFAULT_ANGLE)
      controlServo(servoAngle);
    else
      systemAction = RESET_ALL_STATE;
    localBarrierTimer = millis();
  }
}

static int getHallSensorSignal() {
  if(digitalRead(HALL_PIN1)) return 1;
  if(digitalRead(HALL_PIN2)) return 2;
  if(digitalRead(HALL_PIN3)) return 3;
  return 0;
}

void resetAll() {
  timer = 0;
  // speed = 0;
  totalTime = 0;
  countdownTimer = 0;
  servoAngle = DEFAULT_ANGLE;
  systemAction = WAIT_FIRST_HALL_ON;
  angleGap = DEFAULT_ANGLE / DEFAULT_DANGER_TIME;
  digitalWrite(RED_LED1, LOW);
  digitalWrite(SPEAKER_PIN, LOW);
  controlServo(servoAngle);
  printData(0);
}

void controlSystem()
{
  switch(systemAction) {
    // HALL SENSOR ACTION
    case WAIT_FIRST_HALL_ON:
      if (getHallSensorSignal() == 1) {
        systemAction = WAIT_FIRST_HALL_OFF;
      }
      break;
    case WAIT_FIRST_HALL_OFF:
      if (getHallSensorSignal() == 0) {
        systemAction = WAIT_SECOND_HALL_ON;
        calculateSpeed(millis() / 1000, 0);
      }
      break;
    case WAIT_SECOND_HALL_ON:
      if (getHallSensorSignal() == 2) {
        systemAction = WAIT_SECOND_HALL_OFF;
      }
      break;
    case WAIT_SECOND_HALL_OFF:
      if (getHallSensorSignal() == 0) {
        systemAction = STATION_WARNING;
        calculateSpeed(millis() / 1000, 1);
        timer = millis();
      }
      break;
    case WAIT_THIRD_HALL_ON:
      danger();
      if (getHallSensorSignal() == 3) {
        systemAction = WAIT_THIRD_HALL_OFF;
      }
      break;
    case WAIT_THIRD_HALL_OFF:
      danger();
      if (getHallSensorSignal() == 0) {
        systemAction = STATION_PUT_BARRIER_UP;
      }
      break;
    // HALL SENSOR ACTION END
    case STATION_WARNING:
      printData(
        1,
        (countdownTimer - totalTime) / 1000
      );
      warning();
      handleWarningTime();
      break;
    case STATION_DANGER:
      printData(
        1,
        (countdownTimer - totalTime) / 1000
      );
      danger();
      handleDangerTime();
      break;
    case STATION_WAIT_FOR_TRAIN:
      danger();
      printData(2);
      handleWaitTime();
      break;
    case STATION_PUT_BARRIER_UP:
      danger();
      handlePutUpBarrierTimer();
      break;
    case RESET_ALL_STATE:
      resetAll();
      break;
    case TRAIN_DANGER_ALERT:
      dangerTrain();
      break;
    case TRAIN_DANGER_ACCEPT:
      resetTrainDanger();
      break;
    default:
      break;
  }
}

static bool getEmergencySignal() {
  return digitalRead(EMER_BTN);
}

static bool getResetSignal() {
  return digitalRead(RESET_BTN);
}

void listenTrainAction() {
  if (getEmergencySignal()) {
    systemAction = TRAIN_DANGER_ALERT;
    return;
  }
  if (getResetSignal()) {
    systemAction = TRAIN_DANGER_ACCEPT;
  }
  if (getHallSensorSignal() == 3) {
    systemAction = WAIT_THIRD_HALL_OFF;
  }
}

void loop_station()
{
  clocker = (clocker + 1) % 8;
  listenTrainAction();
  controlSystem();
  delay(25);
}