import adafruit_logging
import board
from busio import UART
from digitalio import DigitalInOut, Direction
from microcontroller import Pin
from time import monotonic, sleep
from sys import print_exception


class CustomHandler(adafruit_logging.LoggingHandler):
    """
    Prints out message both to the serial monitor and the specified UART.
    """

    def __init__(self, uart: UART):
        self.uart = uart

    def emit(self, level: int, msg: str):
        if level == adafruit_logging.DEBUG:
            code = "📝"
        elif level == adafruit_logging.INFO:
            code = "ℹ"
        elif level == adafruit_logging.WARNING:
            code = "⚠"
        elif level == adafruit_logging.ERROR:
            code = "‼"
        elif level == adafruit_logging.CRITICAL:
            code = "⛔"
        else:
            code = "❔"
        line = f"{monotonic():.3f} [{code}] {msg}\r\n"
        print(line, end="")
        self.uart.write(line.encode("utf-8"))


class Hcsr04:
    """
    HC-SR04 ultrasonic sensor workaround.
    """

    def __init__(self, *, trigger_pin: Pin, echo_pin: Pin, timeout=0.1):
        self.trigger = DigitalInOut(trigger_pin)
        self.trigger.direction = Direction.OUTPUT
        self.trigger.value = False
        self.echo = DigitalInOut(echo_pin)
        self.echo.direction = Direction.INPUT
        self.timeout = timeout

    def measure_once(self) -> float:
        """
        Measures the distance and returns it in metres.
        This property has ~17cm resolution.
        """
        self._wait_for(False)

        # Issue the trigger pulse.
        self.trigger.value = True
        sleep(0.000010)
        self.trigger.value = False

        self._wait_for(True)
        return self._wait_for(False) * 170.0

    def measure_n_times(self, n_measurements: int = 3) -> float:
        """
        Fires `n_measurements` requests and averages them.
        """
        return sum(self.measure_once() for _ in range(n_measurements)) / n_measurements

    def measure_for(self, seconds: float = 0.5, min_measurements: int = 10) -> float:
        sum_, n_measurements = 0.0, 0
        end_time = monotonic() + seconds
        while n_measurements < min_measurements or monotonic() < end_time:
            sum_ += self.measure_once()
            n_measurements += 1
        return sum_ / n_measurements

    def _wait_for(self, value: bool) -> float:
        start_time = monotonic()
        timeout_time = start_time + self.timeout
        while self.echo.value != value:
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
            self.logger.info(f"Hello from {self.sonar.measure_for():.2f}m.")
        except TimeoutError as e:
            self.logger.error(str(e))
        # sleep(0.5)


if __name__ == "__main__":
    Main().run()
