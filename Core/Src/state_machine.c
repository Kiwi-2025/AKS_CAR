//
// Created by ROG on 25-7-8.
//
#include "state_machine.h"

// 全局变量
static CarState_t current_state = STATE_INIT; // 当前状态
static CarState_t next_state = STATE_INIT; // 下一个状态
static CarEvent_t current_event = EVENT_NONE; // 当前事件
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

void StateMachine_Update(void) {
    // 获取传感器数据
    StateMachine_GetEvent(&sensor_data);
    // 处理当前状态
    StateMachine_HandleState();
}

void StateMachine_GetEvent(SensorData_t *data) {
    if (strcmp(data->instruction, "FL") == 0) {
        current_event = EVENT_TASK1_BEGIN;
    }
    else if (strcmp(data->instruction, "OB") == 0) {
        current_event = EVENT_TASK2_BEGIN;
    }

}

// 状态处理函数
void StateMachine_HandleState(void) {
    switch (current_state) {
        case STATE_INIT:
            // 模拟初始化延时，给硬件一些缓冲时间
            HAL_Delay(1000);
            break;

        case FETCHING:
            if (current_event == EVENT_IS_FETCH) next_state = WAIT_FOR_MSG;
            break;

        case WAIT_FOR_MSG:
            if (current_event == EVENT_TASK1_BEGIN) {
                next_state = FOLLOW_LINE;
            }
            if (current_event == EVENT_TASK2_BEGIN) {
                next_state = OBSTACLE_AVOID;
            }
            break;

        case FOLLOW_LINE:
            if (current_event == EVENT_OBSERVE_GOAL) {
                next_state = PLACING;
            }
            break;
        case OBSTACLE_AVOID:
            if (current_event == EVENT_OBSERVE_GOAL) {
                next_state = PLACING;
            }
            break;
        case PLACING:
            if (current_event == EVENT_IS_PLACE) {
                next_state = FINISH;
            }
            break;
        case FINISH:
            break;
        default:
            next_state = current_state; // 保持当前状态
            //TODO: 可以添加错误处理逻辑，或者设置自动恢复状态
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
