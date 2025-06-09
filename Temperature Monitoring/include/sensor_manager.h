// File: sensor_manager.h
#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

// Inizializza il sensore
void sensorInit();

// Leggi la temperatura (singola lettura)
float readTemperature();

// Leggi la temperatura con smoothing (lettura mediata)
float readTemperatureSmoothed();

#endif