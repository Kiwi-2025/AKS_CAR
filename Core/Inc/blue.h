/*
 * blue.h
 *
 *  Created on: Jul 5, 2025
 *      Author: sky
 */

#ifndef BLUE_H
#define BLUE_H

# include<stm32f4xx_hal.h>
# include "stm32f4xx_hal_uart.h"
# include <stdint.h>
# include <stdio.h>
# include <string.h>
# include "main.h"
# include "control.h"
# include "usart.h"


extern char openmv_msg[100], blue_msg[100]; // 用于存储OpenMV和蓝牙消息
extern char blue_feedback_msg[100]; // 用于反馈信息
extern float x_error; // 纯跟踪算法的x轴偏差

/*转换函数 ----------------------------------*/
void openmv_msg2x_error(char *msg);

#endif //BLUE_H
