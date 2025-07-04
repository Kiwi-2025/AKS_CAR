//
// Created by ROG on 25-7-3.
//

#ifndef MOTOR_H
#define MOTOR_H

// some usefull constants
#define reduction_ratio 21.3
#define pulse_num 11.0
#define diameter 65.0//mm
#define KP 0.9
#define KI 0.3
#define KD 0.1
#define pi 3.1416
#define maxspeed 800

int left_front_speed, right_front_speed;
int left_back_speed, right_back_speed;
int left_back_feedback,right_back_feedback;
int left_front_feedback,right_front_feedback;
int left_back_error, right_back_error;
int left_front_error, right_front_error;

void Motor_Init(void);

// Motor control functions
// Read the feedback from 4 motors encoders
int read_left_front_feedback(void);//read the left front motor's encoder
int read_right_front_feedback(void);//read the right front motor's encoder
int read_left_back_feedback(void);//read the left back motor's encoder
int read_right_back_feedback(void);//read the right back motor's encoder
// Use PID method to set the speed of 4 motors
int left_front_PID(int target_speed,int speed,int *error);//calculate adjustment of left front motor speed
int right_front_PID(int target_speed,int speed,int *error);//calculate adjustment of right front motor speed
int left_back_PID(int target_speed,int speed,int *error);//calculate adjustment of left back motor speed
int right_back_PID(int target_speed,int speed,int *error);//calculate adjustment of right back motor speed

// test function
void Motor_test(void);
void Motor_Read_Speed_test(void); // read the speed of 4 motors
#endif //MOTOR_H

