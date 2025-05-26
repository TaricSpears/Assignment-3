// File: sensor_manager.cpp
#include "include/sensor_manager.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "include/config.h"

static OneWire oneWire(TEMP_SENSOR_PIN);
static DallasTemperature sensors(&oneWire);

void sensorInit()
{
    sensors.begin();
}

float readTemperature()
{
    sensors.requestTemperatures();
    return sensors.getTempCByIndex(0);
}