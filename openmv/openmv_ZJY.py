# Untitled - By: lenovo - 周二 7月 19 2022

import sensor, image, time, math, pyb
from pyb import LED, UART, Servo, Pin, Timer
import json
import ustruct

uart = UART(3,115200)   # 定义串口3变量, timeout_char=1000
uart.init(115200, bits=8, parity=None, stop=1) # init with given parameters
# 无校验，8位数据位，1位停止位
rx_buff=[] # 串口接收缓冲区
state = 0 # 串口接收状态
MODE=0 # 状态变量
rx_buff1=[] # 串口接收缓冲区1
state1 = 0 # 串口接收状态1
is_fetch1=0 # 标志变量
red_led = LED(1) # 板载LED灯
led1=pyb.LED(1)
led2=pyb.LED(2)
led3=pyb.LED(3)

# 串口接收函数1
def Receive_Prepare1(data):
    global state
    global MODE
    if state==0:
        if data == 0xb3:
            state = 1
        else:
            state = 0
            rx_buff.clear()
    elif state==1:
        MODE=data
        print(MODE)
        state = 2
    elif state == 2:
        if data == 0xb4:
            state = 3
    else:
        state = 0
        rx_buff.clear()

def Receive_Prepare2(data1):
    global state1
    global is_fetch1
    if state1==0:
         if data1 == 0xa0:#帧头0xb3
             state1 = 1
         else:
             state1 = 0
             rx_buff1.clear()
    elif state1==1:
         is_fetch1=data1
         print(is_fetch1)
         state1 = 2
    elif state1 == 2:
         if data1 == 0xa1:#0xb4
            state1 = 3
         else:
            state1 = 0
            rx_buff1.clear()

