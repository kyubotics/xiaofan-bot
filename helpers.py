import threading

threads = set()


def go(func, *args, **kwargs):
    t = threading.Thread(target=func, args=args, kwargs=kwargs)
    threads.add(t)
    t.start()


def wait_for_all_threads():
    for t in threads:
        assert isinstance(t, threading.Thread)
        if t.is_alive():
            t.join()
