import time
import board
import adafruit_icm20x
import math
import threading
from BusLockWrapper import *

def low_pass_filter(prev_value, new_value, a = 0.9, b = 0.1):
    return a * prev_value + b * new_value

class IMU():
    
    def __init__(self):
        
        self.measurement_period = 0.01
    
        self.icm = adafruit_icm20x.ICM20948(board.I2C(), address=0x68)
        self.drift_yaw_sped = self.calibration_gyroscope()
        
        self.magnetic_x = 1
        self.magnetic_y = 1
        self.yaw_speed = 0
        self.yaw = 0

        self.imu_thread = threading.Thread(target=self._imu_proces)
        self.imu_thread.start()
        
    @BusLockWrapper
    def __read_gyroscope(self):
        self.yaw_speed = self.icm.gyro[2]
        return self.yaw_speed
    
    @BusLockWrapper
    def __read_magnetic(self):
        data = self.icm.magnetic
        self.magnetic_x = data[0]
        self.magnetic_y = data[1]
        return self.magnetic_x, self.magnetic_y
    
    def stop(self):
        
        if self.magnetic_thread.is_alive():
            self.sensor_event.clear()
            self.magnetic_thread.join()
        
        if self.gyroscope_thread.is_alive():
            self.sensor_event.set()
            self.gyroscope_thread.join()
        
    
    def calibration_gyroscope(self): 
        """Определение дрейфа нуля гироскопа"""
        drift_yaw_sped = self.__read_gyroscope()
        
        # Дрейф определяется как плавающее среднее по 1000 измерений 
        for i in range(200):
            drift_yaw_sped = drift_yaw_sped * 0.995 + self.icm.gyro[2] * 0.005 
            time.sleep(self.measurement_period)
        
        self.drift_yaw_sped = drift_yaw_sped
        return drift_yaw_sped
        
    def _imu_proces(self):

        self.__read_gyroscope()
        last_time = time.time()
        
        while 1:
            
            self.__read_magnetic()
            
            now_time = time.time()
            
            self.yaw += (self.__read_gyroscope() - self.drift_yaw_sped) * \
                (now_time - last_time) * \
                180 / math.pi
            
            last_time = now_time
            time.sleep(self.measurement_period)
            

    def get_telemetry(self):
        yaw = self.yaw
        
        yaw += 0
        while yaw > 180:  yaw -= 360
        while yaw < -180: yaw += 360
         
        return self.magnetic_x, self.magnetic_y, round(yaw, 4), round(self.yaw_speed, 4)     
        
if __name__ == "__main__":
    imu = IMU()
    print(imu.calibration_gyroscope())

    for i in range(500):
        print(imu.get_telemetry())
        time.sleep(0.05)
    
