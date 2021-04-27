from custom_logging import add_handler as add_logging_handler, info, warning
from event_loop import async_sleep, event_loop
from peripherals import *


class Main:
    async def run(self):
        event_loop.schedule(self._run_telemetry())

    async def _run_telemetry(self):
        while True:
            (info if not event_loop.n_errors else warning)(" | ".join((
                "OK",
            )))
            await async_sleep(0.5)


add_logging_handler(uart0)
info("Initialized.")

if __name__ == "__main__":
    event_loop.schedule(Main().run()).run_until_complete()
