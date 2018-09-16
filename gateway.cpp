
#include "WifiController.h"

enum WifiRequestType { PERFORM_ACTION, UPDATE_IP };

bool connected = false;
char to[65] = {0};

const char * tf[2] = {"false", "true"};

String AP_SSID = "DCDCS System Wifi";
String AP_PASS = "123456789";

String WIFI_SSID = "";
String WIFI_PASS = "";

HTTPClient http;
ESP8266WebServer server(80);
WiFiManager wifiManager;
DNSServer dnsServer;
StaticJsonBuffer<512> jsonBuffer;

// https://gist.github.com/bbx10/5a2885a700f30af75fc5

void setupHttpRequest(char* url) {
  char buf[255] = {0};
  sprintf(buf, host, url);
  http.begin(
    buf,
    fingerPrint
  );
  http.addHeader("Content-Type", "application/json");
  http.addHeader("x-access-token", token);
}

void setupI2C() {
  Wire.begin(D1, D2);
}

bool sendDataArduino() {
  setupHttpRequest("actions/5b3ccdad5cfca61330ba6b92");
  int httpStatus = http.GET();
  if (httpStatus < 0 || httpStatus / 100 != 2) {
    http.end();
    return false;
  }
  JsonObject& root = jsonBuffer.parseObject(http.getString());
  http.end();
  int action = root["payload"]["action"];
  int timer = root["payload"]["timer"];
  if (!action) {
    const char * t = root["from"]["deviceId"];
    if (t != NULL && strlen(t) > 0) {
      strcpy(to, t);
    }
    jsonBuffer.clear();
    return true;
  }
  jsonBuffer.clear();
  Serial.println(action);
  Serial.println(timer);
  delay(1);
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(0);
  Wire.write((byte)action);
  Wire.write((byte)timer);
  Wire.endTransmission();
  return false;
}

void readDataFromArduino() {
  const int len = 8;
  char buf[640] = {0};
  uint16_t data[len] = {0};
  byte index = 0;

  Wire.requestFrom(SLAVE_ADDRESS, len);
  while(Wire.available()) {
    byte c = Wire.read();
    data[index++] = (uint16_t)c;
    // Serial.println(c);
  }
  // Serial.println();
  data[6] %= 2;
  data[7] %= 2;
  sprintf(buf, body,
    (int16_t)((int16_t)data[0] | (int16_t)data[1] << 8),
    data[2] | data[3] << 8,
    data[4],
    data[5],
    tf[data[6]],
    tf[data[7]],
    to
  );
  setupHttpRequest("actions");
  http.POST(buf);
  http.end();
}

void setupESP() {
  // wifiManager.resetSettings();
  wifiManager.setAPStaticIPConfig(_ip, _gw, _sn);
  wifiManager.autoConnect(AP_SSID.c_str(), AP_PASS.c_str());
}


void setup_event() {
  Serial.begin(9600);
  setupI2C();
  setupESP();
}

void loop_event() {
  if(WiFi.status() != WL_CONNECTED) {
    setupESP();
    return;
  }
  if(sendDataArduino()) {
    readDataFromArduino();
  }
  delay(50);
}
