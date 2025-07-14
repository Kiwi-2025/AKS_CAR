# track - By: sky - Wed Jul 9 2025
# OpenMV4 H7 Plus UART(3)是P4-TX P5-RX

#导入函数库
import sensor, image, time, math, pyb

led = pyb.LED(1) # 初始化LED1
# 开启时钟
clock = time.clock() # 跟踪FPS帧率
# 变量初始化
uart = pyb.UART(3, 115200, timeout_char=1000)
# 阈值设置 根据实际情况进行更改 (1, 45, -29, 22, -21, 13)
TRA_RGB = [(20, 45, -34, 0, -10, 12)]
# TRA_TH = [(0, 5)]           # 巡线的灰度值 阈值[(0, 64)][(128, 255)]
# TRA_AngTH = 30                  # 巡线时角度阈值
# ROI区域设置
# (x,y,w,h,weight)=(矩形左上顶点的坐标(x,y), 矩形宽度和高度(w,h),权重)
TRA_ROIS_LEFT = [ # [ROI, weight]
        (0, 200, 160, 60, 0.7), # left
        (0, 140, 160, 60, 0.3),
        (0, 60, 160, 60, 0.1)]

TRA_ROIS_RIGHT = [
        (160, 200, 160, 60, 0.7), # right
        (160, 140, 160, 60, 0.3),
        (160, 60, 160, 60, 0.1)]



""" 输入: N个色块(blobs) 输出: N个色块中最大色块的索引(int i) """
def Find_MaxIndex(blobs):
    max_index = 0                      # 最大色块索引初始化
    max_pixes = 0                      # 最大像素值初始化
    for i in range(len(blobs)):         # 对N个色块进行N次遍历
        if blobs[i].pixels() > max_pixes: # 当某个色块像素大于最大值
            max_pixes = blobs[i].pixels() # 更新最大像素
            max_index = i                 # 更新最大索引

    return  max_index


""" 接受STM32传输的消息，判断运行方式 """
def Recive_Data():
    global OVSys_State
    if uart.any():                  # 如果串口接收到数据
        OVSys_State = int(uart.read())  # 将读到的数据强制转换为整数
        print(OVSys_State)            # 用于串口通信调试


""" send data to STM32 in string """
def Transmit_Data(deviation, deflection_angle):

    global deviation_sum, frame_count, last_send_time

    # 创建数据包
    data_packet = "@{:.2f}".format(deviation)

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
    last_send_time = pyb.millis()



""" 输入: ROI区域r(tuple), 图像TRA_img(image)"""
def Center(ROI, TRA_img):

    # init
    Weight_Sum = 0
    Centroid_Sum = 0

    # 找到视野中ROI区域的色块, merge=true: 将找到的图像区域合并，并返回code值表示合并颜色
    # 设置面积与像素数阈值，少之则不检测
    # blobs = list[blob], blob is a class
    for r in ROI:
        blobs = TRA_img.find_blobs(TRA_RGB, roi=r[0:4], pixels_threshold=24,
                                   area_threshold=5, merge=True)
        TRA_img.draw_rectangle(r[0:4])

        if blobs: # 如果找到了多个色块 计算质心和
            maxb_index = Find_MaxIndex(blobs) # 找到多个色块中的最大色块返回索引值
            # .rect() 返回最大色块外框元组(x,y,w,h)
            TRA_img.draw_rectangle(blobs[maxb_index].rect(), color=[255, 0, 0],
                               thickness = 2, fill = False)
            # 最大色块的中心位置标记十字：.cx(), .cy()返回质心坐标
            TRA_img.draw_cross(blobs[maxb_index].cx(),
                            blobs[maxb_index].cy(), color=[255, 0, 0], size=50)
            # 计算质心和 = (ROI中最大颜色块的中心点横坐标)cx * (ROI权值)w
            Centroid_Sum += blobs[maxb_index].cx() * r[4]
            Weight_Sum += r[4]

        elif blobs is None:
            Centroid_Sum += 0
            print("No blobs found in ROI:", r[0:4])

    # 中间公式: 确定线心位置 = 质心和/权值和
    if Weight_Sum == 0:
        Center = 0
    else:
        Center = (Centroid_Sum / Weight_Sum)

    return Center


