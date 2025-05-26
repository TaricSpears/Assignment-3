// File: mqtt_manager.h
#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <PubSubClient.h>

void mqttInit(WiFiClient &wifiClient);
bool mqttPublish(const char *topic, const char *payload, size_t length);
void mqttLoop();

#endif // MQTT_MANAGER_H