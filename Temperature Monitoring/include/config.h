// File: config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <WiFi.h>

// ——— CONFIGURAZIONE WI-FI ———
const char *ssid = "TUO_SSID";
const char *password = "TUA_PASSWORD";

// ——— CONFIGURAZIONE MQTT ———
const char *mqtt_server = "mqtt.eclipse.org"; // o IP del tuo broker
const uint16_t mqtt_port = 1883;
const char *mqtt_user = "pippo";             // se usi auth
const char *mqtt_pass = "pippo";             // se usi auth
const char *pub_topic = "esp32/temperature"; // topic di pubblicazione

// Pin definitions
#define BUTTON_PIN 26
#define GREEN_LED 27
#define RED_LED 14
#define TEMP_SENSOR_PIN 32

#endif // CONFIG_H

// File: mqtt_manager.h
#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <PubSubClient.h>

void mqttInit(WiFiClient &wifiClient);
bool mqttPublish(const char *topic, const char *payload, size_t length);
void mqttLoop();

#endif // MQTT_MANAGER_H