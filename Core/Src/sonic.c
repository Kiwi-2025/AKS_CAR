/*
 * sonic.c
 *
 *  Created on: Jul 7, 2025
 *      Author: sky
 */

#include "sonic.h"


void Sonic_Init(){
	HAL_TIM_Base_Start(&htim2); // Start Timer 2
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1); // Start Input Capture on Channel 1
};
// 计算距离
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim2 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
        static uint8_t captureState = 0;

        if (captureState == 0) {
            // 捕获到上升沿
            upEdge = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            // 切换为下降沿触发
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
            captureState = 1;
        } else {
            // 捕获到下降沿
            downEdge = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

            // 计算脉冲宽度，考虑定时器溢出情况
            uint32_t pulseWidth;
            if (downEdge > upEdge) {
                pulseWidth = downEdge - upEdge;
            } else {
                pulseWidth = 0xFFFFFFFF - upEdge + downEdge + 1;
            }

            // 计算距离 (340m/s = 0.034cm/μs)
            distance = (float)pulseWidth * 0.034f / 2.0f;

            // 恢复为上升沿触发
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
            captureState = 0;
            measurementComplete = 1;  // 标记测量完成
        }
    }
};

void Trick_Check(uint32_t *lastTriggerTime, const uint32_t*triggerInterval){
// 控制触发频率，避免过于频繁
    if (HAL_GetTick() - (uint32_t)(*lastTriggerTime) >= (uint32_t)(*triggerInterval)) {
        *lastTriggerTime = HAL_GetTick();

        // 仅在上一次测量完成后才触发新的测量
        if (measurementComplete) {
            measurementComplete = 0;

            // 发送触发脉冲
            HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, GPIO_PIN_SET);
            HAL_Delay(1);  // 保持至少10μs
            HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, GPIO_PIN_RESET);


        }
    }
};



void Measure_Sonic(TIM_HandleTypeDef *htim, uint32_t *lastTriggerTime, const uint32_t*triggerInterval){
	HAL_TIM_IC_CaptureCallback(htim);
	Trick_Check(lastTriggerTime, triggerInterval);
}
