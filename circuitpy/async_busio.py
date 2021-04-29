from micro_asyncio import sleep
from collections import namedtuple


class AsyncBus(namedtuple("AsyncBus", ("bus",))):
    """Asynchronous `busio` wrapper."""

    async def __aenter__(self):
        """Lock the bus asynchronously."""
        while not self.bus.try_lock():
            await sleep(0.0)
        return self.bus

    async def __aexit__(self, _exc_type, _exc_val, _exc_tb):
        """Unlock the bus."""
        self.bus.unlock()
