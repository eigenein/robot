import adafruit_logging
import board
from busio import UART
from digitalio import DigitalInOut, Direction
from microcontroller import Pin, cpu
from time import monotonic, sleep
from sys import print_exception


class CustomHandler(adafruit_logging.LoggingHandler):
    """
    Prints out message both to the serial monitor and the specified UART.
    """

    LEVELS = {
        adafruit_logging.DEBUG: ("D", ""),
        adafruit_logging.INFO: ("I", "\033[92m"),
        adafruit_logging.WARNING: ("W", "\033[93m"),
        adafruit_logging.ERROR: ("E", "\033[91m"),
        adafruit_logging.CRITICAL: ("C", "\033[91m\033[1m"),
    }

    def __init__(self, uart: UART):
        self.uart = uart

    def emit(self, level: int, msg: str):
        name, color = self.LEVELS[level]
        line = f"{color}{monotonic():.3f} [{name}] {msg}\033[0m\r\n"
        print(line, end="")
        self.uart.write(line.encode("utf-8"))


class Hcsr04:
    """
    HC-SR04 ultrasonic sensor workaround.
    """

    def __init__(self, *, trigger_pin: Pin, echo_pin: Pin, timeout=0.1):
        self._trigger_io = DigitalInOut(trigger_pin)
        self._trigger_io.direction = Direction.OUTPUT
        self._trigger_io.value = False
        self._echo_io = DigitalInOut(echo_pin)
        self._echo_io.direction = Direction.INPUT
        self._timeout = timeout

    def measure_once(self) -> float:
        """
        Measures the distance and returns it in metres.
        """

        # Issue the trigger pulse.
        self._trigger_io.value = True
        sleep(0.000010)
        self._trigger_io.value = False

        self._wait_for(True)
        return self._wait_for(False) * 170.0

    def measure(self, min_measurements: int = 25, min_duration: float = 0.1) -> (float, int):
        n_measurements = 0
        sum_ = 0.0
        end_time = monotonic() + min_duration
        while monotonic() < end_time or n_measurements < min_measurements:
            sum_ += self.measure_once()
            n_measurements += 1
        return sum_ / n_measurements, n_measurements

    def _wait_for(self, value: bool) -> float:
        start_time = monotonic()
        timeout_time = start_time + self._timeout
        while self._echo_io.value != value:
            if monotonic() > timeout_time:
                raise TimeoutError(f"Timed out while waiting for the {value} value.")
        return monotonic() - start_time


class Main:
    def __init__(self):
        self.led = DigitalInOut(board.LED)
        self.led.direction = Direction.OUTPUT

        self.uart = UART(board.GP0, board.GP1, baudrate=9600)

        self.sonar = Hcsr04(trigger_pin=board.GP12, echo_pin=board.GP13)

        self.logger = adafruit_logging.getLogger("main")
        self.logger.addHandler(CustomHandler(self.uart))
        self.logger.setLevel(adafruit_logging.DEBUG)
        self.logger.info("Initialized.")

    def run(self):
        while True:
            try:
                self.loop()
            except Exception as e:
                self.logger.error(str(e))
                print_exception(e)

    def loop(self):
        try:
            distance, n_measurements = self.sonar.measure()
        except TimeoutError as e:
            self.logger.error(str(e))
        else:
            self.logger.info(f"{distance:.2f}m | {n_measurements} | {cpu.temperature}")


if __name__ == "__main__":
    Main().run()
