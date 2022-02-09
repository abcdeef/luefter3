#include "credentials.h"
#include <ESP8266WiFi.h>

void initHTTPServer() {
  server.serveStatic("/style.css", SPIFFS, "/style.css");
  //server.serveStatic("/system.css", SPIFFS, "/system.css");
  server.serveStatic("/meins.js", SPIFFS, "/meins.js");
  server.serveStatic("/index.js", SPIFFS, "/index.js");
  //server.serveStatic("/system.js", SPIFFS, "/system.js");
  server.serveStatic("/favicon.ico", SPIFFS, "/favicon.ico");

  server.serveStatic("/", SPIFFS, "/index.html");

  httpUpdater.setup(&server);
  server.begin();
}
void webSocketProcess( uint8_t * data, size_t len) {
  String asd;
  switch (data[0]) {
    case 0:
      /* alles */
      asd = "{\"VERSION\": \"" + String(VERSION) + "\",\"RPM\": " + rpm + " , \"TEMP\": " + String(temp.val) + ",\"PWMS\": [";

      for (uint8_t n = 0; n < sizeof(pwms) / sizeof(T_PWM); n++) {
        if (n > 0 ) {
          asd += ',';
        }
        asd +=  pwms[n].toJSON();
      }
      asd += "],\"BR\":125,\"ARGBS\":[";

      for (int i = 0; i < NUMPIXELS; i++) {
        if (i > 0) {
          asd += ",";
        }
        asd += "{\"ID\":" + String(i) + ",\"ENABLED\":" + argb[i].n + ",\"COLOR\":" + argb[i].color + "}";
      }
      asd += "]}";

      webSocket.broadcastTXT(asd);
      break;
    case 1:
      /* set PWM */
      for (uint8_t n = 0; n < sizeof(pwms) / sizeof(T_PWM); n++) {
        if (pwms[n].pin == data[1]) {
          pwms[n].setVal(data[2]);
        }
      }
      break;
    case 2:
      /* set Color einer LED */
      //argb[data[1]].n = true;
      argb[data[1]].color = pixels.Color(data[2], data[3], data[4]);
      break;
    case 3:
      /* set W */
      pixels.setBrightness(data[1]);
      
      break;
  }
}
void webSocketEvent(byte num, WStype_t type, uint8_t * data, size_t len) {
  /*IPAddress ip = webSocket.remoteIP(num);
    Serial.printf("[%u] %d. %d. %d. %d: %d |", num, ip[0], ip[1], ip[2], ip[3], type);
    for (uint8_t n = 0; n < len; n++) {
    Serial.print(data[n]);
    Serial.print(" ");
    }
    Serial.println("|");
  */
  switch (type) {
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("WebSocket client [ % u] connected from % d. % d. % d. % d\n", num, ip[0], ip[1], ip[2], ip[3]);
      } break;
    case WStype_DISCONNECTED:
      Serial.printf("WebSocket client [ % u] Disconnected!\n", num);
      break;
    case WStype_BIN:
      webSocketProcess(data, len);
      break;
    case WStype_TEXT:
      Serial.println("WStype_TEXT");
      break;
  }
}

void onStationModeGotIP(const WiFiEventStationModeGotIP & event) {
  digitalWrite(LED_BUILTIN, HIGH);

  WiFi.softAPdisconnect(true);
  Serial.println(WiFi.localIP());

  ArduinoOTA.setHostname(HOSTNAME);
  //ArduinoOTA.setPassword(OTA_PW);
  ArduinoOTA.begin();

  //initMQTT();
}
void onSoftAPModeStationConnected(const WiFiEventSoftAPModeStationConnected & event) {
  digitalWrite(LED_BUILTIN, HIGH);
}
void onStationModeDisconnected(const WiFiEventStationModeDisconnected & event) {
  digitalWrite(LED_BUILTIN, LOW);
  WiFi.begin(ssid, password);
}
void initWiFi() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_AP_STA);

  stationConnectedHandler = WiFi.onSoftAPModeStationConnected(&onSoftAPModeStationConnected);
  gotIpEventHandler = WiFi.onStationModeGotIP(&onStationModeGotIP);
  disconnectedEventHandler = WiFi.onStationModeDisconnected(&onStationModeDisconnected);


  WiFi.begin(ssid, password);

  initHTTPServer();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}
