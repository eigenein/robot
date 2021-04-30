from ulab.numpy import ndarray

from micro_asyncio import event_loop, sleep
from micro_logging import add_handler as add_logging_handler, error, info
from peripherals import led, orientation_sensor, uart0


class Main:
    def __init__(self):
        self._euler = ndarray((0.0, 0.0, 0.0))

    async def run(self):
        await orientation_sensor.begin()
        event_loop.schedule(self._run_telemetry(), self._run_orientation_sensor())

    async def _run_telemetry(self):
        while True:
            info(" | ".join((
                f"Orient: {self._euler[0]}",
            )))  # FIXME: this line takes too long.
            led.value = not led.value
            await sleep(0.5)

    async def _run_orientation_sensor(self):
        while True:
            await sleep(0.01)  # fusion output rate is 100Hz
            try:
                self._euler = await orientation_sensor.get_euler()
            except OSError as e:
                error(f"BNO055: {e}")


add_logging_handler(uart0)

if __name__ == "__main__":
    info("Starting…")
    event_loop.schedule(Main().run()).run_until_complete()
