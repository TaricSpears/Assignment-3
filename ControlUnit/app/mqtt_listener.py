from time import time
import paho.mqtt.client as mqtt
import json

from app import system_state
from app.state import Mode

# === CONFIGURAZIONE MQTT ===
MQTT_SERVER = "test.mosquitto.org"  # IP locale del broker MQTT
MQTT_PORT = 1883
MQTT_USER = ""         # opzionale, se usi auth
MQTT_PASS = ""     # opzionale, se usi auth

# === TOPIC MQTT DA ASCOLTARE ===
MQTT_TOPICS = [("esp32/temperature", 0)]

# === CALLBACK CONNESSIONE ===
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connesso al broker MQTT")
        client.subscribe(MQTT_TOPICS)
        print(f"📡 Iscritto ai topic: {[t[0] for t in MQTT_TOPICS]}")
    else:
        print(f"Connessione fallita. Codice: {rc}")

# === CALLBACK MESSAGGIO RICEVUTO ===
def on_message(client, userdata, msg):
    print(f"\nMessaggio ricevuto su {msg.topic}: {msg.payload.decode()}")
    
    try:
        if msg.topic == "esp32/temperature":
            data = json.loads(msg.payload.decode())
            temp = float(data.get("temperature"))
            state_str = data.get("state")

            # Aggiungi la temperatura allo stato
            system_state.add_measurement(temp, int(time()))
            print(f"Temperatura registrata: {temp} °C")

            # Esegui azioni in base allo stato (opzionale)
            print(f"Stato remoto: {state_str}")

    except Exception as e:
        print(f"Errore nella gestione del messaggio: {e}")

# === FUNZIONE PRINCIPALE ===
def mqtt_loop():
    client = mqtt.Client()
    client.username_pw_set(MQTT_USER, MQTT_PASS)
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(MQTT_SERVER, MQTT_PORT, 60)
    client.loop_forever()

# === AVVIO SCRIPT ===
if __name__ == "__main__":
    mqtt_loop()
