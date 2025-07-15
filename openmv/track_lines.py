# track_lines - By: sky - Fri Jul 11 2025
# Reference: https://blog.csdn.net/ediykk/article/details/131754537
# gray[255] = white
# Image file: gray threshold limit: [0, 76] --> only read black line
# Real world: LAB (0, 48, -8, 24, 4, 127)(NO mean, NO histeq)

# FIRST EXAMPLE

# #导入函数库
# import sensor, image, time, math, pyb


# # 开启时钟
# clock = time.clock()# 跟踪FPS帧率
# # 变量初始化
# uart = pyb.UART(3, 115200, timeout_char=1000)
# # 阈值设置 根据实际情况进行更改
# TRA_TH = [(0, 5)]           # 巡线的灰度值 阈值[(0, 64)][(128, 255)]
# TRA_AngTH = 30                  # 巡线时角度阈值
# # ROI区域设置
# # (x,y,w,h,weight)=(矩形左上顶点的坐标(x,y), 矩形宽度和高度(w,h),权重)
# TRA_ROIS = [ # [ROI, weight]
#         (0, 100, 160, 20, 0.7), # You'll need to tweak the weights for your app
#         (0,  50, 160, 20, 0.3), # depending on how your robot is setup.
#         (0,   0, 160, 20, 0.1)
#        ]
# Weight_Sum = 0                        #权值和初始化
# for r in TRA_ROIS:
#     Weight_Sum += r[4] #计算权值和

# OBS_ROI = [(30 , 10, 100, 100)] #避障模式ROI区域

# #通信数据包封装
# Run     = bytearray([0x40,0x4F,0x4D,0x56,0x31,0x26,0x2F])# @OMV1/ #  直行
# Left    = bytearray([0x40,0x4F,0x4D,0x56,0x32,0x26,0x2F])# @OMV2/ #  左转
# Right   = bytearray([0x40,0x4F,0x4D,0x56,0x33,0x26,0x2F])# @OMV3/ #  右转
# Stop    = bytearray([0x40,0x4F,0x4D,0x56,0x34,0x26,0x2F])# @OMV4/ #  停止

# # Fun1: 获得最大色块的位置索引函数
# # 输入: N个色块(blobs) 输出: N个色块中最大色块的索引(int i)
# def Find_MaxIndex(blobs):
#     maxb_index = 0                      # 最大色块索引初始化
#     max_pixels = 0                      # 最大像素值初始化
#     for i in range(len(blobs)):         # 对N个色块进行N次遍历
#         if blobs[i].pixels() > max_pixels: # 当某个色块像素大于最大值
#             max_pixels = blobs[i].pixels() # 更新最大像素
#             maxb_index = i                 # 更新最大索引

#     return  maxb_index

# def Recive_Data():
#     global OVSys_State
#     if uart.any():                  # 如果串口接收到数据
#         OVSys_State = int(uart.read())  # 将读到的数据强制转换为整数
#         print(OVSys_State)            # 用于串口通信调试

# # TFT-LCD初始化
# # lcd.init()                #lcd函数初始化
# # lcd.set_direction(1)      #设置LCD显示方向 0和2是竖屏 1和3是横屏

# #摄像头初始化
# sensor.reset()                     # 初始化相机传感器
# sensor.set_pixformat(sensor.RGB565)# 设置相机模块的像素模式 16 bits/像素 GRAY为8
# sensor.set_framesize(sensor.QQVGA)  # 设置相机模块的帧大小 320x240 (QQVGA 160x120)
# sensor.skip_frames(30)             # 跳过30帧 让相机图像在改变相机设置后稳定下来
# sensor.set_auto_gain(False)        # 关闭自动增益
# sensor.set_auto_whitebal(False)    # 关闭默认的白平衡

# #主函数
# while (1):
#     OVSys_State = 1
#     clock.tick()
#     if (OVSys_State == 1) or (OVSys_State == 3): # 循迹模式或者循迹避障模式
#             # Hor_servo.angle(0)            # 循迹模式舵机姿态调整
#             # Ver_servo.angle(90)           # 水平-10 垂直向下70°
#             sensor.set_pixformat(sensor.GRAYSCALE) # 循迹模式 设置摄像头为灰度图
#             TRA_img = sensor.snapshot().histeq() # 截一帧图像 加强对比度好分割
#             TRA_img.mean(1)
#             TRA_img.binary([(0,20)], invert=True) # 在其范围内设为255

#             # # 在LCD上打印帧率和OpenMV工作模式
#             # TRA_img.draw_string(0, 60, "OpenMv Mode: Tracking Mode")
#             # TRA_img.draw_string(0, 100, "FPS: %.2f"%(clock.fps()))

#             # 偏移角度计算
#             Centroid_Sum = 0    # 初始化质心和

