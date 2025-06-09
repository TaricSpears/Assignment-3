// File: config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <WiFi.h>

// ——— CONFIGURAZIONE WI-FI ———
#define SSID "D-Link-381D17"
#define PASSWORD "b9cgaefsqsj64997"

// ——— CONFIGURAZIONE MQTT ———
#define MQTT_SERVER "test.mosquitto.org" // o IP del tuo broker
#define MQTT_PORT 1883
#define MQTT_USER ""                  // se usi auth
#define MQTT_PASS ""                  // se usi auth
#define PUB_TOPIC "esp32/temperature" // topic di pubblicazione

// Pin definitions
#define BUTTON_PIN 40
#define GREEN_LED 21
#define RED_LED 47
#define TEMP_SENSOR_PIN 3
#define TMP36_OFFSET 0.0 // Offset di calibrazione in °C
#define VOLTAGE_REF 3.3  // Tensione di riferimento dell'ESP32

#endif // CONFIG_H