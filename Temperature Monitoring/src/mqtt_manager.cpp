
// File: mqtt_manager.cpp
#include "config.h"
#include "mqtt_manager.h"
#include <ArduinoJson.h>
#include <time.h>

static WiFiClient *netClient = nullptr;
static PubSubClient mqttClient(*netClient);
static unsigned long lastReconnectAttempt = 0;

void mqttInit(WiFiClient &wifiClient)
{
    netClient = &wifiClient;
    mqttClient.setClient(wifiClient);
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

    // Configura NTP per timestamp
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.println("MQTT Manager inizializzato");
}

void mqttSetCallback(void (*callback)(char *, uint8_t *, unsigned int))
{
    mqttClient.setCallback(callback);
}

bool mqttConnect()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return false;
    }

    Serial.print("Tentativo connessione MQTT...");

    // Genera client ID unico
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASS))
    {
        Serial.println(" connesso!");

        // Subscribe ai topic necessari
        mqttClient.subscribe("esp32/state");
        Serial.println("Sottoscritto a: esp32/state");

        return true;
    }
    else
    {
        Serial.print(" fallito, rc=");
        Serial.println(mqttClient.state());
        return false;
    }
}

bool mqttIsConnected()
{
    return mqttClient.connected();
}

void mqttLoop()
{
    if (!mqttClient.connected())
    {
        unsigned long now = millis();
        // Riconnetti ogni 5 secondi
        if (now - lastReconnectAttempt > 5000)
        {
            lastReconnectAttempt = now;
            if (mqttConnect())
            {
                lastReconnectAttempt = 0;
            }
        }
    }
    else
    {
        mqttClient.loop();
    }
}

unsigned long getTimestamp()
{
    time_t now;
    time(&now);
    return now;
}

String getISOTimestamp()
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    char buffer[30];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
    return String(buffer);
}

bool mqttPublish(const char *topic, const char *payload, size_t length)
{
    if (!mqttClient.connected())
    {
        return false;
    }

    return mqttClient.publish(topic, payload, length);
}

bool mqttPublishWithTimestamp(const char *topic, const char *payload, size_t length)
{
    if (!mqttClient.connected())
    {
        return false;
    }

    // Crea JSON con timestamp e dati originali
    StaticJsonDocument<512> doc;
    doc["timestamp"] = getTimestamp();
    doc["iso_timestamp"] = getISOTimestamp();

    // Parse payload originale se è JSON
    StaticJsonDocument<256> originalDoc;
    DeserializationError error = deserializeJson(originalDoc, payload);

    if (!error)
    {
        // Copia tutti i campi dal JSON originale
        for (JsonPair kv : originalDoc.as<JsonObject>())
        {
            doc[kv.key()] = kv.value();
        }
    }
    else
    {
        // Se non è JSON, metti come stringa
        doc["data"] = payload;
    }

    String jsonString;
    serializeJson(doc, jsonString);

    return mqttClient.publish(topic, jsonString.c_str());
}