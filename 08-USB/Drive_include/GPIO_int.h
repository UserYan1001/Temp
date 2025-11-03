

#ifndef _GPIO_int_H
#define _GPIO_int_H
#include "Sys.h"    


//----------------------------------按键--------------------------------------------------
// 按键引脚定义
#define KEY1_PIN    GPIO_Pin_10
#define KEY1_PORT   GPIOE
#define KEY2_PIN    GPIO_Pin_15
#define KEY2_PORT   GPIOC
// 按键状态定义
#define KEY1_PRESSED  1
#define KEY2_PRESSED  2
#define KEY_NONE      0
//按键函数声明
void KEY_Init(void);
uint8_t KEY_Scan(void);
//----------------------------------按键--------------------------------------------------








//----------------------------------四路循迹模块----------------------------------------------


void TRACK_IR4_Init(void);
/* 循迹引脚读取，从右数1-》4 */
#define TRTACK_IR4_X1_PIN GPIO_Pin_0
#define TRTACK_IR4_X1_PORT GPIOE               /* GPIO端口 */
#define TRTACK_IR4_X1_CLK RCC_AHB1Periph_GPIOE /* GPIO端口时钟 */

#define TRTACK_IR4_X2_PIN GPIO_Pin_2
#define TRTACK_IR4_X2_PORT GPIOE               /* GPIO端口 */
#define TRTACK_IR4_X2_CLK RCC_AHB1Periph_GPIOE /* GPIO端口时钟 */

#define TRTACK_IR4_X3_PIN GPIO_Pin_3
#define TRTACK_IR4_X3_PORT GPIOE               /* GPIO端口 */
#define TRTACK_IR4_X3_CLK RCC_AHB1Periph_GPIOE /* GPIO端口时钟 */

#define TRTACK_IR4_X4_PIN GPIO_Pin_4
#define TRTACK_IR4_X4_PORT GPIOE               /* GPIO端口 */
#define TRTACK_IR4_X4_CLK RCC_AHB1Periph_GPIOE /* GPIO端口时钟 */

#define TRTACK_IR4_X1_READ() GPIO_ReadInputDataBit(TRTACK_IR4_X1_PORT,TRTACK_IR4_X1_PIN)
#define TRTACK_IR4_X2_READ() GPIO_ReadInputDataBit(TRTACK_IR4_X2_PORT,TRTACK_IR4_X2_PIN)
#define TRTACK_IR4_X3_READ() GPIO_ReadInputDataBit(TRTACK_IR4_X3_PORT,TRTACK_IR4_X3_PIN)
#define TRTACK_IR4_X4_READ() GPIO_ReadInputDataBit(TRTACK_IR4_X4_PORT,TRTACK_IR4_X4_PIN)

//----------------------------------四路循迹模块----------------------------------------------






 
//------------------------------------超声波模块--------------------------------------------
//引脚定义宏
//传感器一OK
#define TRIG_PORT       GPIOB
#define TRIG_PIN        GPIO_Pin_6
#define ECHO_PORT       GPIOB
#define ECHO_PIN        GPIO_Pin_7
#define TRIG_RCC				RCC_AHB1Periph_GPIOB


//传感器二OK
//#define TRIG_PORT       GPIOE
//#define TRIG_PIN        GPIO_Pin_1
//#define ECHO_PORT       GPIOE
//#define ECHO_PIN        GPIO_Pin_12
//#define TRIG_RCC				RCC_AHB1Periph_GPIOE
// 函数宏定义
#define TRIG_HIGH()     GPIO_SetBits(TRIG_PORT, TRIG_PIN)
#define TRIG_LOW()      GPIO_ResetBits(TRIG_PORT, TRIG_PIN)
#define ECHO_STATE()    GPIO_ReadInputDataBit(ECHO_PORT, ECHO_PIN)

// 函数声明
void ultrasonic_sensor_init(void);
float sensor_sr_ultrasonic_read(void);

//------------------------------------超声波模块--------------------------------------------









//------------------------------------蜂鸣器------------------------------------------------
// 接口函数
void BEEP_Init(void);
// 蜂鸣器操作函数宏定义
#define BEEP_Off()       GPIO_ResetBits(GPIOC, GPIO_Pin_12)       //蜂鸣器不鸣
#define BEEP_On()		GPIO_SetBits(GPIOC, GPIO_Pin_12)     //蜂鸣器鸣叫
#define BEEP_Toggle()    GPIO_WriteBit(GPIOC, GPIO_Pin_12, (BitAction) (1 - GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12)))    //蜂鸣器状态翻转
void beep_on_times(int times, int delay);

//------------------------------------蜂鸣器------------------------------------------------




//------------------------------------LED工作灯---------------------------------------------
// 宏定义LED引脚
#define LED1_PIN    GPIO_Pin_14
#define LED7_PIN    GPIO_Pin_15
#define LED_PORT    GPIOD

// 函数声明
void LED_Init(void);
void LED1_On(void);
void LED1_Off(void);
void LED1_Turn(void);
void LED7_On(void);
void LED7_Off(void);
void LED7_Turn(void);
//------------------------------------LED工作灯---------------------------------------------

#endif   

