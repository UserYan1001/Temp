#include "KalmanFilter.h"
#include "qmi8658a.h"


float gyro_offset_gx = 0.0f;
float gyro_offset_gy = 0.0f;
float gyro_offset_gz = 0.0f;
float z_angle = 0.0f;


KalmanFilter kf_x = {0.1f, 1.0f, 1.0f, 0.0f, 0.0f};
KalmanFilter kf_y = {0.1f, 1.0f, 1.0f, 0.0f, 0.0f};
KalmanFilter kf_z = {0.1f, 1.0f, 1.0f, 0.0f, 0.0f};

// 卡尔曼滤波更新函数
float kalman_update(KalmanFilter *kf, float measurement) {
    // 预测步骤
    kf->P = kf->P + kf->Q;
    
    // 更新步骤
    kf->K = kf->P / (kf->P + kf->R);
    kf->estimate = kf->estimate + kf->K * (measurement - kf->estimate);
    kf->P = (1 - kf->K) * kf->P;
    
    return kf->estimate;
}

void Reduce_error(void) {
	
    QMI8658A_Data qdata_t;
    float sum_gx = 0.0f;
		float sum_gy = 0.0f;
		float sum_gz = 0.0f;
    int count = 5;
    
    // 等待传感器稳定
    Delay_ms(3000);
    
    // 累加1000次数据
    for (int i = 0; i < count; i++) {
        QMI8658A_GetAllData(&qdata_t);
        sum_gx += qdata_t.gx;
				sum_gy += qdata_t.gy;
				sum_gz += qdata_t.gz;
        Delay_ms(20); 
    }
    
    // 直接求均值作为零偏
    gyro_offset_gx = sum_gx / count;
		gyro_offset_gy = sum_gy / count;
		gyro_offset_gz = sum_gz / count;
}


// 更新Z轴角度
// 参数：gyro_z - Z轴陀螺仪数据
void Update_Angle_Z(float gyro_z)
{
    // 忽略微小变化和异常大值（可能是传感器错误）
    if ((gyro_z >= -15.0f && gyro_z <= 15.0f))
    {
        return;
    }
    
    // 陀螺仪数据转换（假设是度/秒）
    gyro_z /= 15.7f;
    
    unsigned long current_time = millis();
    static unsigned long last_time = 0; 
    float dt = 0.0f;
    
    // 首次调用初始化时间
    if (last_time == 0)
    {
        last_time = current_time;
        return;
    }
    
    // 计算时间间隔（秒）
    dt = (current_time - last_time) / 1000.0f;
    
    // 防止时间间隔过大导致的角度跳变
    if (dt > 0.5f)  // 超过0.5秒的数据不可靠
    {
        last_time = current_time;
        return;
    }
    
    // 计算角度变化量
    float angle_delta = gyro_z * dt;
    
    // 过滤过大的角度变化
//    if (angle_delta >= 50.0f || angle_delta <= -50.0f)
//    {
//        last_time = current_time;
//        return;
//    }
    
    // 更新角度并归一化到[-360, 360]范围
    z_angle += angle_delta;
    
    // 归一化角度（修正原逻辑错误，保持角度连续性）
    while (z_angle > 360.0f)  z_angle -= 360.0f;
    while (z_angle < -360.0f) z_angle += 360.0f;
    
    last_time = current_time;
}