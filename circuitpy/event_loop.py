"""My tiny event loop."""

from time import monotonic

from custom_logging import error, warning

_RESULT_NOT_READY = object()


class EventLoop:
    __slots__ = ("_queue", "n_errors")

    def __init__(self, *coroutines):
        self._queue = []
        self.n_errors = 0
        self.schedule(*coroutines)

    def schedule(self, *coroutines) -> "EventLoop":
        """Schedule execution of the coroutines."""
        self._queue.extend((coroutine, _ready_task) for coroutine in coroutines)
        return self

    def run_until_complete(self):
        """Runs the event loop, until there's no tasks in the queue anymore."""
        while self._queue:
            (coroutine, task) = self._queue.pop(0)
            result = task.result
            if result is _RESULT_NOT_READY:
                self._queue.append((coroutine, task))
                continue
            try:
                # The first task for a coroutine is always the `_ready_task`,
                # which starts the coroutine here.
                new_task = coroutine.send(result)
            except StopIteration:
                # The coroutine has finished.
                pass
            except Exception as e:
                error(f"Unhandled exception in coroutine {coroutine}: {e}.", e=e)
                self.n_errors += 1
            else:
                self._queue.append((coroutine, new_task))


class AsyncTask:
    """Dummy task that finishes immediately."""

    __slots__ = ("_is_awaited",)

    def __await__(self):
        self._is_awaited = True
        yield self

    def __del__(self):
        if not self._is_awaited:
            warning(f"{self} was never awaited")

    @property
    def result(self):
        return None


class _SleepTask(AsyncTask):
    __slots__ = ("_ready_time",)

    def __init__(self, ready_time: float):
        self._ready_time = ready_time

    @property
    def result(self):
        if monotonic() < self._ready_time:
            return _RESULT_NOT_READY


event_loop = EventLoop()


def async_back_off():
    """Skip your turn and give control back to the event loop."""
    return _ready_task


def async_sleep(duration: float):
    """Sleep for the specified amount of time."""
    return _SleepTask(monotonic() + duration)


_ready_task = AsyncTask()
