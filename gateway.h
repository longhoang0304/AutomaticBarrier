#ifndef WIFICONTROLLER_H
#define WIFICONTROLLER_H
#include "Arduino.h"

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include "WiFiManager.h"

#define fingerPrint "08:3b:71:72:02:43:6e:ca:ed:42:86:93:ba:7e:df:81:c4:bc:62:30"
#define host "https://dcdcs-api.herokuapp.com/api/%s"
//#define host "http://localhost:3000/api/%s"

#define SLAVE_ADDRESS 8
#define token "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpZCI6IjViM2YxYzI2YWE2YzNkMmUyY2JhZTQxOSIsInBhc3N3b3JkIjoiJDJiJDEwJEE0amtncnloeVZtclQ5Ni55YndnZHVMZGttRVp3NXRlY1pOTUdJLmhRYndvcTRmNmlLNzV5IiwiaWF0IjoxNTMwODYyNjMwfQ.h6zq3nm2I0GLLUC7VyCKheBUa88bUGpgkgMI1uLQwlA"

// static const char * body = "\
//   {\
//     \"payload\": {\
//         \"action\": 3,\
//         \"data\": {\
//           \"temperature\": %d,\
//           \"humidity\": %u,\
//           \"dryingTime\": %u,\
//           \"sysState\": %u,\
//           \"isRain\": %s,\
//           \"isCloud\": %s\
//         }\
//     },\
//     \"to\": \"%s\",\
//     \"from\": {\
//         \"senderId\": \"5b3f1c26aa6c3d2e2cbae419\",\
//         \"deviceId\": \"5b3ccdad5cfca61330ba6b92\"\
//     }\
//   }\
// ";

static const char * body = "{\"payload\":{\"action\":3,\"data\":{\"temperature\":%d,\"humidity\":%u,\"dryingTime\":%u,\"sysState\":%u,\"isRain\":%s,\"isCloud\":%s}},\"to\":\"%s\",\"from\":{\"senderId\":\"5b3f1c26aa6c3d2e2cbae419\",\"deviceId\":\"5b3ccdad5cfca61330ba6b92\"}}";

static IPAddress _ip = IPAddress(10, 0, 1, 1);
static IPAddress _gw = IPAddress(10, 0, 1, 1);
static IPAddress _sn = IPAddress(255, 255, 255, 0);

void setup_event();
void loop_event();

#endif
