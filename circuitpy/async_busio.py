from micro_asyncio import sleep


class AsyncBus:
    """Asynchronous `busio` wrapper."""

    __slots__ = ("_bus",)

    def __init__(self, bus):
        self._bus = bus

    async def __aenter__(self):
        """Lock the bus asynchronously."""
        while not self._bus.try_lock():
            await sleep(0.001)
        return self._bus

    async def __aexit__(self, _exc_type, _exc_val, _exc_tb):
        """Unlock the bus."""
        self._bus.unlock()

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}({repr(self._bus)})"
