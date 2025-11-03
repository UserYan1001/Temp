#include "car.h"

void Car_Stop(void)
{
	MOTOR_A_SetSpeed(0);  // 执行速度控制
	MOTOR_B_SetSpeed(0);  // 执行速度控制
	MOTOR_C_SetSpeed(0); // 执行速度控制
	MOTOR_D_SetSpeed(0);  // 执行速度控制
}

void Car_Forward(float target_rpm)
{
	MOTOR_A_SpeedControl(target_rpm);  // 执行速度控制
	MOTOR_B_SpeedControl(-target_rpm);  // 执行速度控制
	MOTOR_C_SpeedControl(target_rpm);  // 执行速度控制
	MOTOR_D_SpeedControl(-target_rpm);  // 执行速度控制
}



void Car_Backward(float target_rpm)
{
	MOTOR_A_SpeedControl(-target_rpm);  // 执行速度控制
	MOTOR_B_SpeedControl(target_rpm);  // 执行速度控制
	MOTOR_C_SpeedControl(-target_rpm);  // 执行速度控制
	MOTOR_D_SpeedControl(target_rpm);  // 执行速度控制
}


void Car_Turn_Left(float target_rpm)
{
	
	MOTOR_A_SpeedControl(-target_rpm);  // 执行速度控制
	MOTOR_B_SpeedControl(-target_rpm);  // 执行速度控制
	MOTOR_C_SpeedControl(-target_rpm);  // 执行速度控制
	MOTOR_D_SpeedControl(-target_rpm);  // 执行速度控制
}


void Car_Turn_Right(float target_rpm)
{
	
	MOTOR_A_SpeedControl(target_rpm);  // 执行速度控制
	MOTOR_B_SpeedControl(target_rpm);  // 执行速度控制
	MOTOR_C_SpeedControl(target_rpm);  // 执行速度控制
	MOTOR_D_SpeedControl(target_rpm);  // 执行速度控制
}


void Car_Translate_Left(float target_rpm)
{                        
	
	MOTOR_A_SpeedControl(-target_rpm);  // 执行速度控制
	MOTOR_B_SpeedControl(-target_rpm);  // 执行速度控制
	MOTOR_C_SpeedControl(target_rpm);  // 执行速度控制
	MOTOR_D_SpeedControl(target_rpm);  // 执行速度控制
}

void Car_Translate_Right(float target_rpm)
{                        
	
	MOTOR_A_SpeedControl(target_rpm);  // 执行速度控制
	MOTOR_B_SpeedControl(target_rpm);  // 执行速度控制
	MOTOR_C_SpeedControl(-target_rpm);  // 执行速度控制
	MOTOR_D_SpeedControl(-target_rpm);  // 执行速度控制
}

void Car_Tast(float target_rpm)
{
	MOTOR_A_SpeedControl(0);  // 执行速度控制
	MOTOR_B_SpeedControl(-target_rpm);  // 执行速度控制
	MOTOR_C_SpeedControl(target_rpm);  // 执行速度控制
	MOTOR_D_SpeedControl(0);  // 执行速度控制
}
