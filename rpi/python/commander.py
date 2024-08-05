import ctypes
import time


class Commander:

    ptr = ctypes.POINTER(ctypes.c_char)
    lib = ctypes.CDLL("build/libcommander.so")

    lib.createCommander.argtypes = []
    lib.createCommander.restype = ptr

    lib.createSerial.argtypes = [ptr, ctypes.c_char_p, ctypes.c_size_t, ctypes.c_size_t, ctypes.c_size_t]

    lib.connectSerial.argtypes = [ptr]
    lib.connectSerial.restype = ctypes.c_bool

    lib.disconnectSerial.argtypes = [ptr]

    lib.enableSerialVerbose.argtypes = [ptr]

    lib.disableSerialVerbose.argtypes = [ptr]

    lib.led_on.argtypes = [ptr]

    lib.led_off.argtypes = [ptr]

    lib.set_min_ppm.argtypes = [ptr]

    lib.set_reverse_ppm.argtypes = [ptr]

    lib.set_max_ppm.argtypes = [ptr]

    lib.allow_ema.argtypes = [ptr]

    lib.set_velocities.argtypes = [ptr, ctypes.c_int8, ctypes.c_int8]

    lib.init_motors.argtypes = [ptr]

    def __init__(self, serial_port : str, baudrate : int, cmd_data_size : int, msg_data_size : int):
        self.commander = Commander.lib.createCommander()
        if not Commander.lib.createSerial(self.commander, serial_port.encode('utf-8'), baudrate, cmd_data_size, msg_data_size):
            raise SystemError
        
    def __delete__(self):
        Commander.lib.disconnectSerial(self.commander)

    def connectSerial(self):
        return Commander.lib.connectSerial(self.commander)

    def enableSerialVerbose(self):
        Commander.lib.enableSerialVerbose(self.commander)

    def disableSerialVerbose(self):
        Commander.lib.disableSerialVerbose(self.commander)

    def ledOn(self):
        Commander.lib.led_on(self.commander)

    def ledOff(self):
        Commander.lib.led_off(self.commander)

    def setMinPpm(self):
        Commander.lib.set_min_ppm(self.commander)

    def setReversePpm(self):
        Commander.lib.set_reverse_ppm(self.commander)

    def setMaxPpm(self):
        Commander.lib.set_max_ppm(self.commander)

    def allowEMA(self):
        Commander.lib.allow_ema(self.commander)

    def setVelocities(self, vel0 : int, vel1 : int):
        Commander.lib.set_velocities(self.commander, vel0, vel1)


if __name__ == '__main__':
    commander = Commander('/dev/ttyUSB0', 9600, 3, 1)

    if commander.connectSerial():
        print('ok')
    else:
        print('nop')

    on = True

    while True:
        input()
        if on:
            commander.ledOn()
        else:
            commander.ledOff()
        on = not on
