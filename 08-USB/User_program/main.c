#include "main.h"

#include "usb_bsp.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_hid_core.h"
#include "pid.h"
__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_Core_dev __ALIGN_END;
__ALIGN_BEGIN USBH_HOST USB_Host __ALIGN_END;
extern HID_Machine_TypeDef HID_Machine;


int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_ms(5);
    SysTickConfig(); // 1ms定时器初始化
    UART1_Init(115200);
	UART2_Init(115200);
	USART3_Init(115200);
//	USART6_Init(115200);
	//蜂鸣器初始化
	LED_Init();
    BEEP_Init();
	SBUS_Init();
	//舵机专用定时器
    TIM14_Int_Init(5000 - 1, 84 - 1);
	//FLASH初始化
    setup_w25q64();
	//读取偏差
	others_init();
	//舵机初始化
    SERVO_Init();
	//按键初始化
    KEY_Init();
    beep_on_times(3,100);  // 初始化完成提示

	//USB初始化
	USBH_Init(&USB_OTG_Core_dev,
	USB_OTG_FS_CORE_ID,
	&USB_Host, &HID_cb, &USR_Callbacks);
	//任务管理器初始化
	Task_Manage_List_Init();
	while (1)
	{	
		Execute_Task_List_RUN();
	}
}
