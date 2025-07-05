/*
 * blue.c
 *
 *  Created on: Jul 5, 2025
 *      Author: sky
 */


#include "blue.h"
#include "usart.h"
#include "dma.h"
#include "main.h"

extern int velocity_msg_test;


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart == &huart2) {
        // Process the received data   
        HAL_UART_Transmit_DMA(&huart2, receiveData, Size); // Echo back the received data

        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, velocity_msg_test, sizeof(velocity_msg_test)); // Restart receiving data in interrupt mode
        __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT); // Disable half-transfer interrupt to avoid triggering it
    }
}