mode=2
while(True):
    red_led.on()
    if MODE==0:   # 等待stm32发送指令
        if(uart.any()>0):
            c=uart.readchar()
            Receive_Prepare1(c)
            print(MODE)
    if MODE==1:    #循迹部分
        print('MODE==1')
        if mode==2:
            sensor.reset() # Initialize the camera sensor.
            sensor.set_pixformat(sensor.RGB565)
            sensor.set_framesize(sensor.QQVGA)
            sensor.skip_frames(10)
            sensor.set_auto_gain(False) # 颜色跟踪必须关闭自动增益
            sensor.set_auto_whitebal(False) # 颜色跟踪必须关闭白平衡
            clock = time.clock()
            #a=0
            #串口接收函数2
            #rx_buff=[

            while(mode==2):
                def find_max(blobs):
                    max_size=0
                    for blob in blobs:
                        if blob.pixels() > max_size:
                            max_blob=blob
                            max_size = blob.pixels()
                    return max_blob #寻找最大色块并返回最大色块的坐标
                img = sensor.snapshot().lens_corr(strength = 1.8, zoom = 1.0)
                wuti_threshold = (0,100,9,127,0,127)#(67, 100, -47, -5, 16, 127)#(58, 100, -66, -8, 18, 127)#(74, 80, -26, 40, 11, 28)#(68, 100, -13, 0, 16, 33)#
                blobs = img.find_blobs([wuti_threshold])
                if blobs:
                    max_blob=find_max(blobs)
                    img.draw_rectangle(max_blob.rect())#框选最大色块
                    img.draw_cross(max_blob.cx(), max_blob.cy())#在最大色块中心画十字
                    pcx = max_blob.cx()-79.5 #定义pcx为最大色块中心的横坐标
                    pcy = 119-max_blob.cy()
                    if pcy>90:
                        dev_s1=90
                    else:
                        dev_s1=pcy
                    if pcx<0:
                        cy1=1
                    else:
                        cy1=0
                    cw1=int(abs(pcx))
                    cx1=int(abs(pcx*10))%10
                    ch1=int(dev_s1)
                    OUT_DATA1 =bytearray([0x2C,0x12,cy1,cw1,cx1,ch1,0x5B])
                    uart.write(OUT_DATA1)
                    #print(pcx)
                    print(pcy)
                else:
                    OUT_DATA2 =bytearray([0x2C,0x12,0,9,9,90,0x5B])#原地缓慢旋转
                    #print(OUT_DATA2)
                    uart.write(OUT_DATA2)
                    #print('not found!')

                if(uart.any()>0):
                    cc=uart.readchar()
                    Receive_Prepare2(cc)
                if is_fetch1==1:
                    mode=6
                    print('mode=6')

        if mode==3:
            LED(1).on()
            LED(2).on()
            LED(3).on()
            s1 = Servo(1) # P7
            s1.angle(-30)
            sensor.reset()                      # Reset and initialize the sensor.
            sensor.set_pixformat(sensor.GRAYSCALE) # Set pixel format to RGB565 (or GRAYSCALE)
            sensor.set_framesize(sensor.QQVGA)   # Set frame size to QVGA (320x240)
            THRESHOLD = (80, 255)
            sensor.skip_frames(time = 20)     # Wait for settings take effect.
            clock = time.clock()                # Create a clock object to track the FPS.
            comp=0
            startsym=1
            dist=0
            chan=0
            mark1=0
            mark2=0
            #mark3=0

            while(mode==3):
                for i in range(100):
                    clock.tick()
                    img = sensor.snapshot().lens_corr(strength = 1.8, zoom = 1.0)
                    img.binary([THRESHOLD],True)
                    r = (pyb.rng() % 127) + 128
                    g = (pyb.rng() % 127) + 128
                    b = (pyb.rng() % 127) + 128
                    matrix=[0 for k in range(160)]
                    k=0
                    for x in range(160):
                        if img.get_pixel(x, 119):
                            matrix[k]=x
                            k+=1
                    num=0
                    cnt=0
                    P=[]
                    for j in range(1,159):
                        if matrix[j]!=0 and matrix[j]==matrix[j-1]+1:
                            cnt+=1
                            if matrix[j+1]!=matrix[j]+1 and matrix[j+1]!=0:
                                num+=1
                                pausew=j
                                restaw=j+1
                                P.append(j)
                            elif matrix[j+1]==0:
                                num+=0
                                finishw=j
                    dev=0
                    a=0
                    ercorr=0
                    if cnt>70:
                        mark1=1
                        print('mark1=1')
                    elif cnt<20 and mark1==1:
                        mark2=1
                        print('mark2=1')
                        mark1=0
                    #elif cnt<3 and mark3==1:
                        #mark3=mark3+1
                    elif cnt==0:
                        if a!=1:
                            if comp<0 and comp!=-90:
                                dev=35.1
                            elif comp>0 and comp!=90:
                                dev=-35.1
                            elif  comp==90:
                                if a!=3:
                                    dev=-35.1
                                elif a==3 and ercorr<3:
                                    dev=35.1
                                    ercorr+=1
                                else:
                                    dev=-35.1
                                    ercorr=-3
                            else:
                                if a!=3:
                                    dev=35.1
                                elif a==3 and ercorr<3:
                                    dev=-35.1
                                    ercorr+=1
                                else:
                                    dev=35.1
                                    ercorr=-3

                        elif a==1:
                            chan=1
                            dev=0
                        a=3
                    elif cnt!=0 and num==0:
                        a=2
                        devp=(matrix[0]+matrix[finishw])/2-79.5
                        if startsym!=0:
                            if devp>0:
                                dev=-10.1
                            else:
                                dev=10.1
                        elif devp-comp>35 and startsym==0:
                            dev=-35.1
                        elif comp-devp>35 and startsym==0:
                            dev=35.1
                        #elif devp-comp>0 and devp-comp<=35 and startsym==0:
                            #devf=devp-dist-15
                            #if devf<0:
                                #dev=-35.1
                            #else:
                                #dev=-10.1
                        #elif comp-devp>0 and comp-devp<=35 and startsym==0:
                            #devz=devp+dist+15
                            #if devz<0:
                                #dev=10.1
                            #else:
                                #dev=35.1
                    elif num==1:
                        a=1
                        #mark3=1
                        startsym=0
                        devcomp=(matrix[restaw]+matrix[pausew])/2-79.5
                        #distcomp=(matrix[restaw]-matrix[pausew])/2
                        if comp!=90 and comp!=-90:
                            dev=devcomp
                            #dist=distcomp
                        elif comp==90:
                            if devcomp>20:
                                dev=-35.1
                            else:
                                dev=devcomp
                                #dist=distcomp
                        else:
                            if devcomp<-20:
                                dev=35.1
                            else:
                                dev=devcomp
                                #dist=distcomp
                    elif num==2:
                        a=0
                        dev1=(matrix[P[0]+1]+matrix[P[0]])/2-79.5
                        dev2=(matrix[P[1]+1]+matrix[P[1]])/2-79.5
                        if comp!=90 and comp!=-90:
                            if dev1-comp<10:
                                dev=dev1
                            elif dev2-comp<10:
                                dev=dev2
                        elif comp==90:
                            if dev1<-35 or dev2<-35:
                                dev=35.1
                            else:
                                dev=min(dev1,dev2)
                        else:
                            if dev1>35 or dev2>35:
                                dev=-35.1
                            else:
                                dev=min(dev1,dev2)

                    else:
                        a=0
                        comp1=(matrix[P[0]+1]+matrix[P[0]])/2-79.5
                        comp2=(matrix[P[2]+1]+matrix[P[2]])/2-79.5
                        if abs(comp1)<abs(comp2):
                            dev=comp1
                        else:
                            dev=comp2
                    if dev!=35.1 and dev!=-35.1 and dev!=10.1 and dev!=-10.1:
                        comp=dev
                    elif dev==35.1 or dev==10.1:
                        comp=90
                    else:
                        comp=-90
                    if mark2==1 or chan==1:
                        dev=0
                        #s1 = Servo(1) # P7
                        #s2 = Servo(2) # P8
                        mode=5
                        print('mode=5')
                        for b in range(3):
                            OUT_DATA3 =bytearray([0x2C,0x12,2,1,5,0,0x5B])#改变模式5
                            uart.write(OUT_DATA3)
                            pyb.delay(10)
                    x=round(dev+79.5)
                    img.draw_cross(x, 119, color = (r, g, b), size = 5, thickness = 2)
                    if dev<0:
                        sgn=1
                    else:
                        sgn=0
                    dec=int(abs(dev)*10)%10
                    bit=int(abs(dev))
                    OUT_DATA4 =bytearray([0x2C,0x12,3,sgn,bit,dec,0x5B])
                    uart.write(OUT_DATA4)
                    #print(OUT_DATA)
                    #time.sleep_ms(50)
                    print(OUT_DATA4)


        if mode==5:
            dev=0
            sensor.reset()
            sensor.set_pixformat(sensor.GRAYSCALE)
            sensor.set_framesize(sensor.QQVGA)
            THRESHOLD = (80, 255)
            sensor.skip_frames(time = 20)
            clock = time.clock()
            while(mode==5):
                clock.tick()
                img = sensor.snapshot().lens_corr(strength = 1.8, zoom = 1.0)
                img.binary([THRESHOLD],True)
                r = (pyb.rng() % 127) + 128
                g = (pyb.rng() % 127) + 128
                b = (pyb.rng() % 127) + 128
                lenn=0
                midd=79.5
                endy=119
                a=0
                bb=0
                c=0
                d=0
                for y in range(99):
                    mark=[]
                    number=0
                    for x in range(20,140):
                        if img.get_pixel(x,119-y)!=img.get_pixel(x-1, 119-y):
                            mark.append(x)
                            number+=1
                    if number==4:
                        a=1
                        leng=mark[-1]-mark[0]
                        mid=(mark[0]+mark[-1])/2
                    elif number==2:
                        bb=1
                        leng=mark[-1]-mark[0]
                        mid=(mark[0]+mark[-1])/2
                    elif number==3:
                        c=1
                        if img.get_pixel(0,119-y):
                            leng=2*mark[2]-mark[0]-mark[1]
                            mid=(mark[0]+mark[1])/2
                        else:
                            leng=mark[1]+mark[2]-2*mark[0]
                            mid=(mark[2]+mark[1])/2
                    else:
                        leng=0
                        mid=79.5
                    if leng>lenn:
                        lenn=leng
                        endy=y
                        midd=mid
                dev=midd-79.5
                dev_s=endy

                #print(clock.fps())
                img.draw_cross(int(midd), int(119-endy), color = (r, g, b), size = 5, thickness = 2)
                if a==1 and bb==1:
                    print(dev)
                    print(dev_s)
                    if dev_s>90:
                        dev_s=90
                    if dev<0:
                        cy1=1
                    else:
                        cy1=0
                    cw1=int(abs(dev))
                    cx1=int(abs(dev*10))%10
                    ch1=int(dev_s)
                    OUT_DATA5 =bytearray([0x2C,0x12,cy1,cw1,cx1,ch1,0x5B])
                    uart.write(OUT_DATA5)
                else:
                    print('not')
                    OUT_DATA51 =bytearray([0x2C,0x12,0,0,0,90,0x5B])
                    uart.write(OUT_DATA51)

        if mode==6:
            LED(1).on()
            LED(2).on()
            LED(3).on()

            sensor.reset()                      # Reset and initialize the sensor.
            sensor.set_pixformat(sensor.GRAYSCALE) # Set pixel format to RGB565 (or GRAYSCALE)
            sensor.set_framesize(sensor.QQVGA)   # Set frame size to QVGA (320x240)
            THRESHOLD = (80, 255)
            sensor.skip_frames(time = 20)     # Wait for settings take effect.
            clock = time.clock()                # Create a clock object to track the FPS.
            mark=0
            a=0
            is_three=0
            while(mode==6):
                img = sensor.snapshot().lens_corr(strength = 1.8, zoom = 1.0)
                clock.tick()
                img.binary([THRESHOLD],True)

                matrix=[0 for k in range(160)]
                k=0
                for x in range(160):
                    if img.get_pixel(x, 119):
                        matrix[k]=x
                        k+=1
                cnt=0
                num=0
                sinflag=0
                for j in range(1,159):
                    if matrix[j]!=0 and matrix[j]==matrix[j-1]+1:
                        cnt+=1
                        if matrix[j+1]!=matrix[j]+1 and matrix[j+1]!=0:
                            num+=1
                        elif matrix[j+1]==0:
                            num+=0
                            finishw=j
                        else:
                            finishw=159
                if cnt>70:
                    mark=1
                if cnt!=0 and num==0:
                    if matrix[0]>1 and matrix[finishw]<158:
                        sinflag=1
                    else:
                        sinflag=0
                if cnt<40 and num==1 and mark==1:
                    mark+=1
                elif cnt<20 and sinflag==1 and mark==1:
                    mark+=1
                #sum=0
                #number=0
                if mark==2:
                    mode=3
                    b=0
                    for b in range (5):
                        OUT_DATA6 =bytearray([0x2C,0x12,2,1,3,0,0x5B])#改变模式3
                        uart.write(OUT_DATA6)
                        #b+=1
                        pyb.delay(20)
                    print('mode=3')
                else:
                    print('NOT')
                    OUT_DATA61=bytearray([0x2C,0x12,0,0,0,90,0x5B])
                    uart.write(OUT_DATA61)
    elif MODE==2:    #避障部分
        if mode==2:
            LED(1).on()
            LED(2).on()
            LED(3).on()

            sensor.reset() # Initialize the camera sensor.
            sensor.set_pixformat(sensor.RGB565)
            sensor.set_framesize(sensor.QQVGA)
            sensor.skip_frames(10)
            #sensor.set_auto_whitebal(False)
            clock = time.clock()

            #串口接收函数2
            rx_buff=[]
            def Receive_Prepare2(data):
                global state
                global is_fetch
                if state==0:
                    if data == 0xaa:#帧头0xb3
                        state = 1
                    else:
                        state = 0
                        rx_buff.clear()
                elif state==1:
                    is_fetch=data
                    print(is_fetch)
                    state = 2
                elif state == 2:
                    if data == 0xdd:#0xb4
                        state = 3
                else:
                    state = 0
                    rx_buff.clear()

            while(mode==2):
                def find_max(blobs):
                    max_size=0
                    for blob in blobs:
                        if blob.pixels() > max_size:
                            max_blob=blob
                            max_size = blob.pixels()
                    return max_blob #寻找最大色块并返回最大色块的坐标

                img = sensor.snapshot()
                red_threshold = (0,100,9,127,0,127)#(67, 100, -47, -5, 16, 127)#(58, 100, -66, -8, 18, 127)#(74, 80, -26, 40, 11, 28)#(68, 100, -13, 0, 16, 33)#
                blobs = img.find_blobs([red_threshold])
                if blobs:
                    max_blob=find_max(blobs)
                    img.draw_rectangle(max_blob.rect())#框选最大色块
                    img.draw_cross(max_blob.cx(), max_blob.cy())#在最大色块中心画十字
                    pcx = max_blob.cx()-79.5#定义pcx为最大色块中心的横坐标
                    pcy=119-max_blob.cy()
                    if pcy>90:
                        pcy=90
                    if pcx<0:
                        cy1=1
                    else:
                        cy1=0
                    cw1=int(abs(pcx))
                    cx1=int(abs(pcx*10))%10
                    ch1=int(pcy)
                    OUT_DATA7 =bytearray([0x2C,0x12,cy1,cw1,cx1,ch1,0x5B])
                    uart.write(OUT_DATA7)
                    print(pcx)
                else:
                    OUT_DATA8 =bytearray([0x2C,0x12,0,9,9,50,0x5B])#原地缓慢旋转
                    uart.write(OUT_DATA8)
                    print('not found!')
                state = 0
                is_fetch=0
                if(uart.any()>0):
                    c=uart.readchar()
                    Receive_Prepare2(c)
                if(is_fetch==1):
                    mode=6
        if mode==4:
            LED(1).on()
            LED(2).on()
            LED(3).on()

            sensor.reset()                      # Reset and initialize the sensor.
            sensor.set_pixformat(sensor.GRAYSCALE) # Set pixel format to RGB565 (or GRAYSCALE)
            sensor.set_framesize(sensor.QQVGA)   # Set frame size to QVGA (320x240)
            THRESHOLD = (80, 255)
            sensor.skip_frames(time = 2000)     # Wait for settings take effect.
            clock = time.clock()                # Create a clock object to track the FPS.

            while(mode==4):
                img = sensor.snapshot().lens_corr(strength = 1.8, zoom = 1.0)
                clock.tick()
                img.binary([THRESHOLD],True)
                matrix=[0 for k in range(160)]
                k=0
                for x in range(160):
                    if img.get_pixel(x, 119):
                        matrix[k]=x
                        k+=1
                cnt=0
                for j in range(1,159):
                    if matrix[j]!=0 and matrix[j]==matrix[j-1]+1:
                        cnt+=1
                if cnt>150:
                    #s1 = Servo(1) # P7
                    #s2 = Servo(2) # P8
                    #s1.angle(i)
                    mode=5
                    OUT_DATA12 =bytearray([0x2C,0x12,2,2,5,0,0x5B])#改变模式3
                    uart.write(OUT_DATA12)

        if mode==5:
            dev=0
            sensor.reset()
            sensor.set_pixformat(sensor.GRAYSCALE) # grayscale is faster
            sensor.set_framesize(sensor.QQVGA)
            sensor.skip_frames(time = 20)
            THRESHOLD = (60, 255)
            clock = time.clock()
            r = (pyb.rng() % 127) + 128
            g = (pyb.rng() % 127) + 128
            b = (pyb.rng() % 127) + 128

            while(mode==5):
                clock.tick()
                img = sensor.snapshot().lens_corr(strength = 1.8, zoom = 1.0)
                img.binary([THRESHOLD],True)
                lenn=0
                midd=79.5
                endy=119
                a=0
                bb=0
                c=0
                d=0
                for y in range(99):
                    mark=[]
                    number=0
                    for x in range(20,140):
                        if img.get_pixel(x,119-y)!=img.get_pixel(x-1, 119-y):
                            mark.append(x)
                            number+=1
                    if number==4:
                        a=1
                        leng=mark[-1]-mark[0]
                        mid=(mark[0]+mark[-1])/2
                    elif number==2:
                        bb=1
                        leng=mark[-1]-mark[0]
                        mid=(mark[0]+mark[-1])/2
                    elif number==3:
                        c=1
                        if img.get_pixel(0,119-y):
                            leng=2*mark[2]-mark[0]-mark[1]
                            mid=(mark[0]+mark[1])/2
                        else:
                            leng=mark[1]+mark[2]-2*mark[0]
                            mid=(mark[2]+mark[1])/2
                    else:
                        d=1
                        leng=0
                        mid=79.5
                    if leng>lenn:
                        lenn=leng
                        endy=y
                        midd=mid
                dev=midd-79.5
                dev_s=endy
                #print(dev)
                #print(dev_s)
                #print(clock.fps())
                img.draw_cross(int(midd), int(119-endy), color = (r, g, b), size = 5, thickness = 2)
                if a==1 and bb==1 and d==1:
                    if dev_s>90:
                        pcy=90
                    else:
                        pcy=dev_s
                    if dev<0:
                        cy1=1
                    else:
                        cy1=0
                    cw1=int(abs(dev))
                    cx1=int(abs(dev*10))%10
                    ch1=int(pcy)
                    OUT_DATA9 =bytearray([0x2C,0x12,cy1,cw1,cx1,ch1,0x5B])
                    uart.write(OUT_DATA9)

                else:
                    print('not')
                    OUT_DATA10 =bytearray([0x2C,0x12,0,9,9,50,0x5B])
                    uart.write(OUT_DATA10)
        if mode==6:
            enable_lens_corr = False
            sensor.reset()
            sensor.set_pixformat(sensor.RGB565)
            sensor.set_framesize(sensor.QQVGA)
            sensor.skip_frames(time = 20)
            clock = time.clock()

            min_degree = 89
            max_degree = 91

            while(mode==6):
                clock.tick()
                img = sensor.snapshot()
                if enable_lens_corr: img.lens_corr(1.8)
                if img.find_lines(threshold = 1000, theta_margin = 25, rho_margin = 25):
                    for l in img.find_lines(threshold = 1000, theta_margin = 25, rho_margin = 25):
                        if (min_degree <= l.theta()) and (l.theta() <= max_degree):
                            img.draw_line(l.line(), color = (255, 0, 0))
                            print(l.theta())
                            mode=4
                else:
                    print('not')
                    OUT_DATA11 =bytearray([0x2C,0x12,0,9,9,50,0x5B])
                    uart.write(OUT_DATA11)
