// File: main.cpp
#include "config.h"
#include "mqtt_manager.h"
#include "sensor_manager.h"
#include "state_machine.h"
#include <Arduino.h>
#include <ArduinoJson.h>

void setup()
{
  Serial.begin(115200);
  WiFi.begin(SSID, PASSWORD);
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

  mqttPublish(PUB_TOPIC, buf, n);
  delay(5000);
}
