//
// Created by ROG on 25-7-3.
//

# include "main.h"
# include "servo.h"
# include "tim.h"

void Servo1_Init(void) {
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
  Servo1_SetAngle(0); // 初始化舵机1角度为0
}

void Servo2_Init(void) {
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
  Servo2_SetAngle(0); // 初始化舵机2角度为0
}

void Servo1_SetAngle(uint16_t angle) {
  // 舵机1对应的引脚为TIM2的通道1，PA5
  const short degree = 25+5*angle/9;
  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,degree);
}

void Servo2_SetAngle(uint16_t angle) {
  // 舵机2对应的引脚为TIM2的通道2，PA1
  const short degree = 25+5*angle/9;
  __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,degree);
}

void Servo_catch(void) {
  Servo1_SetAngle(90); // 舵机1抓取物体
}
void Servo_put(void) {
  Servo1_SetAngle(0); // 舵机1放下物体
}
void Servo_turnUp(void) {
  Servo2_SetAngle(90); // 舵机2向上转动
}
void Servo_turnDown(void) {
  Servo2_SetAngle(0); // 舵机2向下转动
}

// 测试函数，舵机1和舵机2从0度到180度再回到0度
void Servo_test(void) {
  for (uint16_t angle = 0; angle < 180; angle += 10) {
    Servo1_SetAngle(angle);
    Servo2_SetAngle(angle);
    HAL_Delay(500); // 延时500毫秒
  }
  for(uint16_t angle = 180; angle > 0; angle -= 10) {
    Servo2_SetAngle(angle);
    Servo1_SetAngle(angle);
    HAL_Delay(500); // 延时500毫秒
  }
}
