//
// Created by ROG on 25-7-8.
//
#include "state_machine.h"
#include "sonic.h"
#include "motor.h"
#include "control.h"
#include "blue.h"

// 全局变量
static CarState_t current_state = STATE_INIT; // 当前状态
static uint32_t state_start_time = 0;
static SensorData_t sensor_data;

// 状态机配置参数
#define OBSTACLE_THRESHOLD 20.0f    // 障碍物距离阈值(cm)
#define TURN_TIMEOUT 2000          // 转向超时时间(ms)
#define AVOID_TIMEOUT 3000         // 避障超时时间(ms)

// 状态机初始化
void StateMachine_Init() {
    current_state = STATE_INIT;
    state_start_time = HAL_GetTick();
}

// 状态机主循环更新函数
void StateMachine_Update(void) {
    // 获取传感器数据

    // 获取当前事件
    CarEvent_t event = StateMachine_GetEvent(&sensor_data);

    // 处理状态转换
    StateMachine_HandleState(current_state, event);
}



// 状态处理函数
void StateMachine_HandleState(CarState_t state, CarEvent_t event) {
    CarState_t next_state = current_state;

    switch (current_state) {
        case STATE_INIT:

            break;
        case FETCHING:

            break;
        case OBSTACLE_AVOID:

            break;
        case PLACING:

            break;
        case FOLLOW_LINE:

            break;
        case REACH_GOAL:

            break;
        default:

    }

    // 状态切换
    if (next_state != current_state) {
        current_state = next_state;
        state_start_time = HAL_GetTick();
    }
}

// 获取当前状态（用于调试）
CarState_t StateMachine_GetCurrentState(void) {
    return current_state;
}