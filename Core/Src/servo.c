//
// Created by ROG on 25-7-3.
//

# include "main.h"
# include "servo.h"
# include "tim.h"

void Servo1_Init(void) {
  HAL_TIM_PWM_Start(&htim12,TIM_CHANNEL_1);
  Servo1_SetAngle(0); // 初始化舵机1角度为0
}

void Servo2_Init(void) {
  HAL_TIM_PWM_Start(&htim12,TIM_CHANNEL_2);
  Servo2_SetAngle(10); // 初始化舵机2角度为0
}

void Servo1_SetAngle(uint16_t angle) {
  // 舵机1对应的引脚为TIM2的通道1，PA5
  const short degree = 25+5*angle/9;
  __HAL_TIM_SET_COMPARE(&htim12,TIM_CHANNEL_1,degree);
}

void Servo2_SetAngle(uint16_t angle) {
  // 舵机2对应的引脚为TIM2的通道2，PA1
  const short degree = 25+5*angle/9;
  __HAL_TIM_SET_COMPARE(&htim12,TIM_CHANNEL_2,degree);
}

void Servo_catch(void) {
  Servo1_SetAngle(10); // 舵机1抓取物体前先将角度设置为10度
  HAL_Delay(100);
  // TODO: 舵机1抓取物体,这个参数需要根据实际情况调整
  Servo1_SetAngle(140);
  HAL_Delay(1000);
}
void Servo_put(void) {
  Servo1_SetAngle(10); // 舵机1放下物体
}
void Servo_turnUp(void) {
  for (short angle = 0; angle < 105; angle += 15) {
    Servo2_SetAngle(angle); // 舵机2向上转动
    HAL_Delay(30); // 延时30毫秒
  }
}
void Servo_turnDown(void) {
  for (short angle = 105; angle > 0; angle -= 15) {
    Servo2_SetAngle(angle); // 舵机2向下转动
    HAL_Delay(30); // 延时30毫秒
  }
}

// 测试函数，舵机1和舵机2从0度到180度再回到0度
void Servo_test(void) {
  for (uint16_t angle = 0; angle < 80; angle += 10) {
    Servo1_SetAngle(angle);
    Servo2_SetAngle(angle);
    HAL_Delay(100); // 延时500毫秒
  }
  for(uint16_t angle = 80; angle > 0; angle -= 10) {
    Servo2_SetAngle(angle);
    Servo1_SetAngle(angle);
    HAL_Delay(100); // 延时500毫秒
  }
}
