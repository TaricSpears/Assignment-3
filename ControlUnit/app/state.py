from enum import Enum
import threading
import time


class State(Enum):
    NORMAL = 0
    HOT = 1
    TOOHOT = 2
    ALARM = 3


class Mode(Enum):
    MANUAL = 0
    AUTOMATIC = 1


N_temp_measurements = 10
T1 = 20
T2 = 32
DT = 3


class TemperatureMeasurement:
    def __init__(self, temperature: float, timestamp: int):
        self.temperature = temperature
        self.timestamp = timestamp


class SystemState:
    def __init__(self):
        print("initializing state")
        self.lock = threading.Lock()
        self.state = State.NORMAL
        self.measurements = []
        self.window_opening = 0.0
        self.mode = Mode.AUTOMATIC
        self.too_hot_start_time = None

    def add_measurement(self, temperature: float, timestamp: int):
        with self.lock:
            if len(self.measurements) >= N_temp_measurements:
                self.measurements.pop(0)
            self.measurements.append(TemperatureMeasurement(temperature, timestamp))

            if temperature >= T2:
                if self.state != State.TOOHOT:
                    self.state = State.TOOHOT
                    self.too_hot_start_time = timestamp
                    print("State changed to TOOHOT")
            elif temperature >= T1:
                self.state = State.HOT
            else:
                self.state = State.NORMAL

    def set_window_opening(self, value: float):
        with self.lock:
            if value < 0.0 or value > 100.0:
                raise ValueError("Window opening must be between 0 and 100")
            self.window_opening = value

    def set_mode(self, mode: Mode):
        with self.lock:
            print(f"Setting mode 111 to {mode.name}")
            self.mode = mode

    def set_state(self, state: State):
        with self.lock:
            self.state = state

    def get_mode(self):
        with self.lock:
            return self.mode

    def get_window_opening(self):
        with self.lock:
            temperature = self.measurements[-1].temperature if self.measurements else 0.0
            if self.mode == Mode.AUTOMATIC:  # and dashboard is in automatic mode
                if self.state == State.TOOHOT or self.state == State.ALARM:
                    self.window_opening = 100.0
                elif self.state == State.HOT:
                    self.window_opening = (temperature - T1) / (T2 - T1) * 100.0
                else:
                    self.window_opening = 0.0
            return self.window_opening

    def get_measurements(self):
        with self.lock:
            return list(self.measurements)

    def get_state(self):
        with self.lock:
            # Check if too hot for too long
            if self.state == State.TOOHOT and (time.time() - self.too_hot_start_time) > DT:
                self.state = State.ALARM
                print("State changed to ALARM due to prolonged high temperature")
            return self.state
