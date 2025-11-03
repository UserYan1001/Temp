#ifndef __PID_H
#define __PID_H

#include "main.h"




// 电机速度PID参数
#define MOTOR_KP  20.0f
#define MOTOR_KI  0.6f
#define MOTOR_KD  0.1f
#define MOTOR_MIN -4200   // 电机最小输出值
#define MOTOR_MAX 4200    // 电机最大输出值

// PID控制器结构体
typedef struct {
    // PID参数
    float kp;               // 比例系数
    float ki;               // 积分系数
    float kd;               // 微分系数
    
    // 输出限制
    int16_t output_min;     // 最小输出
    int16_t output_max;     // 最大输出
    
    // 内部变量
    float target;           // 目标值
    float current;          // 当前值
    float error;            // 当前误差
    float last_error;       // 上一次误差
    float integral;         // 积分项
    float derivative;       // 微分项
    int16_t output;         // 输出值
} PID_HandleTypeDef;



// 函数声明
void PID_Init(PID_HandleTypeDef *pid, float kp, float ki, float kd, 
             int16_t output_min, int16_t output_max);
void PID_SetTarget(PID_HandleTypeDef *pid, float target);
int16_t PID_Compute(PID_HandleTypeDef *pid, float current);
void PID_Reset(PID_HandleTypeDef *pid);

#endif /* __PID_H */
