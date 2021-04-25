"""
HC-SR04 ultrasonic distance sensor.

The Adafruit driver doesn't work for me on Pi Pico, thus wrote my own.
"""

from digitalio import DigitalInOut, Direction
from microcontroller import Pin
from pulseio import PulseIn
from time import monotonic, sleep


class Hcsr04:
    def __init__(self, *, trigger_pin: Pin, echo_pin: Pin, timeout=1.0):
        self.trigger = DigitalInOut(trigger_pin)
        self.trigger.direction = Direction.OUTPUT
        self.trigger.value = False
        self.echo_pin = echo_pin
        self.echo = PulseIn(echo_pin, maxlen=1)
        self.timeout = timeout

    @property
    def distance(self) -> float:
        """
        Measures the distance and returns it in metres.
        """
        self.echo.clear()

        # Issue the trigger pulse.
        self.trigger.value = True
        sleep(0.000010)
        self.trigger.value = False

        # Wait for the echo pulse.
        timeout_time = monotonic() + self.timeout
        while not self.echo:
            if monotonic() > timeout_time:
                raise TimeoutError(f"Timed out while waiting for an echo.")

        # Finally, convert the pulse duration to the distance.
        return self.echo[0] * 0.00017
