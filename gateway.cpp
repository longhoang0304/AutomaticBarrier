
// #include "gateway.h"

// enum WifiRequestType { PERFORM_ACTION, UPDATE_IP };

// bool connected = false;
// char to[65] = {0};

// const char * tf[2] = {"false", "true"};

// String AP_SSID = "Automatic Barrier Wifi";
// String AP_PASS = "123456789";

// String WIFI_SSID = "";
// String WIFI_PASS = "";

// HTTPClient http;
// ESP8266WebServer server(80);
// WiFiManager wifiManager;
// DNSServer dnsServer;

// // https://gist.github.com/bbx10/5a2885a700f30af75fc5

// void setupHttpRequest() {
//   http.begin(
//     host,
//     fingerPrint
//   );
//   http.addHeader("Content-Type", "application/json");
// }

// void setupI2C() {
//   Wire.begin(D1, D2);
// }

// void readDataFromArduino() {
//   int len = 4;
//   char buf[640] = {0};
//   int speed = 0;
//   int decimal = 0;


//   Wire.requestFrom(SLAVE_ADDRESS, len);
//   while(Wire.available()) {
//     int j = 0;
//     for(;len > 2; len--) {
//       uint16_t c = (uint16_t)Wire.read();
//       speed += c << j;
//       j += 8;
//     }
//     j = 0;
//     for(; len > 0; len--) {
//       uint16_t c = (uint16_t)Wire.read();
//       decimal += c << j;
//       j += 8;
//     }
//   }
//   // Serial.println(speed);
//   // Serial.println(decimal);
//   // Serial.println();
//   if (speed == 0 && decimal == 0) return;
//   if (speed >= 500 && decimal >=500) return;
//   if (speed < 0 || decimal < 0) return;

//   sprintf(buf, body,
//     speed,
//     decimal
//   );
//   setupHttpRequest();
//   http.POST(buf);
//   http.end();
// }

// void setupESP() {
//   // wifiManager.resetSettings();
//   wifiManager.setAPStaticIPConfig(_ip, _gw, _sn);
//   wifiManager.autoConnect(AP_SSID.c_str(), AP_PASS.c_str());
// }


// void setup_event() {
//   setupI2C();
//   setupESP();
//   // Serial.begin(9600);
// }

// void loop_event() {
//   if(WiFi.status() != WL_CONNECTED) {
//     setupESP();
//     return;
//   }
//   readDataFromArduino();
//   delay(1000);
// }
