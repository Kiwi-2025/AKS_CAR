//
// Created by ROG on 25-7-3.
//

# include "main.h"
# include "motor.h"
#include <stdlib.h>
# include "tim.h"
// # include "usart.h"
# include "blue.h"


extern float velocity_msg_test; // 用于测试的速度消息变量

void motor_init(void) {
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // 启动TIM1通道1的PWM输出
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); // 启动TIM1通道2的PWM输出
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); // 启动TIM1通道3的PWM输出
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4); // 启动TIM1通道4的PWM输出

    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1 | TIM_CHANNEL_2); // 启动TIM3的编码器模式
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_1 | TIM_CHANNEL_2); // 启动TIM4的编码器模式
    HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_1 | TIM_CHANNEL_2); // 启动TIM5的编码器模式
    HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_1 | TIM_CHANNEL_2); // 启动TIM8的编码器模式

    // 写入TIM1的占空比，实现平滑启动
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
    HAL_Delay(300);
    HAL_TIM_Base_Start_IT(&htim7); // 启动TIM7的基本定时器中断
}

void motor_brake(void) {
    // 设置刹车状态
    // HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET); // PD1高电平
    // HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET); // PD2高电平
    // HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET); // PD3高电平
    // HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET); // PD4高电平
    // HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET); // PE1高电平
    // HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET); // PE2高电平
    // HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET); // PE3高电平
    // HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET); // PE4高电平

    // 设置PWM占空比为0，停止输出
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
}

/* 硬件抽象 -------------------------------------------------------------*/
float read_left_front_feedback(void) {
    int count_num = (short) __HAL_TIM_GET_COUNTER(&htim3); //读取编码器数据
    __HAL_TIM_SET_COUNTER(&htim3, 0); // 清零计数器
    float speed = (float) (count_num / 44 / read_period / reduction_ratio); //rps
    speed = speed * diameter * pi; // 转化为mm/s
    // speed *= 60;
    return speed;
}

float read_right_front_feedback(void) {
    int count_num = (short) __HAL_TIM_GET_COUNTER(&htim4); //读取编码器数据
    __HAL_TIM_SET_COUNTER(&htim4, 0); // 清零计数器
    float speed = (float) (count_num / 44 / read_period / reduction_ratio); //rps
    speed = speed * diameter * pi; // 转化为mm/s
    // speed *= 60;
    return speed;
}

float read_left_back_feedback(void) {
    int count_num = (short) __HAL_TIM_GET_COUNTER(&htim5); //读取编码器数据
    __HAL_TIM_SET_COUNTER(&htim5, 0); // 清零计数器
    float speed = (float) (count_num / 44 / read_period / reduction_ratio); //rps
    speed = speed * diameter * pi; // 转化为mm/s
    // speed *= 60;
    return speed;
}

float read_right_back_feedback(void) {
    int count_num = (short) __HAL_TIM_GET_COUNTER(&htim8); //读取编码器数据
    __HAL_TIM_SET_COUNTER(&htim8, 0); // 清零计数器
    float speed = (float) (count_num / 44 / read_period / reduction_ratio); //rps
    speed = speed * diameter * pi; // 转化为mm/s
    // speed *= 60;
    return speed;
}

float left_front_PID(float target_speed, float speed, float *error) {
    float left_front_error = target_speed - speed;
    static float left_front_error_last = -30, left_front_error_before = 30;
    float pwm_pid = 0;
    // *error = left_front_error;
    // *error = left_front_error_last;
    // *error = left_front_error_before;
    // 参考基准：KP = 0.7, KI = 0.035, KD = 0.23;
    pwm_pid =
            0.52 * (left_front_error - left_front_error_last)
            + 0.04 * left_front_error
            + 0.26 * (left_front_error - 2 * left_front_error_last + left_front_error_before);
    left_front_error_before = left_front_error_last; // 保存上上次误差
    left_front_error_last = left_front_error; // 保存上次偏差
    // *error = pwm_pid;
    return pwm_pid;
}

float right_front_PID(float target_speed, float speed, float *error) {
    float right_front_error = target_speed - speed;
    static float right_front_error_last = 0, right_front_error_before = 0;
    float pwm_pid = 0;
    *error = right_front_error;
    // 参考基准：KP = 0.7, KI = 0.035, KD = 0.23;
    pwm_pid =
            0.52 * (right_front_error - right_front_error_last)
            + 0.031 * right_front_error
            + 0.26 * (right_front_error - 2 * right_front_error_last + right_front_error_before);
    right_front_error = right_front_error_last; // 保存上上次误差
    right_front_error_last = right_front_error; // 保存上次偏差
    return pwm_pid;
}

float left_back_PID(float target_speed, float speed, float *error) {
    float left_back_error = target_speed - speed;
    static float left_back_error_last = 0, left_back_error_before = 0;
    float pwm_pid = 0;
    *error = left_back_error;
    // 参考基准：KP = 0.7, KI = 0.035, KD = 0.23;
    pwm_pid =
            0.51 * (left_back_error - left_back_error_last)
            + 0.02 * left_back_error
            + 0.3 * (left_back_error - 2 * left_back_error_last + left_back_error_before);
    left_back_error = left_back_error_last; // 保存上上次误差
    left_back_error_last = left_back_error; // 保存上次偏差
    return pwm_pid;
}

