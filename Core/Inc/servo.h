//
// Created by ROG on 25-7-3.
//

#ifndef SERVO_H
#define SERVO_H
void Servo_Control_Init(uint16_t Prescaler, uint16_t Period);
void Servo_PWM_Init(uint16_t Prescaler, uint16_t Period);
void Servo_SetAngle(uint16_t angle);

void TIM_SetCompare_CH1(uint16_t Compare, TIM_TypeDef* TIMx);
void TIM_SetCompare_CH2(uint16_t Compare, TIM_TypeDef* TIMx);

void Servo_test(void);
#endif //SERVO_H
