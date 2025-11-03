/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __Y_MOTOR_H
#define __Y_MOTOR_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "main.h"



// 编码器每转脉冲数
#define ENCODER_PPR 1555   // 26 * 2 * 30



//电机PID闭环速度控制函数
int16_t SPEED_PidCtlA(float spd_target, float spd_current);   //PID控制函数，电机A
int16_t SPEED_PidCtlB(float spd_target, float spd_current);    //PID控制函数，电机B
int16_t SPEED_PidCtlC(float spd_target, float spd_current);    //PID控制函数，电机C
int16_t SPEED_PidCtlD(float spd_target, float spd_current);    //PID控制函数，电机D

void SPEED_PidResetA(void);
void SPEED_PidResetB(void);
void SPEED_PidResetC(void);
void SPEED_PidResetD(void);

void MOTOR_A_SetSpeed(int16_t speed);   //电机A控制
void MOTOR_B_SetSpeed(int16_t speed);   //电机B控制

void MOTOR_C_SetSpeed(int16_t speed);   //电机C控制
void MOTOR_D_SetSpeed(int16_t speed);   //电机D控制



void MOTORS_AllRunCircle(int8_t cntA, int8_t cntB, int8_t cntC, int8_t cntD);//四个电机一起动
void MOTOR_A_RunCircle(int8_t cnt);		//电机A单圈运行
void MOTOR_B_RunCircle(int8_t cnt);		//电机B单圈运行
void MOTOR_C_RunCircle(int8_t cnt);		//电机C单圈运行
void MOTOR_D_RunCircle(int8_t cnt);		//电机D单圈运行

float Calculate_Motor_A_RPM(void);			// 计算电机A的转速
float Calculate_Motor_B_RPM(void);			// 计算电机B的转速
float Calculate_Motor_C_RPM(void);			// 计算电机C的转速
float Calculate_Motor_D_RPM(void);			// 计算电机D的转速

float MOTOR_A_SpeedControl(float target_rpm);		//电机A单PID控制
float MOTOR_B_SpeedControl(float target_rpm);		//电机B单PID控制
float MOTOR_C_SpeedControl(float target_rpm);		//电机C单PID控制
float MOTOR_D_SpeedControl(float target_rpm);		//电机D单PID控制





#endif

/******************* (C) 版权 2022 XTARK **************************************/
