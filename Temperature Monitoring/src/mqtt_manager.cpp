// File: mqtt_manager.cpp
#include "config.h"
#include "mqtt_manager.h"
#include <ArduinoJson.h>
#include <time.h>

static WiFiClient *netClient = nullptr;
static PubSubClient mqttClient(*netClient);

void mqttInit(WiFiClient &wifiClient)
{
    netClient = &wifiClient;
    mqttClient.setClient(wifiClient);
    mqttClient.setServer(MQTT_SERVER, 1883);

    // Configura NTP per ottenere il timestamp
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
}

void mqttSetCallback(void (*callback)(char *, uint8_t *, unsigned int))
{
    mqttClient.setCallback(callback);
}

void mqttSubscribe(const char *topic)
{
    if (mqttClient.connected())
    {
        mqttClient.subscribe(topic);
        Serial.print("Sottoscritto al topic: ");
        Serial.println(topic);
    }
}

static void mqttReconnect()
{
    while (!mqttClient.connected())
    {
        Serial.print("Tentativo connessione MQTT...");
        if (mqttClient.connect("ESP32Client", MQTT_USER, MQTT_PASS))
        {
            Serial.println(" connesso!");
            // Risubscribe ai topic dopo la riconnessione
            mqttSubscribe(SUB_TOPIC);
        }
        else
        {
            Serial.print(" fallito, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" riprovo tra 5 secondi");
            delay(5000);
        }
    }
}

// Funzione per ottenere timestamp in formato epoch
unsigned long getTimestamp()
{
    time_t now;
    time(&now);
    return now;
}

// Funzione per ottenere timestamp in formato ISO 8601
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

// Versione originale senza timestamp
bool mqttPublish(const char *topic, const char *payload, size_t length)
{
    if (!mqttClient.connected())
        mqttReconnect();
    mqttClient.loop();
    return mqttClient.publish(topic, payload, length);
}

// Nuova versione con timestamp automatico (formato JSON)
bool mqttPublishWithTimestamp(const char *topic, const char *payload, size_t length)
{
    if (!mqttClient.connected())
        mqttReconnect();
    mqttClient.loop();

    // Crea un JSON con timestamp e payload originale
    StaticJsonDocument<512> doc;
    doc["timestamp"] = getTimestamp();
    doc["iso_timestamp"] = getISOTimestamp();

    // Parse del payload originale se è JSON
    StaticJsonDocument<256> originalDoc;
    DeserializationError error = deserializeJson(originalDoc, payload);

    if (!error)
    {
        // Se è un JSON valido, copia i campi
        for (JsonPair kv : originalDoc.as<JsonObject>())
        {
            doc[kv.key()] = kv.value();
        }
    }
    else
    {
        // Altrimenti mettilo come stringa
        doc["data"] = payload;
    }

    String jsonString;
    serializeJson(doc, jsonString);

    return mqttClient.publish(topic, jsonString.c_str());
}

// Versione alternativa: aggiunge timestamp come prefisso
bool mqttPublishWithTimestampPrefix(const char *topic, const char *payload, size_t length)
{
    if (!mqttClient.connected())
        mqttReconnect();
    mqttClient.loop();

    String timestampedPayload = String(getTimestamp()) + "|" + String(payload);
    return mqttClient.publish(topic, timestampedPayload.c_str());
}

// Versione per payload JSON esistente - aggiunge timestamp al JSON
bool mqttPublishJsonWithTimestamp(const char *topic, const char *jsonPayload, size_t length)
{
    if (!mqttClient.connected())
        mqttReconnect();
    mqttClient.loop();

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, jsonPayload);

    if (error)
    {
        return mqttPublishWithTimestampPrefix(topic, jsonPayload, length);
    }

    doc["timestamp"] = getTimestamp();
    doc["iso_timestamp"] = getISOTimestamp();

    String jsonString;
    serializeJson(doc, jsonString);

    return mqttClient.publish(topic, jsonString.c_str());
}

void mqttLoop()
{
    if (!mqttClient.connected())
        mqttReconnect();
    mqttClient.loop();
}