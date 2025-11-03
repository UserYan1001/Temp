#ifndef __QMI8658A_H
#define __QMI8658A_H

#include "stm32f4xx.h"
#include "myiic.h"

// QMI8658A设备地址
#define QMI8658A_ADDR 0x6B << 1  // 假设SA0接GND

// 传感器数据结构体
typedef struct {
    float ax;  // 加速度X轴 (g)
    float ay;  // 加速度Y轴 (g)
    float az;  // 加速度Z轴 (g)
    float gx;  // 陀螺仪X轴 (°/s)
    float gy;  // 陀螺仪Y轴 (°/s)
    float gz;  // 陀螺仪Z轴 (°/s)
    float temp;// 温度 (°C)
} QMI8658A_Data;

uint8_t QMI8658A_Init(void) ;
uint8_t QMI8658A_GetAllData(QMI8658A_Data *data) ;
static uint8_t QMI8658A_WriteReg(uint8_t reg, uint8_t data) ;
static uint8_t QMI8658A_ReadReg(uint8_t reg) ;
static uint8_t QMI8658A_ReadRegs(uint8_t reg, uint8_t *buf, uint8_t len) ;
void Get_Gyr_Z(QMI8658A_Data *data);
#endif



