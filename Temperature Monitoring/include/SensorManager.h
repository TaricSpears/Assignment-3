// SensorManager.h
#pragma once
#include <OneWire.h>
#include <DallasTemperature.h>

class SensorManager
{
public:
    void init(int pin);
    float readTemperature();

private:
    OneWire oneWire;
    DallasTemperature sensors;
};