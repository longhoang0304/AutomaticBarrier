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
#define GAP_SIZE 40
#define DIST_TO_STATION 150

Servo servo;
// RF24 stationRf(CE, CS);
LiquidCrystal_I2C lcd(0x3f, 16, 2);

void printData(int sig, int second = 0);

static bool isTrainComming = false;
static bool trainAlert = false;
static int countdownTimer = -11;
static unsigned long startTime = 0;
static double speed = 0;
static double speedForEsp = 0;
static short int servoAngle = DEFAULT_ANGLE;
static short int angleGap = DEFAULT_ANGLE / DEFAULT_DANGER_TIME;

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
  copyValueToByteArray((int)speedForEsp, data, i);
  copyValueToByteArray((int)((speedForEsp - (int)speedForEsp) * 1000), data, i);

  speedForEsp = 0;
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
}

void setup_station()
{
  setupServo();
  // setupRf();
  setupLCD();
  setupPins();
  setupI2C();
  Serial.begin(9600);
}

// ===============================================================

static void handleCalculatedSpeed()
{
  isTrainComming = true;
  countdownTimer = DIST_TO_STATION / speed;;
  if (countdownTimer < DEFAULT_DANGER_TIME) {
    angleGap = ((MAX_ANGLE - DEFAULT_ANGLE) / countdownTimer);
  } else {
    angleGap = ((MAX_ANGLE - DEFAULT_ANGLE) / DEFAULT_DANGER_TIME);
  }

  startTime = millis();
  speed = -1;
  lcd.clear();
}

void playAlertSoundOnly()
{
  digitalWrite(SPEAKER_PIN, HIGH);
}

void alertTrain()
{
  digitalWrite(SPEAKER_PIN2, HIGH);
  digitalWrite(RED_LED2, HIGH);
  delay(100);
  digitalWrite(RED_LED2, LOW);
  delay(100);
}


void playAlert()
{
  digitalWrite(RED_LED1, HIGH);
  delay(100);
  digitalWrite(RED_LED1, LOW);
  delay(100);
}

void stopAlert()
{
  digitalWrite(SPEAKER_PIN, LOW);
  digitalWrite(RED_LED1, LOW);
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
  if (passedTime < 2) {
    digitalWrite(YELLOW_LED, HIGH);
  } else {
    digitalWrite(YELLOW_LED, LOW);
    playAlert();
  }
  if (
    countdownTimer > DEFAULT_DANGER_TIME && countdownTimer <= DEFAULT_DANGER_TIME + 3
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
  if (countdownTimer < 0) {
    servoAngle += angleGap;
    controlServo(servoAngle);
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

static int getHallSensorSignal() {
  if(digitalRead(HALL_PIN1)) return 1;
  if(digitalRead(HALL_PIN2)) return 2;
  if(digitalRead(HALL_PIN3)) return 3;
  return 0;
}

static double calculateSpeed() {
  static byte count = 0;
  static int firstHitTime = 0;
  static int lastHitTime = 0;
  int hallSignal = getHallSensorSignal();

  // wait for first signal
  if (hallSignal == 1 && count == 0) {
    count = 1;
    return -1;
  }

  if (hallSignal == 0 && count == 1) {
    count = 2;
    firstHitTime = millis() / 1000;
    return -1;
  }

  // wait for second signal
  if (hallSignal == 2 && count == 2) {
    count = 3;
    return -1;
  }

  if (hallSignal == 0 && count == 3) {
    count = 4;
    lastHitTime = millis() / 1000;
    double speed = (double)GAP_SIZE / abs((lastHitTime - firstHitTime));
    return speed;
  }

  // waiting for done signal
  if (hallSignal == 3 && count == 4) {
    count = 5;
    return -1;
  }

  if (hallSignal == 0 && count == 5) {
    count = 0;
    return -1;
  }

  return -1;
}

static bool getEmergencySignal() {
  return digitalRead(EMER_BTN);
}

void loop_station()
{
  if (digitalRead(EMER_BTN))
  {
    trainAlert = true;
  }
  // if (digitalRead(ALERT_BTN))
  // {
  //   trainAlert = true;
  // }
  if (digitalRead(RESET_BTN))
  {
    trainAlert = false;
  }
  speed = calculateSpeed();
  if (speed > 0)
  {
    speedForEsp = speed;
    handleCalculatedSpeed();
  }
  if (isTrainComming)
  {
    controlSystem();
  }
  if (trainAlert) {
    alertTrain();
  }
  delay(25);
}