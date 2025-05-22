from threading import Thread
from app import create_app
from app.serial_arduino import serial_loop

app = create_app()

if __name__ == "__main__":
    Thread(target=serial_loop, daemon=True).start()

    app.run(debug=True)
