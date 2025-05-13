// temperature-monitoring-subsystem/src/TemperatureFSM.cpp
#include <Arduino.h>
#include "MQTTManager.h"

// Define FSM States
enum class State
{
    INIT,
    CONNECTING,
    SAMPLING,
    PUBLISHING,
    ERROR
};

State currentState = State::INIT;
float currentTemperature = 0.0;
unsigned long lastSampleTime = 0;
unsigned long samplingInterval = 1000; // Default F1 = 1s (set via MQTT)

// MQTT Manager (assume you have a class for this)
MQTTManager mqttClient;

void transitionTo(State newState)
{
    currentState = newState;
}

void handleInit()
{
    // Initialize sensors, WiFi, MQTT
    if (sensorsReady() && mqttClient.connect())
    {
        transitionTo(State::CONNECTING);
    }
    else
    {
        transitionTo(State::ERROR);
    }
}

void handleConnecting()
{
    if (WiFi.status() == WL_CONNECTED && mqttClient.isConnected())
    {
        digitalWrite(GREEN_LED, HIGH);
        digitalWrite(RED_LED, LOW);
        transitionTo(State::SAMPLING);
    }
    else
    {
        transitionTo(State::ERROR);
    }
}

void handleSampling()
{
    if (millis() - lastSampleTime >= samplingInterval)
    {
        currentTemperature = readTemperatureSensor();
        transitionTo(State::PUBLISHING);
    }
}

void handlePublishing()
{
    if (mqttClient.publish("temperature/update", String(currentTemperature)))
    {
        lastSampleTime = millis();
        transitionTo(State::SAMPLING);
    }
    else
    {
        transitionTo(State::ERROR);
    }
}

void handleError()
{
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    // Attempt reconnection after delay
    delay(5000);
    transitionTo(State::INIT);
}

void loop()
{
    switch (currentState)
    {
    case State::INIT:
        handleInit();
        break;
    case State::CONNECTING:
        handleConnecting();
        break;
    case State::SAMPLING:
        handleSampling();
        break;
    case State::PUBLISHING:
        handlePublishing();
        break;
    case State::ERROR:
        handleError();
        break;
    }
}