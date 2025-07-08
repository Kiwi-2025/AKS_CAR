/*
 * blue.h
 *
 *  Created on: Jul 5, 2025
 *      Author: sky
 */


#ifndef BLUE_H
#define BLUE_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

extern uint8_t message[7]; // 用于存储发送到蓝牙的数据
extern float velocity_msg_test; // 用于测试的速度消息变量
extern uint8_t flag_500ms; // 用于标志是否需要发送数据到蓝牙
extern UART_HandleTypeDef huart2;
void ReturnToBlue(uint8_t msg[7], float *value);

#endif //BLUE_H