#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdint.h>
#include "main.h"  // 包含 GPIO、TIM 等硬件定义

// 传感器ID枚举，和实现文件保持一致
typedef enum {
    ULTRASONIC_FRONT = 0,  // 前方传感器，Trig1+Echo1
    ULTRASONIC_LEFT  = 1,  // 左侧传感器，Trig2+Echo2
    ULTRASONIC_RIGHT = 2   // 右侧传感器，Trig3+Echo3
} UltrasonicSensor_t;

// Echo引脚配置（与CubeMX对应）
#define FRONT_ECHO_PIN    GPIO_PIN_5    // TIM2_CH1, PA5
#define FRONT_ECHO_PORT   GPIOA
#define FRONT_ECHO_AF     GPIO_AF1_TIM2

#define LEFT_ECHO_PIN     GPIO_PIN_2    // TIM2_CH3, PA2
#define LEFT_ECHO_PORT    GPIOA
#define LEFT_ECHO_AF      GPIO_AF1_TIM2

#define RIGHT_ECHO_PIN    GPIO_PIN_3    // TIM2_CH4, PA3
#define RIGHT_ECHO_PORT   GPIOA
#define RIGHT_ECHO_AF     GPIO_AF1_TIM2

// Trig引脚定义，在 main.h 里已有定义
// #define Trig1_Pin GPIO_PIN_2
// #define Trig1_GPIO_Port GPIOG
// #define Trig2_Pin GPIO_PIN_3
// #define Trig2_GPIO_Port GPIOG
// #define Trig3_Pin GPIO_PIN_4
// #define Trig3_GPIO_Port GPIOG

/**
 * @brief 初始化所有超声波传感器相关硬件和定时器
 */
void Ultrasonic_Init(void);

/**
 * @brief 触发所有超声波传感器测距，开始采集距离
 *        (非阻塞，内部管理测距流程)
 */
void Ultrasonic_UpdateAll(void);

/**
 * @brief 获取指定传感器当前测得距离值（单位：毫米）
 * @param sensorId 传感器ID，取 ULTRASONIC_FRONT / LEFT / RIGHT
 * @return 距离值，单位mm。无效或超时可返回0或特殊值
 */
float Ultrasonic_GetDistance(uint8_t sensorId);

/**
 * @brief 打印当前所有传感器距离状态（调试用）
 */
void Ultrasonic_PrintStatus(void);

/**
 * @brief 超声波传感器调度函数，管理测距周期和状态机（如果有实现）
 */
void Ultrasonic_Scheduler(void);

#endif /* ULTRASONIC_H */
