//
// Created by ROG on 25-7-3.
//

#ifndef SERVO_H
#define SERVO_H
void Servo1_Init(void);
void Servo2_Init(void);

void Servo_catch(void);     //抓取物体
void Servo_turnUp(void);    //向上转动
void Servo_turnDown(void);  //向下转动
void Servo_put(void);       //放下物体

void Servo1_SetAngle(uint16_t angle);
void Servo2_SetAngle(uint16_t angle);

void Servo_test(void);
#endif //SERVO_H
