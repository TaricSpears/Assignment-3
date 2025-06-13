// File: main.cpp
#include "config.h"
#include "mqtt_manager.h"
#include "sensor_manager.h"
#include "state_machine.h"
#include <Arduino.h>
#include <ArduinoJson.h>

// Callback per messaggi MQTT in arrivo
void mqttCallback(char *topic, uint8_t *payload, unsigned int length)
{
  // Converti payload in stringa
  String message = "";
  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  Serial.print("Messaggio ricevuto su topic: ");
  Serial.print(topic);
  Serial.print(" -> ");
  Serial.println(message);

  // Gestisci il messaggio di allarme
  if (String(topic) == SUB_TOPIC)
  {
    message.toLowerCase(); // Converti in minuscolo per comparazione

    if (message == "alarm_on" || message == "1" || message == "true")
    {
      forceAlarmState(true);
    }
    else if (message == "alarm_off" || message == "0" || message == "false")
    {
      forceAlarmState(false);
    }
    else
    {
      // Prova a parsare come JSON
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, message);

      if (!error && doc.containsKey("alarm"))
      {
        bool alarmStatus = doc["alarm"];
        forceAlarmState(alarmStatus);
      }
      else
      {
        Serial.println("Formato messaggio allarme non riconosciuto");
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);

  Serial.println("Connessione WiFi...");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connesso!");

  sensorInit();
  stateInit();

  static WiFiClient wifiClient;
  mqttInit(wifiClient);

  // Imposta il callback per i messaggi MQTT
  mqttSetCallback(mqttCallback);

  // Aspetta un po' prima di fare la prima connessione MQTT
  delay(1000);

  Serial.println("Setup completato");
}

void loop()
{
  // Gestisci i messaggi MQTT in arrivo
  mqttLoop();

  float temp = readTemperature();
  updateState(temp);
  updateLEDs();

  // Prepara JSON con informazioni aggiuntive
  StaticJsonDocument<250> doc;
  doc["temperature"] = temp;
  doc["state"] = getStateString();
  doc["external_alarm"] = isExternalAlarmActive();

  char buf[200];
  size_t n = serializeJson(doc, buf);

  mqttPublishWithTimestamp(PUB_TOPIC, buf, n);

  delay(5000);
}