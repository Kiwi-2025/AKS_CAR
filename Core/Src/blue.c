/*
 * blue.c
 *
 *  Created on: Jul 5, 2025
 *      Author: sky
 */
#include "blue.h"

void ReturnToBlue(uint8_t msg[5], float *value) {
    int integerPart = (int)(*value); // 提取整数部分
    int decimalPart = (int)((*value - integerPart) * 10); // 提取第一个小数位
    // 提取千、百、十、个位
    int thousand = integerPart / 1000 % 10;
    int hundred = integerPart / 100 % 10;
    int ten = integerPart / 10 % 10;
    int one = integerPart % 10;
    // 转换为ASCII码
    msg[0] = thousand + '0';
    msg[1] = hundred + '0';
    msg[2] = ten + '0';
    msg[3] = one + '0';
    msg[4] = decimalPart + '0'; // 第一个小数位转换为ASCII码

    HAL_UART_Transmit_DMA(&huart2, msg, 5);
}
