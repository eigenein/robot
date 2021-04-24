from busio import UART
from time import monotonic

# noinspection PyUnresolvedReferences
import adafruit_logging


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
