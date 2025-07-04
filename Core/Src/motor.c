//
// Created by ROG on 25-7-3.
//

# include "main.h"
# include "motor.h"
# include "tim.h"
const double compute_factor =reduction_ratio*4*pulse_num*delay;

void Motor_Init(void) {
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // 启动TIM1通道1的PWM输出
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); // 启动TIM1通道2的PWM输出
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); // 启动TIM1通道3的PWM输出
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4); // 启动TIM1通道4的PWM输出
}

int read_left_front_feedback(void) {
    int count_num =(short)__HAL_TIM_GET_COUNTER(&htim2);	  //读取编码器数据
	__HAL_TIM_SET_COUNTER(&htim2, 0); // 清零计数器
	int speed = 1050 * diameter * pi * count_num / compute_factor;
	return speed;
}
int read_right_front_feedback(void) {
    int count_num =(short)__HAL_TIM_GET_COUNTER(&htim2);	  //读取编码器数据
    __HAL_TIM_SET_COUNTER(&htim2, 0); // 清零计数器
    int speed = 1050 * diameter * pi * count_num / compute_factor;
    return speed;
}
int read_left_back_feedback(void) {
    int count_num =(short)__HAL_TIM_GET_COUNTER(&htim2);	  //读取编码器数据
    __HAL_TIM_SET_COUNTER(&htim2, 0); // 清零计数器
    int speed = 1050 * diameter * pi * count_num / compute_factor;
    return speed;
}
int read_right_back_feedback(void) {
    int count_num =(short)__HAL_TIM_GET_COUNTER(&htim2);	  //读取编码器数据
    __HAL_TIM_SET_COUNTER(&htim2, 0); // 清零计数器
    int speed = 1050 * diameter * pi * count_num / compute_factor;
    return speed;
}

int left_front_PID(int target_speed, int speed, int *error) {
    int left_front_error = target_speed - speed;
    static int left_front_error_last = 0,left_front_error_before = 0;
    int pwm_pid=0;
    *error = left_front_error;
    pwm_pid = KP*(left_front_error-left_front_error_last)+KI*left_front_error
            +KD*(left_front_error-2*left_front_error_last+left_front_error_before);
    left_front_error = left_front_error_last;	  	    // 保存上上次误差
    left_front_error_last = left_front_error;	              // 保存上次偏差
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

void Motor_Read_Speed_test(void) {

}