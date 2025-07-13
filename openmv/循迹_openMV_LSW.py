
import sensor, image, time
from machine import UART
import json
import pyb, utime


sensor.reset() # Initialize the camera sensor.
sensor.set_pixformat(sensor.RGB565) # use RGB565.
sensor.set_framesize(sensor.QQVGA) # use QQVGA for speed.
sensor.skip_frames(10) # Let new settings take affect.
sensor.set_auto_whitebal(False) # turn this off.
clock = time.clock() # Tracks FPS.

# OpenMV4 H7 Plus, OpenMV4 H7, OpenMV3 M7, OpenMV2 M4 的UART(3)是P4-TX P5-RX
uart = UART(3, 115200)


##货物识别#########################################################################################

#找最大色块函数
def find_max(blobs):
    max_size=0
    for blob in blobs:
        if blob.pixels() > max_size:
            max_blob=blob
            max_size = blob.pixels()
    return max_blob

#识别货物函数
def huowu():
#    print('0')
    sensor.set_pixformat(sensor.RGB565)

    orange   = (0, 78, 14, 69, -120, 87)
    blue = (18, 70, 1, 119, -120, -14)
    green = (15, 68, -42, -15, 10, -81)

#    while(received_data==0):
    img = sensor.snapshot() # 截图，显示图像

    blobs_orange = img.find_blobs([orange])
    blobs_blue = img.find_blobs([blue])
    blobs_green = img.find_blobs([green])

    if blobs_orange:
            max_blob_orange=find_max(blobs_orange)
            img.draw_rectangle(max_blob_orange.rect())#画框
            img.draw_cross(max_blob_orange.cx(), max_blob_orange.cy())#画十字
            orange_xy = bytearray([0x2C,0x12,max_blob_orange.cx(),max_blob_orange.cy(),0x5B])
            uart.write(orange_xy)#发送货物的x和y坐标
            print(max_blob_orange.cx(),max_blob_orange.cy())

    else:
        orange_xy = bytearray([0x2C,0x12,200,200,0x5B])#没识别到就发送200,200
        0000000000
        print(200,200)

    if blobs_blue:
            max_blob_blue=find_max(blobs_blue)
            img.draw_rectangle(max_blob_blue.rect(),color=(255, 255, 0))#黄色画框
            img.draw_cross(max_blob_blue.cx(), max_blob_blue.cy(),color=(255, 255, 0))#画十字
#                print(max_blob_blue.cx(),max_blob_blue.cy())

    if blobs_green:
            max_blob_green=find_max(blobs_green)
            img.draw_rectangle(max_blob_green.rect(),color=(255, 0, 0))#红色画框
            img.draw_cross(max_blob_green.cx(), max_blob_green.cy(),color=(255, 0, 0))#画十字
#            print(max_blob_green.cx(),max_blob_green.cy())

##巡线###########################################################################################
GRAYSCALE_THRESHOLD = [(0, 93)]
#ROIS = [ # [ROI, weight]
#        (0, 80, 80, 30, 13),#从近到远 ,左边
#        (0, 50, 80, 30, 7),
#        (0, 20, 80, 30, 3),
#        (80, 80, 80, 40, 13),#从近到远 ,右边
#        (80, 50, 80, 30, 7),
#        (80, 20, 80, 30, 3)
#       ]
#ROIS_PART1 = ROIS[:3]  # 左
#ROIS_PART2 = ROIS[3:]  # 右
x_diff=30


#储存x坐标的函数
def process_rois(img, rois):
    x_coords = {}
    for r in rois:
        blobs = img.find_blobs(GRAYSCALE_THRESHOLD, roi=r[0:4], x_stride=2, y_stride=2, merge=False)

        if blobs:
            for blob in blobs:
#                img.draw_rectangle(blob.rect())#画框
#                img.draw_cross(blob.cx(), blob.cy())#画十字
                x_coords[r] = blob.cx()
        else:
            x_coords[r]=210#没有检测到色块
    return x_coords

#求权重x坐标
def calculate_x_ave(x_coords, rois):
    x_sum =0
    weight=0
    weight_sum=0
    n=0
    current_x=0
    next_x=0

    for i in range(len(rois)):
        if x_coords[rois[i]]==210:
            if i==len(rois)-1:
                if rois[0][0]==0:
                    return 160
                elif rois[0][0]==80:
                    return 0
            continue
        else:
            current_x = x_coords[rois[i]] #if rois[0] in x_coords else 0
            if i==len(rois)-1:
                x_sum=current_x
                weight_sum=1
            n=i
            break
