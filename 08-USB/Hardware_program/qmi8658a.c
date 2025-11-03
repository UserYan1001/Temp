/****************************************************************************
 *	@笔者	：	YJH
 *	@日期	：	2025-09-09
 *	@所属	：	杭州星呗科技有限公司
 *****************************************************************************/
#include "qmi8658a.h"
//#include <stdio.h>
// 寄存器地址
#define QMI8658A_WHO_AM_I 0x00
#define QMI8658A_CTRL1    0x02
#define QMI8658A_CTRL2    0x03
#define QMI8658A_CTRL3    0x04
#define QMI8658A_CTRL5    0x06
#define QMI8658A_CTRL7    0x08
#define AccX_L 0x35
#define AccX_H 0x36
#define AccY_L 0x37
#define AccY_H 0x38
#define AccZ_L 0x39
#define AccZ_H 0x3A


#define GyrX_L 0x3B
#define GyrX_H 0x3C
#define GyrY_L 0x3D
#define GyrY_H 0x3E
#define GyrZ_L 0x3F
#define GyrZ_H 0x40

// 设备ID
#define QMI8658A_ID 0x05

#include "stdio.h"
// 转换因子
static const float acc_factor = 16384.0f;  // ±2g量程
static const float gyr_factor = 1.025f;   // ±2048°/s量程

/**
 * @brief  初始化QMI8658A传感器
 * @retval 0: 成功, 1: 失败
 */
uint8_t QMI8658A_Init(void) {
    uint8_t id;
    
    // 初始化IIC
    IIC_Init();
    
    // 读取设备ID
    id = QMI8658A_ReadReg(QMI8658A_WHO_AM_I);
		printf(" Id : %2d\r\n",id);
    if (id != QMI8658A_ID) {
        return 1; // 设备ID不正确
    }
    
		
    // 软件复位
    QMI8658A_WriteReg(QMI8658A_CTRL1, 0x80);
    Delay_ms(10);
    
    // 配置传感器: 使能加速度和陀螺仪，默认量程和输出速率
    QMI8658A_WriteReg(QMI8658A_CTRL1, 0x40);  
    QMI8658A_WriteReg(QMI8658A_CTRL2, 0x06);  // <±2g  125Hz>
		QMI8658A_WriteReg(QMI8658A_CTRL3, 0x76);  // < ±2048dps 125Hz>
    QMI8658A_WriteReg(QMI8658A_CTRL5, 0x11);  // <Enable Gyroscope Accelerometer 低通滤波>
		QMI8658A_WriteReg(QMI8658A_CTRL7, 0x03);  // 使能加速度和陀螺仪
		

    return 0;
}

// 定义采样次数
#define SAMPLE_COUNT 5
/**
 * @brief  直接读取所有传感器数据
 * @param  data: 存储数据的结构体指针
 * @retval 0: 成功, 1: 失败
 */
uint8_t QMI8658A_GetAllData(QMI8658A_Data *data) {
    
		uint8_t buf[14];
		

		// 解析加速度X轴数据（10次采样求平均）
		memset(buf, 0, sizeof(buf));
		int16_t ax_samples[SAMPLE_COUNT];
		for (int i = 0; i < SAMPLE_COUNT; i++) {
				QMI8658A_ReadRegs(AccX_L, buf, 2);
				ax_samples[i] = (int16_t)((buf[1] << 8) | buf[0]);
		}
		// 计算平均值
		int32_t ax_sum = 0;
		for (int i = 0; i < SAMPLE_COUNT; i++) {
				ax_sum += ax_samples[i];
		}
		data->ax = (float)(ax_sum / SAMPLE_COUNT) / acc_factor;

		// 解析加速度Y轴数据（10次采样求平均）
		memset(buf, 0, sizeof(buf));
		int16_t ay_samples[SAMPLE_COUNT];
		for (int i = 0; i < SAMPLE_COUNT; i++) {
				QMI8658A_ReadRegs(AccY_L, buf, 2);
				ay_samples[i] = (int16_t)((buf[1] << 8) | buf[0]);
		}
		// 计算平均值
		int32_t ay_sum = 0;
		for (int i = 0; i < SAMPLE_COUNT; i++) {
				ay_sum += ay_samples[i];
		}
		data->ay = (float)(ay_sum / SAMPLE_COUNT) / acc_factor;

		// 解析加速度Z轴数据（10次采样求平均）
		memset(buf, 0, sizeof(buf));
		int16_t az_samples[SAMPLE_COUNT];
		for (int i = 0; i < SAMPLE_COUNT; i++) {
				QMI8658A_ReadRegs(AccZ_L, buf, 2);
				az_samples[i] = (int16_t)((buf[1] << 8) | buf[0]);
		}
		// 计算平均值
		int32_t az_sum = 0;
		for (int i = 0; i < SAMPLE_COUNT; i++) {
				az_sum += az_samples[i];
		}
		data->az = (float)(az_sum / SAMPLE_COUNT) / acc_factor;

		// 解析陀螺仪X轴数据（10次采样求平均）
		memset(buf, 0, sizeof(buf));
		int16_t gx_samples[SAMPLE_COUNT];
		for (int i = 0; i < SAMPLE_COUNT; i++) {
				QMI8658A_ReadRegs(GyrX_L, buf, 2);
				gx_samples[i] = (int16_t)((buf[1] << 8) | buf[0]);
		}
		// 计算平均值
		int32_t gx_sum = 0;
		for (int i = 0; i < SAMPLE_COUNT; i++) {
				gx_sum += gx_samples[i];
		}
		data->gx = (float)(gx_sum / SAMPLE_COUNT) / gyr_factor;

		// 解析陀螺仪Y轴数据（10次采样求平均）
		memset(buf, 0, sizeof(buf));
		int16_t gy_samples[SAMPLE_COUNT];
		for (int i = 0; i < SAMPLE_COUNT; i++) {
				QMI8658A_ReadRegs(GyrY_L, buf, 2);
				gy_samples[i] = (int16_t)((buf[1] << 8) | buf[0]);
		}
		// 计算平均值
		int32_t gy_sum = 0;
		for (int i = 0; i < SAMPLE_COUNT; i++) {
				gy_sum += gy_samples[i];
		}
		data->gy = (float)(gy_sum / SAMPLE_COUNT) / gyr_factor;

		// 解析陀螺仪Z轴数据（10次采样求平均）
		memset(buf, 0, sizeof(buf));
		int16_t gz_samples[SAMPLE_COUNT];
		for (int i = 0; i < SAMPLE_COUNT; i++) {
				QMI8658A_ReadRegs(GyrZ_L, buf, 2);
				gz_samples[i] = (int16_t)((buf[1] << 8) | buf[0]);
		}
		// 计算平均值
		int32_t gz_sum = 0;
		for (int i = 0; i < SAMPLE_COUNT; i++) {
				gz_sum += gz_samples[i];
		}
		data->gz = (float)(gz_sum / SAMPLE_COUNT) / gyr_factor;

    
    return 0;
}

