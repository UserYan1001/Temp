/****************************************************************************
 *	@笔者	：	YJH
 *	@日期	：	2025-09-09
 *	@所属	：	杭州星呗科技有限公司
 *****************************************************************************/
#include "motor.h"

PID_HandleTypeDef motor_A_pid;
PID_HandleTypeDef motor_B_pid;
PID_HandleTypeDef motor_C_pid;
PID_HandleTypeDef motor_D_pid;
extern int reset_flag_A;
extern int reset_flag_B;
extern int reset_flag_C;
extern int reset_flag_D;

/**
 * @简  述  电机PID控制函数
 * @参  数  spd_target:编码器速度目标值 ,范围（±250）
 *          spd_current: 编码器速度当前值
 * @返回值  电机PWM速度
 */



int16_t SPEED_PidCtlA(float spd_target, float spd_current)
{
	static int16_t motor_pwm_out;
	static float bias, bias_last;
	static float integral;
	
	if(reset_flag_A == 1)
	{
		bias = 0;
		bias_last = 0;
		motor_pwm_out = 0;
		integral = 0;
		reset_flag_A = 0;
	}
	
	// 获得偏差值
	bias = spd_target - spd_current;
	
	integral += bias;
	
		
	// PID计算电机输出PWM值
//	motor_pwm_out += motor_kp * bias + motor_kd * (bias - bias_last);
	motor_pwm_out += motor_kp * bias + motor_ki * integral / 10.0f + motor_kd * (bias - bias_last);
	// 记录上次偏差
	bias_last = bias;
	// 限制最大输出
	if (motor_pwm_out > 3500)
		motor_pwm_out = 3500;
	if (motor_pwm_out < -3500)
		motor_pwm_out = -3500;

	// 返回PWM控制值
	return motor_pwm_out;
}

/**
 * @简  述  电机PID控制函数
 * @参  数  spd_target:编码器速度目标值
 *          spd_target: 编码器速度当前值
 * @返回值  电机PWM速度
 */
int16_t SPEED_PidCtlB(float spd_target, float spd_current)
{
	static int16_t motor_pwm_out;
	static float bias, bias_last;
	static float integral;
	
	if(reset_flag_B == 1)
	{
		bias = 0;
		bias_last = 0;
		motor_pwm_out = 0;
		integral = 0;
		reset_flag_B = 0;
	}
	
	// 获得偏差值
	bias = spd_target - spd_current;
	
	integral += bias;
	
		
	// PID计算电机输出PWM值
//	motor_pwm_out += motor_kp * bias + motor_kd * (bias - bias_last);
	motor_pwm_out += motor_kp * bias + motor_ki * integral / 10.0f + motor_kd * (bias - bias_last);
	// 记录上次偏差
	bias_last = bias;

	// 限制最大输出
	if (motor_pwm_out > 3500)
		motor_pwm_out = 3500;
	if (motor_pwm_out < -3500)
		motor_pwm_out = -3500;

	// 返回PWM控制值
	return motor_pwm_out;
}

/**
 * @简  述  电机PID控制函数
 * @参  数  spd_target:编码器速度目标值
 *          spd_target: 编码器速度当前值
 * @返回值  电机PWM速度
 */
int16_t SPEED_PidCtlC(float spd_target, float spd_current)
{
	static int16_t motor_pwm_out;
	static float bias, bias_last;
	static float integral;
	
	if(reset_flag_C == 1)
	{
		bias = 0;
		bias_last = 0;
		motor_pwm_out = 0;
		integral = 0;
		reset_flag_C = 0;
	}
	
	// 获得偏差值
	bias = spd_target - spd_current;
	
	integral += bias;
	
		
	// PID计算电机输出PWM值
//	motor_pwm_out += motor_kp * bias + motor_kd * (bias - bias_last);
	motor_pwm_out += motor_kp * bias + motor_ki * integral / 10.0f + motor_kd * (bias - bias_last);
	// 记录上次偏差
	bias_last = bias;

	// 限制最大输出
	if (motor_pwm_out > 3500)
		motor_pwm_out = 3500;
	if (motor_pwm_out < -3500)
		motor_pwm_out = -3500;

	// 返回PWM控制值
	return motor_pwm_out;
}