float right_back_PID(float target_speed, float speed, float *error) {
    float right_back_error = target_speed - speed;
    static float right_back_error_last = 0, right_back_error_before = 0;
    float pwm_pid = 0;
    *error = right_back_error;
    // 参考基准：KP = 0.7, KI = 0.035, KD = 0.23;
    pwm_pid =
            0.51 * (right_back_error - right_back_error_last)
            + 0.02 * right_back_error
            + 0.25 * (right_back_error - 2 * right_back_error_last + right_back_error_before);
    right_back_error = right_back_error_last; // 保存上上次误差
    right_back_error_last = right_back_error; // 保存上次偏差
    return pwm_pid;
}

// 测试函数，控制电机前进、后退和停止
void motor_vel(float left_front_speed, float right_front_speed, float left_back_speed, float right_back_speed) {
    // 判断前进方向，控制
    if (left_front_speed > vel_tolerance) {
        // 前进
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_SET); // 设置PD3为高电平
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_RESET); // 设置PD4为低电平
    } else if (left_front_speed < -vel_tolerance) {
        // 后退
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_RESET); // 设置PD3为低电平
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_SET); // 设置PD4为高电平
    } else {
        // 刹车
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_SET); // 设置PD3为高电平
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_SET); // 设置PD4为高电平
    }
    // HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_SET); // 设置PD3为低电平
    // HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_RESET); // 设置PD4为高电平

    if (right_front_speed > vel_tolerance) {
        // 前进
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET); // 设置PD1为高电平
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET); // 设置PD2为低电平
    } else if (right_front_speed < -vel_tolerance) {
        // 后退
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET); // 设置PD1为低电平
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET); // 设置PD2为高电平
    } else {
        // 刹车
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET); // 设置PD3为高电平
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET); // 设置PD4为高电平
    }
    if (left_back_speed > vel_tolerance) {
        // 前进
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET); // 设置PE3为高电平
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET); // 设置PE4为低电平
    } else if (left_back_speed < -vel_tolerance) {
        // 后退
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET); // 设置PE3为低电平
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET); // 设置PE4为高电平
    } else {
        // 刹车
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET); // 设置PE3为高电平
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET); // 设置PE4为高电平
    }
    if (right_back_speed > vel_tolerance) {
        // 前进
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET); // 设置PE1为高电平
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET); // 设置PE2为低电平
    } else if (right_back_speed < -vel_tolerance) {
        // 后退
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET); // 设置PE1为低电平
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET); // 设置PE2为高电平
    } else {
        // 刹车
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET); // 设置PE1为高电平
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET); // 设置PE2为高电平
    }

    // 计算占空比
    uint32_t left_front_duty = (uint32_t) (abs(left_front_speed) / 8.1);
    uint32_t right_front_duty = (uint32_t) (abs(right_front_speed) / 8.14);
    uint32_t left_back_duty = (uint32_t) (abs(left_back_speed) / 8.63);
    uint32_t right_back_duty = (uint32_t) (abs(right_back_speed) / 8.14);

    // 设置PWM占空比
    // #define LEFT_FRONT TIM_CHANNEL_1
    // #define RIGHT_FRONT TIM_CHANNEL_2
    // #define LEFT_BACK TIM_CHANNEL_3
    // #define RIGHT_BACK TIM_CHANNEL_4

    __HAL_TIM_SET_COMPARE(&htim1, RIGHT_FRONT, right_front_duty);
    __HAL_TIM_SET_COMPARE(&htim1, LEFT_FRONT, left_front_duty);
    __HAL_TIM_SET_COMPARE(&htim1, RIGHT_BACK, right_back_duty);
    __HAL_TIM_SET_COMPARE(&htim1, LEFT_BACK, left_back_duty);
}

float read_omega(void) {
    int count_num = (short) __HAL_TIM_GET_COUNTER(&htim3); //读取编码器数据
    __HAL_TIM_SET_COUNTER(&htim3, 0); // 清零计数器
    float rpm = (float) (60 * count_num / 44 / 0.1 / reduction_ratio); // 计算转速
    // 转速 = 10ms内计数的脉冲数 / 44（每转44脉冲） / 0.01（10ms转换为秒） /21.3（减速比）
    // 如果需要转化为其他单位，应该进行适当转换
    return rpm;
}

void Motor_test(void) {
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 800); // 设置占空比为80%
    for (int state = 0; state < 3; state++) {
        switch (state) {
            case 0: // 前进
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 800); // 设置占空比为80%
                HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);
                HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
                break;
            case 1: // 后退
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 300); // 设置占空比为80%
                HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
                break;
            case 2: // 停止
                HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);
                HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
                break;
        }
        HAL_Delay(1000);
    }
}

// 测试函数，读取电机速度
void Motor_Read_Speed_test(void) {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, 100); // 设置PWM初始值为100%
}
