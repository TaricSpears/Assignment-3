from enum import Enum
import threading


class State(Enum):
    NORMAL = 0
    HOT = 1
    TOOHOT = 2
    ALARM = 3


class Mode(Enum):
    MANUAL = 0
    AUTOMATIC = 1


N_temp_measurements = 10


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

    def add_measurement(self, temperature: float, timestamp: int):
        with self.lock:
            if len(self.measurements) >= N_temp_measurements:
                self.measurements.pop(0)
            self.measurements.append(TemperatureMeasurement(temperature, timestamp))

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
            return self.window_opening

    def get_measurements(self):
        with self.lock:
            return list(self.measurements)

    def get_state(self):
        with self.lock:
            return self.state
