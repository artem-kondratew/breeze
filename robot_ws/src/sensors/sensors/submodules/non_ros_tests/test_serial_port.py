import serial


def testSerialPort(port : str, baudrate : int) -> None:
    s = serial.Serial(port, baudrate=baudrate)
    for i in range(100):
        data = s.read(10)  # Read 10 bytes from the COM port
        print(data)
    s.close()


if __name__ == '__main__':
    testSerialPort('/dev/ttyUSB0', 9600)
