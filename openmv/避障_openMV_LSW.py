#识别货物，发送货物中心坐标

import sensor, image, time,math
from machine import UART
import json

blue = (0, 35, -92, 127, -98, -11)
black_yz =(4, 21, -3, -128, -1, 21)# (4, 21, -3, -128, -23, -97)#(2, 21, -3, -128, 114, -128)
#dangban =(5, 18, -89, 127, -88, 0)
#hengxian=(0, 4, -4, -128, -82, 25)
#bai1 =(0, 100, -17, 12, 7, 35)

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
    orange   = (0, 78, 14, 69, -120, 87)

#    while(received_data==0):
    img = sensor.snapshot() # 截图，显示图像

    blobs_orange = img.find_blobs([orange])


    if blobs_orange:
            max_blob_orange=find_max(blobs_orange)
            img.draw_rectangle(max_blob_orange.rect())#画框
            img.draw_cross(max_blob_orange.cx(), max_blob_orange.cy())#画十字
            orange_xy = bytearray([0x2C,0x12,max_blob_orange.cx(),max_blob_orange.cy(),0x5B])
            uart.write(orange_xy)#发送货物的x和y坐标
            print(max_blob_orange.cx(),max_blob_orange.cy())

    else:
        orange_xy = bytearray([0x2C,0x12,200,200,0x5B])#没识别到就发送200,200
        uart.write(orange_xy)
        print(200,200)



##识别蓝色障碍################################################################################
ROIS=[ # [ROI]
      (0, 0, 40, 100),#左边
      (120, 0, 40, 100),#右
      (20,70,120,40)#危险区域
     ]

#计算面积
def area(img,r):
    blue_area=0
    blue_blobs=img.find_blobs([blue],roi=r,pixels_threshold=100 , merge=True)
    if blue_blobs:
        blue_blobs=find_max(blue_blobs)
        blue_area=blue_blobs.pixels()
#        img.draw_rectangle(blue_blobs.rect(), color=(0,0,0)) # 用黑色覆盖蓝色色块
    return blue_area

def weixian(img,r):
    blue_blobs=img.find_blobs([blue],roi=r,pixels_threshold=20 , merge=True)
    if blue_blobs:
        return 1
    else:
        return 0

def zhangai():
    img = sensor.snapshot() # 截图，显示图像
    left_area=0
    right_area=0
    left_area=area(img,ROIS[0])
    right_area=area(img,ROIS[1])
    a=0#哪边有空
    b=0#危险区域是否有蓝色
    b=weixian(img,ROIS[2])
    if left_area>right_area:
        print(b,1,left_area,right_area,)#右边空
        a=1
    else:
        print(b,0,left_area,right_area)#左边空
        a=0

    output = bytearray([0x2C,0x12,b,a,0x5B])
    uart.write(output)

##识别蓝色障碍并且找终点############################################################################
#flag_end = 0
def zhangai_and_zhongdian():
#    global flag_end
    flag_end = 0
    img = sensor.snapshot() # 截图，显示图像

    #################
   #找终点蓝色挡板
#    blobs_zhongdain = img.find_blobs([zhongdian],x_stride=100)
#    if blobs_zhongdain:
#        max_blob_zhongdian=find_max(blobs_zhongdain)
#        img.draw_rectangle(max_blob_zhongdian.rect(),color=(255, 255, 0))#黄色画框
#        img.draw_cross(max_blob_zhongdian.cx(), max_blob_zhongdian.cy(),color=(255, 255, 0))#画十字

#        if max_blob_zhongdian:
#            width = max_blob_zhongdian.w()
#            angle = max_blob_zhongdian.rotation()
#            cy =max_blob_zhongdian.cy()
#            height =max_blob_zhongdian.h()
#           # 根据宽度和倾斜角度判断是否为可能的横线
#            if width > 110 and cy>10 and height>10:
#               # 绘制可能的横线
#                img.draw_rectangle(max_blob_zhongdian.rect(), color=(255, 0, 0))
#                img.draw_cross(max_blob_zhongdian.cx(), max_blob_zhongdian.cy(),color=(255, 0, 0))
#                flag_end=1

