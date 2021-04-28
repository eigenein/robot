"""My tiny event loop."""

from collections import namedtuple

from time import monotonic

from micro_logging import error, warning

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


class Awaitable(namedtuple("_AwaitableBase", ("resume_time",))):
    def __await__(self):
        actual_time = yield self.resume_time  # type: float
        if actual_time - self.resume_time > 0.001:
            warning(f"{self} resumed too late: {actual_time}")


event_loop = EventLoop()


def sleep(duration: float):
    """Sleep for the specified amount of time."""
    return Awaitable(monotonic() + duration)
