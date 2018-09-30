#ifndef TRAIN_H
#define TRAIN_H
#include <Arduino.h>

// RF24
#define CE 7
#define CS 8
#define MOSI 11
#define MISO 12
#define SCK 13

// I2C
#define SDA A4
#define SCL A5

// Hall sensors
#define HALL_PIN 2

// LED
#define LED_PIN 3

// Button
#define EMER_BTN 4 // emergency button
#define RETS_BTN 5 // reset button

// Speaker
#define SPEAKER_PIN 6

void setup_train();
void loop_train();

#endif /* TRAIN_H */
