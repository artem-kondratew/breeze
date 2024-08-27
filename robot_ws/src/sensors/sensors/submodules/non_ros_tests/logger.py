import adafruit_icm20x
import board
import os
import time
import pickle


N = 10000

icm = adafruit_icm20x.ICM20948(board.I2C(), address=0x68)

file = open(os.path.join(os.path.dirname(__file__), 'logs', 'data.log'), 'wb')

filtered_magx, filtered_magy, filtered_magz = icm.magnetic

data = []


def low_pass_filter(prev_value, new_value) -> float:
    return 0.5 * prev_value + 0.50 * new_value


for i in range(N):
    try:
        magx, magy, magz = icm.magnetic
        #filtered_magx = low_pass_filter(filtered_magx, magx)
        #filtered_magy = filtered_magy, magy)
        #filtered_magz = magz
        data.append([magx, magy, magz])
    except:
        print('err')
        continue
    time.sleep(0.01)
    if i % 100 == 0:
        print(f"{i // 100}%")

pickle.dump(data, file)
