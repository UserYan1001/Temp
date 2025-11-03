#include "pid.h"
#include <math.h>

/**
 * @brief 初始化PID控制器
 * @param pid: PID结构体指针
 * @param kp: 比例系数
 * @param ki: 积分系数
 * @param kd: 微分系数
 * @param output_min: 最小输出限制
 * @param output_max: 最大输出限制
 */
void PID_Init(PID_HandleTypeDef *pid, float kp, float ki, float kd, 
             int16_t output_min, int16_t output_max) {
    if (pid == NULL) return;
    
    // 初始化PID参数
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    
    // 设置输出限制
    pid->output_min = output_min;
    pid->output_max = output_max;
    
    // 初始化内部变量
    PID_Reset(pid);
}

/**
 * @brief 设置PID控制器的目标值
 * @param pid: PID结构体指针
 * @param target: 目标值
 */
void PID_SetTarget(PID_HandleTypeDef *pid, float target) {
    if (pid == NULL) return;
    pid->target = target;
}

/**
 * @brief 计算PID输出
 * @param pid: PID结构体指针
 * @param current: 当前测量值
 * @return PID输出值
 */
int16_t PID_Compute(PID_HandleTypeDef *pid, float current) {
    if (pid == NULL) return 0;
    
    // 保存当前值
    pid->current = current;
    
    // 计算当前误差
    pid->error = pid->target - current;
    
    // 计算积分项 (带抗积分饱和)
    pid->integral += pid->error * pid->ki;
    
    // 积分限幅，防止积分饱和
    if (pid->integral > pid->output_max) {
        pid->integral = pid->output_max;
    } else if (pid->integral < pid->output_min) {
        pid->integral = pid->output_min;
    }
    
    // 计算微分项 (带一阶低通滤波思想，减少噪声影响)
    pid->derivative = pid->kd * (pid->error - pid->last_error);
    
    // 计算PID输出
    pid->output = pid->kp * pid->error + pid->integral + pid->derivative;
    
    // 输出限幅
    if (pid->output > pid->output_max) {
        pid->output = pid->output_max;
    } else if (pid->output < pid->output_min) {
        pid->output = pid->output_min;
    }
    
    // 保存当前误差用于下次计算微分
    pid->last_error = pid->error;
    if(pid->output < 100 && pid->output > -100)
			pid->output = 0;
    return (int16_t)pid->output;
}

/**
 * @brief 重置PID控制器内部状态
 * @param pid: PID结构体指针
 */
void PID_Reset(PID_HandleTypeDef *pid) {
    if (pid == NULL) return;
    
    pid->target = 0;
    pid->current = 0;
    pid->error = 0;
    pid->last_error = 0;
    pid->integral = 0;
    pid->derivative = 0;
    pid->output = 0;
}
