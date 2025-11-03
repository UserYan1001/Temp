#ifndef __DRIVE_MOTOR_H
#define __DRIVE_MOTOR_H

#include "Sys.h"    

void MOTOR_AB_Init(void); //电机PWM控制初始化
void MOTOR_CD_Init(void); //电机PWM控制初始化
void MOTOR_AB_Init(void);
void MOTOR_CD_Init(void);
void ENCODER_A_Init(void);
short ENCODER_A_GetCounter(void);
void ENCODER_A_SetCounter(uint16_t count);
void ENCODER_B_Init(void);
short ENCODER_B_GetCounter(void);
void ENCODER_B_SetCounter(uint16_t count);
void ENCODER_C_Init(void);
short ENCODER_C_GetCounter(void);
void ENCODER_C_SetCounter(uint16_t count);
void ENCODER_D_Init(void);
short ENCODER_D_GetCounter(void);
void ENCODER_D_SetCounter(uint16_t count);
uint32_t TIM_IsActiveFlag_UPDATE(TIM_TypeDef *TIMx);
uint32_t TIM_IsEnabledIT_UPDATE(TIM_TypeDef *TIMx);
uint32_t TIM_GetDirection(TIM_TypeDef *TIMx);

#endif
