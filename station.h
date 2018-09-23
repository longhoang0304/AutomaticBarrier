#ifndef STATION_H
#define STATION_H
#include "Arduino.h"

// RF24
#define CE 7
#define CS 8
#define MOSI 11
#define MISO 12
#define SCK 13

// I2C
#define SDA A4
#define SCL A5

// LED
#define YELLOW_LED 2
#define RED_LED1 3
#define RED_LED2 4

// Button
#define EMER_BTN 5

// Barrier, Servo (PWM)
#define BAR_PIN 6

// speaker
#define SPEAKER_PIN 9

// I2C Address
#define SLAVE_ADDRESS 0x34

void setup_station();
void loop_station();

#endif /* STATION_H */
