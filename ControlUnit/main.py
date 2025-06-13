from threading import Thread
from app import create_app
from app.serial_arduino import serial_loop
from app.mqtt_listener import mqtt_loop
from app import system_state
import time

app = create_app()

if __name__ == "__main__":
    Thread(target=serial_loop, daemon=True).start()

    Thread(target=mqtt_loop, daemon=True).start()

    app.run(debug=True, use_reloader=False)
