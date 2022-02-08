#include <ArduinoOTA.h>
#include <Wire.h>
#include <stdio.h>
#include <FS.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WebSocketsServer.h>
#include <time.h>
//#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include "pwm.h"
#include "temp.h"

#define VERSION  3.03
#define HOSTNAME "luefter"

WiFiEventHandler gotIpEventHandler, disconnectedEventHandler, stationConnectedHandler;
WebSocketsServer webSocket(81);
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

//PubSubClient mqtt(MQTT_HOST, MQTT_PORT, callback, wifiClient);


#define NUMPIXELS 14
#define PIN       12
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
struct T_ARGB {
  uint32_t color;
  bool n;
};
T_ARGB argb[NUMPIXELS];


int rpm = 0;

T_TEMP temp;
T_PWM pwms[] = {
  {13, "FRONT2"},
  {5, "FRONT1"},
  {15, "TOP"},
  {4, "WAKU"}
};

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A0, INPUT);
  digitalWrite(LED_BUILTIN, LOW);

  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  initWiFi();
  configTime("CET - 1CEST, M3.5.0 / 02, M10.5.0 / 03", "pool.ntp.org");

  for (uint8_t n = 0; n < sizeof(pwms) / sizeof(T_PWM); n++) {
    pinMode(pwms[n].pin, OUTPUT);
    delay(500);
    pwms[n].setVal(pwms[n].val);
  }
  //analogWriteFreq(25000);

  pinMode(14, INPUT);
  pixels.begin();
  pixels.clear();

  for (int i = 0; i < NUMPIXELS; i++) {
    argb[i].n = false;
    argb[i].color = 0;
  }
}

uint8_t getF1(int t) {
  int a = 4 * t / 3 - 20;
  a = (a > 60) ? 60 : a;
  return (a < 20) ? 20 : a;
}
uint8_t getF2(int t) {
  int a = 3 * t / 2 - 40;
  a = ( a > 50) ? 50 : a;

  a = floor(a / 5) * 5;
  return (a < 20) ? 20 : a;
}

#define DELAYVAL 100
unsigned long old_millis = 0;
bool argb_show = false;
void loop() {
  if (millis() - old_millis > 2000 ) {
    unsigned long pulse = pulseIn(14, LOW);
    rpm = (pulse == 0) ? 0 : 15000000 / pulse;

    temp.get();
    uint8_t asd = getF1(temp.val);
    pwms[0].setVal(asd);
    pwms[1].setVal(asd);

    pwms[3].setVal(getF2(temp.val));

    pixels.clear();
    for (int i = 0; i < NUMPIXELS; i++) {
      if (argb[i].n) {
        Serial.println(i);
        pixels.setPixelColor(i, argb[i].color);
        argb_show = true;
      }
    }
    pixels.show();
    /*if (argb_show) {
      pixels.show();
      argb_show = false;
      }*/

    old_millis = millis();
  }

  server.handleClient();
  webSocket.loop();
  ArduinoOTA.handle();

  delay(50);
}