/**
 * @简  述  电机PID控制函数
 * @参  数  spd_target:编码器速度目标值
 *          spd_target: 编码器速度当前值
 * @返回值  电机PWM速度
 */
int16_t SPEED_PidCtlD(float spd_target, float spd_current)
{
	static int16_t motor_pwm_out;
	static float bias, bias_last;
	static float integral;
	
	if(reset_flag_D == 1)
	{
		bias = 0;
		bias_last = 0;
		motor_pwm_out = 0;
		integral = 0;
		reset_flag_D = 0;
	}
	
	// 获得偏差值
	bias = spd_target - spd_current;
	
	integral += bias;
	
		
	// PID计算电机输出PWM值
//	motor_pwm_out += motor_kp * bias + motor_kd * (bias - bias_last);
	motor_pwm_out += motor_kp * bias + motor_ki * integral / 10.0f + motor_kd * (bias - bias_last);
	// 记录上次偏差
	bias_last = bias;

	// 限制最大输出
	if (motor_pwm_out > 3500)
		motor_pwm_out = 3500;
	if (motor_pwm_out < -3500)
		motor_pwm_out = -3500;

	// 返回PWM控制值
	return motor_pwm_out;
}

/**
 * @简  述 电机PWM速度控制
 * @参  数 speed 电机转速数值，范围-4200~4200
 * @返回值 无
 */
void MOTOR_A_SetSpeed(int16_t speed)
{
	int16_t temp;

	temp = speed;

	if (temp > 4200)
		temp = 4200;
	if (temp < -4200)
		temp = -4200;

	if (temp > 0)
	{
		TIM_SetCompare1(TIM1, 4200);
		TIM_SetCompare2(TIM1, (4200 - temp));
	}
	else
	{
		TIM_SetCompare2(TIM1, 4200);
		TIM_SetCompare1(TIM1, (4200 + temp));
	}
}

/**
 * @简  述 电机PWM速度控制
 * @参  数 speed 电机转速数值，范围-4200~4200
 * @返回值 无
 */
void MOTOR_B_SetSpeed(int16_t speed)
{
	int16_t temp;

	temp = speed;

	if (temp > 4200)
		temp = 4200;
	if (temp < -4200)
		temp = -4200;

	if (temp > 0)
	{
		TIM_SetCompare3(TIM1, 4200);
		TIM_SetCompare4(TIM1, (4200 - temp));
	}
	else
	{
		TIM_SetCompare4(TIM1, 4200);
		TIM_SetCompare3(TIM1, (4200 + temp));
	}
}

/**
 * @简  述 电机PWM速度控制
 * @参  数 speed 电机转速数值，范围-4200~4200
 * @返回值 无
 */
void MOTOR_D_SetSpeed(int16_t speed)
{
	int16_t temp;

	temp = speed;

	if (temp > 4200)
		temp = 4200;
	if (temp < -4200)
		temp = -4200;

	if (temp > 0)
	{
		TIM_SetCompare1(TIM9, 4200);
		TIM_SetCompare2(TIM9, (4200 - temp));
	}
	else
	{
		TIM_SetCompare2(TIM9, 4200);
		TIM_SetCompare1(TIM9, (4200 + temp));
	}
}

/**
 * @简  述 电机PWM速度控制
 * @参  数 speed 电机转速数值，范围-4200~4200
 * @返回值 无
 */
