// File: sensor_manager.cpp
#include "sensor_manager.h"
#include "config.h"

void sensorInit()
{
    // Il TMP36 non richiede inizializzazione specifica
    // Configurazione del pin analogico (opzionale su ESP32)
    analogReadResolution(12);       // ESP32 supporta fino a 12 bit di risoluzione
    analogSetAttenuation(ADC_11db); // Per leggere fino a 3.3V
}

float readTemperature()
{
    // Leggi il valore analogico dal TMP36
    int adcValue = analogRead(TEMP_SENSOR_PIN);

    // Converti il valore ADC in voltaggio
    // ESP32: ADC a 12 bit (0-4095) con Vref = 3.3V
    float voltage = (adcValue * 3.3) / 4095.0;

    // Converti il voltaggio in temperatura (°C)
    // TMP36: 0.5V = 0°C, 10mV/°C
    float temperatureC = (voltage - 0.5) * 100.0;

    return temperatureC;
}

float readTemperatureSmoothed()
{
    // Leggi più volte e fai la media per ridurre il rumore
    const int numReadings = 10;
    float sum = 0;

    for (int i = 0; i < numReadings; i++)
    {
        int adcValue = analogRead(TEMP_SENSOR_PIN);
        float voltage = (adcValue * 3.3) / 4095.0;
        float temp = (voltage - 0.5) * 100.0;
        sum += temp;
        delay(10); // Piccola pausa tra le letture
    }

    return sum / numReadings;
}