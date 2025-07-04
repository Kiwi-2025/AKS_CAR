//
// Created by ROG on 25-7-3.
//

# include "main.h"
# include "motor.h"
# include "tim.h"

void Motor_Init(void) {
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // 启动TIM1通道1的PWM输出
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); // 启动TIM1通道2的PWM输出
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); // 启动TIM1通道3的PWM输出
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4); // 启动TIM1通道4的PWM输出
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