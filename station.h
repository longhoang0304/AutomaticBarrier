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

void setup_station();
void loop_station();

#endif /* STATION_H */
