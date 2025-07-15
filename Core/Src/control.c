//
// Created by ROG on 25-7-7.
//
# include "control.h"

#include "blue.h"

// 根据运动需求设置四轮转速
void set_speed(float x_err, float y_err) {
    float rounding_omega = x_err * FLP; // 将角度转换为弧度
    float linear_velocity = y_err * LLP; // 将x轴偏差转换为线速度
    // TODO:根据输入的偏差值改变直线前进的速度，如果偏差值比较大，那么写入一个比较小的线速度，也可以划分为几个挡位
    // // 特殊情况：纯直线运动
    // if (fabs(rounding_omega) < omega_eps && fabs(linear_velocity) > lin_vel_eps) {
    //     left_front_target = linear_velocity;
    //     right_front_target = linear_velocity;
    //     left_back_target = linear_velocity;
    //     right_back_target = linear_velocity;
    // }
    // // 特殊情况：保持静止
    // if (fabs(linear_velocity) < lin_vel_eps && fabs(rounding_omega) < omega_eps) {
    //     // 静止状态
    //     left_front_target = 0;
    //     right_front_target = 0;
    //     left_back_target = 0;
    //     right_back_target = 0;
    // }

    // 计算前后轮的目标速度
    left_front_target = linear_velocity - rounding_omega * half_width;
    left_back_target = linear_velocity - rounding_omega * half_width;
    right_front_target = linear_velocity + rounding_omega * half_width;
    right_back_target = linear_velocity + rounding_omega * half_width;
    // motor_pid_control();
}

/* 轮速度控制函数 --------------------------------------------------------------------------------*/
// PID 控制函数
void motor_pid_control(void) {
    left_front_feedback = read_left_front_feedback();
    right_front_feedback = read_right_front_feedback();
    left_back_feedback = read_left_back_feedback();
    right_back_feedback = read_right_back_feedback();

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

    // 调试用函数
    sprintf(msg, "LF:%.2f RF:%.2f LB:%.2f RB:%.2f",left_front_feedback, right_front_feedback, left_back_feedback, right_back_feedback);
    HAL_UART_Transmit_DMA(&huart2, (uint8_t *) msg, strlen(msg));
}
void set_parameters(char *input) {
    // 检查输入是否为空
    if (input == NULL || strlen(input) < 4) {
        sprintf(blue_feedback_msg, "FAIL");
        HAL_UART_Transmit_DMA(&huart2, (uint8_t *) blue_feedback_msg, strlen(blue_feedback_msg));
        return;
    }

    // 检查第0个字符是否为@
    if (input[0] != '@') {
        sprintf(blue_feedback_msg, "FAIL");
        HAL_UART_Transmit_DMA(&huart2, (uint8_t *) blue_feedback_msg, strlen(blue_feedback_msg));
        return;
    }

    // 检查第1个字符是否为F或者L
    if (input[1] != 'F' && input[1] != 'L') {
        sprintf(blue_feedback_msg, "FAIL");
        HAL_UART_Transmit_DMA(&huart2, (uint8_t *) blue_feedback_msg, strlen(blue_feedback_msg));
        return;
    }

    // 查找结束符'/'的位置
    char *end_pos = strchr(input, '/');
    if (end_pos == NULL) {
        sprintf(blue_feedback_msg, "FAIL");
        HAL_UART_Transmit_DMA(&huart2, (uint8_t *) blue_feedback_msg, strlen(blue_feedback_msg));
        return;
    }

    // 计算数字部分的长度
    int num_length = end_pos - input - 2; // 减去@和字母
    if (num_length <= 0 || num_length >= 32) {
        sprintf(blue_feedback_msg, "FAIL");
        HAL_UART_Transmit_DMA(&huart2, (uint8_t *) blue_feedback_msg, strlen(blue_feedback_msg));
        return;
    }

    // 提取数字部分
    char temp_str[32];
    strncpy(temp_str, &input[2], num_length);
    temp_str[num_length] = '\0';

    // 验证数字格式
    int has_decimal = 0;
    int start_idx = 0;
    int length = strlen(temp_str);

    // 检查负号
    if (temp_str[0] == '-') {
        start_idx = 1;
        if (length == 1) {
            sprintf(blue_feedback_msg, "FAIL");
            HAL_UART_Transmit_DMA(&huart2, (uint8_t *) blue_feedback_msg, strlen(blue_feedback_msg));
            return;
        }
    }

    // 验证数字字符
    for (int i = start_idx; i < length; i++) {
        if (temp_str[i] == '.') {
            if (has_decimal) {
                sprintf(blue_feedback_msg, "FAIL");
                HAL_UART_Transmit_DMA(&huart2, (uint8_t *) blue_feedback_msg, strlen(blue_feedback_msg));
                return; // 多个小数点
            }
            has_decimal = 1;
        } else if (temp_str[i] < '0' || temp_str[i] > '9') {
            sprintf(blue_feedback_msg, "FAIL");
            HAL_UART_Transmit_DMA(&huart2, (uint8_t *) blue_feedback_msg, strlen(blue_feedback_msg));
            return; // 非法字符
        }
    }

    // 根据类型设置对应参数
    if (input[1] == 'F') {
        FLP = atof(temp_str);
    } else if (input[1] == 'L') {
        LLP = atof(temp_str);
    }

    sprintf(blue_feedback_msg, "YES!F:%.3f,L:%.3f", FLP, LLP);
    HAL_UART_Transmit_DMA(&huart2, (uint8_t *) blue_feedback_msg, strlen(blue_feedback_msg));
}

/* 测试用函数 ------------------------------------------------------------------------------*/
// 原地旋转运动函数
void spin(void) {
    set_speed(100, 0); // 设置线速度为0，角速度为90度
}

// 前后运动函数
void move_forward(void) {
    set_speed(0, 500); // 设置线速度为300，角速度为0
    HAL_Delay(5000);
}
