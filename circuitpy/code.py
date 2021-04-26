import adafruit_logging
import board
from busio import UART
from digitalio import DigitalInOut, Direction
from microcontroller import Pin, cpu
from time import monotonic, sleep
from sys import print_exception
from custom_logging import Handler as LoggingHandler
from event_loop import EventLoop, async_sleep


class Hcsr04:
    """HC-SR04 ultrasonic sensor workaround."""

    def __init__(self, *, trigger_pin: Pin, echo_pin: Pin, timeout=0.1):
        self._trigger_io = DigitalInOut(trigger_pin)
        self._trigger_io.direction = Direction.OUTPUT
        self._trigger_io.value = False
        self._echo_io = DigitalInOut(echo_pin)
        self._echo_io.direction = Direction.INPUT
        self._timeout = timeout

    def measure_once(self) -> float:
        """Measures the distance and returns it in metres."""

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
    def run(self):
        while True:
            try:
                self.loop()
            except Exception as e:
                logger.error(str(e))
                print_exception(e)

    def loop(self):
        try:
            distance, n_measurements = sonar.measure()
        except TimeoutError as e:
            logger.error(str(e))
        else:
            logger.info(f"{distance:.2f}m | {n_measurements} | {cpu.temperature}")


led = DigitalInOut(board.LED)
led.direction = Direction.OUTPUT

uart0 = UART(board.GP0, board.GP1, baudrate=9600)

sonar = Hcsr04(trigger_pin=board.GP12, echo_pin=board.GP13)

logger = adafruit_logging.getLogger("code")
logger.addHandler(LoggingHandler(uart0))
logger.setLevel(adafruit_logging.DEBUG)
logger.info("Initialized.")

if __name__ == "__main__":
    # Main().run()
    sleep(1.0)
    async def worker1():
        for _ in range(5):
            logger.info("%s", await answer())
            await async_sleep(0.5)
    async def worker2():
        for _ in range(5):
            logger.info("2")
            await async_sleep(1.0)
    async def answer():
        return 42
    EventLoop(worker1(), worker2()).run_until_complete()
    logger.info("Done.")
