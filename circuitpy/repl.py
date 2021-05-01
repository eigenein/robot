from micro_asyncio import sleep
from micro_logging import error
from peripherals import uart0


async def run():
    while True:
        uart0.write(b"> ")
        input_ = b""
        while input_[-1:] not in (b"\r", b"\r\n", b"\n"):
            while not uart0.in_waiting:
                await sleep(0.01)
            input_ += uart0.read(uart0.in_waiting)
        input_ = input_.decode("utf-8").strip()
        if not input_:
            continue
        try:
            result = eval(input_)
        except Exception as e:
            error(f"Error in the remote terminal: `{e}` for input `{input_}`", e=e)
        else:
            await sleep(0.0)
            uart0.write(str(result).encode("utf-8"))
