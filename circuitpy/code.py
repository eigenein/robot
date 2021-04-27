from custom_logging import add_handler as add_logging_handler, info, warning
from event_loop import async_sleep, event_loop
from peripherals import *


class Main:
    def __init__(self):
        self._sonar_distance = float("+inf")

    async def run(self):
        event_loop.schedule(self._run_telemetry(), self._run_sonar())

    async def _run_telemetry(self):
        while True:
            (info if not event_loop.n_errors else warning)(" | ".join((
                f"sonar: {self._sonar_distance:.2f} ({sonar.n_timeouts} errors)",
            )))
            await async_sleep(0.5)

    async def _run_sonar(self):
        while True:
            try:
                self._sonar_distance = await sonar.measure()
            except TimeoutError:
                pass


add_logging_handler(uart0)
info("Initialized.")

if __name__ == "__main__":
    event_loop.schedule(Main().run()).run_until_complete()
