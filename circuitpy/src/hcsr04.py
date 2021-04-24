from digitalio import DigitalInOut, Direction
from microcontroller import Pin
from pulseio import PulseIn
from time import sleep


class HCSR04:
    def __init__(self, *, trigger_pin: Pin, echo_pin: Pin, timeout=0.1):
        self.trigger = DigitalInOut(trigger_pin)
        self.trigger.direction = Direction.OUTPUT
        self.echo = PulseIn(echo_pin, maxlen=1)

    @property
    def distance(self) -> float:
        """
        Measures the distance and returns it in metres.
        """
        self.trigger.value = True
        sleep(0.00001)
        self.trigger.value = False

        # TODO