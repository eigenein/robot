import board
from busio import I2C, UART
from digitalio import DigitalInOut, Direction

from async_busio import AsyncBus
from bno055 import Bno055

led = DigitalInOut(board.LED)
led.direction = Direction.OUTPUT

uart0 = UART(board.GP0, board.GP1, baudrate=9600)
i2c1 = AsyncBus(I2C(board.GP15, board.GP14))

bno055 = Bno055(i2c1)
