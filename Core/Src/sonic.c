#include "ultrasonic.h"
#include "main.h"
#include "tim.h"
#include <stdio.h>

#define FRONT_TRIG_PORT  GPIOG
#define FRONT_TRIG_PIN   GPIO_PIN_2
#define LEFT_TRIG_PORT   GPIOG
#define LEFT_TRIG_PIN    GPIO_PIN_3
#define RIGHT_TRIG_PORT  GPIOG
#define RIGHT_TRIG_PIN   GPIO_PIN_4

#define TRIGGER_INTERVAL  50       // ms
#define MAX_DISTANCE      400.0f   // cm
#define INVALID_DISTANCE  -1.0f    // 无效距离标识

static uint8_t front_measComplete = 1;
static uint8_t left_measComplete  = 1;
static uint8_t right_measComplete = 1;

static uint8_t front_state = 0;
static uint8_t left_state  = 0;
static uint8_t right_state = 0;

static float front_dist = INVALID_DISTANCE;
static float left_dist  = INVALID_DISTANCE;
static float right_dist = INVALID_DISTANCE;

static uint32_t front_upEdge, front_downEdge;
static uint32_t left_upEdge,  left_downEdge;
static uint32_t right_upEdge, right_downEdge;

static uint32_t front_lastTrigTime = 0;
static uint32_t left_lastTrigTime  = 0;
static uint32_t right_lastTrigTime = 0;

void Ultrasonic_Init(void) {
    HAL_TIM_Base_Start(&htim2);
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3);
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_4);
    printf("Ultrasonic module initialized.\r\n");
}

void Ultrasonic_TriggerFront(void) {
    front_measComplete = 0;
    HAL_GPIO_WritePin(FRONT_TRIG_PORT, FRONT_TRIG_PIN, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(FRONT_TRIG_PORT, FRONT_TRIG_PIN, GPIO_PIN_RESET);
    front_lastTrigTime = HAL_GetTick();
}

void Ultrasonic_TriggerLeft(void) {
    left_measComplete = 0;
    HAL_GPIO_WritePin(LEFT_TRIG_PORT, LEFT_TRIG_PIN, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(LEFT_TRIG_PORT, LEFT_TRIG_PIN, GPIO_PIN_RESET);
    left_lastTrigTime = HAL_GetTick();
}

void Ultrasonic_TriggerRight(void) {
    right_measComplete = 0;
    HAL_GPIO_WritePin(RIGHT_TRIG_PORT, RIGHT_TRIG_PIN, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(RIGHT_TRIG_PORT, RIGHT_TRIG_PIN, GPIO_PIN_RESET);
    right_lastTrigTime = HAL_GetTick();
}

void Ultrasonic_Scheduler(void) {
    static uint8_t currentSensor = 0;
    static uint32_t lastTriggerTime = 0;
    uint32_t now = HAL_GetTick();

    switch (currentSensor) {
        case 0:
            if (front_measComplete && (now - lastTriggerTime > TRIGGER_INTERVAL)) {
                Ultrasonic_TriggerFront();
                lastTriggerTime = now;
                currentSensor = 1;
            }
            break;
        case 1:
            if (left_measComplete && (now - lastTriggerTime > TRIGGER_INTERVAL)) {
                Ultrasonic_TriggerLeft();
                lastTriggerTime = now;
                currentSensor = 2;
            }
            break;
        case 2:
            if (right_measComplete && (now - lastTriggerTime > TRIGGER_INTERVAL)) {
                Ultrasonic_TriggerRight();
                lastTriggerTime = now;
                currentSensor = 0;
            }
            break;
    }

    // 超时保护：超时后不修改距离，只标记测量完成，保持上一次有效距离
    if (!front_measComplete && now - front_lastTrigTime > 200) {
        front_measComplete = 1;
        // 保持 front_dist 不变
    }
    if (!left_measComplete && now - left_lastTrigTime > 200) {
        left_measComplete = 1;
        // 保持 left_dist 不变
    }
    if (!right_measComplete && now - right_lastTrigTime > 200) {
        right_measComplete = 1;
        // 保持 right_dist 不变
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim != &htim2) return;

    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
        uint32_t val = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        if (front_state == 0) {
            front_upEdge = val;
            front_state = 1;
        } else {
            front_downEdge = val;
            uint32_t pulse = (front_downEdge >= front_upEdge) ?
                (front_downEdge - front_upEdge) :
                (0xFFFFFFFF - front_upEdge + front_downEdge + 1);
            float distance_cm = pulse * 0.034f / 2.0f;
            front_dist = (distance_cm > MAX_DISTANCE) ? MAX_DISTANCE : distance_cm;
            front_state = 0;
            front_measComplete = 1;
        }
    }

    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
        uint32_t val = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
        if (left_state == 0) {
            left_upEdge = val;
            left_state = 1;
        } else {
            left_downEdge = val;
            uint32_t pulse = (left_downEdge >= left_upEdge) ?
                (left_downEdge - left_upEdge) :
                (0xFFFFFFFF - left_upEdge + left_downEdge + 1);
            float distance_cm = pulse * 0.034f / 2.0f;
            left_dist = (distance_cm > MAX_DISTANCE) ? MAX_DISTANCE : distance_cm;
            left_state = 0;
            left_measComplete = 1;
        }
    }

    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
        uint32_t val = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
        if (right_state == 0) {
            right_upEdge = val;
            right_state = 1;
        } else {
            right_downEdge = val;
            uint32_t pulse = (right_downEdge >= right_upEdge) ?
                (right_downEdge - right_upEdge) :
                (0xFFFFFFFF - right_upEdge + right_downEdge + 1);
            float distance_cm = pulse * 0.034f / 2.0f;
            right_dist = (distance_cm > MAX_DISTANCE) ? MAX_DISTANCE : distance_cm;
            right_state = 0;
            right_measComplete = 1;
        }
    }
}

float Ultrasonic_GetDistance(uint8_t sensorId) {
    float distance = 0.0f;
    __disable_irq();
    switch(sensorId) {
        case FRONT_SENSOR:  distance = front_dist; break;
        case LEFT_SENSOR:   distance = left_dist; break;
        case RIGHT_SENSOR:  distance = right_dist; break;
        default:            distance = INVALID_DISTANCE; break;
    }
    __enable_irq();
    return distance;
}

void Ultrasonic_PrintStatus(void) {
    printf("Distance(cm) - Front: %.2f, Left: %.2f, Right: %.2f\r\n",
           front_dist, left_dist, right_dist);
    printf("Measurement Flags - Front: %d, Left: %d, Right: %d\r\n",
           front_measComplete, left_measComplete, right_measComplete);
}
