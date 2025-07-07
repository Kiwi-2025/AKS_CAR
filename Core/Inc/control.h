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

double roundPID(double targetposition,double position,double *error); //寻线误差的PID计算函数
void setspeed(int targetspeed,int curvature);  //根据运动需求设置两轮转速
void spin(short spin_dir);   //原地旋转运动函数
void groundturn(short spin_dir,short angle); //原地旋转固定角度
void alongside(double deviation); //巡线总函数
void avoidance();  //避障总函数
void fetchplace();  //自动取物放物控制函数
void avoid_stop();

void control_test(void); // 控制初始化函数
#endif //CONTROL_H
