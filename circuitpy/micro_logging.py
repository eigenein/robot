from sys import print_exception

from time import monotonic

_GREEN = "\033[92m"
_YELLOW = "\033[93m"
_RED = "\033[91m"
_BOLD = "\033[1m"
_END_COLOR = "\033[0m"
_END = f"{_END_COLOR}\r\n"

_handlers = []


def add_handler(file_):
    _handlers.append(file_)


def debug(message: str):
    _emit(f"{monotonic():.3f} [D] {message}\r\n")


def info(message: str):
    _emit(f"{_GREEN}{monotonic():.3f} [I] {message}{_END}")


def warning(message: str):
    _emit(f"{_YELLOW}{monotonic():.3f} [W] {message}{_END}")


def error(message: str, e: Exception = None):
    _emit(f"{_RED}{_BOLD}{monotonic():.3f} [E] {message}{_END}")
    if e is not None:
        print(f"{_RED}", end="")
        print_exception(e)
        print(_END_COLOR, end="")


def _emit(formatted_message: str):
    print(formatted_message, end="")
    encoded_message = formatted_message.encode("utf-8")
    for handler in _handlers:
        handler.write(encoded_message)
