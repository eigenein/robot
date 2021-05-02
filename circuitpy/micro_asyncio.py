"""My tiny event loop."""

from collections import namedtuple

from time import monotonic, sleep as blocking_sleep

from micro_logging import error, warning

_RESULT_NOT_READY = object()
_MINIMAL_SLEEP = 0.001

QueueItem = namedtuple("QueueItem", ("coroutine", "resume_time"))


class EventLoop:
    __slots__ = ("_queue", "_load")

    def __init__(self, *coroutines):
        self._queue = []
        self.schedule(*coroutines)

    def schedule(self, *coroutines) -> "EventLoop":
        """Schedule execution of the coroutines."""
        now = monotonic()
        self._queue.extend(QueueItem(coroutine, now) for coroutine in coroutines)
        return self

    def run_until_complete(self):
        """Runs the event loop, until there's no tasks in the queue anymore."""
        while self._queue:
            # TODO: should use a heap.
            index = min(range(len(self._queue)), key=lambda i: self._queue[i].resume_time)  # type: int
            item = self._queue.pop(index)  # type: QueueItem
            sleep_duration = item.resume_time - monotonic()
            if sleep_duration > 0.0:
                blocking_sleep(sleep_duration)
            elif sleep_duration < -0.01:
                warning(f"{item.coroutine} is {-sleep_duration}s late.")
            try:
                next_resume_time = item.coroutine.send(None)
            except StopIteration:
                pass  # the coroutine has finished
            except Exception as e:
                error(f"Unhandled exception in coroutine {item.coroutine}: {e}.")
                raise
            else:
                self._queue.append(QueueItem(item.coroutine, next_resume_time))


class Awaitable(namedtuple("Awaitable", ("resume_time",))):
    def __await__(self):
        yield self.resume_time


event_loop = EventLoop()


def sleep(duration: float = _MINIMAL_SLEEP):
    """Sleep for the specified amount of time."""
    return Awaitable(monotonic() + duration)
