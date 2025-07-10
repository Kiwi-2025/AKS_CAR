//
// Created by ROG on 25-7-7.
//

#ifndef CONTROL_H
#define CONTROL_H
extern int left_front_target, right_front_target, left_back_target, right_back_target;

# define  wheel_base      100.0;          // 前后轮距 (轴距)
# define  track_width    100.0;          // 左右轮距 (轮距)
# define  wheel_radius   65.0 ;           // 车轮半径

// TODO:最后将这部分参数根据模型直接写死

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
