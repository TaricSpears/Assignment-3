import serial
import time
from app import system_state
from state import State, Mode

SERIAL_PORT = "/dev/ttyUSB0"  # or COM3 on Windows
BAUD_RATE = 115200


def serial_loop():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print("Serial connected to Arduino")
    except serial.SerialException as e:
        print(f"Serial connection failed: {e}")
        return

    while True:
        try:
            while line := ser.readline().decode('utf-8').strip():
                print(f"Received: {line}")
                if line == "SETMODE:MANUAL":
                    system_state.set_mode(Mode.MANUAL)
                elif line == "SETMODE:AUTOMATIC":
                    system_state.set_mode(Mode.AUTOMATIC)
                elif line.startswith("SETOPENING:"):
                    value = float(line.split(":")[1])
                    system_state.set_window_opening(value)

            send_command_to_arduino(ser, f"SETOPENING:{system_state.get_window_opening()}")
            send_command_to_arduino(ser, f"SETMODE:{system_state.get_mode().name}")
            temps = system_state.get_measurements()
            send_command_to_arduino(ser, f"TEMPERATURE:{temps[-1].temperature if temps else 0}")
        except Exception as e:
            print(f"Serial read error: {e}")
        time.sleep(0.1)


def send_command_to_arduino(ser: serial.Serial, command: str):
    try:
        ser.write(f"{command}\n".encode())
    except Exception as e:
        print(f"Serial write error: {e}")
