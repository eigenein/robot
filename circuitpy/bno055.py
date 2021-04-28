from busio import I2C
from struct import unpack_from
from time import monotonic
from ulab.numpy import ndarray

from async_busio import AsyncBus
from micro_asyncio import sleep

I2C_ADDRESS_A = 0x28
I2C_ADDRESS_B = 0x29

CHIP_ID = 0xA0

POWER_MODE_NORMAL = 0x00

OPERATION_MODE_CONFIG = 0x00
OPERATION_MODE_NDOF = 0x0C

REGISTER_CHIP_ID = 0x00
REGISTER_PAGE_ID = 0x07

REGISTER_EULER_HEADING_LSB = 0x1A
REGISTER_EULER_HEADING_MSB = 0x1B
REGISTER_EULER_ROLL_LSB = 0x1C
REGISTER_EULER_ROLL_MSB = 0x1D
REGISTER_EULER_PITCH_LSB = 0x1E
REGISTER_EULER_PITCH_MSB = 0x1F

REGISTER_UNIT_SELECTION = 0x3B
REGISTER_OPERATION_MODE = 0x3D
REGISTER_POWER_MODE = 0x3E


class Bno055:
    __slots__ = ["_bus", "_address", "_mode", "_buffer"]

    def __init__(self, bus: AsyncBus, address: int = I2C_ADDRESS_B, mode: int = OPERATION_MODE_NDOF):
        self._bus = bus
        self._address = address
        self._mode = mode
        self._buffer = bytearray(6)

    async def begin(self):
        await self._wait_online()
        await self._write_byte(REGISTER_PAGE_ID, 0)
        await self._set_mode(OPERATION_MODE_CONFIG)
        await sleep(0.020)  # switch to config mode
        await self._write_byte(REGISTER_POWER_MODE, POWER_MODE_NORMAL)
        await self._write_byte(REGISTER_UNIT_SELECTION, 0b00000110)  # m/s², radians, Celsius
        await self._set_mode(self._mode)
        await sleep(0.008)  # switch to operation mode

    async def get_euler(self) -> ndarray:
        async with self._bus as bus:  # type: I2C
            self._buffer[0] = REGISTER_EULER_HEADING_LSB
            bus.writeto(self._address, self._buffer, end=1)
            bus.readfrom_into(self._address, self._buffer, end=6)
            return ndarray(unpack_from("<hhh", self._buffer)) / 900.0

    async def _wait_online(self):
        """Wait until we're able to read the chip ID."""
        timeout_time = monotonic() + 1.0
        while monotonic() < timeout_time:
            try:
                if (await self._read_byte(REGISTER_CHIP_ID)) == CHIP_ID:
                    return True
            except OSError:
                pass
            await sleep(0.01)
        raise TimeoutError("device is not online")

    async def _set_mode(self, mode: int):
        await self._write_byte(REGISTER_OPERATION_MODE, mode)

    async def _read_byte(self, from_register: int) -> int:
        async with self._bus as bus:  # type: I2C
            self._buffer[0] = from_register
            bus.writeto(self._address, self._buffer, end=1)
            bus.readfrom_into(self._address, self._buffer, end=1)
            return self._buffer[0]

    async def _write_byte(self, to_register: int, byte: int) -> None:
        async with self._bus as bus:  # type: I2C
            self._buffer[0] = to_register
            self._buffer[1] = byte
            bus.writeto(self._address, self._buffer, end=2)

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}({repr(self._bus)}, {self._address})"
