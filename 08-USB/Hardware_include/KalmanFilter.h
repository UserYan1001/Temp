#ifndef __KALMANFILTER_H
#define __KALMANFILTER_H

#include <stm32f4xx.h>

// 卡尔曼滤波器结构体
typedef struct {
    float Q; // 过程噪声方差
    float R; // 测量噪声方差
    float P; // 估计误差协方差
    float K; // 卡尔曼增益
    float estimate; // 估计值
} KalmanFilter;

// 为三个轴创建滤波器实例
extern KalmanFilter kf_x;
extern KalmanFilter kf_y;
extern KalmanFilter kf_z;


extern float gyro_offset_gx;
extern float gyro_offset_gy;
extern float gyro_offset_gz;
extern float z_angle;




// 卡尔曼滤波更新函数
float kalman_update(KalmanFilter *kf, float measurement);
void Reduce_error(void);
void Update_Angle_Z(float gyro_z);


#endif

