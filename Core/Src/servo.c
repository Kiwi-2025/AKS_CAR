//
// Created by ROG on 25-7-3.
//

# include "main.h"
# include "servo.h"
# include "tim.h"

void Servo_Control_Init(uint16_t Prescaler, uint16_t Period) {
  Servo_PWM_Init(Prescaler, Period); // 初始化PWM
}

void Servo_PWM_Init(uint16_t Prescaler, uint16_t Period) {

}

void Servo_SetAngle(uint16_t angle) {

}

void Servo_test(void) {
  for (int angle = 0; angle < 180; angle += 10) {
    Servo_SetAngle(angle);
    HAL_Delay(500); // 延时500毫秒
  }
  for(int angle = 180; angle >= 0; angle -= 10) {
    Servo_SetAngle(angle);
    HAL_Delay(500); // 延时500毫秒
  }
}