from ulab.numpy import ndarray

from micro_asyncio import event_loop, sleep
from micro_logging import error, info
from peripherals import led, orientation_sensor
from repl import run as run_repl


class Main:
    def __init__(self):
        self._euler = ndarray((0.0, 0.0, 0.0))

    async def run(self):
        await orientation_sensor.begin()
        event_loop.schedule(
            self._run_telemetry(),
            self._run_orientation_sensor(),
            run_repl(),
        )

    async def _run_telemetry(self):
        """Continuously print telemetry to the console."""
        while True:
            info(" | ".join((
                f"Orientation: {self._euler[0]}",
            )))
            led.value = not led.value
            await sleep(0.5)

    async def _run_orientation_sensor(self):
        """Continuously read the orientation sensor."""
        while True:
            await sleep(0.01)  # fusion output rate is 100Hz
            try:
                self._euler = await orientation_sensor.get_euler()
            except OSError as e:
                error(f"orientation sensor error: {e}")


if __name__ == "__main__":
    info("Starting…")
    event_loop.schedule(Main().run()).run_until_complete()
