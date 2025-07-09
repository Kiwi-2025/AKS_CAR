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

void Motor_Init(void) {
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // 启动TIM1通道1的PWM输出
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); // 启动TIM1通道2的PWM输出
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); // 启动TIM1通道3的PWM输出
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4); // 启动TIM1通道4的PWM输出

    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1|TIM_CHANNEL_2); // 启动TIM3的编码器模式
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_1|TIM_CHANNEL_2); // 启动TIM4的编码器模式
    HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_1|TIM_CHANNEL_2); // 启动TIM5的编码器模式
    HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_1|TIM_CHANNEL_2); // 启动TIM8的编码器模式

    HAL_TIM_Base_Start_IT(&htim7); // 启动TIM7的基本定时器中断
}

/* 硬件抽象 -------------------------------------------------------------*/
float read_left_front_feedback(void) {
    int count_num =(short)__HAL_TIM_GET_COUNTER(&htim3);	  //读取编码器数据
	__HAL_TIM_SET_COUNTER(&htim3, 0); // 清零计数器
    float speed = (float) (count_num / 44 / 0.1 / reduction_ratio);       //rps
    speed = speed * diameter * pi;                            // 转化为mm/s
	return speed;
}
float read_right_front_feedback(void) {
    int count_num =(short)__HAL_TIM_GET_COUNTER(&htim4);	  //读取编码器数据
    __HAL_TIM_SET_COUNTER(&htim4, 0); // 清零计数器
    float speed = (float) (count_num / 44 / 0.1 / reduction_ratio);       //rps
    speed = speed * diameter * pi;                            // 转化为mm/s
    return speed;
}
float read_left_back_feedback(void) {
    int count_num =(short)__HAL_TIM_GET_COUNTER(&htim5);	  //读取编码器数据
    __HAL_TIM_SET_COUNTER(&htim5, 0); // 清零计数器
    float speed = (float) (count_num / 44 / 0.1 / reduction_ratio);       //rps
    speed = speed * diameter * pi;                            // 转化为mm/s
    return speed;
}
float read_right_back_feedback(void) {
    int count_num =(short)__HAL_TIM_GET_COUNTER(&htim8);	  //读取编码器数据
    __HAL_TIM_SET_COUNTER(&htim8, 0); // 清零计数器
    float speed = (float) (count_num / 44 / 0.1 / reduction_ratio);       //rps
    speed = speed * diameter * pi;                            // 转化为mm/s
    return speed;
}

int left_front_PID(int target_speed, int speed, int *error) {
    int left_front_error = target_speed - speed;
    static int left_front_error_last = 0,left_front_error_before = 0;
    int pwm_pid=0;
    *error = left_front_error;
    // *error = left_front_error_last;
    // *error = left_front_error_before;
    pwm_pid = KP*(left_front_error-left_front_error_last)+KI*left_front_error
            +KD*(left_front_error-2*left_front_error_last+left_front_error_before);
    left_front_error_before = left_front_error_last;	  	    // 保存上上次误差
    left_front_error_last = left_front_error;	              // 保存上次偏差
    // *error = pwm_pid;
    return pwm_pid;
}
int right_front_PID(int target_speed, int speed, int *error) {
    int right_front_error = target_speed - speed;
    static int right_front_error_last = 0,right_front_error_before = 0;
    int pwm_pid=0;
    *error = right_front_error;
    pwm_pid = KP*(right_front_error-right_front_error_last)+KI*right_front_error
            +KD*(right_front_error-2*right_front_error_last+right_front_error_before);
    right_front_error = right_front_error_last;	  	    // 保存上上次误差
    right_front_error_last = right_front_error;	              // 保存上次偏差
    return pwm_pid;
}
int left_back_PID(int target_speed, int speed, int *error) {
    int left_back_error = target_speed - speed;
    static int left_back_error_last = 0,left_back_error_before = 0;
    int pwm_pid=0;
    *error = left_back_error;
    pwm_pid = KP*(left_back_error-left_back_error_last)+KI*left_back_error
            +KD*(left_back_error-2*left_back_error_last+left_back_error_before);
    left_back_error = left_back_error_last;	  	    // 保存上上次误差
    left_back_error_last = left_back_error;	              // 保存上次偏差
    return pwm_pid;
}
int right_back_PID(int target_speed, int speed, int *error) {
    int right_back_error = target_speed - speed;
    static int right_back_error_last = 0,right_back_error_before = 0;
    int pwm_pid=0;
    *error = right_back_error;
    pwm_pid = KP*(right_back_error-right_back_error_last)+KI*right_back_error
            +KD*(right_back_error-2*right_back_error_last+right_back_error_before);
    right_back_error = right_back_error_last;	  	    // 保存上上次误差
    right_back_error_last = right_back_error;	              // 保存上次偏差
    return pwm_pid;
}

// 测试函数，控制电机前进、后退和停止
void motor_vel(int left_front_speed, int right_front_speed, int left_back_speed, int right_back_speed) {
    // // 限制PWM占空比范围
    // left_front_speed = left_front_speed > maxspeed ? maxspeed : left_front_speed;
    // left_front_speed = left_front_speed < -maxspeed ? -maxspeed : left_front_speed;
    // right_front_speed = right_front_speed > maxspeed ? maxspeed : right_front_speed;
    // right_front_speed = right_front_speed < -maxspeed ? -maxspeed : right_front_speed;
    // left_back_speed = left_back_speed > maxspeed ? maxspeed : left_back_speed;
    // left_back_speed = left_back_speed < -maxspeed ? -maxspeed : left_back_speed;
    // right_back_speed = right_back_speed > maxspeed ? maxspeed : right_back_speed;
    // right_back_speed = right_back_speed < -maxspeed ? -maxspeed : right_back_speed;

    // 计算占空比

    uint32_t left_front_duty = (uint32_t)(abs(left_front_speed) * 113.0 + 1700.0)/10.0;
    uint32_t right_front_duty = (uint32_t)(abs(right_front_speed) * 113.0 + 1700.0)/10.0;
    uint32_t left_back_duty = (uint32_t)(abs(left_back_speed) * 113.0 + 1700.0)/10.0;
    uint32_t right_back_duty = (uint32_t)(abs(right_back_speed) * 113.0 + 1700.0)/10.0;

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
    int count_num =(short)__HAL_TIM_GET_COUNTER(&htim3);	  //读取编码器数据
    __HAL_TIM_SET_COUNTER(&htim3, 0); // 清零计数器
    float rpm = (float) (60 * count_num / 44 / 0.1 / reduction_ratio); // 计算转速
    // 转速 = 10ms内计数的脉冲数 / 44（每转44脉冲） / 0.01（10ms转换为秒） /21.3（减速比）
    // 如果需要转化为其他单位，应该进行适当转换
    return rpm;
}

void Motor_test(void) {
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 800); // 设置占空比为80%
  for (int state =0; state < 3 ; state++) {
    switch (state) {
      case 0 : // 前进
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 800); // 设置占空比为80%
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
            break;
      case 1 : // 后退
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 300); // 设置占空比为80%
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
        break;
      case 2 : // 停止
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
    //velocity_msg_test = (uint8_t)read_rps();
    // HAL_Delay(5000);

    // HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
     // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 300); // 设置占空比为30%
    //velocity_msg_test = (uint8_t)read_rps();
    // HAL_Delay(5000);

}