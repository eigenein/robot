from micro_asyncio import event_loop, sleep
from micro_logging import add_handler as add_logging_handler, info
from peripherals import *


class Main:
    async def run(self):
        await bno055.begin()
        event_loop.schedule(self._run_telemetry())

    async def _run_telemetry(self):
        while True:
            euler = await bno055.get_euler()
            info(" | ".join((
                f"{euler}",
            )))
            led.value = not led.value
            await sleep(0.5)


add_logging_handler(uart0)

if __name__ == "__main__":
    info("Starting…")
    event_loop.schedule(Main().run()).run_until_complete()