#             for r in TRA_ROIS:  # 是ROI的元组
#                 # 找到视野中ROI区域的色块, merge=true: 将找到的图像区域合并，并返回code值表示合并颜色
#                 # 设置面积与像素数阈值，少之则不检测
#                 # blobs = list[blob], blob is a class
#                 blobs = TRA_img.find_blobs(TRA_TH, roi=r[0:4], pixels_threshold=100,
#                                            area_threshold=100, merge=True, margin=10)
#                 if blobs: # 如果找到了多个色块 计算质心和
#                     maxb_index = Find_MaxIndex(blobs) # 找到多个色块中的最大色块返回索引值
#                     # .rect() 返回最大色块外框元组(x,y,w,h) 绘制线宽为2的矩形框 不填充矩形
#                     TRA_img.draw_rectangle(blobs[maxb_index].rect(), color=[0, 255, 0],
#                                            thickness = 2, fill = False)
#                     # 最大色块的中心位置标记十字：.cx(), .cy()返回质心坐标
#                     TRA_img.draw_cross(blobs[maxb_index].cx(), blobs[maxb_index].cy())
#                     # 计算质心和 = (ROI中最大颜色块的中心点横坐标)cx * (ROI权值)w
#                     Centroid_Sum += blobs[maxb_index].cx() * r[4]

#             # 中间公式: 确定线心位置 = 质心和/权值和
#             Center_Pos = (Centroid_Sum / Weight_Sum)
#             Deflection_Angle = 0 # 需要将线心Center_Pos转换为偏角 偏角初始化为0

#             # 与帧中间作比，其与帧格式设置相关
#             # 前进方向作为x轴，左偏为正
#             Deflection_Angle = -math.atan((Center_Pos - 80)/ 60) # 计算偏角

#             Deflection_Angle = math.degrees(Deflection_Angle) # 弧度值转换为角度
#             # 当偏角大于偏角阈值 且小于最大偏角 和 STM32通信 小车左转
#             Angle_err = Deflection_Angle
#             if abs(Deflection_Angle) > TRA_AngTH:
#                 if Deflection_Angle>0:
#                         uart.write(Right) # 和STM32通信 小车左转
#                         # time.sleep(10)
#                         TRA_img.draw_string(0, 0, "Car: Right")
#                         print("Turn Right") # 用于程序终端调试
#                         print("Turn Angle: %f" %(Deflection_Angle))

#                 # 当偏角小于负的偏角阈值 且大于最小偏角
#                 if Deflection_Angle < 0:
#                         uart.write(Left) # 和STM32通信 小车右转
#                         # time.sleep(10)
#                         TRA_img.draw_string(0,0,"Car: Left")
#                         print("Turn ：Left")#用于程序终端调试
#                         print("Turn Angle: %f" %(Deflection_Angle))

#             # 当小车角度绝对值小于阈值
#             elif abs(Deflection_Angle) <= TRA_AngTH:
#                 uart.write(Run)#和STM32通信 小车直行
#                 TRA_img.draw_string(0, 0, "Car: Run")
#                 print("Run")#用于程序终端调试
#                 print("Turn Angle: %f" %(Deflection_Angle))

#     # time.sleep(1)
#             # lcd.display(TRA_img)#在LCD上显示img图像



# SECOND EXAMPLE

# Error； rho and theta can't be declared about their axes
# THRESHOLD = (21, 0, -77, 5, -110, 127)
# import sensor, image, time
# from pyb import UART

# uart = UART(3,15200)        # 创建一个UART对象uart，用于串口通信，波特率为19200。
# sensor.reset()              # 重置图像传感器
# sensor.set_pixformat(sensor.RGB565) # 设置图像传感器的像素格式为RGB565。
# sensor.set_framesize(sensor.QVGA) # 设置图像传感器的帧大小为QQQVGA。
# sensor.skip_frames(time = 2000)     # 跳过2000毫秒的图像帧，使传感器稳定。
# clock = time.clock()                # 创建一个时间对象clock，用于计时。

# while(True):
#     clock.tick() #记录当前时间。
#     img = sensor.snapshot().binary([THRESHOLD])

#     # return image.line; True use Theil-Sen Median
#     line = img.get_regression([(100,100)], robust = True)

#     if (line):
#         # rho() 极坐标系下直线到原点的垂直距离
#         rho_err = (abs(line.rho()) - img.width())/2

#         theta_err = line.theta()
#         if theta_err < -90:
#             theta_err = theta_err + 180

#         img.draw_line(line.line(), color = 127)
#         print(rho_err, line.magnitude(), theta_err)
#         pass



# THIRD EXAMPLE

# Error: only when openmv lower, can it find black line, but can't find two together
# import sensor, image, math, time
# from pyb import UART
# import ustruct
# from image import SEARCH_EX, SEARCH_DS

