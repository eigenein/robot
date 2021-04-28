"""My tiny event loop."""

from time import monotonic

from custom_logging import error, warning

_RESULT_NOT_READY = object()


class EventLoop:
    __slots__ = ("_queue",)

    def __init__(self, *coroutines):
        self._queue = []
        self.schedule(*coroutines)

    def schedule(self, *coroutines) -> "EventLoop":
        """Schedule execution of the coroutines."""
        self._queue.extend((coroutine, None) for coroutine in coroutines)
        return self

    def run_until_complete(self):
        """Runs the event loop, until there's no tasks in the queue anymore."""
        while self._queue:
            now = monotonic()
            (coroutine, resume_time) = self._queue.pop(0)
            if resume_time is not None and resume_time > now:
                self._queue.append((coroutine, resume_time))
                continue
            try:
                new_task = coroutine.send(now if resume_time is not None else None)
            except StopIteration:
                # The coroutine has finished.
                pass
            except Exception as e:
                error(f"Unhandled exception in coroutine {coroutine}: {e}.")
                raise
            else:
                self._queue.append((coroutine, new_task))


class Awaitable:
    __slots__ = ("_resume_time",)

    def __init__(self, resume_time: float):
        self._resume_time = resume_time

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}({self._resume_time})"

    def __await__(self):
        actual_time = yield self._resume_time  # type: float
        if actual_time - self._resume_time > 0.001:
            warning(f"{self} resumed too late: {actual_time}")


event_loop = EventLoop()


def async_sleep(duration: float):
    """Sleep for the specified amount of time."""
    return Awaitable(monotonic() + duration)
