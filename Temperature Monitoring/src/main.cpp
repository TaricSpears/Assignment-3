// File: main.cpp
#include "include/config.h"
#include "include/mqtt_manager.h"
#include "include/sensor_manager.h"
#include "include/state_machine.h"
#include <Arduino.h>

void setup()
{
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
    delay(500);

  sensorInit();
  stateInit();

  static WiFiClient wifiClient;
  mqttInit(wifiClient);

  Serial.println("Setup completato");
}

void loop()
{
  float temp = readTemperature();
  updateState(temp);
  updateLEDs();

  // Prepara JSON
  StaticJsonDocument<200> doc;
  doc["temperature"] = temp;
  doc["state"] = getStateString();
  char buf[128];
  size_t n = serializeJson(doc, buf);

  mqttPublish(MQTT_TOPIC, buf, n);
  delay(5000);
}
