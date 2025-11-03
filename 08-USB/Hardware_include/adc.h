#ifndef __ADC_H
#define __ADC_H

#include "stm32f4xx.h"
#include "main.h"
// 电池电压测量函数
void BatteryVoltage_Init(void);
uint16_t BatteryVoltage_GetRawAD(void);
uint16_t BatteryVoltage_GetVol_X100(void);

// 电机电流测量函数
void MotorCurrent_Init(void);
uint16_t MotorCurrent_PA2_GetRawAD(void);
uint16_t MotorCurrent_PA2_GetCurrent_X100(void);
uint16_t MotorCurrent_PA3_GetRawAD(void);
uint16_t MotorCurrent_PA3_GetCurrent_X100(void);
uint16_t MotorCurrent_PA4_GetRawAD(void);
uint16_t MotorCurrent_PA4_GetCurrent_X100(void);
uint16_t MotorCurrent_PC1_GetRawAD(void);
uint16_t MotorCurrent_PC1_GetCurrent_X100(void);

#endif /* __ADC_H */
