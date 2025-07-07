//
// Created by ROG on 25-7-7.
//
# include "control.h"
# include "motor.h"
extern float velocity_msg_test; // 用于测试的速度消息变量

void control_test(void) {
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