void MOTOR_C_SetSpeed(int16_t speed)
{
	int16_t temp;

	temp = speed;

	if (temp > 4200)
		temp = 4200;
	if (temp < -4200)
		temp = -4200;

	if (temp > 0)
	{
		TIM_SetCompare1(TIM12, 4200);
		TIM_SetCompare2(TIM12, (4200 - temp));
	}
	else
	{
		TIM_SetCompare2(TIM12, 4200);
		TIM_SetCompare1(TIM12, (4200 + temp));
	}
}
// 四电机同步运行函数（修复负数方向问题）
// cntA, cntB, cntC, cntD: 分别对应四个电机的运行圈数(正数顺时针，负数逆时针)
void MOTORS_AllRunCircle(int8_t cntA, int8_t cntB, int8_t cntC, int8_t cntD)
{
    // 检查所有参数是否都为0，避免无效操作
    if (cntA == 0 && cntB == 0 && cntC == 0 && cntD == 0) {
        return;
    }
    
    const uint16_t full_speed = 1500;            // 全速参数
    const int32_t reduce_distance = ENCODER_PPR / 2;  // 减速切换距离（脉冲数）
    const uint16_t reduced_speed = 1500;          // 减速参数
    
    // 计算每个电机的目标位置
    const int32_t target_posA = cntA * ENCODER_PPR;
    const int32_t target_posB = cntB * ENCODER_PPR;
    const int32_t target_posC = cntC * ENCODER_PPR;
    const int32_t target_posD = cntD * ENCODER_PPR;
    
    // 计算每个电机的总距离和方向
    const uint32_t total_distanceA = abs(target_posA);
    const uint32_t total_distanceB = abs(target_posB);
    const uint32_t total_distanceC = abs(target_posC);
    const uint32_t total_distanceD = abs(target_posD);
    
    const u8 dirA_clockwise = (target_posA > 0);
    const u8 dirB_clockwise = (target_posB > 0);
    const u8 dirC_clockwise = (target_posC > 0);
    const u8 dirD_clockwise = (target_posD > 0);
    
    // 重置所有编码器计数
    ENCODER_A_SetCounter(0);
    ENCODER_B_SetCounter(0);
    ENCODER_C_SetCounter(0);
    ENCODER_D_SetCounter(0);
    
    // 初始速度设置（明确区分顺时针和逆时针方向）
    int16_t current_speedA = (cntA != 0) ? (dirA_clockwise ? -full_speed : full_speed) : 0;
    int16_t current_speedB = (cntB != 0) ? (dirB_clockwise ? -full_speed : full_speed) : 0;
    int16_t current_speedC = (cntC != 0) ? (dirC_clockwise ? -full_speed : full_speed) : 0;
    int16_t current_speedD = (cntD != 0) ? (dirD_clockwise ? -full_speed : full_speed) : 0;
    
    // 设置初始速度
    MOTOR_A_SetSpeed(current_speedA);
    MOTOR_B_SetSpeed(current_speedB);
    MOTOR_C_SetSpeed(current_speedC);
    MOTOR_D_SetSpeed(current_speedD);
    
    // 标记每个电机是否已到达目标位置
    u8 motorA_done = (cntA == 0);
    u8 motorB_done = (cntB == 0);
    u8 motorC_done = (cntC == 0);
    u8 motorD_done = (cntD == 0);
    
    while (!(motorA_done && motorB_done && motorC_done && motorD_done)) {
        // 获取每个电机的当前位置（绝对值，只关注距离）
        uint32_t current_distA = abs(ENCODER_A_GetCounter());
        uint32_t current_distB = abs(ENCODER_B_GetCounter());
        uint32_t current_distC = abs(ENCODER_C_GetCounter());
        uint32_t current_distD = abs(ENCODER_D_GetCounter());
        
        // 处理电机A
        if (!motorA_done) {
            int32_t remaining_distanceA = total_distanceA - current_distA;
            
            // 检查是否需要减速
            if (remaining_distanceA <= reduce_distance && 
                current_speedA != (dirA_clockwise ? -reduced_speed : reduced_speed)) {
                current_speedA = dirA_clockwise ? -reduced_speed : reduced_speed;
                MOTOR_A_SetSpeed(current_speedA);
            }
            
            // 检查是否到达目标位置（使用距离判断，与方向无关）
            if (current_distA >= total_distanceA) {
                MOTOR_A_SetSpeed(0);
                motorA_done = 1;
            }
        }
        
        // 处理电机B（与A相同逻辑）
        if (!motorB_done) {
            int32_t remaining_distanceB = total_distanceB - current_distB;
            
            if (remaining_distanceB <= reduce_distance && 
                current_speedB != (dirB_clockwise ? -reduced_speed : reduced_speed)) {
                current_speedB = dirB_clockwise ? -reduced_speed : reduced_speed;
                MOTOR_B_SetSpeed(current_speedB);
            }
            
            if (current_distB >= total_distanceB) {
                MOTOR_B_SetSpeed(0);
                motorB_done = 1;
            }
        }
        
        // 处理电机C（与A相同逻辑）
        if (!motorC_done) {
            int32_t remaining_distanceC = total_distanceC - current_distC;
            
            if (remaining_distanceC <= reduce_distance && 
                current_speedC != (dirC_clockwise ? -reduced_speed : reduced_speed)) {
                current_speedC = dirC_clockwise ? -reduced_speed : reduced_speed;
                MOTOR_C_SetSpeed(current_speedC);
            }
            
            if (current_distC >= total_distanceC) {
                MOTOR_C_SetSpeed(0);
                motorC_done = 1;
            }
        }
        
        // 处理电机D（与A相同逻辑）
        if (!motorD_done) {
            int32_t remaining_distanceD = total_distanceD - current_distD;
            
            if (remaining_distanceD <= reduce_distance && 
                current_speedD != (dirD_clockwise ? -reduced_speed : reduced_speed)) {
                current_speedD = dirD_clockwise ? -reduced_speed : reduced_speed;
                MOTOR_D_SetSpeed(current_speedD);
            }
            
            if (current_distD >= total_distanceD) {
                MOTOR_D_SetSpeed(0);
                motorD_done = 1;
            }
        }
    }
}




