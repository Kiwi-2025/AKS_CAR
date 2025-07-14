//
// Created by ROG on 25-7-8.
//

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <string.h>

#include "sonic.h"
# include "motor.h"
# include "servo.h"
# include "blue.h"
# include "control.h"
# include "fuzzy_control.h"

/* 小车状态 ---------------------------------------------------------*/
typedef enum {
    STATE_INIT,         // 初始化状态
    FETCHING,           // 取物状态
    WAIT_FOR_MSG,       // 等待蓝牙消息状态
    OBSTACLE_AVOID,     // 避障状态
    FOLLOW_LINE,        // 巡线状态
    PLACING,            // 放置状态
    FINISH              // 到达目标
    // TODO：添加自动回复状态，如果长时间未能够出现任何事件，则自动回复
} CarState_t;

/* 事件类型枚举 ---------------------------------------------------------*/
typedef enum {
    EVENT_NONE,
    EVENT_TASK1_BEGIN,      // 接收到任务1：巡线 开始事件
    EVENT_TASK2_BEGIN,      // 接收到任务2：避障 开始事件
    EVENT_IS_FETCH,         // 抓取到物体
    EVENT_OBSERVE_GOAL,     // 观察到结束目标
    EVENT_IS_PLACE          // 成功放置物体事件
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
void StateMachine_GetEvent(SensorData_t *sensor_data);
void StateMachine_HandleState(void);
CarState_t StateMachine_GetCurrentState(void);
#endif //STATE_MACHINE_H
