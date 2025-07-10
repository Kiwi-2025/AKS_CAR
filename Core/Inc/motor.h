//
// Created by ROG on 25-7-3.
//

#ifndef MOTOR_H
#define MOTOR_H

// some usefull constants
#define reduction_ratio 21.3 // 9.6:1
#define pulse_num 44.0
#define diameter 65.0       // 车轮直径 mm
#define KP 0.38
#define KI 0.00171
#define KD 0
#define pi 3.1416
#define maxspeed 1600 // 最大速度

#define LEFT_FRONT TIM_CHANNEL_1
#define RIGHT_FRONT TIM_CHANNEL_2
#define LEFT_BACK TIM_CHANNEL_3
#define RIGHT_BACK TIM_CHANNEL_4

extern float left_front_speed, right_front_speed, left_back_speed, right_back_speed;
extern int left_back_feedback,right_back_feedback;
extern int left_front_feedback,right_front_feedback;
extern int left_back_error, right_back_error;
extern int left_front_error, right_front_error;

// test used variants
void Motor_Init(void);

/* 硬件抽象 -------------------------------------------------------------*/
// Read the feedback from 4 motors encoders
float read_left_front_feedback(void);//read the left front motor's encoder
float read_right_front_feedback(void);//read the right front motor's encoder
float read_left_back_feedback(void);//read the left back motor's encoder
float read_right_back_feedback(void);//read the right back motor's encoder
// Use PID method to set the speed of 4 motors
int left_front_PID(int target_speed,int speed,int *error);//calculate adjustment of left front motor speed
int right_front_PID(int target_speed,int speed,int *error);//calculate adjustment of right front motor speed
int left_back_PID(int target_speed,int speed,int *error);//calculate adjustment of left back motor speed
int right_back_PID(int target_speed,int speed,int *error);//calculate adjustment of right back motor speed
// Set the PWM of 4 motors
void motor_vel(int left_front_speed,int right_front_speed,int left_back_speed,int right_back_speed);


// test function
void Motor_test(void);
float read_omega(void); //
void Motor_Read_Speed_test(void); // read the speed of 4 motors
#endif //MOTOR_H

