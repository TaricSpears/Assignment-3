// File: mqtt_manager.cpp
#include "config.h"
#include "mqtt_manager.h"
#include <ArduinoJson.h>

static WiFiClient *netClient = nullptr;
static PubSubClient mqttClient(*netClient);

void mqttInit(WiFiClient &wifiClient)
{
    netClient = &wifiClient;
    mqttClient.setClient(wifiClient);
    mqttClient.setServer(MQTT_SERVER, 1883);
}

static void mqttReconnect()
{
    while (!mqttClient.connected())
    {
        if (mqttClient.connect("ESP32Client", MQTT_USER, MQTT_PASS))
        {
            // Connected
        }
        else
        {
            delay(5000);
        }
    }
}

bool mqttPublish(const char *topic, const char *payload, size_t length)
{
    if (!mqttClient.connected())
        mqttReconnect();
    mqttClient.loop();
    return mqttClient.publish(topic, payload, length);
}

void mqttLoop()
{
    if (!mqttClient.connected())
        mqttReconnect();
    mqttClient.loop();
}