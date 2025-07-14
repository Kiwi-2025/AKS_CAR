//
// Created by ROG on 25-7-3.
//

#ifndef MOTOR_H
#define MOTOR_H

/* some usefull constants -----------------------------------------------*/
#define pi 3.1416
#define reduction_ratio 21.3        // 减速比
#define pulse_num       44.0        // 1圈脉冲数
#define diameter        66.0        // 车轮直径mm✔
#define read_period     0.2         // 读取编码器的周期s，与读取周期匹配即可
// 参考基准：KP = 0.7, KI = 0.035, KD = 0.23;
// #define KP 0.7
// #define KI 0.035
// #define KD 0.23
#define vel_tolerance 1.0f          // 速度误差容忍度 mm/s，将速度小于容忍度的速度视作刹车
#define maxspeed 900.0f                // 最大速度限制，保护电机

/* 定义电机PWM通道 ---------------------------------------------------*/
#define RIGHT_FRONT TIM_CHANNEL_1
#define LEFT_FRONT TIM_CHANNEL_2
#define RIGHT_BACK TIM_CHANNEL_3
#define LEFT_BACK TIM_CHANNEL_4

/* 定义电机GPIO口 ---------------------------------------------------*/

/* 外部变量 --------------------------------------------------------------------------------*/
extern float left_front_speed, right_front_speed, left_back_speed, right_back_speed;
extern float left_back_feedback, right_back_feedback, left_front_feedback, right_front_feedback;
extern float left_back_error, right_back_error, left_front_error, right_front_error;

void motor_init(void);
void motor_brake(void);
/* 硬件抽象 -------------------------------------------------------------*/
// Read the feedback from 4 motors encoders
float read_left_front_feedback(void); //read the left front motor's encoder
float read_right_front_feedback(void); //read the right front motor's encoder
float read_left_back_feedback(void); //read the left back motor's encoder
float read_right_back_feedback(void); //read the right back motor's encoder
// Use PID method to set the speed of 4 motors
float left_front_PID(float target_speed, float speed, float *error); //calculate adjustment of left front motor speed
float right_front_PID(float target_speed, float speed, float *error); //calculate adjustment of right front motor speed
float left_back_PID(float target_speed, float speed, float *error); //calculate adjustment of left back motor speed
float right_back_PID(float target_speed, float speed, float *error); //calculate adjustment of right back motor speed
// Set the PWM of 4 motors
void motor_vel(float left_front_speed, float right_front_speed, float left_back_speed, float right_back_speed);


// test function
void Motor_test(void);
float read_omega(void);
void Motor_Read_Speed_test(void); // read the speed of 4 motors
#endif //MOTOR_H