void Get_Gyr_Z(QMI8658A_Data *data)
{
	uint8_t buf[14];
		memset(buf, 0, sizeof(buf));
	int16_t gz_samples[SAMPLE_COUNT];
	for (int i = 0; i < SAMPLE_COUNT; i++) {
			QMI8658A_ReadRegs(GyrZ_L, buf, 2);
			gz_samples[i] = (int16_t)((buf[1] << 8) | buf[0]);
	}
	// 计算平均值
	int32_t gz_sum = 0;
	for (int i = 0; i < SAMPLE_COUNT; i++) {
			gz_sum += gz_samples[i];
	}
	data->gz = (float)(gz_sum / SAMPLE_COUNT) / gyr_factor;
}


/**
 * @brief  向QMI8658A写入一个寄存器
 * @param  reg: 寄存器地址
 * @param  data: 要写入的数据
 * @retval 0: 成功, 1: 失败
 */
static uint8_t QMI8658A_WriteReg(uint8_t reg, uint8_t data) {
    IIC_Start();
    IIC_Send_Byte(QMI8658A_ADDR);
    if (IIC_Wait_Ack()) { IIC_Stop(); return 1; }
    
    IIC_Send_Byte(reg);
    if (IIC_Wait_Ack()) { IIC_Stop(); return 1; }
    
    IIC_Send_Byte(data);
    if (IIC_Wait_Ack()) { IIC_Stop(); return 1; }
    
    IIC_Stop();
    return 0;
}

/**
 * @brief  从QMI8658A读取一个寄存器
 * @param  reg: 寄存器地址
 * @retval 读取到的数据
 */
static uint8_t QMI8658A_ReadReg(uint8_t reg) {
    uint8_t data;
    
    IIC_Start();
    IIC_Send_Byte(QMI8658A_ADDR);
    if (IIC_Wait_Ack()) { IIC_Stop(); return 0xFF; }
    IIC_Send_Byte(reg);
    if (IIC_Wait_Ack()) { IIC_Stop(); return 0xFF; }
    IIC_Start();
    IIC_Send_Byte(QMI8658A_ADDR | 0x01);
    if (IIC_Wait_Ack()) { IIC_Stop(); return 0xFF; }
    
    data = IIC_Read_Byte(0);
    IIC_Stop();
    
    return data;
}

/**
 * @brief  从QMI8658A读取多个寄存器
 * @param  reg: 起始寄存器地址
 * @param  buf: 存储读取数据的缓冲区
 * @param  len: 要读取的字节数
 * @retval 0: 成功, 1: 失败
 */
static uint8_t QMI8658A_ReadRegs(uint8_t reg, uint8_t *buf, uint8_t len) {
    uint8_t i;
    
    IIC_Start();
    IIC_Send_Byte(QMI8658A_ADDR);
    if (IIC_Wait_Ack()) { IIC_Stop(); return 1; }
    
    IIC_Send_Byte(reg);
    if (IIC_Wait_Ack()) { IIC_Stop(); return 1; }
    
    IIC_Start();
    IIC_Send_Byte(QMI8658A_ADDR | 0x01);
    if (IIC_Wait_Ack()) { IIC_Stop(); return 1; }
    
    for (i = 0; i < len; i++) {
        buf[i] = IIC_Read_Byte(i == len - 1 ? 0 : 1);
    }
    
    IIC_Stop();
    return 0;
}


