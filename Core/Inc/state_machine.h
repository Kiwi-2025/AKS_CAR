//
// Created by ROG on 25-7-8.
//

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "main.h"

// 小车状态枚举
typedef enum {
    STATE_INIT,         // 初始化状态
    FETCHING,           // 取物状态
    OBSTACLE_AVOID,     // 避障状态
    PLACING,            // 放置状态
    FOLLOW_LINE,        // 巡线状态
    REACH_GOAL          // 到达目标
} CarState_t;

// 事件类型枚举
typedef enum {
    EVENT_NONE,
    EVENT_OBSTACLE_DETECTED,    // 检测到障碍物
    EVENT_OBSTACLE_CLEARED,     // 障碍物清除
    EVENT_BLUETOOTH_COMMAND,    // 蓝牙命令
    EVENT_TIMEOUT,             // 超时事件
    EVENT_EMERGENCY_STOP       // 紧急停止
} CarEvent_t;

// 传感器数据结构
typedef struct {
    float distance_front;      // 前方距离
    float distance_left;       // 左侧距离
    float distance_right;      // 右侧距离
    uint8_t bluetooth_cmd;     // 蓝牙命令
    uint32_t timestamp;        // 时间戳
} SensorData_t;

// 状态机函数声明
void StateMachine_Init(void);
void StateMachine_Update(void);
CarEvent_t StateMachine_GetEvent(SensorData_t *sensor_data);
void StateMachine_HandleState(CarState_t state, CarEvent_t event);

#endif //STATE_MACHINE_H
