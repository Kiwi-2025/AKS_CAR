//
// Created by ROG on 25-7-7.
//
# include "control.h"
# include "motor.h"
#include <math.h>

extern float velocity_msg_test; // 用于测试的速度消息变量

void set_speed(float linear_velocity, float turn_angle_degrees) {
    float turn_angle_rad = turn_angle_degrees * pi / 180.0f;
    float curvature_radius;

    // 将转动角度转化为转弯半径
    if (fabs(turn_angle_rad) < 0.001f) {
        curvature_radius = 1e6f; // 近似直线
    } else {
        //curvature_radius = wheel_base  tanf(turn_angle_rad);
    }

    // 特殊情况：纯直线运动
    if (fabs(turn_angle_rad) < 0.001f) {
        left_front_target   =   linear_velocity;
        right_front_target  =   linear_velocity;
        left_back_target    =   linear_velocity;
        right_back_target   =   linear_velocity;
    }
    // 特殊情况：保持静止
    if (fabs(linear_velocity) < 0.001f) {
        // 静止状态
        left_front_target   =   0;
        right_front_target  =   0;
        left_back_target    =   0;
        right_back_target   =   0;
    }

    // 计算角速度 ω = v / R
    float angular_velocity = linear_velocity / curvature_radius;

    // 计算转向中心 (ICR - Instantaneous Center of Rotation)
    // 假设小车几何中心的速度方向沿 x 轴
    float icr_x = 0;
    float icr_y = curvature_radius;

    // 考虑转向方向的符号修正
    if (curvature_radius > 0) {
        // 左转：左侧轮子速度较小，右侧轮子速度较大
        // 速度符号已经通过几何计算自动处理
    } else {
        // 右转：右侧轮子速度较小，左侧轮子速度较大
        // 速度符号已经通过几何计算自动处理
    }
    motor_pid_control();
}

// 测试函数
void motor_pid_control(void) {
    // velocity_msg_test = read_rpm(); // 每次定时器溢出时读取一次转速

    left_front_feedback = read_left_front_feedback();
    right_front_feedback = read_right_front_feedback();
    left_back_feedback = read_left_back_feedback();
    right_back_feedback = read_right_back_feedback();
    //获取当前速度

    int left_front_diff = left_front_PID(left_front_target,left_front_feedback,&left_front_error);
    int right_front_diff = right_front_PID(right_front_target,right_front_feedback,&right_front_error);
    int left_back_diff = left_back_PID(left_back_target,left_back_feedback,&left_back_error);
    int right_back_diff = right_back_PID(right_back_target,right_back_feedback,&right_back_error);

    left_back_speed += left_back_diff;
    right_back_speed += right_back_diff;
    left_front_speed += left_front_diff;
    right_front_speed += right_front_diff;

    if(left_front_target == 0 && left_front_feedback ==0){left_front_speed = 0;}
    if(right_front_target == 0 && right_front_feedback ==0){right_front_speed = 0;}
    if(left_back_target == 0 && left_back_feedback ==0){left_back_speed = 0;}
    if(right_back_target == 0 && right_back_feedback ==0){right_back_speed = 0;}
    //将PID处理后的目标速度写入系统
    motor_vel(left_front_speed,right_front_speed,left_back_speed,right_back_speed);
}