import board
from busio import UART
from digitalio import DigitalInOut, Direction

led = DigitalInOut(board.LED)
led.direction = Direction.OUTPUT

uart0 = UART(board.GP0, board.GP1, baudrate=9600)