#            else:
#                pass
#        else:
#            pass

    ################
    #找终点黑线
    blobs_black = img.find_blobs([black_yz],x_stride=20)
    if blobs_black:
        max_blob_black=find_max(blobs_black)
#        img.draw_rectangle(max_blob_black.rect(),color=(255, 255, 0))#黄色画框
#        img.draw_cross(max_blob_black.cx(), max_blob_black.cy(),color=(255, 255, 0))#画十字

        if max_blob_black:
            width = max_blob_black.w()
            angle = max_blob_black.rotation()
            # 根据宽度和倾斜角度判断是否为可能的横线
            if width > 80 and abs(angle) < 90 and max_blob_black.cy()>50 and max_blob_black.h()<70:
                # 绘制可能的横线
                img.draw_rectangle(max_blob_black.rect(), color=(255, 0, 0))
                img.draw_cross(max_blob_black.cx(), max_blob_black.cy(),color=(255, 0, 0))
                flag_end=1

            else:
                pass
        else:
            pass
    ##############


    #找蓝色色块
    left_area=0
    right_area=0
    left_area=area(img,ROIS[0])
    right_area=area(img,ROIS[1])
    a=0#哪边有空
    b=0#危险区域是否有蓝色
    b=weixian(img,ROIS[2])
    if left_area>right_area:
        #右边空
        a=1
    else:
       #左边空
        a=0
    if flag_end==1:
        a=2
        b=2

    print(b,a)
    output = bytearray([0x2C,0x12,b,a,0x5B])
    uart.write(output)


##调整黑线角度##########################################################################
def tiaozheng():
    sensor.set_pixformat(sensor.GRAYSCALE)
    hengxian=(0,63)
    flag=0
    img = sensor.snapshot() # 截图，显示图像
    blobs_black = img.find_blobs([hengxian],x_stride=20)
    if blobs_black:
        max_blob_black=find_max(blobs_black)
#        img.draw_rectangle(max_blob_black.rect(),color=(255, 255, 0))#黄色画框
#        img.draw_cross(max_blob_black.cx(), max_blob_black.cy(),color=(255, 255, 0))#画十字

        if max_blob_black:
            width = max_blob_black.w()
            height=max_blob_black.h()
            angle = max_blob_black.rotation()
            area =max_blob_black.pixels()
            angle = math.degrees(angle)
            print(angle)

#            # 检查斑块是否有清晰的边缘
#            gray_img = img.to_grayscale(copy=True) # 将图像转换为灰度图像
#            gray_img.laplacian(2, sharpen=True) # 锐化灰度图像
#            edges = gray_img.find_edges(image.EDGE_CANNY, threshold=(50, 80))
#            has_clear_edges = False

#            # 手动检查边缘点是否在斑块矩形内
#            for edge in edges:
#                for point in edge:
#                    x, y = point[0], point[1]
#                    if max_blob_black.rect()[0] <= x <= max_blob_black.rect()[0] + max_blob_black.rect()[2] and \
#                       max_blob_black.rect()[1] <= y <= max_blob_black.rect()[1] + max_blob_black.rect()[3]:
#                        has_clear_edges = True
#                        break
#                if has_clear_edges:
#                    break
#            # 根据宽度和倾斜角度判断是否为可能的横线
            if  150> angle>140 and  width>70 and height>60 and area<3000:
#            if  5> angle and angle>175 and  width>40 and height< 60:
                # 绘制可能的横线
                img.draw_rectangle(max_blob_black.rect(), color=(255, 255, 0))
                img.draw_cross(max_blob_black.cx(), max_blob_black.cy(),color=(255, 255, 0))
                flag=1

            else:
                pass
        else:
            pass
    if flag==1:
        print(4,4)
        output = bytearray([0x2C,0x12,4,4,0x5B])
        uart.write(output)
    else:
        print(5,5)
        output = bytearray([0x2C,0x12,5,5,0x5B])
        uart.write(output)


##找白块1#########################################################################
def no():
    bai2 = bytearray([0x2C,0x12,200,220,0x5B])
    uart.write(bai2)
    print(200,220)