# 摄像头初始化
sensor.reset()                     # 初始化相机传感器
sensor.set_pixformat(sensor.RGB565)# 设置相机模块的像素模式 16 bits/像素 GRAY为8
sensor.set_framesize(sensor.QVGA)  # 设置相机模块的帧大小 320x240 (QQVGA 160x120)
sensor.skip_frames(time=200)       # 跳过30帧 让相机图像在改变相机设置后稳定下来
sensor.set_auto_gain(False)        # 关闭自动增益
sensor.set_auto_whitebal(False)    # 关闭默认的白平衡

# 200ms平均统计变量初始化
deviation_sum = 0.0      # 偏差累计值
frame_count = 0          # 有效帧计数
last_send_time = pyb.millis()  # 上一次发送时间
SEND_INTERVAL = 200      # 发送间隔(ms)


# 主循环
while (1):
    clock.tick()
    current_time = pyb.millis()

    Sys_State = 1
    # OVSys_State = Recive_Data()
    led.on()

    if (Sys_State == 1): # 循迹模式

        # 事实证明，灰度模式循迹效果 not good
        # sensor.set_pixformat(sensor.GRAYSCALE) # 循迹模式 设置摄像头为灰度图
        TRA_img = sensor.snapshot().lens_corr(1.5) # .histeq() 截一帧图像 加强对比度好分割
        # TRA_img.mean(1)
        # TRA_img.binary([(0, 64)]) # 在其范围内设为255

        Center_Left = Center(ROI=TRA_ROIS_LEFT, TRA_img=TRA_img)
        Center_Right = Center(ROI=TRA_ROIS_RIGHT, TRA_img=TRA_img)

        if (Center_Left != 0) and (Center_Right != 0):
            Center_Pos = (Center_Left + Center_Right) / 2
        elif (Center_Left != 0) and (Center_Right == 0):
            Center_Pos = Center_Left
        elif (Center_Left == 0) and (Center_Right != 0):
            Center_Pos = Center_Right
        else:
            Center_Pos = 0

        Deflection_Angle = 0 # 需要将线心Center_Pos转换为偏角 偏角初始化为0

        if (Center_Pos == 0):
            deviation = 0
            deviation_sum += deviation
            print("Not Find Any Black Lines.")
        else:
            # 与帧中间作比，其与帧格式设置相关
            # 前进方向作为x轴，左偏为正
            Deflection_Angle = -math.atan((Center_Pos - 180)/ 120) # 计算偏角
            Deflection_Angle = math.degrees(Deflection_Angle) # 弧度值转换为角度

            TRA_img.draw_cross(int(Center_Pos), 120, color=(0, 0, 255), size=10) # 绘制十字线标记图像中心
            TRA_img.draw_circle(180, 120, 5, color=(0, 255, 0))
            deviation = 180 - Center_Pos # 计算偏离中心的距离
            # print(deviation, Center_Pos) test whether deviation is correct

            # 累计偏差值用于平均计算
            deviation_sum += deviation
            frame_count += 1

        if deviation_sum != 0:
            # 计算平均偏差
            avg_deviation = deviation_sum / frame_count

        # 检查是否达到发送时间间隔
        if current_time - last_send_time >= SEND_INTERVAL:
            if frame_count > 0:
                # print(f"Track midpoint: {Center_Pos:.3f}, Deviation: {avg_deviation:.3f}, Deflection_Angle: {Deflection_Angle:.1f}")
                Transmit_Data(avg_deviation, Deflection_Angle)

    # clock.fps()
    # print(clock.fps()) # 打印FPS帧率








