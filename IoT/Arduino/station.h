#ifndef STATION_H
#define STATION_H
#include "Arduino.h"

// I2C
#define SDA A4
#define SCL A5

// LED
#define YELLOW_LED 2
#define RED_LED1 3
#define RED_LED2 4

// Button
#define EMER_BTN 5
#define RESET_BTN 6

// Hall Sensor
#define HALL_PIN1 7
#define HALL_PIN2 8
#define HALL_PIN3 9

// Barrier, Servo (PWM)
#define BAR_PIN 10

// speaker
#define SPEAKER_PIN 11
#define SPEAKER_PIN2 12

// I2C Address
#define SLAVE_ADDRESS 0x34

enum SYSTEM_SYNC_ACTION {
  // HALL ACTION EVENT
  WAIT_FIRST_HALL_ON,
  WAIT_FIRST_HALL_OFF,
  WAIT_SECOND_HALL_ON,
  WAIT_SECOND_HALL_OFF,
  WAIT_THIRD_HALL_ON,
  WAIT_THIRD_HALL_OFF,
  // STATION ACTION EVENT
  STATION_WARNING, // yellow light on
  STATION_DANGER,
  STATION_WAIT_FOR_TRAIN,
  STATION_PUT_BARRIER_UP,
  // ANOTHER EVENT
  RESET_ALL_STATE,
  TRAIN_DANGER_ALERT,
  TRAIN_DANGER_ACCEPT,
};


void setup_station();
void loop_station();

#endif /* STATION_H */
