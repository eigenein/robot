import board
from busio import UART
from digitalio import DigitalInOut, Direction

from hcsr04 import Hcsr04

led = DigitalInOut(board.LED)
led.direction = Direction.OUTPUT

uart0 = UART(board.GP0, board.GP1, baudrate=9600)

sonar = Hcsr04(trigger_pin=board.GP12, echo_pin=board.GP13)
