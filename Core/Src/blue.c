/*
 * blue.c
 *
 *  Created on: Jul 5, 2025
 *      Author: sky
 */
#include "blue.h"

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
