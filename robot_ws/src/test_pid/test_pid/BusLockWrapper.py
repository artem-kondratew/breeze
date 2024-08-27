import threading, time   
_busLock = threading.Lock()

def BusLockWrapper(func):
    def _wrapper(*args, **kwargs):
        try:
            while _busLock.locked(): time.sleep(0.001)
            _busLock.acquire()
            time.sleep(0.001)
            return func(*args, **kwargs)
        except IOError as err:
            print(err)
            pass
        finally:
            time.sleep(0.001)
            _busLock.release()
    return _wrapper