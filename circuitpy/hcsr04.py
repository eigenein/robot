"""Asynchronous HC-SR04 driver."""

from digitalio import DigitalInOut, Direction
from microcontroller import Pin
from time import monotonic

from event_loop import async_back_off, async_sleep

# TODO: switch to `pulseio` once the following build is ready:
# https://github.com/adafruit/circuitpython/commit/f99deeda68f351da619da4621be3e87a5067e1ba
# https://github.com/adafruit/circuitpython/runs/2450646805


class Hcsr04:
    """HC-SR04 ultrasonic sensor workaround."""

    __slots__ = ("_trigger_io", "_echo_io", "_timeout", "n_timeouts")

    def __init__(self, *, trigger_pin: Pin, echo_pin: Pin, timeout=0.05):
        self._trigger_io = DigitalInOut(trigger_pin)
        self._trigger_io.direction = Direction.OUTPUT
        self._trigger_io.value = False
        self._echo_io = DigitalInOut(echo_pin)
        self._echo_io.direction = Direction.INPUT
        self._timeout = timeout
        self.n_timeouts = 0

    async def measure_once(self) -> float:
        """Measures the distance and returns it in metres."""

        await self._wait_for(False)

        # Issue the trigger pulse.
        self._trigger_io.value = True
        await async_sleep(0.000010)
        self._trigger_io.value = False

        await self._wait_for(True)
        return await self._wait_for(False) * 170.0

    async def measure(
        self,
        min_measurements: int = 50,
        min_duration: float = 0.1,
        max_timeouts: int = 5,
    ) -> float:
        n_measurements = n_timeouts = 0
        sum_ = 0.0
        end_time = monotonic() + min_duration
        while monotonic() < end_time or n_measurements < min_measurements:
            try:
                distance = await self.measure_once()
            except TimeoutError as e:
                await async_sleep(self._timeout)  # suppress all possible echos
                n_timeouts += 1
                if n_timeouts > max_timeouts:
                    raise TimeoutError(f"{n_timeouts} measurements timed out.") from e
            else:
                sum_ += distance
                n_measurements += 1
        return sum_ / n_measurements

    async def _wait_for(self, value: bool) -> float:
        start_time = monotonic()
        timeout_time = start_time + self._timeout
        while self._echo_io.value != value:
            await async_back_off()
            if monotonic() > timeout_time:
                self.n_timeouts += 1
                raise TimeoutError(f"Timed out while waiting for the {value} value.")
        return monotonic() - start_time