#    current_x = x_coords[rois[0]] #if rois[0] in x_coords else 0
    for i in range(n,len(rois) - 1):
        next_x = x_coords[rois[i + 1]] #if rois[i + 1] in x_coords else 0
        if next_x==210:#下一格没色块，跳过
            if i==len(rois) - 2:
                x_sum=current_x
                weight_sum=1
            continue
        else:
            if abs(next_x - current_x) < x_diff:
                weight = rois[i][4]
                x_sum += current_x * weight
                weight_sum += weight
                current_x = next_x
                if i==len(rois) - 2:
                    weight = rois[i+1][4]
                    x_sum += current_x * weight
                    weight_sum += weight
            else:#相差太大，跳过
                if i==len(rois) - 2:
                    weight = rois[i][4]
                    x_sum += current_x * weight
                    weight_sum += weight
                continue

#    if weight_sum == 0:#全都没色块，全都被跳过了
#        if rois[0][0]==0:
#            return 160
#        elif rois[0][0]==80:
#            return 0

    x_ave = x_sum / weight_sum

    return x_ave


#终点判断
def zhongdian(img):
    flag = 0
    ZHONGDAIN=[(0,66)]
    blobs = img.find_blobs(ZHONGDAIN, x_stride=20, merge=False)
    if blobs:
        blob = find_max(blobs)

        if blob:

                width = blob.w()
                angle = blob.rotation()
                # 根据宽度和倾斜角度判断是否为可能的横线
                if width > 100 and abs(angle) < 4 and blob.cy()>80 and blob.h()<40 :
                    # 绘制可能的横线
                    img.draw_rectangle(blob.rect(), color=(0, 0, 0))
                    img.draw_cross(blob.cx(), blob.cy(), color=(0, 0, 0))
                    flag=1

                else:
                    flag=0
        else:
            flag=0
    else:
        pass

#    print(flag)
    return flag

flag2=0
#巡线函数
def xunxian():
    #print('11')
    sensor.set_pixformat(sensor.GRAYSCALE)

    global flag2

    ROIS = [ # [ROI, weight]
            (0, 80, 80, 30, 10),#从近到远 ,左边
            (0, 50, 80, 30, 7),
            (0, 20, 80, 30, 4),
            (80, 80, 80, 30, 10),#从近到远 ,右边
            (80, 50, 80, 30, 7),
            (80, 20, 80, 30, 4)
           ]
    ROIS_PART1 = ROIS[:3]  # 左
    ROIS_PART2 = ROIS[3:]  # 右


    x_center=0



#    while(received_data==1):
    img = sensor.snapshot().lens_corr(1.8)


    flag1=zhongdian(img)

    x_coords_left = process_rois(img, ROIS_PART1)
    x_ave_left = calculate_x_ave(x_coords_left, ROIS_PART1)

    x_coords_right = process_rois(img, ROIS_PART2)
    x_ave_right = calculate_x_ave(x_coords_right, ROIS_PART2)

    x_center=(x_ave_left+x_ave_right)/2
    x_center=int(x_center)#转化成整数

    if x_ave_left==160 and x_ave_right==0:
        x_center=210

    if x_center==0:
        x_center=210 #0xD2  #如果没找到黑线就发210


    if flag1==1:
        flag2=1 #如果找到终点横线就发1

    print(int(x_ave_left),int(x_ave_right),x_center,flag2)


    output_x_center = bytearray([0x2C,0x12,x_center,flag2,0x5B])#发送中点x和0
    uart.write(output_x_center)

##圆圈###################################################################################

def yuan():
    sensor.set_pixformat(sensor.GRAYSCALE)

    black=(51,0)

#    while(received_data==0):
    img = sensor.snapshot() # 截图，显示图像

    blobs_black = img.find_blobs([black],x_stride=5, y_stride=4)


    if blobs_black:
            max_blob_black=find_max(blobs_black)
            img.draw_rectangle(max_blob_black.rect())#画框
            img.draw_cross(max_blob_black.cx(), max_blob_black.cy())#画十字
            yuan_xy = bytearray([0x2C,0x12,max_blob_black.cx(),max_blob_black.cy(),0x5B])
            uart.write(yuan_xy)#发送货物的x和y坐标
            print(max_blob_black.cx(),max_blob_black.cy())

    else:
        orange_xy = bytearray([0x2C,0x12,220,220,0x5B])#没识别到就发送200,200
        uart.write(orange_xy)
        print(220,220)



###########################################################################################

x=1
received_data = 1
m = 0

while (True):


    if uart.any():
        m = uart.readchar()  #接收到的数据存进received_data
        if m==48: #0的ASCII码
            received_data=0
        elif m==49: #1的ASCII码
            received_data=1
        elif m==50: #2的ASCII码
            received_data=2


    # 为0时，识别货物
    if received_data == 0:
        x=2
#        print('0')
        huowu()

    # 为1时，巡线
    elif received_data == 1:
        x=2
#        print('1')
        xunxian()

    # 为2时，识别圆圈
    elif received_data == 2:
        x=2
#        print('2')
        yuan()

#    print(m)




#    end_time = utime.ticks_ms()  # 结束时间
#    time_diff = utime.ticks_diff(end_time, start_time)  # 计算时间差
#    print(time_diff)
