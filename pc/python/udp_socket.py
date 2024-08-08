import os
import socket


class Socket:

    def __init__(self, ip, port, data_size):
        self.ip = ip
        self.port = port
        self.data_size = data_size
        print(f'UDP target IP: {ip}')
        print(f'UDP target port: {port}')
  
        self.socket = socket.socket(socket.AF_INET,  # Internet
                                  socket.SOCK_DGRAM) # UDP
        
        self.verbose = True
        
    def send(self, data : list):
        if len(data) != self.data_size:
            print('wrong data size')
            return
        msg = bytes(data)
        self.socket.sendto(msg, (self.ip, self.port))
        if self.verbose:
            print(f'msg: {msg}')

    def receive(self):
        msg, _ = self.socket.recvfrom(self.data_size)
        data = int.from_bytes(msg, 'big')
        if self.verbose:
            print(f'received message: {data}')
        return data

    def enableVerbose(self):
        self.verbose = True

    def disableVerbose(self):
        self.verbose = False


def get_ip():
        gw = os.popen("ip -4 route show default").read().split()
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect((gw[2], 0))
        return s.getsockname()[0]
