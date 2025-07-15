# Untitled - By: sky - Tue Jul 15 2025

import sensor, image, time, math, pyb
from pyb import LED, UART


clock = time.clock() # 跟踪FPS帧率
uart = UART(3,115200)   # 定义串口3变量, timeout_char=1000
uart.init(115200, bits=8, parity=None, stop=1) # init with given parameters
# 无校验，8位数据位，1位停止位
rx_buff=[] # 串口接收缓冲区
state = 0 # 串口接收状态
MODE = 1 # 状态变量
rx_buff1=[] # 串口接收缓冲区1
state1 = 0 # 串口接收状态1
is_fetch1=0 # 标志变量

red_led=pyb.LED(1)
green_led=pyb.LED(2)
blue_led=pyb.LED(3)
wuti_threshold = (0,100,9,127,0,127)
last_Theta = 0
Theta = 0

def find_max(blobs):
    max_size = 0
    for blob in blobs:
        if blob.pixels() > max_size:
            max_blob = blob
            max_size = blob.pixels()
    return max_blob # 寻找最大色块并返回最大色块的坐标


""" send data to STM32 in string """
def Transmit_Data(deviation, y):

    global deviation_sum, frame_count, last_send_time, last_Theta

    # 创建数据包
    data_packet = "@{:.2f},{:.2f}".format(deviation, y)


    # 添加结束符并发送
    uart.write(data_packet + "/")

    # 调试输出
    print("Sent Avg:", data_packet + "/")

    # check whether data need to be read
    if uart.any():
        receive_data = uart.read()
        cleaned_data = receive_data.decode('utf-8').rstrip('\x00')
        print("Received:", cleaned_data)

    # 重置统计变量
    deviation_sum = 0
    frame_count = 0
    # last_send_time = pyb.millis()
    last_Theta = deviation


mode = 5
alpha = 0.7

while True:
    # clock.tick()
    # current_time = pyb.millis()

    red_led.on()


    sensor.reset()
    sensor.set_pixformat(sensor.GRAYSCALE)
    sensor.set_framesize(sensor.QQVGA)
    THRESHOLD = (80, 255)
    sensor.skip_frames(time = 20)
    clock = time.clock()
    while (mode == 5):

        img = sensor.snapshot().lens_corr(strength = 1.8, zoom = 1.0)
        img.binary([THRESHOLD], invert=True)
        lenbest=0
        lenworse=200
        midd1=79.5
        midd2=79.5
        endy=119
        # aa=0
        # bb=0
        # cc=0
        # dd=0


        lx = []
        rx = []
        thetas_l = []
        thetas_r = []

        y_lists = [110, 85, 60, 35, 10] # 赛道线y坐标列表
        x_range = [20, 140] # 赛道线x坐标范围
        img.draw_rectangle(x_range[0], y_lists[0], x_range[-1]-x_range[0],
                            y_lists[-1]-y_lists[0], color=(155, 155, 155), thickness=2)
        # 321 is none
        for i in range(0, 5):
            y = y_lists[i]

            mark=[]
            number=0
            for x in x_range:
                if img.get_pixel(x, y) != img.get_pixel(x-1, y):
                    mark.append(x) # 记录边界
                    number = number + 1


            if number == 4: # 双线
                lx.append((mark[0]+mark[1])/2)
                rx.append((mark[2]+mark[3])/2)


            elif number == 3: # 1.5线
                if img.get_pixel(0,y):
                    lx.append(mark[0])
                    rx.append((mark[1]+mark[2])/2)

                else:
                    lx.append((mark[0]+mark[1])/2)
                    rx.append(mark[2])


            elif number==2: # 单线
                if i == 0:
                    lx.append((mark[0]+mark[1])/2)
                    rx.append(321)

                else:
                    if any(x == 321 for x in rx[:i]):
                        lx.append((mark[0]+mark[1])/2)
                        rx.append(321)
                    else:
                        if any(x == 0 for x in lx[:i]):
                            lx.append(0)
                            rx.append((mark[0]+mark[1])/2)
                        if any(x == 160 for x in rx[:i]):
                            lx.append((mark[0]+mark[1])/2)
                            rx.append(160)

            elif number==1:
                if i == 0:
                    lx.append(mark[0])
                    rx.append(321)

                else:
                    if any(x == 321 for x in rx[:i]):
                        lx.append(mark[0])
                        rx.append(321)
                    else:
                        if any(x == 0 for x in lx[:i]):
                            lx.append(0)
                            rx.append(mark[0])
                        if any(x == 160 for x in rx[:i]):
                            lx.append(mark[0])
                            rx.append(160)

            else:
                Theta = 0 # 没有找到黑线
                # leng=0
                # mid=79.5


        if all(x == 321 for x in rx[:5]):
            for j in range(0, len(thetas_l)):
                thetas_l.append(-math.atan((lx[j+1]-lx[j])/25))
            thetas_r = [0 for _ in thetas_r]
        else:
            for j in range(0, len(thetas_l)):
                thetas_l.append(-math.atan((lx[j+1]-lx[j])/25))
            for j in range(0, len(thetas_r)):
                thetas_r.append(-math.atan((rx[j+1]-rx[j])/25))


        if all(x == 0 for x in thetas_r):
            Theta = sum(thetas_r) / 4
        else:
            Theta = (sum(thetas_r) + sum(thetas_r)) / (2 * (len(thetas_l) + len(thetas_r)))

        # midd = (midd1 + midd2)/2
        # dev = 79.5 - midd # 偏差
        # dev_s = 119 - endy # 赛道最长处的y

        # Theta = alpha * Theta + (1 - alpha) * last_Theta
        Theta = math.degrees(Theta)
        # print(clock.fps())
        # img.draw_cross(int(midd), int(119-endy), color=(100, 100, 100))
        # 计算终点 (x1, y1)
        x0, y0, length = 80, 60, 50
        x1 = x0 + int(-length * math.sin(Theta))
        y1 = y0 - int(length * math.cos(Theta))

        # 在图像上绘制直线（红色，线宽 2）
        img.draw_line(x0, y0, x1, y1, color=(255, 0, 0), thickness=2)
        img.draw_circle(80, 60, 10, color=(0, 255, 0), size=40)
        # if aa==1 and bb==1:
            # print(dev)
            # print(dev_s)
        print(Theta)
        print(lx)

        # Transmit_Data(Theta, 60)
