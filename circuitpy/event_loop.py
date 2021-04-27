"""My tiny event loop."""

from time import monotonic

from custom_logging import error

_RESULT_NOT_READY = object()


class EventLoop:
    def __init__(self, *coroutines):
        # Ready task finishes immediately, causing the coroutine to start.
        self._queue = [(coroutine, _ready_task) for coroutine in coroutines]

    def run_until_complete(self):
        while self._queue:
            (coroutine, task) = self._queue.pop(0)
            result = task.result
            if result is _RESULT_NOT_READY:
                self._queue.append((coroutine, task))
                continue
            try:
                new_task = coroutine.send(result)
            except StopIteration:
                pass
            except Exception as e:
                error(f"Unhandled exception in coroutine {coroutine}: {e}.", e=e)
            else:
                self._queue.append((coroutine, new_task))


class AsyncTask:
    """Dummy task that finishes immediately."""

    def __await__(self):
        yield self

    @property
    def result(self):
        return None


class _SleepTask(AsyncTask):
    __slots__ = ["_ready_time"]

    def __init__(self, ready_time: float):
        self._ready_time = ready_time

    @property
    def result(self):
        if monotonic() < self._ready_time:
            return _RESULT_NOT_READY


def async_skip():
    """Skip your turn and give control back to the event loop."""
    return _ready_task


def async_sleep(duration: float):
    """Sleep for the specified amount of time."""
    return _SleepTask(monotonic() + duration)


_ready_task = AsyncTask()
