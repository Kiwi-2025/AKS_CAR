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


extern NameValuePair name_value_pairs[]; // 用于存储键值对的数组
extern int numPairs; // 键值对的数量
extern char buffer[100]; // 用于存储发送到蓝牙的数据
// extern float velocity_msg_test; // 用于测试的速度消息变量
extern uint8_t flag_500ms; // 用于标志是否需要发送数据到蓝牙
extern UART_HandleTypeDef huart2;
void ReturnToBlue(const NameValuePair *pairs, int count, char *msg, size_t buf_size) ;

#endif //BLUE_H