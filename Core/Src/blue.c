/*
 * blue.c
 *
 *  Created on: Jul 5, 2025
 *      Author: sky
 */
#include "blue.h"


// void ReturnToBlue(const NameValuePair *pairs, int count, char *msg, size_t buf_size) {
//     char *ptr = msg;
//     size_t remaining = buf_size;
//
//     for (int i = 0; i < count; i++) {
//         // 提取整数和小数部分（保留三位小数）
//         int integerPart = (int)pairs[i].value;
//         float fractional = pairs[i].value - integerPart;
//         if (fractional < 0) fractional = -fractional;
//         int decimalPart = (int)(fractional * 1000 + 0.5); // 四舍五入
//
//         // 格式化字符串并安全拼接
//         const int written = snprintf(ptr, remaining, "%s: %d.%03d",
//                               pairs[i].name, integerPart, decimalPart);
//
//         // 检查缓冲区空间
//         if (written < 0 || (size_t)(written) >= remaining) {
//             // 缓冲区不足
//             break;
//         }
//
//         ptr += written;
//         remaining -= written;
//
//         // 添加分隔符（最后一个不添加）
//         if (i < count - 1 && remaining > 1) {
//             *ptr++ = ' '; // 用空格分隔
//             *ptr = '\0';  // 保持字符串终止
//             remaining--;
//         }
//     }
//
//     // 确保以null结尾
//     if (buf_size > 0) {
//         msg[buf_size - 1] = '\0';
//     }
//
//     HAL_UART_Transmit_DMA(&huart2, (uint8_t *)msg, strlen(msg));
// }

 void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	 if (huart == &huart1){
		 HAL_UART_Transmit_DMA(&huart1, (uint8_t*)openmv_message, sizeof(openmv_message));


		 HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)openmv_message, sizeof(openmv_message));
	 }
 }

// Initial implementation of initNameValuePairs
// __weak void initNameValuePairs() {
//     // 默认实现，可以被其他文件中的强符号覆盖
// }
