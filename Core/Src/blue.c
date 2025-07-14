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

void openmv_msg2x_error(char *msg) {
    // 检查输入是否为空
    if (msg == NULL || strlen(msg) < 4) {
        return; // 最小格式: @1/
    }

    // 检查第0个字符是否为'@'
    if (msg[0] != '@') {
        return;
    }

    // 查找结束符'/'的位置
    char *end_pos = strchr(msg, '/');
    if (end_pos == NULL) {
        return;
    }

    // 计算数字部分的长度
    int num_length = end_pos - msg - 1;
    if (num_length <= 0) {
        return;
    }

    // 提取数字部分
    char temp_str[32];
    if (num_length >= sizeof(temp_str)) {
        return; // 数字太长
    }

    strncpy(temp_str, &msg[1], num_length);
    temp_str[num_length] = '\0';

    // 验证数字格式（允许负号、数字和一个小数点）
    int has_decimal = 0;
    int start_idx = 0;

    // 检查负号
    if (temp_str[0] == '-') {
        start_idx = 1;
    }

    for (int i = start_idx; i < num_length; i++) {
        if (temp_str[i] == '.') {
            if (has_decimal) {
                return; // 多个小数点，不合法
            }
            has_decimal = 1;
        } else if (temp_str[i] < '0' || temp_str[i] > '9') {
            return; // 非法字符
        }
    }

    // 确保至少有一个数字
    if ((start_idx == 1 && num_length == 1) || num_length == 0) {
        return; // 只有负号或空字符串
    }

    // 解析并更新x_error
    x_error = atof(temp_str);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart == &huart1) {
        openmv_msg2x_error(openmv_msg);
        HAL_UART_Transmit_DMA(&huart2, (uint8_t *) openmv_msg, sizeof(openmv_msg));
        // HAL_Delay(100);
        // HAL_UART_Transmit_DMA(&huart1, (uint8_t*)openmv_message, sizeof(openmv_message));
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t *) openmv_msg, sizeof(openmv_msg));

    }
    if (huart == &huart2) {
        // 处理从蓝牙接收到的数据
        set_parameters(blue_msg);
        // 将状态反馈发送到蓝牙
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, (uint8_t*)blue_msg, sizeof(blue_msg));
    }
}

// void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
//     if (huart == &huart2) {
//         // 处理从蓝牙接收到的数据
//         set_parameters(blue_msg);
//         // 将状态反馈发送到蓝牙
//     }
// }
// Initial implementation of initNameValuePairs
// __weak void initNameValuePairs() {
//     // 默认实现，可以被其他文件中的强符号覆盖
// }
