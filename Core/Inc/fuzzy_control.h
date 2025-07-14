//
// Created by Aether Li on 25-7-12.
//

#ifndef FUZZY_CONTROL_H
#define FUZZY_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "control.h"
#include <math.h>
#include <string.h>

/* 外部变量 --------------------------------------------------------------------------------*/
extern float left_front_target, right_front_target, left_back_target, right_back_target;
extern char msg[1024]; // 用于存储发送到蓝牙的数据
/**
 * @brief 模糊控制主函数：根据三个方向距离输入，输出四轮理论速度
 *
 * @param frontDist_cm   前方距离（单位：cm）
 * @param leftDist_cm    左前方距离（单位：cm）
 * @param rightDist_cm   右前方距离（单位：cm）
 * @param motion         输出结构体，包含四个轮子的目标速度（单位 mm/s）
 */
void FuzzyControl_Calculate(float frontDist_cm, float leftDist_cm, float rightDist_cm);

/**
 * @brief 替换模糊控制规则表（可选）
 *
 * @param newRules 四维规则表：维度为 [3][3][3][3]，对应：
 *        - 三个方向：front / left / right
 *        - 每个方向 3 个模糊等级：near / medium / far
 *        - 每条规则包含 [线速度比例, 角速度比例, 权重]
 */
void FuzzyControl_UpdateRules(float newRules[3][3][3][3]);

#ifdef __cplusplus
}
#endif

#endif /* FUZZY_CONTROL_H */
