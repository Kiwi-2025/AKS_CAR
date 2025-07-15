//
// Created by ROG on 25-7-7.
//

#ifndef CONTROL_H
#define CONTROL_H

# include "usart.h"
# include "main.h"
# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

/* 外部变量 --------------------------------------------------------------------------------*/
extern float left_front_target, right_front_target, left_back_target, right_back_target;
extern char msg[1024]; // 用于存储发送到蓝牙的数据
extern float FLP, LLP;
/* some useful constants------------------------------------------------------------------*/
// TODO : 需要根据实际情况调整以下参数,确定后请打✔
# define  half_width        79.5             // 半左右轮距✔
# define  omega_eps         0.0f             // 角速度容忍度
# define  lin_vel_eps       50.0f           // 线性速度容忍度

/* 控制函数 --------------------------------------------------------------------------------*/
double roundPID(double targetposition, double position, double *error); //寻线误差的PID计算函数
void set_speed(float x_error, float y_error); //根据运动需求设置四轮转速
void groundturn(short spin_dir, short angle); //原地旋转固定角度
void alongside(double deviation); //巡线总函数
void avoidance(); //避障总函数
void fetchplace(); //自动取物放物控制函数
void avoid_stop();

void motor_pid_control(void);

/* 远程调参函数 --------------------------------------------------------------------------------*/
void set_parameters(char *input); //设置参数函数
/* 测试用函数 ------------------------------------------------------------------------------*/
void spin(void); //原地旋转运动函数
void move_forward(void); //前后运动函数
#endif //CONTROL_H
