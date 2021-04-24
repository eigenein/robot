import board
from busio import UART
from digitalio import DigitalInOut, Direction
from time import sleep

import adafruit_logging

from src.logging import CustomHandler
from src.hcsr04 import HCSR04

led = DigitalInOut(board.LED)
led.direction = Direction.OUTPUT

uart = UART(board.GP0, board.GP1, baudrate=9600)

sonar = HCSR04(trigger_pin=board.GP12, echo_pin=board.GP13, timeout=0.5)

logger = adafruit_logging.getLogger("main")
logger.addHandler(CustomHandler(uart))
logger.setLevel(adafruit_logging.DEBUG)


def main():
    logger.info("Initialized.")
    while True:
        try:
            logger.info(f"Hello from {sonar.distance:.1f}cm.")
        except RuntimeError as e:
            logger.error(str(e))
        sleep(0.5)
