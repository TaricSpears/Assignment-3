// File: mqtt_manager.h
#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <WiFi.h>
#include <PubSubClient.h>

void mqttInit(WiFiClient &wifiClient);
void mqttSetCallback(void (*callback)(char *, uint8_t *, unsigned int));
void mqttLoop();
bool mqttConnect();
bool mqttPublish(const char *topic, const char *payload, size_t length);
bool mqttPublishWithTimestamp(const char *topic, const char *payload, size_t length);
bool mqttIsConnected();
unsigned long getTimestamp();
String getISOTimestamp();

#endif
