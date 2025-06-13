import serial
import time
from app import system_state
from app.state import Mode

SERIAL_PORT = "/dev/ttyACM0"  # or COM3 on Windows
BAUD_RATE = 115200


def serial_loop():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        ser.reset_input_buffer()
        print("Serial connected to Arduino")
    except serial.SerialException as e:
        print(f"Serial connection failed: {e}")
        return
    time.sleep(2)  # Wait for Arduino to reset

    while True:
        while ser.in_waiting:
            try:
                line = ser.readline().decode('utf-8', errors='replace').strip()
                if line:
                    print(f"Received: x{line}x")
                    if line == "SETMODE:MANUAL":
                        print("Setting mode123 to MANUAL")
                        system_state.set_mode_arduino(Mode.MANUAL)
                    elif line == "SETMODE:AUTOMATIC":
                        print("Setting mode123 to AUTOMATIC")
                        system_state.set_mode_arduino(Mode.AUTOMATIC)
                    elif line.startswith("SETOPENING:"):
                        value = float(line.split(":")[1])
                        system_state.set_window_opening(value)
                    else:
                        print(f"Unknown command: {line}")

            except Exception as e:
                print(f"Serial read error: {e}")

        send_command_to_arduino(ser, f"SETOPENING:{system_state.get_window_opening()}")
        time.sleep(0.3)
        temps = system_state.get_measurements()
        send_command_to_arduino(ser, f"TEMPERATURE:{temps[-1].temperature if temps else 0}")
        time.sleep(0.3)


def send_command_to_arduino(ser: serial.Serial, command: str):
    try:
        print("sending message to arduino", f"{command}")
        ser.write(f"{command}\n\n".encode())
    except Exception as e:
        print(f"Serial write error: {e}")
