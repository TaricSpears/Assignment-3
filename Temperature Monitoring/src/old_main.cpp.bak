#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>

// ——— CONFIGURAZIONE WI-FI ———
const char *ssid = "TUO_SSID";
const char *password = "TUA_PASSWORD";

// ——— CONFIGURAZIONE MQTT ———
const char *mqtt_server = "broker.hivemq.com"; // o IP del tuo broker
const uint16_t mqtt_port = 1883;
const char *mqtt_user = nullptr;             // se usi auth
const char *mqtt_pass = nullptr;             // se usi auth
const char *pub_topic = "esp32/temperature"; // topic di pubblicazione

WiFiClient netClient;
PubSubClient mqttClient(netClient);

// ——— SENSORI & PIN ———
#define BUTTON_PIN 26
#define GREEN_LED 27
#define RED_LED 14
#define TEMP_SENSOR_PIN 32

OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature sensors(&oneWire);

enum State
{
    NORMAL,
    HOT,
    TOOHOT,
    ALARM
};
State currentState = NORMAL;
const float TEMP_HOT = 30.0;
const float TEMP_TOOHOT = 40.0;

// ——— FUNZIONI AUSILIARIE ———
float getTemperature()
{
    sensors.requestTemperatures();
    return sensors.getTempCByIndex(0);
}

bool isButtonPressed()
{
    return digitalRead(BUTTON_PIN) == LOW;
}

void mqttReconnect()
{
    while (!mqttClient.connected())
    {
        Serial.print("[MQTT] Connessione a broker ");
        Serial.print(mqtt_server);
        Serial.print(":");
        Serial.print(mqtt_port);
        Serial.print("… ");
        if (mqttClient.connect("ESP32Client", mqtt_user, mqtt_pass))
        {
            Serial.println("OK!");
        }
        else
        {
            Serial.print("FAILED, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" → riprovo tra 5s");
            delay(5000);
        }
    }
}

void updateState(float temperature)
{
    bool buttonState = isButtonPressed();
    switch (currentState)
    {
    case NORMAL:
        if (temperature >= TEMP_HOT)
        {
            currentState = HOT;
            Serial.println("Transizione: NORMAL -> HOT");
        }
        break;
    case HOT:
        if (temperature > TEMP_TOOHOT)
        {
            currentState = TOOHOT;
            Serial.println("Transizione: HOT -> TOOHOT");
        }
        else if (temperature < TEMP_HOT)
        {
            currentState = NORMAL;
            Serial.println("Transizione: HOT -> NORMAL");
        }
        break;
    case TOOHOT:
    {
        static unsigned long toohot_start_time = 0;
        if (temperature > TEMP_TOOHOT)
        {
            if (toohot_start_time == 0)
                toohot_start_time = millis();
            else if (millis() - toohot_start_time > 3000)
            {
                currentState = ALARM;
                toohot_start_time = 0;
                Serial.println("Transizione: TOOHOT -> ALARM");
            }
        }
        else
        {
            toohot_start_time = 0;
            currentState = HOT;
            Serial.println("Transizione: TOOHOT -> HOT");
        }
        break;
    }
    case ALARM:
        if (buttonState)
        {
            currentState = NORMAL;
            Serial.println("Transizione: ALARM -> NORMAL");
        }
        break;
    }
    Serial.print("Stato: ");
    switch (currentState)
    {
    case NORMAL:
        Serial.print("NORMAL");
        break;
    case HOT:
        Serial.print("HOT");
        break;
    case TOOHOT:
        Serial.print("TOOHOT");
        break;
    case ALARM:
        Serial.print("ALARM");
        break;
    }
    Serial.print(" | Temp: ");
    Serial.print(temperature);
    if (currentState == ALARM)
    {
        Serial.print(" | Pulsante: ");
        Serial.print(buttonState ? "PREMUTO" : "NON PREMUTO");
    }
    Serial.println();
}

void updateLEDs()
{
    if (currentState == TOOHOT || currentState == ALARM)
    {
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);
    }
    else
    {
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
    }
}

void publishStatus(float temperature)
{
    // Costruisce payload JSON
    StaticJsonDocument<200> doc;
    doc["temperature"] = temperature;
    const char *states[] = {"NORMAL", "HOT", "TOOHOT", "ALARM"};
    doc["state"] = states[currentState];
    char buffer[128];
    size_t n = serializeJson(doc, buffer);

    // Riconnessione se serve
    if (!mqttClient.connected())
        mqttReconnect();
    mqttClient.loop();

    // Pubblica
    bool ok = mqttClient.publish(pub_topic, buffer, n);
    Serial.printf("[MQTT] Pubblicato: %s -> %s\n", pub_topic, ok ? "OK" : "ERRORE");
}

void setup()
{
    Serial.begin(115200);
    sensors.begin();

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);

    // Connessione Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("[WiFi] Connetto a ");
    Serial.print(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" OK!");

    // Imposta broker MQTT
    mqttClient.setServer(mqtt_server, mqtt_port);

    Serial.println("Sistema avviato, pronto a pubblicare dati MQTT");
}

void loop()
{
    float temp = getTemperature();
    updateState(temp);
    updateLEDs();
    publishStatus(temp);
    delay(5000); // pubblica ogni 5s
}