# sensor.set_contrast(1) # set the contrast of the sensor
# sensor.set_gainceiling(16) # 设置图像增益上限
# clock = time.clock()
# uart = UART(3,115200, bits=8, parity=None, stop=1, timeout_char = 1000)
# # 划分五个区域
# roi1 =     [(0, 40, 20, 40),     #  x y w h 分别对应x、y坐标，框的宽度、高度
#             (35, 40, 20, 40),
#             (70, 40, 10, 10),
#             (105, 40, 20, 40),
#             (140, 40, 20, 40)]

# sensor.reset()
# sensor.set_pixformat(sensor.RGB565)
# sensor.set_framesize(sensor.QQVGA)
# sensor.skip_frames(time=2000)
# sensor.set_auto_whitebal(True)
# sensor.set_auto_gain(False)
# clock = time.clock()

# GRAYSCALE_THRESHOLD = [(20,100)]
# low_threshold = (0, 50)

# while(True):
#     clock.tick()
#     blob1=None
#     blob2=None
#     blob4=None
#     blob5=None
#     cx=cy=cw=i=0
#     # 矫正以消除鱼眼效应，（程度，缩放）
#     img = sensor.snapshot().lens_corr(strength = 1.7 , zoom = 1.0)
#     img.mean(2)
#     binary_img = img.binary(GRAYSCALE_THRESHOLD)
#     blob1 = binary_img.find_blobs([low_threshold], roi=roi1[0])
#     blob2 = binary_img.find_blobs([low_threshold], roi=roi1[1])
#     blob3 = binary_img.find_blobs([low_threshold], roi=roi1[2])
#     blob4 = binary_img.find_blobs([low_threshold], roi=roi1[3])
#     blob5 = binary_img.find_blobs([low_threshold], roi=roi1[4])

#     if blob1:
#         cx = 1
#     if blob2:
#         cy = 1
#     if blob3:
#         cw = 1
#     if blob4:
#         ch = 1
#     if blob5:
#         ci = 1

#     # FH = bytearray([0x2C, 0x12, cx, cy, ch, ci, 0x5B])
#     # uart.write(FH)
#     for rec in roi1:
#         img.draw_rectangle(rec, color=(255,0,0))#绘制出roi区域


# FOUR EXAMPLE

# import sensor, image, time
# from pyb import UART
# import json
# sensor.reset()
# sensor.set_pixformat(sensor.RGB565)
# sensor.set_framesize(sensor.QVGA)
# sensor.skip_frames(time = 200)
# sensor.set_auto_gain(False)
# sensor.set_auto_whitebal(False)
# clock = time.clock()
# uart = UART(3, 115200)
# control_val=0
# Roi1=[40,100,240,30]
# Roi2=[0,190,320,30]
# aaa=55
# while(True):
#     x1=0;
#     x2=0;
#     ans_l_1=0
#     ans_l_1_id=-2
#     ans_l_2=0
#     ans_l_2_id=-2
#     img = sensor.snapshot(1.8)
#     blobs1 = img.find_blobs([(0, 48, -8, 24, 4, 127)],roi=Roi1,pixels_threshold = 24,area_threshold = 5,merge = True)
#     blobs2 = img.find_blobs([(0, 48, -8, 24, 4, 127)],roi=Roi2,pixels_threshold = 24,area_threshold = 5,merge = True)
#     img.draw_rectangle(Roi1, color=[255,0,0])
#     img.draw_rectangle(Roi2, color=[255,0,0])
#     for b in blobs1:
#         if ans_l_1<b.pixels():
#             ans_l_1=b.pixels()
#             ans_l_1_id=b

#     for b in blobs2:
#         if ans_l_2<b.pixels():
#             ans_l_2=b.pixels()
#             ans_l_2_id=b


#     if ans_l_1 > 0:
#         x = ans_l_1_id[0]
#         y = ans_l_1_id[1]
#         width = ans_l_1_id[2]
#         height = ans_l_1_id[3]
#         img.draw_rectangle([x,y,width,height], color=[255,0,0])
#         img.draw_cross(ans_l_1_id[5], ans_l_1_id[6], color=[255,0,0])
#         x1=ans_l_1_id[5]
#     if ans_l_2 > 0:
#         x = ans_l_2_id[0]
#         y = ans_l_2_id[1]
#         width = ans_l_2_id[2]
#         height = ans_l_2_id[3]
#         img.draw_rectangle([x,y,width,height], color=[255,0,0])
#         img.draw_cross(ans_l_2_id[5], ans_l_2_id[6], color=[255,0,0])
#         x2=ans_l_2_id[5]
#     uart.write(aaa.to_bytes(1,'int')+x1.to_bytes(2,'int')+x2.to_bytes(2,'int'))
#     print(x1,x2)




