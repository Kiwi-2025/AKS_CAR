/*
 * blue.c
 *
 *  Created on: Jul 5, 2025
 *      Author: sky
 */
#include "blue.h"

void openmv_msg2error(char *msg) {
    // 检查输入是否为空
    if (msg == NULL || strlen(msg) < 6) {
        return; // 最小格式: @1,2/
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

    // 查找分隔符','的位置
    char *comma_pos = strchr(msg, ',');
    if (comma_pos == NULL || comma_pos >= end_pos) {
        return; // 没有找到逗号或逗号在'/'之后
    }

    // 计算第一个数字部分的长度
    int first_num_length = comma_pos - msg - 1;
    if (first_num_length <= 0) {
        return;
    }

    // 计算第二个数字部分的长度
    int second_num_length = end_pos - comma_pos - 1;
    if (second_num_length <= 0) {
        return;
    }

    // 提取第一个数字部分
    char first_str[32];
    if (first_num_length >= sizeof(first_str)) {
        return; // 数字太长
    }
    strncpy(first_str, &msg[1], first_num_length);
    first_str[first_num_length] = '\0';

    // 提取第二个数字部分
    char second_str[32];
    if (second_num_length >= sizeof(second_str)) {
        return; // 数字太长
    }
    strncpy(second_str, &comma_pos[1], second_num_length);
    second_str[second_num_length] = '\0';

    // 验证第一个数字格式
    if (!validate_float_string(first_str)) {
        return;
    }

    // 验证第二个数字格式
    if (!validate_float_string(second_str)) {
        return;
    }

    // 解析并更新x_error和y_error
    x_error = atof(first_str);
    y_error = atof(second_str);
}

// 辅助函数：验证浮点数字符串格式
int validate_float_string(char *str) {
    if (str == NULL || strlen(str) == 0) {
        return 0;
    }

    int has_decimal = 0;
    int start_idx = 0;
    int length = strlen(str);

    // 检查负号
    if (str[0] == '-') {
        start_idx = 1;
        if (length == 1) {
            return 0; // 只有负号
        }
    }

    for (int i = start_idx; i < length; i++) {
        if (str[i] == '.') {
            if (has_decimal) {
                return 0; // 多个小数点，不合法
            }
            has_decimal = 1;
        } else if (str[i] < '0' || str[i] > '9') {
            return 0; // 非法字符
        }
    }

    return 1; // 格式正确
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart == &huart1) {
        openmv_msg2error(openmv_msg);
        HAL_UART_Transmit_DMA(&huart2, (uint8_t *) openmv_msg, strlen(openmv_msg));
        // HAL_Delay(100);
        // HAL_UART_Transmit_DMA(&huart1, (uint8_t*)openmv_message, sizeof(openmv_message));
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t *) openmv_msg, sizeof(openmv_msg));

    }
    if (huart == &huart2) {
        // 处理从蓝牙接收到的数据
        // HAL_UART_Transmit_DMA(&huart2, (uint8_t *)"Re", sizeof("Re"));
        // set_parameters(blue_msg);
        // 将状态反馈发送到蓝牙
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, (uint8_t*)blue_msg, sizeof(blue_msg));
    }
}
