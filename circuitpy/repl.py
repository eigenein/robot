import peripherals
from micro_asyncio import sleep
from peripherals import uart0

globals_ = {
    "peripherals": peripherals,
}


async def run():
    while True:
        await sleep()
        uart0.write(b"> ")
        input_ = []

        while True:
            await sleep()
            while not uart0.in_waiting:
                await sleep()
            char = uart0.read(1)
            if char == b"\n":
                pass
            elif char == b"\r":
                if input_:
                    uart0.write(b"\r\n")
                    break
            else:
                if char == b"\x7F":
                    uart0.write(b"\b")
                    input_ = input_[:-1]
                else:
                    uart0.write(char)  # echo
                    input_.append(char)

        source = b"".join(input_).decode("utf-8").strip()
        if not source or source.startswith("CONNECTED") or source.startswith("+CONNECTING") or "+DISC:" in source:
            continue
        try:
            result = eval(source, globals_)
        except Exception as e:
            result = f"ERROR: {e} for source {repr(source)}"
        uart0.write(f"{result}\r\n".encode("utf-8"))
