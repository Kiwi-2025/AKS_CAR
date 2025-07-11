//
// Created by ROG on 25-7-7.
//
# include "control.h"
// 根据运动需求设置四轮转速
void set_speed(float linear_velocity, float turn_angle_degrees) {
    float rounding_omega = turn_angle_degrees * KLP; // 将角度转换为弧度

    // 特殊情况：纯直线运动
    if (fabs(rounding_omega) < omega_eps) {
        left_front_target = linear_velocity;
        right_front_target = linear_velocity;
        left_back_target = linear_velocity;
        right_back_target = linear_velocity;
    }
    // 特殊情况：保持静止
    if (fabs(linear_velocity) < lin_vel_eps) {
        // 静止状态
        left_front_target = 0;
        right_front_target = 0;
        left_back_target = 0;
        right_back_target = 0;
    }

    // 计算前后轮的目标速度
    left_front_target   = linear_velocity + rounding_omega * half_width;
    right_front_target  = linear_velocity - rounding_omega * half_width;
    left_back_target    = linear_velocity + rounding_omega * half_width;
    right_back_target   = linear_velocity - rounding_omega * half_width;
    motor_pid_control();
}

// PID 控制函数
void motor_pid_control(void) {
    left_front_feedback = read_left_front_feedback();
    right_front_feedback = read_right_front_feedback();
    left_back_feedback = read_left_back_feedback();
    right_back_feedback = read_right_back_feedback();
    sprintf(msg, "LF:%.2f RF:%.2f LB:%.2f RB:%.2f",
            left_front_feedback, right_front_feedback, left_back_feedback, right_back_feedback);
    HAL_UART_Transmit_DMA(&huart2, (uint8_t *) msg, strlen(msg));
    //获取当前速度

    int left_front_diff = left_front_PID(left_front_target, left_front_feedback, &left_front_error);
    int right_front_diff = right_front_PID(right_front_target, right_front_feedback, &right_front_error);
    int left_back_diff = left_back_PID(left_back_target, left_back_feedback, &left_back_error);
    int right_back_diff = right_back_PID(right_back_target, right_back_feedback, &right_back_error);

    left_back_speed += left_back_diff;
    right_back_speed += right_back_diff;
    left_front_speed += left_front_diff;
    right_front_speed += right_front_diff;

    // 限制PWM占空比范围，提供电机保护机制
    left_front_speed = left_front_speed > maxspeed ? maxspeed : left_front_speed;
    left_front_speed = left_front_speed < -maxspeed ? -maxspeed : left_front_speed;
    right_front_speed = right_front_speed > maxspeed ? maxspeed : right_front_speed;
    right_front_speed = right_front_speed < -maxspeed ? -maxspeed : right_front_speed;
    left_back_speed = left_back_speed > maxspeed ? maxspeed : left_back_speed;
    left_back_speed = left_back_speed < -maxspeed ? -maxspeed : left_back_speed;
    right_back_speed = right_back_speed > maxspeed ? maxspeed : right_back_speed;
    right_back_speed = right_back_speed < -maxspeed ? -maxspeed : right_back_speed;

    if (left_front_target == 0 && left_front_feedback == 0) { left_front_speed = 0; }
    if (right_front_target == 0 && right_front_feedback == 0) { right_front_speed = 0; }
    if (left_back_target == 0 && left_back_feedback == 0) { left_back_speed = 0; }
    if (right_back_target == 0 && right_back_feedback == 0) { right_back_speed = 0; }
    //将PID处理后的目标速度写入系统
    motor_vel(left_front_speed, right_front_speed, left_back_speed, right_back_speed);
}

// 原地旋转运动函数
void spin(short spin_dir) {
    if (spin_dir == 1) { // 顺时针旋转
        set_speed(0, 90); // 设置线速度为0，角速度为90度
    } else if (spin_dir == -1) { // 逆时针旋转
        set_speed(0, -90); // 设置线速度为0，角速度为-90度
    }
}