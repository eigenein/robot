from event_loop import async_sleep


class AsyncIo:
    """Asynchronous `busio` wrapper."""

    __slots__ = ("_io",)

    def __init__(self, io):
        self._io = io

    async def __aenter__(self):
        """Lock the bus asynchronously."""
        while not self._io.try_lock():
            await async_sleep(0.0)
        return self._io

    async def __aexit__(self, _exc_type, _exc_val, _exc_tb):
        """Unlock the bus."""
        self._io.unlock()

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}({repr(self._io)})"
