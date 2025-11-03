// ############################################################
//  FILE:  Task_manager.c
//  Created on: 2021年8月14日
//  Author: lee
//  summary: Task_manager
// ############################################################

#include "main.h"
#include "stdio.h"
#include "usbh_usr.h"

#define Task_Num 6

#define HFPeriod_COUNT 1	// 5ms
#define FaulPeriod_COUNT 10 // 10ms
#define Robot_COUNT 5		// 10ms
#define KEY_COUNT 80		// 10ms
#define LEDPeriod_COUNT 600 // 500ms
#define filter_N 12
#define IMU_COUNT 8	

extern __ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_Core_dev __ALIGN_END;
extern __ALIGN_BEGIN USBH_HOST USB_Host __ALIGN_END;

TaskTime TasksPare[Task_Num];
float dir_anglecmd = 135.0;

void Timer_Task_Count(void) // 1ms的中断计数
{
	u16 Task_Count = 0;

	for (Task_Count = 0; Task_Count < Task_Num; Task_Count++) // TASK_NUM=5
	{
		if ((TasksPare[Task_Count].Task_Count < TasksPare[Task_Count].Task_Period) && (TasksPare[Task_Count].Task_Period > 0))
		{
			TasksPare[Task_Count].Task_Count++; // 计数 事件任务计数
		}
	}
}

void Execute_Task_List_RUN(void)
{
		
	uint16_t Task_Count = 0;

	for (Task_Count = 0; Task_Count < Task_Num; Task_Count++)
	{
		if ((TasksPare[Task_Count].Task_Count >= TasksPare[Task_Count].Task_Period) && (TasksPare[Task_Count].Task_Period > 0))
		{
			TasksPare[Task_Count].Task_Function(); // 运行计数的时间任务函数
			TasksPare[Task_Count].Task_Count = 0;
		}
	}
}


//1ms进入一次（传感器应用（已注释）、USB获取数据、手柄数据处理、串口指令解析）
void HFPeriod_1msTask(void) // 备用
{
	//app_sensor_run();
	
	USBH_Process(&USB_OTG_Core_dev, &USB_Host);//USB获取数据
	app_ps2();//手柄数据处理
	
	if (uart1_get_ok) /* 接收指令 */
	{
		//printf("\r\n  app_uart_run %d= %s \r\n",uart1_get_ok, uart_receive_buf);
		if (uart1_mode == 1)
		{
			uart3_send_str((u8*)"cmdOk");
			// 命令模式
			parse_cmd(uart_receive_buf);
		}
		else if (uart1_mode == 2)
		{
			// 单个舵机调试
			parse_action(uart_receive_buf);
		}
		else if (uart1_mode == 3)
		{
			// 多路舵机调试
			parse_action(uart_receive_buf);
		}
		else if (uart1_mode == 4)
		{
			// 存储模式
			save_action(uart_receive_buf);
		}
		uart1_mode = 0;
		uart1_get_ok = 0;
		//printf("\r\n  app_uart_run %d= %s \r\n",uart1_get_ok, uart_receive_buf);
	}
	
}


//IMU获取数据
QMI8658A_Data qdata;
void IMU_Get_Data()
{
	static uint32_t previous_millis = 0;
	if(QMI8658A_GetAllData(&qdata) != 0)
		return ;
	// 对陀螺仪数据进行卡尔曼滤波
	float gx_filtered = kalman_update(&kf_x, qdata.gx);
	float gy_filtered = kalman_update(&kf_y, qdata.gy);
	float gz_filtered = kalman_update(&kf_z, qdata.gz);
	
	//解析角度只需要用Z轴角速度

	Update_Angle_Z(gz_filtered - gyro_offset_gz);//角速度转化为角度
	uint32_t current_millis = millis();
	if (current_millis - previous_millis >= 100)
	{
			previous_millis = current_millis;  // 更新上一次打印时间
//			printf("%.1f,%.1f,%.1f\n", 
//			 gx_filtered - gyro_offset_gx, gy_filtered - gyro_offset_gy, gz_filtered - gyro_offset_gz);
		
		//滤波前零漂矫正数据
//		printf("%.1f,%.1f,%.1f\n", 
//					 qdata.gx - gyro_offset_gx, qdata.gy - gyro_offset_gy, qdata.gz - gyro_offset_gz);

		
//		printf("Angle :%.1f\r\n",z_angle); 
	
	}
}



void task_send_Rece(void) 
{
	//动作组任务
	loop_action();
}

void Robot_Control(void) // 机器人控制任务，含电量管理
{
//	printf("mpu gy= %d   %d   %d     ac= %d    %d    %d \r\n", imu_gyro_data[0], imu_gyro_data[1], imu_gyro_data[2],
//		   imu_acc_data[0], imu_acc_data[1], imu_acc_data[2]);
	Robot_Task();
}

void KEY_RUN(void) // 外部按键处理任务
{
	
	//软件复位，按键二
	if(KEY_Scan() == 2)
	{
		while(KEY_Scan() == 2){}
		soft_reset();
	}
}

void Task_LED(void) // 最小系统500ms的LED的闪烁
{
	GPIO_ToggleBits(GPIOB, GPIO_Pin_6); //  500ms的LED的闪烁
}

void Task_Manage_List_Init(void)
{
	TasksPare[0].Task_Period = HFPeriod_COUNT; // PERIOD_COUNT=5    5ms
	TasksPare[0].Task_Count = 1;			   // Task_Count的初值不一样，避免500ms到时，所有任务都执行一遍。
	TasksPare[0].Task_Function = HFPeriod_1msTask;

		
	
	TasksPare[1].Task_Period = FaulPeriod_COUNT; // 10ms
	TasksPare[1].Task_Count = 8;
	TasksPare[1].Task_Function = task_send_Rece; //

	TasksPare[2].Task_Period = Robot_COUNT; // 20ms
	TasksPare[2].Task_Count = 15;
	TasksPare[2].Task_Function = Robot_Control; //

	TasksPare[3].Task_Period = KEY_COUNT; // 100ms
	TasksPare[3].Task_Count = 80;
	TasksPare[3].Task_Function = KEY_RUN; //

	TasksPare[4].Task_Period = LEDPeriod_COUNT; // 500ms
	TasksPare[4].Task_Count = 300;
	TasksPare[4].Task_Function = Task_LED; // 500ms的LED的闪烁
	
	TasksPare[5].Task_Period = IMU_COUNT; // PERIOD_COUNT=5    5ms
	TasksPare[5].Task_Count = 1;			   // Task_Count的初值不一样，避免500ms到时，所有任务都执行一遍。
	TasksPare[5].Task_Function = IMU_Get_Data;
}
// USER CODE END
