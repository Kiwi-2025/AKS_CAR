//
// Created by ROG on 25-7-3.
//

#ifndef MOTOR_H
#define MOTOR_H
void Motor_Control_Init(uint16_t Prescaler, uint16_t Period);
void Motor_PWM_Init(uint16_t Prescaler, uint16_t Period);
void Motor_GPIO_Init(void);

//占空比调节函数，可以给TIM1的每一个通道分配不同的占空比值
//compare为希望的占空比值，在0~100之间
void TIM_SetCompare_CH1(uint16_t Compare, TIM_TypeDef* TIMx);
void TIM_SetCompare_CH2(uint16_t Compare, TIM_TypeDef* TIMx);
void TIM_SetCompare_CH3(uint16_t Compare, TIM_TypeDef* TIMx);
void TIM_SetCompare_CH4(uint16_t Compare, TIM_TypeDef* TIMx);


#endif //MOTOR_H

