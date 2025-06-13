// File: main.cpp
#include "config.h"
#include "mqtt_manager.h"
#include "sensor_manager.h"
#include <Arduino.h>
#include <ArduinoJson.h>

// Stato corrente dell'ESP32
enum ESP32State
{
  ESP32_NORMAL,
  ESP32_HOT,
  ESP32_ALARM
};

static ESP32State currentState = ESP32_NORMAL;
static unsigned long lastSensorRead = 0;
static unsigned long readInterval = 5000; // Default: 5 secondi
static bool networkError = false;
static unsigned long lastNetworkCheck = 0;

void mqttCallback(char *topic, uint8_t *payload, unsigned int length)
{
  String message = "";
  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  Serial.print("Comando ricevuto su ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);

  // Gestisci comandi di stato dalla control unit
  if (String(topic) == "esp32/state")
  {
    message.toLowerCase();

    if (message == "3000")
    {
      currentState = ESP32_NORMAL;
      readInterval = 3000;
      Serial.println("Stato cambiato a: NORMAL");
    }
    else if (message == "5000")
    {
      currentState = ESP32_NORMAL;
      readInterval = 5000;
      Serial.println("Stato cambiato a: NORMAL (5 secondi)");
    }
    else
    {
      Serial.println("Comando stato non riconosciuto: " + message);
    }

    // Reset errore di rete se riceviamo comandi
    networkError = false;
  }
}

void checkNetworkStatus()
{
  // Controlla connessione WiFi
  if (WiFi.status() != WL_CONNECTED)
  {
    networkError = true;
    Serial.println("Errore: WiFi disconnesso");
    return;
  }

  // Controlla connessione MQTT
  if (!mqttIsConnected())
  {
    networkError = true;
    Serial.println("Errore: MQTT disconnesso");
    return;
  }

  networkError = false;
}

void updateLEDs()
{
  if (networkError)
  {
    // LED rosso fisso per errori di rete
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
  }
  else
  {
    // LED verde quando tutto ok
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
  }
}

String getStateString()
{
  switch (currentState)
  {
  case ESP32_NORMAL:
    return "NORMAL";
  case ESP32_HOT:
    return "HOT";
  case ESP32_ALARM:
    return "ALARM";
  default:
    return "UNKNOWN";
  }
}

void sendTemperatureReading()
{
  float temp = readTemperatureSmoothed();

  // Crea JSON con dati sensore
  StaticJsonDocument<200> doc;
  doc["temperature"] = temp;
  doc["state"] = getStateString();
  doc["read_interval"] = readInterval;
  doc["network_error"] = networkError;

  char buffer[200];
  size_t len = serializeJson(doc, buffer);

  bool success = mqttPublishWithTimestamp("esp32/temperature", buffer, len);

  if (success)
  {
    Serial.print("Temperatura inviata: ");
    Serial.print(temp);
    Serial.print("°C (Stato: ");
    Serial.print(getStateString());
    Serial.println(")");
  }
  else
  {
    Serial.println("Errore invio temperatura");
    networkError = true;
  }
}

void sendStatusUpdate()
{
  // Invia stato generale dell'ESP32
  StaticJsonDocument<150> doc;
  doc["status"] = networkError ? "error" : "ok";
  doc["wifi_connected"] = (WiFi.status() == WL_CONNECTED);
  doc["mqtt_connected"] = mqttIsConnected();
  doc["current_state"] = getStateString();
  doc["uptime"] = millis();

  char buffer[150];
  serializeJson(doc, buffer);

  mqttPublish("esp32/status", buffer, strlen(buffer));
}

void setup()
{
  Serial.begin(115200);
  Serial.println("ESP32 Temperature Monitor - Avvio...");

  // Inizializza GPIO
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH); // Rosso durante setup

  // Connessione WiFi
  Serial.println("Connessione WiFi...");
  WiFi.begin(SSID, PASSWORD);

  int wifiAttempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifiAttempts < 20)
  {
    delay(500);
    Serial.print(".");
    wifiAttempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi connesso!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("\nErrore connessione WiFi!");
    networkError = true;
  }

  // Inizializza sensori e MQTT
  sensorInit();

  static WiFiClient wifiClient;
  mqttInit(wifiClient);
  mqttSetCallback(mqttCallback);
  mqttConnect();
  Serial.println("Setup completato");

  // Prima lettura
  lastSensorRead = millis();
  sendTemperatureReading();
}

void loop()
{
  Serial.println("Loop principale...");
  unsigned long currentTime = millis();

  // Gestisci MQTT
  mqttLoop();

  // Controlla stato rete ogni 30 secondi
  if (currentTime - lastNetworkCheck > 30000)
  {
    checkNetworkStatus();
    sendStatusUpdate();
    lastNetworkCheck = currentTime;
  }

  // Aggiorna LED
  updateLEDs();

  // Leggi temperatura in base all'intervallo corrente
  if (currentTime - lastSensorRead >= readInterval)
  {
    sendTemperatureReading();
    lastSensorRead = currentTime;
  }

  delay(100); // Piccola pausa per non sovraccaricare
}