// 电机A单圈运行函数
// cnt: 运行圈数(正数顺时针，负数逆时针)
void MOTOR_A_RunCircle(int8_t cnt)
{
    // 检查参数有效性，避免无效操作
    if (cnt == 0) {
        return;
    }
    
    const uint16_t full_speed = 2000;           // 全速参数
    
    const int32_t reduce_distance = ENCODER_PPR / 2;       // 减速切换距离（脉冲数）
    const uint16_t reduced_speed = 200; // 减速参数
		
    int32_t current_pos = 0;                    // 当前位置
    const int32_t target_pos = cnt * ENCODER_PPR;  // 目标位置
    const uint32_t total_distance = abs(target_pos); // 总距离
    
    // 重置编码器计数
    ENCODER_A_SetCounter(0);
    
    // 初始速度设置为全速
    int16_t current_speed = (target_pos > 0) ? -full_speed : full_speed;
    MOTOR_A_SetSpeed(current_speed);
    
    while (1) {
        current_pos = ENCODER_A_GetCounter(); 
        
        // 计算剩余距离
        int32_t remaining_distance = total_distance - abs(current_pos);
        
        // 当剩余距离小于等于设定值时切换到减速模式
        if (remaining_distance <= reduce_distance && 
            current_speed != (target_pos > 0 ? -reduced_speed : reduced_speed)) {
            current_speed = (target_pos > 0) ? -reduced_speed : reduced_speed;
            MOTOR_A_SetSpeed(current_speed);
        }
        
        // 检查是否到达目标位置
        if ((target_pos > 0 && current_pos >= target_pos) || 
            (target_pos < 0 && current_pos <= target_pos)) {
            break;
        }
    }
    
    // 停止电机
    MOTOR_A_SetSpeed(0);
}
// 电机B单圈运行函数
// cnt: 运行圈数(正数顺时针，负数逆时针)
void MOTOR_B_RunCircle(int8_t cnt)
{
    // 检查参数有效性，避免无效操作
    if (cnt == 0) {
        return;
    }
    
    const uint16_t full_speed = 2000;           // 全速参数
    
    const int32_t reduce_distance = ENCODER_PPR / 2;       // 减速切换距离（脉冲数）
    const uint16_t reduced_speed = 200; // 减速参数
		
    int32_t current_pos = 0;                    // 当前位置
    const int32_t target_pos = cnt * ENCODER_PPR;  // 目标位置
    const uint32_t total_distance = abs(target_pos); // 总距离
    
    // 重置编码器计数
    ENCODER_B_SetCounter(0);
    
    // 初始速度设置为全速
    int16_t current_speed = (target_pos > 0) ? -full_speed : full_speed;
    MOTOR_B_SetSpeed(current_speed);
    
    while (1) {
        current_pos = ENCODER_B_GetCounter(); 
        
        // 计算剩余距离
        int32_t remaining_distance = total_distance - abs(current_pos);
        
        // 当剩余距离小于等于设定值时切换到减速模式
        if (remaining_distance <= reduce_distance && 
            current_speed != (target_pos > 0 ? -reduced_speed : reduced_speed)) {
            current_speed = (target_pos > 0) ? -reduced_speed : reduced_speed;
            MOTOR_B_SetSpeed(current_speed);
        }
        
        // 检查是否到达目标位置
        if ((target_pos > 0 && current_pos >= target_pos) || 
            (target_pos < 0 && current_pos <= target_pos)) {
            break;
        }
    }
    
    // 停止电机
    MOTOR_B_SetSpeed(0);
}
// 电机C单圈运行函数
// cnt: 运行圈数(正数顺时针，负数逆时针)
void MOTOR_C_RunCircle(int8_t cnt)
{
    // 检查参数有效性，避免无效操作
    if (cnt == 0) {
        return;
    }
    
    const uint16_t full_speed = 2000;           // 全速参数
    
    const int32_t reduce_distance = ENCODER_PPR / 2;       // 减速切换距离（脉冲数）
    const uint16_t reduced_speed = 200; // 减速参数
		
    int32_t current_pos = 0;                    // 当前位置
    const int32_t target_pos = cnt * ENCODER_PPR;  // 目标位置
    const uint32_t total_distance = abs(target_pos); // 总距离
    
    // 重置编码器计数
    ENCODER_C_SetCounter(0);
    
    // 初始速度设置为全速
    int16_t current_speed = (target_pos > 0) ? -full_speed : full_speed;
    MOTOR_C_SetSpeed(current_speed);
    
    while (1) {
        current_pos = ENCODER_C_GetCounter(); 
        
        // 计算剩余距离
        int32_t remaining_distance = total_distance - abs(current_pos);
        
        // 当剩余距离小于等于设定值时切换到减速模式
        if (remaining_distance <= reduce_distance && 
            current_speed != (target_pos > 0 ? -reduced_speed : reduced_speed)) {
            current_speed = (target_pos > 0) ? -reduced_speed : reduced_speed;
            MOTOR_C_SetSpeed(current_speed);
        }
        
        // 检查是否到达目标位置
        if ((target_pos > 0 && current_pos >= target_pos) || 
            (target_pos < 0 && current_pos <= target_pos)) {
            break;
        }
    }
    
    // 停止电机
    MOTOR_C_SetSpeed(0);
}
// 电机D单圈运行函数
// cnt: 运行圈数(正数顺时针，负数逆时针)
void MOTOR_D_RunCircle(int8_t cnt)
{
    // 检查参数有效性，避免无效操作
    if (cnt == 0) {
        return;
    }
    
    const uint16_t full_speed = 2000;           // 全速参数
    
    const int32_t reduce_distance = ENCODER_PPR / 2;       // 减速切换距离（脉冲数）
    const uint16_t reduced_speed = 200; // 减速参数
		
    int32_t current_pos = 0;                    // 当前位置
    const int32_t target_pos = cnt * ENCODER_PPR;  // 目标位置
    const uint32_t total_distance = abs(target_pos); // 总距离
    
    // 重置编码器计数
    ENCODER_D_SetCounter(0);
    
    // 初始速度设置为全速
    int16_t current_speed = (target_pos > 0) ? -full_speed : full_speed;
    MOTOR_D_SetSpeed(current_speed);
    
    while (1) {
        current_pos = ENCODER_D_GetCounter(); 
        
        // 计算剩余距离
        int32_t remaining_distance = total_distance - abs(current_pos);
        
        // 当剩余距离小于等于设定值时切换到减速模式
        if (remaining_distance <= reduce_distance && 
            current_speed != (target_pos > 0 ? -reduced_speed : reduced_speed)) {
            current_speed = (target_pos > 0) ? -reduced_speed : reduced_speed;
            MOTOR_D_SetSpeed(current_speed);
        }
        
        // 检查是否到达目标位置
        if ((target_pos > 0 && current_pos >= target_pos) || 
            (target_pos < 0 && current_pos <= target_pos)) {
            break;
        }
    }
    
    // 停止电机
    MOTOR_D_SetSpeed(0);
}

