import threading
import paho.mqtt.client as mqtt
import json

from app import system_state
from app.state import Mode, State

MQTT_SERVER = "test.mosquitto.org"
MQTT_PORT = 1883
MQTT_USER = ""
MQTT_PASS = ""
MQTT_TOPICS = [("esp32/temperature", 0)]

client = mqtt.Client()

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connesso al broker MQTT")
        client.subscribe(MQTT_TOPICS)
        print(f"📡 Iscritto ai topic: {[t[0] for t in MQTT_TOPICS]}")
    else:
        print(f"Connessione fallita. Codice: {rc}")

def on_message(client, userdata, msg):
    print(f"\nMessaggio ricevuto su {msg.topic}: {msg.payload.decode()}")
    try:
        if msg.topic == "esp32/temperature":
            data = json.loads(msg.payload.decode())
            timestamp = data["timestamp"]
            temp = float(data["temperature"])
            state_str = data["state"]
            system_state.add_measurement(temp, timestamp)
            print(f"Temperatura registrata: {temp} °C")
            print(f"Stato remoto: {state_str}")
            if system_state.get_state() == State.NORMAL:
                send_message("esp32/state", {"state": "5000"})
            else:
                send_message("esp32/state", {"state": "3000"})

    except Exception as e:
        print(f"Errore nella gestione del messaggio: {e}")

def mqtt_background_loop():
    client.username_pw_set(MQTT_USER, MQTT_PASS)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, MQTT_PORT, 60)
    client.loop_forever()

def send_message(topic, payload):
    result = client.publish(topic, json.dumps(payload))
    status = result[0]
    if status == 0:
        print(f"✅ Inviato a {topic}: {payload}")
    else:
        print(f"❌ Errore durante l'invio a {topic}")

def mqtt_loop():
    # Start MQTT loop in a background thread
    mqtt_thread = threading.Thread(target=mqtt_background_loop)
    mqtt_thread.daemon = True
    mqtt_thread.start()