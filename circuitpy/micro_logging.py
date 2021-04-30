from sys import print_exception

from time import monotonic

_GREEN = "\033[92m"
_YELLOW = "\033[93m"
_RED = "\033[91m"
_BOLD = "\033[1m"
_END_COLOR = "\033[0m"
_END = f"{_END_COLOR}"


def debug(message: str):
    print(f"{monotonic():.3f} [D] {message}{_END}")


def info(message: str):
    print(f"{_GREEN}{monotonic():.3f} [I] {message}{_END}")


def warning(message: str):
    print(f"{_YELLOW}{monotonic():.3f} [W] {message}{_END}")


def error(message: str, e: Exception = None):
    print(f"{_RED}{_BOLD}{monotonic():.3f} [E] {message}{_END}")
    if e is not None:
        print(f"{_RED}", end="")
        print_exception(e)
        print(_END_COLOR, end="")
