# track - By: sky - Wed Jul 9 2025
# OpenMV4 H7 Plus UART(3)是P4-TX P5-RX

import pyb, time

uart = pyb.UART(3, 115200, timeout_char=1000)
# set the data to transmit
buffer = bytearray()
# define the frame format of data: 0x40, data, 0x2F(8 bits)
buffer.append(0x40)
buffer.extend(bytearray([0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38]))
buffer.append(0x2F)

while True:

    uart.write(buffer)
    # print("HAVE SENT")
    # time.sleep_ms(10)  # 等待10ms确保数据接收完成

    # check whether data need to be read
    if uart.any():
        receive_data = uart.read()
        cleaned_data = receive_data.decode('utf-8').rstrip('\x00')
        print("Received:", cleaned_data)

    time.sleep(1)







