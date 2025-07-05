/*
 * blue.c
 *
 *  Created on: Jul 5, 2025
 *      Author: sky
 */


#include "blue.h"
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart == &huart2) {
        // Process the received data
        HAL_UART_Transmit_DMA(&huart2, receiveData, Size); // Echo back the received data

        if (receiveData[0] == 0xAA){
          if (receiveData[1] == Size) {
            
              uint8_t sum = 0; 
              for (int i =0; i < Size -1; i++){
                sum += receiveData[i];
              }

              if (sum == receiveData[Size - 1]) {
                  for (int i = 2; i < Size - 1; i+=2) {
                      GPIO_PinState state = GPIO_PIN_SET;
                      if (receiveData[i+1] == 0xFF) {
                          state = GPIO_PIN_RESET; // Set pin high for 0x01
                      }
                      if (receiveData[i] == 0x01) {
                          HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, state); // Red
                          // HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, 1);
                          // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 1);
                          // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);
                      } else if (receiveData[i] == 0x02) {
                          // HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, 1); 
                          HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, state);// Blue
                          // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 1);
                          // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);
                      } else if (receiveData[i] == 0x03) {
                          // HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, 1); 
                          // HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, 1);
                          HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, state);// Green
                          // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1);
                      } else if (receiveData[i] == 0x04) {
                          // HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, 1); 
                          // HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, 1);
                          // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 1);
                          HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, state);// Yellow
                      } 
                  }
              }
            }
        }
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, receiveData, sizeof(receiveData)); // Restart receiving data in interrupt mode
        __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT); // Disable half-transfer interrupt to avoid triggering it
    }
}