// 计算电机A的转速
float Calculate_Motor_A_RPM(void)
{
			// 转速测量变量
		static uint32_t last_time = 0;               // 上一次测速时间
		static int16_t last_count_A = 0;             // 上一次编码器计数
    uint32_t current_time = millis();  // 获取当前系统时间(ms)
    int16_t current_count = ENCODER_A_GetCounter();
    
    // 计算时间差(秒)
    float time_diff = (current_time - last_time) / 1000.0f;
		static float motor_A_rpm = 0;
    float motor_A_rpm_t;
    
    // 只在时间差大于0.1秒时计算，避免除以零
    if(time_diff > 0.1f)
    {
        // 计算脉冲差
        int32_t count_diff = current_count - last_count_A;
        
        // 计算转速 RPM = (脉冲差 / PPR) / (时间差/60)，符号根据电机转向调整
        motor_A_rpm_t = (count_diff / (float)ENCODER_PPR) / (time_diff / 60.0f) * -1;
        
        // 限制异常值（可根据实际情况调整范围）
        if(fabs(motor_A_rpm_t) < 500)
        {
            motor_A_rpm = motor_A_rpm_t;
        }
        
        // 保存当前值作为下次计算的基准
        last_time = current_time;
        last_count_A = current_count;
				
    }
		return  motor_A_rpm;
							  
}

