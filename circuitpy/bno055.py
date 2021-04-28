from busio import I2C

from async_busio import AsyncIo
from custom_logging import error

ADDRESS_A = 0x28
ADDRESS_B = 0x29

CHIP_ID = 0xA0

CHIP_ID_ADDR = 0x00


class Bno055:
    def __init__(self, io: AsyncIo, address: int = ADDRESS_B):
        self._io = io  # TODO: add I2C helper mixin.
        self._address = address
        self._buffer = bytearray(1)

    async def ping(self) -> bool:
        # TODO: add ping loop with timeout.
        async with self._io as io:  # type: I2C
            io.readfrom_into(self._address, self._buffer, start=CHIP_ID_ADDR, end=(CHIP_ID_ADDR + 1))
            is_ok = self._buffer[0] == CHIP_ID
        if not is_ok:
            error(f"Ping failed on {self}, buffer: {self._buffer}")
        return is_ok

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}({repr(self._io)}, {self._address})"
