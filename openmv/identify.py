# identify - By: sky - Sat Jul 12 2025


import sensor, image, time, math, pyb

sensor.reset()                     # 初始化相机传感器
sensor.set_pixformat(sensor.RGB565)# 设置相机模块的像素模式 16 bits/像素 GRAY为8
sensor.set_framesize(sensor.QVGA) # 设置相机模块的帧大小 320x240 (QQVGA 160x120)
sensor.skip_frames(30)             # 跳过30帧 让相机图像在改变相机设置后稳定下来
sensor.set_auto_gain(False)        # 关闭自动增益
sensor.set_auto_whitebal(False)    # 关闭默认的白平衡


def Find_MaxIndex(blobs):
    maxb_index = 0                      # 最大色块索引初始化
    max_pixels = 0                      # 最大像素值初始化
    for i in range(len(blobs)):         # 对N个色块进行N次遍历
        if blobs[i].pixels() > max_pixels: # 当某个色块像素大于最大值
            max_pixels = blobs[i].pixels() # 更新最大像素
            maxb_index = i                 # 更新最大索引

    return  maxb_index



while True:
    TRA_img = sensor.snapshot().histeq() # 截一帧图像 加强对比度好分割
    TRA_img.mean(1) # 均值滤波, 1代表内核大小为（3x3）
    try:
        blobs = TRA_img.find_blobs([(35, 71, 22, 68, -30, 36)],
                                 pixels_threshold=24, area_threshold=8, merge=True)

        maxb_index = Find_MaxIndex(blobs) # 找到多个色块中的最大色块返回索引值

        x_center, y_center = blobs[maxb_index].cx(), blobs[maxb_index].cy()
        TRA_img.draw_circle(x_center, y_center, 10, color=[255, 0, 0], size=50)
        TRA_img.draw_cross(x_center, y_center, 100)
        print(x_center, y_center) # 绘制最大色块的外框矩形
    except Exception as e:
        print("Error:", e) # 捕获异常并打印错误信息
        continue
