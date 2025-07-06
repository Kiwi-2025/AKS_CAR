/*
 * blue.h
 *
 *  Created on: Jul 5, 2025
 *      Author: sky
 */


#ifndef BLUE_H
#define BLUE_H

#include "stm32f4xx_hal.h"
#include "main.h"
#include <stdio.h>
#include <stdint.h>

extern UART_HandleTypeDef huart2;
void ReturnToBlue(uint8_t msg[5], float *value);

#endif //BLUE_H