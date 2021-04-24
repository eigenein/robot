import board
from busio import UART
from digitalio import DigitalInOut, Direction

# noinspection PyUnresolvedReferences
LED = DigitalInOut(board.LED)
LED.direction = Direction.OUTPUT


def main():
    initialize()


def initialize():
    print("👍 Initialized.")

