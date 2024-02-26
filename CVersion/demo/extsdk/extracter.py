import struct
import time
from extsdk.taskStruct import ext_task
import socket
import pandas as pd


class extracter:
    def __init__(self):
        self.task = ext_task()
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.ip = None
        self.port = None

    def connect(self, ip, port):
        # 连接到提取服务器
        self.ip = ip
        self.port = port

    def start_extract(self, raw=False):
        if self.ip is None or self.port is None:
            return None, None
        # for test
        test_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        test_socket.connect((self.ip, self.port))
        test_socket.send(b'\x00')
        test_socket.close()

        self.server_socket.connect((self.ip, self.port))
        self.server_socket.send(self.task.pack())
        # 向服务器发送提取任务
        hadm_ids = []
        datas = []
        counter = 0
        sum_data_len = 0
        while True:
            len_data = self.server_socket.recv(4)
            data_len = struct.unpack("i", len_data)[0]
            if data_len == 0:
                break
            sum_data_len += data_len
            id_data = self.server_socket.recv(4)
            id = struct.unpack("i", id_data)[0]
            result_buff = b""
            while len(result_buff) < data_len:
                result_buff += self.server_socket.recv(data_len - len(result_buff))
            hadm_ids.append(id)
            datas.append(result_buff)
            if counter % 100 == 0:
                print(f"\rrecv: {counter}  recv data size: {self.__byte_fmt(sum_data_len)}  {sum_data_len}bytes     ",
                      end="")
            counter += 1
        print(f"\rrecv: {counter}  recv data size: {self.__byte_fmt(sum_data_len)}  {sum_data_len}bytes     ",
              end="")
        print()
        quit_flag = self.server_socket.recv(4)
        self.server_socket.close()
        if raw:
            return hadm_ids, datas
        else:
            return datas

    def __byte_fmt(self, l):
        u = ["B", "KB", "MB", "GB", "TB"]
        index = 0
        while l > 1024 and index < 4:
            l /= 1024
            index += 1
        return str(int(l)) + u[index]
