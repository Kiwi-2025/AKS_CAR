//
// Created by ROG on 25-7-12.
//
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : fuzzy_control.c
  * @brief          : 模糊控制模块实现文件
  ******************************************************************************
  */
/* USER CODE END Header */

#include "fuzzy_control.h"


/* 距离阈值（单位：mm） */
#define DISTANCE_NEAR     210.0f
#define DISTANCE_MEDIUM   350.0f
#define DISTANCE_FAR      600.0f

#define CRITICAL_DISTANCE 210.0f   // 正前避障触发阈值（mm）
#define WALL_TOO_CLOSE     60.0f   // 侧墙极限贴近判断（mm）

/* 小车运动参数 */
#define MAX_LINEAR_SPEED   400.0f   // 最大线速度 mm/s
#define MAX_ANGULAR_SPEED  3.0f     // 最大角速度 rad/s
#define WHEEL_BASE         159.0f   // 轮距（单位 mm）

/* 模糊规则表 [front][left][right][{linear_ratio, angular_ratio, weight}] */
static float rules[3][3][3][3] = {
    {
        {{-0.2f, -1.0f, 1.0f}, {-0.2f, -1.0f, 1.0f}, {-0.2f, -0.8f, 1.0f}},
        {{0.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 1.0f}, {0.2f, -0.8f, 1.0f}},
        {{0.0f, -0.6f, 1.0f}, {0.0f, -0.4f, 1.0f}, {0.2f, -0.2f, 1.0f}}
    },
    {
        {{0.2f, -0.8f, 1.0f}, {0.4f, -0.4f, 1.0f}, {0.6f, 0.0f, 1.0f}},
        {{0.4f, -0.4f, 1.0f}, {0.6f, 0.0f, 1.0f}, {0.6f, 0.0f, 1.0f}},
        {{0.6f, 0.4f, 1.0f}, {0.6f, 0.2f, 1.0f}, {0.6f, 0.0f, 1.0f}}
    },
    {
        {{0.7f, -0.2f, 1.0f}, {0.9f, -0.1f, 1.0f}, {1.0f, 0.0f, 1.0f}},
        {{0.8f, -0.1f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
        {{0.8f, 0.2f, 1.0f}, {0.9f, 0.3f, 1.0f}, {1.0f, 0.0f, 1.0f}}
    }
};

/* 模糊化函数 */
static void FuzzifyDistance(float dist, float *near, float *med, float *far) {
    *near = fmaxf(0.0f, fminf(1.0f, (DISTANCE_MEDIUM - dist) / (DISTANCE_MEDIUM - DISTANCE_NEAR)));
    *med = fmaxf(0.0f, fminf(1.0f, (dist - DISTANCE_NEAR) / (DISTANCE_MEDIUM - DISTANCE_NEAR)));
    *far = fmaxf(0.0f, fminf(1.0f, (dist - DISTANCE_MEDIUM) / (DISTANCE_FAR - DISTANCE_MEDIUM)));
}

/* 推理函数 */
static void EvaluateRules(float fn, float fm, float ff,
                          float ln, float lm, float lf,
                          float rn, float rm, float rf,
                          float *linear, float *angular) {
    float sumL = 0.0f, sumA = 0.0f, sumW = 0.0f;

    for (int f = 0; f < 3; ++f) {
        float mf = (f == 0) ? fn : (f == 1) ? fm : ff;
        for (int l = 0; l < 3; ++l) {
            float ml = (l == 0) ? ln : (l == 1) ? lm : lf;
            for (int r = 0; r < 3; ++r) {
                float mr = (r == 0) ? rn : (r == 1) ? rm : rf;
                float μ = mf * ml * mr * rules[f][l][r][2];
                sumL += μ * rules[f][l][r][0];
                sumA += μ * rules[f][l][r][1];
                sumW += μ;
            }
        }
    }

    if (sumW > 0.0f) {
        *linear = (sumL / sumW) * MAX_LINEAR_SPEED;
        *angular = (sumA / sumW) * MAX_ANGULAR_SPEED;
    } else {
        *linear = 0.0f;
        *angular = 0.0f;
    }
}

/* 更新模糊规则表（可选） */
void FuzzyControl_UpdateRules(float newRules[3][3][3][3]) {
    memcpy(rules, newRules, sizeof(rules));
}

/* 主控制接口（输入：cm；输出：mm/s） */
void FuzzyControl_Calculate(float front_cm, float left_cm, float right_cm) {
    float front = front_cm * 10.0f;
    float left = left_cm * 10.0f;
    float right = right_cm * 10.0f;

    float linear = 0.0f, angular = 0.0f;
    float fn, fm, ff, ln, lm, lf, rn, rm, rf;

    // 极端情况判断（墙角贴墙等）
    if (front < CRITICAL_DISTANCE || left < WALL_TOO_CLOSE || right < WALL_TOO_CLOSE) {
        linear = 0.0f;

        if (right < WALL_TOO_CLOSE && right < left) {
            angular = MAX_ANGULAR_SPEED; // 强制左转
        } else if (left < WALL_TOO_CLOSE && left < right) {
            angular = -MAX_ANGULAR_SPEED; // 强制右转
        } else {
            angular = (left > right) ? MAX_ANGULAR_SPEED : -MAX_ANGULAR_SPEED;
        }
    } else {
        // 进入模糊控制模块
        FuzzifyDistance(front, &fn, &fm, &ff);
        FuzzifyDistance(left, &ln, &lm, &lf);
        FuzzifyDistance(right, &rn, &rm, &rf);
        EvaluateRules(fn, fm, ff, ln, lm, lf, rn, rm, rf, &linear, &angular);

        // 若模糊线速度低于设定下限，提升线速度以保持流畅性
        if (linear < 400.0f) linear = 400.0f;
    }

    // 转换为左右轮速度（mm/s）
    float linear_cm_s = linear / 10.0f;
    float vl_cm_s = linear_cm_s - (angular * (WHEEL_BASE / 20.0f));
    float vr_cm_s = linear_cm_s + (angular * (WHEEL_BASE / 20.0f));
    float vl = vl_cm_s * 10.0f;
    float vr = vr_cm_s * 10.0f;

    // 限幅保护
    float maxSpeed = fmaxf(fabsf(vl), fabsf(vr));
    if (maxSpeed > MAX_LINEAR_SPEED) {
        float scale = MAX_LINEAR_SPEED / maxSpeed;
        vl *= scale;
        vr *= scale;
    }

    left_front_target = vl;
    left_back_target = vl;
    right_front_target = vr;
    right_back_target = vr;
}
