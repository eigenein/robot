from adafruit_logging import DEBUG, INFO, WARNING, ERROR, CRITICAL, LoggingHandler
from busio import UART
from time import monotonic


class Handler(LoggingHandler):
    """
    Prints out message both to the serial monitor and the specified UART.
    """

    LEVELS = {
        DEBUG: ("D", ""),
        INFO: ("I", "\033[92m"),
        WARNING: ("W", "\033[93m"),
        ERROR: ("E", "\033[91m"),
        CRITICAL: ("C", "\033[91m\033[1m"),
    }

    def __init__(self, uart: UART):
        self.uart = uart

    def emit(self, level: int, msg: str):
        name, color = self.LEVELS[level]
        line = f"{color}{monotonic():.3f} [{name}] {msg}\033[0m\r\n"
        print(line, end="")
        self.uart.write(line.encode("utf-8"))  # type: ignore