// 计算电机B的转速
float Calculate_Motor_B_RPM(void)
{
			// 转速测量变量
		static uint32_t last_time = 0;               // 上一次测速时间
		static int16_t last_count_B = 0;             // 上一次编码器计数
    uint32_t current_time = millis();  // 获取当前系统时间(ms)
    int16_t current_count = ENCODER_B_GetCounter();
    
    // 计算时间差(秒)
    float time_diff = (current_time - last_time) / 1000.0f;
		static float motor_B_rpm = 0;
    float motor_B_rpm_t;
    
    // 只在时间差大于0.1秒时计算，避免除以零
    if(time_diff > 0.1f)
    {
        // 计算脉冲差
        int32_t count_diff = current_count - last_count_B;
        
        // 计算转速 RPM = (脉冲差 / PPR) / (时间差/60)，符号根据电机转向调整
        motor_B_rpm_t = (count_diff / (float)ENCODER_PPR) / (time_diff / 60.0f) * -1;
        
        // 限制异常值（可根据实际情况调整范围）
        if(fabs(motor_B_rpm_t) < 500)
        {
            motor_B_rpm = motor_B_rpm_t;
        }
        
        // 保存当前值作为下次计算的基准
        last_time = current_time;
        last_count_B = current_count;
				
    }
		return motor_B_rpm;
}

