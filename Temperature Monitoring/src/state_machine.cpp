// File: state_machine.cpp
#include "state_machine.h"
#include "config.h"
#include <Arduino.h>

static State currentState;
static unsigned long toohotStart = 0;
const float TEMP_HOT = 30.0;
const float TEMP_TOOHOT = 40.0;

void stateInit()
{
    currentState = NORMAL;
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
}

void updateState(float temperature)
{
    bool buttonPressed = digitalRead(BUTTON_PIN) == LOW;
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
        if (buttonPressed)
            currentState = NORMAL;
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
        return "ALARM";
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