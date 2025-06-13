// File: mqtt_manager.h
#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <PubSubClient.h>

void mqttInit(WiFiClient &wifiClient);
bool mqttPublish(const char *topic, const char *payload, size_t length);
bool mqttPublishWithTimestampPrefix(const char *topic, const char *payload, size_t length);
bool mqttPublishJsonWithTimestamp(const char *topic, const char *jsonPayload, size_t length);
bool mqttPublishWithTimestamp(const char *topic, const char *payload, size_t length);
unsigned long getTimestamp();
String getISOTimestamp();
static void mqttReconnect();
void mqttLoop();
void mqttSubscribe(const char *topic);                                   // Nuova funzione per subscription
void mqttSetCallback(void (*callback)(char *, uint8_t *, unsigned int)); // Nuova funzione per callback

#endif // MQTT_MANAGER_H