def bai1():
    sensor.set_pixformat(sensor.RGB565)
    img = sensor.snapshot() # 截图，显示图像

    ROI=(75,30,30,30)
    bai1=(0, 100, -17, 12, -5, 18)
#    (0, 100, -17, 12, 7, 35)
    blobs_bai = img.find_blobs([bai1],roi=ROI,pixels_threshold=400)
    if blobs_bai:
        max_blob_bai=find_max(blobs_bai)
        if max_blob_bai:
            if max_blob_bai.pixels()>600:
                img.draw_rectangle(max_blob_bai.rect(),color=(255, 255, 0))#黄色画框
                img.draw_cross(max_blob_bai.cx(), max_blob_bai.cy(),color=(255, 255, 0))#画十字
                bai2 = bytearray([0x2C,0x12,3,3,0x5B])
                uart.write(bai2)
                print(3,3)
            else:
                no()#输出200,220
        else:
            no()
    else:
        no()

##找圆（黑块）向右走############################################################################

def yuan():
    img = sensor.snapshot() # 截图，显示图像

    blobs_black = img.find_blobs([black_yz],pixels_threshold=200,x_stride=5, y_stride=4)

    if blobs_black:#有黑色色块
        max_blob_black=find_max(blobs_black)
        angle =abs( max_blob_black.rotation())
        height=max_blob_black.h()
        width=max_blob_black.w()
        area=max_blob_black.pixels()
        if 50>height>20 and 60>width>20 and area>300 and angle<20:#有挡板，有黑色块，有圆
            img.draw_rectangle(max_blob_black.rect())#画框
            img.draw_cross(max_blob_black.cx(), max_blob_black.cy())#画十字
            yuan_xy = bytearray([0x2C,0x12,max_blob_black.cx(),max_blob_black.cy(),0x5B])
            uart.write(yuan_xy)
            print(max_blob_black.cx(),max_blob_black.cy())
        else:#有黑色块，没有圆
            yuan_xy = bytearray([0x2C,0x12,220,220,0x5B])
            uart.write(yuan_xy)
            print(220,220)
    else:#，没有黑色色块
        yuan_xy = bytearray([0x2C,0x12,220,220,0x5B])
        uart.write(yuan_xy)
        print(220,220)

##找白块2#########################################################################
def bai2():

    img = sensor.snapshot() # 截图，显示图像
    ROI=(55,30,30,30)
    bai=(0, 100, -17, 12, -5, 18)
    blobs_bai = img.find_blobs([bai],roi=ROI,pixels_threshold=400)
    if blobs_bai:
        max_blob_bai=find_max(blobs_bai)
        if max_blob_bai:
            if max_blob_bai.pixels()>600:
                img.draw_rectangle(max_blob_bai.rect(),color=(255, 255, 0))#黄色画框
                img.draw_cross(max_blob_bai.cx(), max_blob_bai.cy(),color=(255, 255, 0))#画十字
                bai = bytearray([0x2C,0x12,3,3,0x5B])
                uart.write(bai)
                print(3,3)
            else:
                no()#输出200,220
        else:
            no()
    else:
        no()



###########################################################################################

x=1
received_data = 0
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
        elif m==51: #3的ASCII码
            received_data=3
        elif m==52: #4的ASCII码
            received_data=4
        elif m==53: #5的ASCII码
            received_data=5
        elif m==54: #6的ASCII码
            received_data=6
        elif m==55: #6的ASCII码
            received_data=7

    # 为0时，识别货物
    if received_data == 0:
        print('0')
        huowu()

    # 为1时，识别蓝色障碍
    elif received_data == 1:
        print('1')
        zhangai()

    # 为2时，识别蓝色障碍并且找终点
    elif received_data == 2:
        print('2')
        zhangai_and_zhongdian()

    # 为7时，调整角度
    elif received_data == 7:
        print('7')
        tiaozheng()


    # 为3时，找白块右
    elif received_data == 3:
        print('3')
        bai1()

    # 为4时，找圆（黑块）
    elif received_data == 4:
        print('4')
        yuan()

    # 为5时，找白块左
    elif received_data == 5:
        print('5')
        bai2()



#    print(m)
