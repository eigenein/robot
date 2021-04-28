from custom_logging import add_handler as add_logging_handler, info
from event_loop import async_sleep, event_loop
from peripherals import *


class Main:
    async def run(self):
        await async_sleep(1.0)  # FIXME: allow the peripherals to boot up
        if not await bno055.ping():
            return
        event_loop.schedule(self._run_telemetry())

    async def _run_telemetry(self):
        while True:
            info(" | ".join((
                "OK",
            )))
            await async_sleep(0.5)


add_logging_handler(uart0)

if __name__ == "__main__":
    info("Starting…")
    event_loop.schedule(Main().run()).run_until_complete()
