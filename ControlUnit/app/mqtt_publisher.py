import paho.mqtt.client as mqtt
import json
import threading
import time
from datetime import datetime

# === CONFIGURAZIONE MQTT ===
MQTT_SERVER = "test.mosquitto.org"
MQTT_PORT = 1883
MQTT_USER = ""
MQTT_PASS = ""

# === TOPIC MQTT PER COMANDI ===
ALARM_TOPIC = "esp32/alarm"

class MQTTPublisher:
    def __init__(self):
        self.client = mqtt.Client()
        self.client.username_pw_set(MQTT_USER, MQTT_PASS)
        self.client.on_connect = self._on_connect
        self.client.on_publish = self._on_publish
        self.connected = False
        self.connect()
        
    def _on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            print("Publisher MQTT connesso al broker")
            self.connected = True
        else:
            print(f"Connessione publisher fallita. Codice: {rc}")
            self.connected = False
    
    def _on_publish(self, client, userdata, mid):
        print(f"Messaggio pubblicato con ID: {mid}")
    
    def connect(self):
        """Connette al broker MQTT"""
        try:
            self.client.connect(MQTT_SERVER, MQTT_PORT, 60)
            self.client.loop_start()  # Avvia loop in background
        except Exception as e:
            print(f"Errore connessione MQTT publisher: {e}")
    
    def disconnect(self):
        """Disconnette dal broker MQTT"""
        self.client.loop_stop()
        self.client.disconnect()
    
    def send_alarm_command(self, alarm_on: bool):
        """
        Invia comando di allarme all'ESP32
        
        Args:
            alarm_on (bool): True per attivare allarme, False per disattivare
        """
        if not self.connected:
            print("Publisher MQTT non connesso")
            return False
        
        try:
            # Formato semplice
            command = "alarm_on" if alarm_on else "alarm_off"
            
            # Oppure formato JSON (commenta la riga sopra e decommenta sotto)
            # command_data = {
            #     "alarm": alarm_on,
            #     "timestamp": int(time.time()),
            #     "source": "flask_app"
            # }
            # command = json.dumps(command_data)
            
            result = self.client.publish(ALARM_TOPIC, command)
            
            if result.rc == mqtt.MQTT_ERR_SUCCESS:
                status = "ATTIVATO" if alarm_on else "DISATTIVATO"
                print(f"Allarme {status} - Comando inviato: {command}")
                return True
            else:
                print(f"Errore nell'invio comando allarme: {result.rc}")
                return False
                
        except Exception as e:
            print(f"Errore nell'invio comando allarme: {e}")
            return False
    
    def send_custom_command(self, topic: str, payload: str):
        """
        Invia un comando personalizzato
        
        Args:
            topic (str): Topic MQTT
            payload (str): Payload da inviare
        """
        if not self.connected:
            print("Publisher MQTT non connesso")
            return False
        
        try:
            result = self.client.publish(topic, payload)
            if result.rc == mqtt.MQTT_ERR_SUCCESS:
                print(f"Comando inviato su {topic}: {payload}")
                return True
            else:
                print(f"Errore nell'invio comando: {result.rc}")
                return False
        except Exception as e:
            print(f"Errore nell'invio comando: {e}")
            return False

# Istanza globale del publisher
_publisher = None

def get_publisher():
    """Ottiene l'istanza globale del publisher MQTT"""
    global _publisher
    if _publisher is None:
        _publisher = MQTTPublisher()
    return _publisher

def send_alarm(alarm_on: bool):
    """Funzione di convenienza per inviare comando allarme"""
    publisher = get_publisher()
    return publisher.send_alarm_command(alarm_on)

def cleanup():
    """Pulisce le risorse del publisher"""
    global _publisher
    if _publisher:
        _publisher.disconnect()
        _publisher = None