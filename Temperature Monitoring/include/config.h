// File: config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <WiFi.h>

// ——— CONFIGURAZIONE WI-FI ———
#define SSID "D-Link-381D17"
#define PASSWORD "b9cgaefsqsj64997"

// ——— CONFIGURAZIONE MQTT ———
#define MQTT_SERVER "test.mosquitto.org"
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASS ""

// ——— TOPIC MQTT ———
#define TEMP_TOPIC "esp32/temperature" // Invio dati temperatura
#define STATUS_TOPIC "esp32/status"    // Invio stato ESP32
#define STATE_TOPIC "esp32/state"      // Ricezione comandi stato dalla control unit

// ——— PIN DEFINITIONS ———
// Rimosso BUTTON_PIN - non più necessario
#define GREEN_LED 21      // Verde = tutto OK
#define RED_LED 47        // Rosso = errore di rete
#define TEMP_SENSOR_PIN 3 // Pin sensore temperatura

// ——— CONFIGURAZIONE SENSORE ———
#define TMP36_OFFSET 0.0 // Offset calibrazione in °C
#define VOLTAGE_REF 3.3  // Tensione riferimento ESP32

// ——— SOGLIE E TIMING (backup - la logica principale è nella control unit) ———
#define TEMP_HOT_BACKUP 30.0   // Soglia temperatura alta (backup)
#define TEMP_ALARM_BACKUP 40.0 // Soglia allarme (backup)

#endif // CONFIG_H