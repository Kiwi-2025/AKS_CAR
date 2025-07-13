# track - By: sky - Wed Jul 9 2025
# OpenMV4 H7 Plus UART(3)是P4-TX P5-RX

#导入函数库
import sensor, image, time, math, pyb


# 开启时钟
clock = time.clock()# 跟踪FPS帧率
# 变量初始化
uart = pyb.UART(3, 115200, timeout_char=1000)
# 阈值设置 根据实际情况进行更改
TRA_RGB = [(20, 45, -34, 0, -10, 12)]
TRA_TH = [(0, 5)]           # 巡线的灰度值 阈值[(0, 64)][(128, 255)]
TRA_AngTH = 30                  # 巡线时角度阈值
# ROI区域设置
# (x,y,w,h,weight)=(矩形左上顶点的坐标(x,y), 矩形宽度和高度(w,h),权重)
TRA_ROIS = [ # [ROI, weight]
        (50, 140, 80, 20, 0.7), # left
        (60, 100, 80, 40, 0.3),
        (80, 60, 80, 40, 0.1),

        (220, 140, 80, 40, 0.7), # right
        (210, 100, 80, 40, 0.3),
        (200, 60, 80, 40, 0.1),
       ]

OBS_ROI = [(30 , 10, 100, 100)] #避障模式ROI区域

#通信数据包封装
Run     = bytearray([0x40,0x4F,0x4D,0x56,0x31,0x26,0x2F])# @OMV1/ #  直行
Left    = bytearray([0x40,0x4F,0x4D,0x56,0x32,0x26,0x2F])# @OMV2/ #  左转
Right   = bytearray([0x40,0x4F,0x4D,0x56,0x33,0x26,0x2F])# @OMV3/ #  右转
Stop    = bytearray([0x40,0x4F,0x4D,0x56,0x34,0x26,0x2F])# @OMV4/ #  停止

# Fun1: 获得最大色块的位置索引函数
# 输入: N个色块(blobs) 输出: N个色块中最大色块的索引(int i)
def Find_MaxIndex(blobs):
    maxb_index = 0                      # 最大色块索引初始化
    max_pixels = 0                      # 最大像素值初始化
    for i in range(len(blobs)):         # 对N个色块进行N次遍历
        if blobs[i].pixels() > max_pixels: # 当某个色块像素大于最大值
            max_pixels = blobs[i].pixels() # 更新最大像素
            maxb_index = i                 # 更新最大索引

    return  maxb_index

def Recive_Data():
    global OVSys_State
    if uart.any():                  # 如果串口接收到数据
        OVSys_State = int(uart.read())  # 将读到的数据强制转换为整数
        print(OVSys_State)            # 用于串口通信调试

def Center(r, TRA_img):

    Centroid_Sum = 0
    # 找到视野中ROI区域的色块, merge=true: 将找到的图像区域合并，并返回code值表示合并颜色
    # 设置面积与像素数阈值，少之则不检测
    # blobs = list[blob], blob is a class
    blobs = TRA_img.find_blobs(TRA_RGB, roi=r[0:4], pixels_threshold=24,
                               area_threshold=5, merge=True)
    TRA_img.draw_rectangle(r[0:4])

    if blobs: # 如果找到了多个色块 计算质心和
        maxb_index = Find_MaxIndex(blobs) # 找到多个色块中的最大色块返回索引值
        # .rect() 返回最大色块外框元组(x,y,w,h) 绘制线宽为2的矩形框 不填充矩形
        TRA_img.draw_rectangle(blobs[maxb_index].rect(), color=[255, 0, 0],
                               thickness = 2, fill = False)
        # 最大色块的中心位置标记十字：.cx(), .cy()返回质心坐标
        TRA_img.draw_cross(blobs[maxb_index].cx(),
                            blobs[maxb_index].cy(), color=[255, 0, 0])
        # 计算质心和 = (ROI中最大颜色块的中心点横坐标)cx * (ROI权值)w
        Centroid_Sum += blobs[maxb_index].cx() * r[4]


    return Centroid_Sum