// 计算电机C的转速
float Calculate_Motor_C_RPM(void)
{
			// 转速测量变量
		static uint32_t last_time = 0;               // 上一次测速时间
		static int16_t last_count_C = 0;             // 上一次编码器计数
    uint32_t current_time = millis();  // 获取当前系统时间(ms)
    int16_t current_count = ENCODER_C_GetCounter();
    
    // 计算时间差(秒)
    float time_diff = (current_time - last_time) / 1000.0f;
		static float motor_C_rpm = 0;
    float motor_C_rpm_t;
    
    // 只在时间差大于0.1秒时计算，避免除以零
    if(time_diff > 0.1f)
    {
        // 计算脉冲差
        int32_t count_diff = current_count - last_count_C;
        
        // 计算转速 RPM = (脉冲差 / PPR) / (时间差/60)，符号根据电机转向调整
        motor_C_rpm_t = (count_diff / (float)ENCODER_PPR) / (time_diff / 60.0f) * -1;
        
        // 限制异常值（可根据实际情况调整范围）
        if(fabs(motor_C_rpm_t) < 500)
        {
            motor_C_rpm = motor_C_rpm_t;
        }
        
        // 保存当前值作为下次计算的基准
        last_time = current_time;
        last_count_C = current_count;
				
    }
		return motor_C_rpm;
}


// 计算电机D的转速
float Calculate_Motor_D_RPM(void)
{
			// 转速测量变量
		static uint32_t last_time = 0;               // 上一次测速时间
		static int16_t last_count_D = 0;             // 上一次编码器计数
    uint32_t current_time = millis();  // 获取当前系统时间(ms)
    int16_t current_count = ENCODER_D_GetCounter();
    
    // 计算时间差(秒)
    float time_diff = (current_time - last_time) / 1000.0f;
		static float motor_D_rpm = 0;
    float motor_D_rpm_t;
    
    // 只在时间差大于0.1秒时计算，避免除以零
    if(time_diff > 0.1f)
    {
        // 计算脉冲差
        int32_t count_diff = current_count - last_count_D;
        
        // 计算转速 RPM = (脉冲差 / PPR) / (时间差/60)，符号根据电机转向调整
        motor_D_rpm_t = (count_diff / (float)ENCODER_PPR) / (time_diff / 60.0f) * -1;
        
        // 限制异常值（可根据实际情况调整范围）
        if(fabs(motor_D_rpm_t) < 500)
        {
            motor_D_rpm = motor_D_rpm_t;
        }
        
        // 保存当前值作为下次计算的基准
        last_time = current_time;
        last_count_D = current_count;
				
    }
		return motor_D_rpm;
}


// 电机A速度控制函数
float MOTOR_A_SpeedControl(float target_rpm)
{
		float motor_A_rpm = Calculate_Motor_A_RPM();
    // 设置目标转速
    PID_SetTarget(&motor_A_pid, target_rpm);
    
    // 计算PID输出
    int16_t output = PID_Compute(&motor_A_pid, motor_A_rpm);
    
    // 应用输出到电机
    MOTOR_A_SetSpeed(output);
		return motor_A_rpm;
}


// 电机B速度控制函数
float MOTOR_B_SpeedControl(float target_rpm)
{
		float motor_B_rpm = Calculate_Motor_B_RPM();
    // 设置目标转速
    PID_SetTarget(&motor_B_pid, target_rpm);
    
    // 计算PID输出
    int16_t output = PID_Compute(&motor_B_pid, motor_B_rpm);
    
    // 应用输出到电机
    MOTOR_B_SetSpeed(output);
		return motor_B_rpm;
}


// 电机C速度控制函数
float MOTOR_C_SpeedControl(float target_rpm)
{
	
		float motor_C_rpm = Calculate_Motor_C_RPM();
    // 设置目标转速
    PID_SetTarget(&motor_C_pid, target_rpm);
    
    // 计算PID输出
    int16_t output = PID_Compute(&motor_C_pid, motor_C_rpm);
    
    // 应用输出到电机
    MOTOR_C_SetSpeed(output);
		return motor_C_rpm;
}

// 电机D速度控制函数
float MOTOR_D_SpeedControl(float target_rpm)
{
	
		float motor_D_rpm = Calculate_Motor_D_RPM();
    // 设置目标转速
    PID_SetTarget(&motor_D_pid, target_rpm);
    
    // 计算PID输出
    int16_t output = PID_Compute(&motor_D_pid, motor_D_rpm);
    
    // 应用输出到电机
    MOTOR_D_SetSpeed(output);
		return motor_D_rpm;
}

