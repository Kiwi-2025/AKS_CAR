/*
 * sonic.h
 *
 *  Created on: Jul 7, 2025
 *      Author: sky
 */

#ifndef INC_SONIC_H_
#define INC_SONIC_H_

#include "main.h"
#include "stdio.h"
#include "tim.h"

extern uint32_t upEdge;      // 存储上升沿时间
extern uint32_t downEdge;    // 存储下降沿时间
extern float distance;       // 计算得到的距离(cm)
extern uint8_t measurementComplete;  // 测量完成标志

void Sonic_Init();
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void Trick_Check(uint32_t *lastTriggerTime, const uint32_t *triggerInterval);
void Measure_Sonic(TIM_HandleTypeDef *htim, uint32_t *lastTriggerTime, const uint32_t *triggerInterval);

#endif /* INC_SONIC_H_ */