# 摄像头初始化
sensor.reset()                     # 初始化相机传感器
sensor.set_pixformat(sensor.RGB565)# 设置相机模块的像素模式 16 bits/像素 GRAY为8
sensor.set_framesize(sensor.QVGA)  # 设置相机模块的帧大小 320x240 (QQVGA 160x120)
sensor.skip_frames(time=200)       # 跳过30帧 让相机图像在改变相机设置后稳定下来
sensor.set_auto_gain(False)        # 关闭自动增益
sensor.set_auto_whitebal(False)    # 关闭默认的白平衡

#主函数
while (1):
    OVSys_State = 1
    clock.tick()
    Center_Left_Sum = 0
    Center_Right_Sum = 0
    Weight_Left_Sum = 0
    Weight_Right_Sum = 0

    if (OVSys_State == 1) or (OVSys_State == 3): # 循迹模式或者循迹避障模式

            # 事实证明，灰度模式循迹效果 not good
            # sensor.set_pixformat(sensor.GRAYSCALE) # 循迹模式 设置摄像头为灰度图
            TRA_img = sensor.snapshot().lens_corr(1.5) #.histeq() 截一帧图像 加强对比度好分割
            # TRA_img.mean(1)
            # TRA_img.binary([(0, 64)]) # 在其范围内设为255

            # # 在LCD上打印帧率和OpenMV工作模式
            # TRA_img.draw_string(0, 60, "OpenMv Mode: Tracking Mode")
            # TRA_img.draw_string(0, 100, "FPS: %.2f"%(clock.fps()))

            # 偏移角度计算

            for index, r in enumerate(TRA_ROIS):
                if index < 3:
                    Center_Left_Sum += Center(r, TRA_img)
                    Weight_Left_Sum += r[4]
                else:
                    Center_Right_Sum += Center(r, TRA_img)
                    Weight_Right_Sum += r[4]

            # 中间公式: 确定线心位置 = 质心和/权值和
            Center_Left = (Center_Left_Sum / Weight_Left_Sum)
            Center_Right = (Center_Right_Sum / Weight_Right_Sum)

            Center_Pos = (Center_Left + Center_Right) / 2
            Deflection_Angle = 0 # 需要将线心Center_Pos转换为偏角 偏角初始化为0

            # 与帧中间作比，其与帧格式设置相关
            # 前进方向作为x轴，左偏为正
            Deflection_Angle = -math.atan((Center_Pos - 180)/ 120) # 计算偏角

            Deflection_Angle = math.degrees(Deflection_Angle) # 弧度值转换为角度
            TRA_img.draw_cross(int(Center_Pos), 120, color=(0, 0, 255), size=10) # 绘制十字线标记图像中心
            TRA_img.draw_circle(180, 120, 5, color=(0, 255, 0))
            print(Center_Pos, Deflection_Angle)
            # # 当偏角大于偏角阈值 且小于最大偏角 和 STM32通信 小车左转s
            # Angle_err = Deflection_Angle
            # if abs(Deflection_Angle) > TRA_AngTH:
            #     if Deflection_Angle>0:
            #             uart.write(Right) # 和STM32通信 小车左转
            #             # time.sleep(10)
            #             TRA_img.draw_string(0, 0, "Car: Right", color=[255, 0, 0])
            #             print("Turn Right") # 用于程序终端调试
            #             print("Turn Angle: %f" %(Deflection_Angle))

            #     # 当偏角小于负的偏角阈值 且大于最小偏角
            #     if Deflection_Angle < 0:
            #             uart.write(Left) # 和STM32通信 小车右转
            #             # time.sleep(10)
            #             TRA_img.draw_string(0,0,"Car: Left", color=[255, 0, 0])
            #             print("Turn ：Left")#用于程序终端调试
            #             print("Turn Angle: %f" %(Deflection_Angle))

            # # 当小车角度绝对值小于阈值
            # elif abs(Deflection_Angle) <= TRA_AngTH:
            #     uart.write(Run)#和STM32通信 小车直行
            #     TRA_img.draw_string(0, 0, "Car: Run", color=[255, 0, 0])
            #     print("Run")#用于程序终端调试
            #     print("Turn Angle: %f" %(Deflection_Angle))
    clock.fps()
    print(clock.fps()) # 打印FPS帧率
    # time.sleep(1)
            # lcd.display(TRA_img)#在LCD上显示img图像








