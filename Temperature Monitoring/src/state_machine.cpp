// File: state_machine.cpp
#include "state_machine.h"
#include "config.h"
#include <Arduino.h>

static State currentState;
static unsigned long toohotStart = 0;
static bool externalAlarm = false; // Flag per allarme esterno
const float TEMP_HOT = 30.0;
const float TEMP_TOOHOT = 40.0;

void stateInit()
{
    currentState = NORMAL;
    externalAlarm = false;
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
}

void forceAlarmState(bool alarmOn)
{
    externalAlarm = alarmOn;
    if (alarmOn)
    {
        currentState = ALARM;
        Serial.println("Allarme esterno attivato!");
    }
    else
    {
        Serial.println("Allarme esterno disattivato");
        // Se non siamo in allarme per temperatura, torna a NORMAL
        if (currentState == ALARM)
        {
            currentState = NORMAL;
        }
    }
}

bool isExternalAlarmActive()
{
    return externalAlarm;
}

void updateState(float temperature)
{
    bool buttonPressed = digitalRead(BUTTON_PIN) == LOW;

    // Se c'è un allarme esterno attivo, rimani in ALARM
    if (externalAlarm)
    {
        currentState = ALARM;
        return;
    }

    switch (currentState)
    {
    case NORMAL:
        if (temperature >= TEMP_HOT)
            currentState = HOT;
        break;
    case HOT:
        if (temperature > TEMP_TOOHOT)
            currentState = TOOHOT;
        else if (temperature < TEMP_HOT)
            currentState = NORMAL;
        break;
    case TOOHOT:
        if (temperature > TEMP_TOOHOT)
        {
            if (toohotStart == 0)
                toohotStart = millis();
            else if (millis() - toohotStart > 3000)
                currentState = ALARM;
        }
        else
        {
            toohotStart = 0;
            currentState = HOT;
        }
        break;
    case ALARM:
        // Il pulsante può resettare sia allarme temperatura che esterno
        if (buttonPressed)
        {
            currentState = NORMAL;
            externalAlarm = false; // Reset anche allarme esterno
            toohotStart = 0;
        }
        break;
    }
}

const char *getStateString()
{
    switch (currentState)
    {
    case NORMAL:
        return "NORMAL";
    case HOT:
        return "HOT";
    case TOOHOT:
        return "TOOHOT";
    case ALARM:
        return externalAlarm ? "ALARM_EXTERNAL" : "ALARM_TEMP";
    }
    return "UNKNOWN";
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