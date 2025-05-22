#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>
#define BUTTON_PIN 26     // Pin del pulsante
#define GREEN_LED 27      // Tornato al pin originale per il LED verde
#define RED_LED 14        // Tornato al pin originale per il LED rosso

// Define the 1-Wire bus pin
#define TEMP_SENSOR_PIN 32

// Create a DallasTemperature object
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature sensors(&oneWire);

enum State {
  NORMAL,
  HOT,
  TOOHOT,
  ALARM
};

State currentState = NORMAL;
const float TEMP_HOT = 30.0;
const float TEMP_TOOHOT = 40.0;

// Legge la temperatura dal sensore NTC
float getTemperature() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0); 
  return tempC;
}

// Simula input da pulsante
bool isButtonPressed() {
  // Leggi il pulsante (attivo basso con INPUT_PULLUP)
  // Restituisce true quando il pulsante è premuto
  return digitalRead(BUTTON_PIN) == LOW;
}

void updateState(float temperature) {
  Serial.print("Stato attuale: ");
  switch (currentState) {
    case NORMAL: Serial.print("NORMAL"); break;
    case HOT: Serial.print("HOT"); break;
    case TOOHOT: Serial.print("TOOHOT"); break;
    case ALARM: Serial.print("ALARM"); break;
  }
  Serial.print(", Temperatura: ");
  Serial.print(temperature);
  
  // Controllo pulsante nello stato ALARM
  bool buttonState = isButtonPressed();
  if (currentState == ALARM) {
    Serial.print(", Pulsante: ");
    Serial.print(buttonState ? "PREMUTO" : "NON PREMUTO");
  }
  Serial.println();
  
  switch (currentState) {
    case NORMAL:
      if (temperature >= TEMP_HOT) {
        currentState = HOT;
        Serial.println("Transizione: NORMAL -> HOT");
      }
      break;
      
    case HOT:
      if (temperature > TEMP_TOOHOT) {
        currentState = TOOHOT;
        Serial.println("Transizione: HOT -> TOOHOT");
      } else if (temperature < TEMP_HOT) {
        currentState = NORMAL;
        Serial.println("Transizione: HOT -> NORMAL");
      }
      break;
      
    case TOOHOT:
      if (temperature <= TEMP_TOOHOT) {
        currentState = HOT;
        Serial.println("Transizione: TOOHOT -> HOT");
      } else if (temperature > TEMP_TOOHOT) {
        // Attendi un po' prima di passare ad ALARM per evitare transizioni rapide
        static unsigned long toohot_start_time = 0;
        if (toohot_start_time == 0) {
          toohot_start_time = millis();
        } else if (millis() - toohot_start_time > 3000) { // 3 secondi in TOOHOT
          currentState = ALARM;
          toohot_start_time = 0;
          Serial.println("Transizione: TOOHOT -> ALARM (dopo 3 secondi)");
        }
      }
      break;
      
    case ALARM:
      // Esci da ALARM SOLO se il pulsante viene premuto
      if (buttonState) {
        currentState = NORMAL;
        Serial.println("Transizione: ALARM -> NORMAL (pulsante premuto)");
      }
      break;
  }
}

void updateLEDs() {
  if (currentState == TOOHOT || currentState == ALARM) {
    // LED rosso acceso, verde spento per stati TOOHOT e ALARM
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    Serial.println("LED STATO: ROSSO ACCESO");
  } else {
    // LED verde acceso, rosso spento per stati NORMAL e HOT
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    Serial.println("LED STATO: VERDE ACCESO");
  }
}

void setup() {
  Serial.begin(115200);
  sensors.begin();
  
  // Configurazione dei pin
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  
  // Inizializzazione dei LED
  digitalWrite(GREEN_LED, HIGH);  // LED verde acceso all'avvio (stato NORMAL)
  digitalWrite(RED_LED, LOW);     // LED rosso spento all'avvio
  
  Serial.println("Sistema di monitoraggio temperatura avviato");
  Serial.println("LED verde e rosso inizializzati");
}

void loop() {
  // Leggi temperatura
  float temp = getTemperature();
  
  // Aggiorna stato e LED
  updateState(temp);
  updateLEDs();
  
  // Breve output di debug compatto
  Serial.print("Temp: "); 
  Serial.print(temp);
  Serial.print("°C | Stato: ");
  
  switch (currentState) {
    case NORMAL: Serial.println("NORMAL"); break;
    case HOT: Serial.println("HOT"); break;
    case TOOHOT: Serial.println("TOOHOT"); break;
    case ALARM: 
      Serial.print("ALARM");
      Serial.println(" (premi pulsante per resettare)");
      break;
  }
  
  delay(500); // Polling più veloce per una migliore risposta del pulsante
}