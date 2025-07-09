//
// Created by ROG on 25-7-7.
//

#ifndef CONTROL_H
#define CONTROL_H
//int targetspeed;
extern int left_front_target, right_front_target, left_back_target, right_back_target;
// int curvature;
// double curv_error;
// extern double deviation;
// extern double deviation_f;
// extern int deviation_s;
// short is_fetch;
// short is_down;
// short is_shut;
// short is_groundturn;
// short is_stopping;
// short avoid_dir;
// short avoid_comfirm;
// short turn_flag;
// short stop_flag;
// short check_flag;
// short back_flag;
// int total_distance;
// short turn_target;

float wheelbase     = 100;          // 前后轮距 (轴距)
float track_width   = 100;          // 左右轮距 (轮距)
float wheel_radius  = 65;           // 车轮半径

// TODO:最后将这部分参数根据模型直接写死
// 小车几何参数
// 计算小车几何中心到各轮子的距离和角度
float half_wheelbase = wheelbase / 2.0f;
float half_track = track_width / 2.0f;
// 各轮子相对于小车几何中心的位置
float lf_x = wheelbase / 2.0f;          // 左前轮 x 坐标
float lf_y = track_width / 2.0f;        // 左前轮 y 坐标
float rf_x = wheelbase / 2.0f;          // 右前轮 x 坐标
float rf_y = -track_width / 2.0f;       // 右前轮 y 坐标
float lb_x = - wheelbase / 2.0f;        // 左后轮 x 坐标
float lb_y = track_width / 2.0f;        // 左后轮 y 坐标
float rb_x = - wheelbase / 2.0f;        // 右后轮 x 坐标
float rb_y = -track_width / 2.0f;       // 右后轮 y 坐标


double roundPID(double targetposition,double position,double *error); //寻线误差的PID计算函数
void set_speed(float linear_velocity, float turn_angle_degrees);  //根据运动需求设置两轮转速
void spin(short spin_dir);   //原地旋转运动函数
void groundturn(short spin_dir,short angle); //原地旋转固定角度
void alongside(double deviation); //巡线总函数
void avoidance();  //避障总函数
void fetchplace();  //自动取物放物控制函数
void avoid_stop();
void motor_pid_control(void);
#endif //CONTROL_H
