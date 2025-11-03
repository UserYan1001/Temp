#ifndef __WS2812B_H
#define __WS2812B_H

#include "main.h"

#define RCC_DIN     RCC_AHB1Periph_GPIOA
#define PORT_DIN    GPIOA
#define GPIO_DIN    GPIO_Pin_8


//用户修改参数区
//#define WS2812_FREQUENCY
#define RGB_PIN_L()          GPIO_WriteBit(PORT_DIN, GPIO_DIN, Bit_RESET)  //控制彩灯引脚（需要配置为强推挽输出）
#define RGB_PIN_H()          GPIO_WriteBit(PORT_DIN, GPIO_DIN, Bit_SET)    //控制彩灯引脚（需要配置为强推挽输出）
#define WS2812_MAX        60                        //彩灯最大个数
#define WS2812_NUMBERS    60                       //彩灯个数


// 颜色定义
#define RED     0xFF0000
#define GREEN   0x00FF00
#define BLUE    0x0000FF
#define YELLOW  0xFFFF00
#define CYAN    0x00FFFF
#define MAGENTA 0xFF00FF
#define WHITE   0xFFFFFF
#define BLACK   0x000000




//8.3 -8  0.000000083
//4.16 -9 0.00000000416
void Ws2812b_WriteByte(unsigned char byte);//发送一个字节数据（@12.000MHz,理论每个机器周期83ns,测试约为76ns）
void setLedCount(unsigned char count);//设置彩灯数目，范围0-25.
unsigned char getLedCount(void);//彩灯数目查询函数
void rgb_SetColor(unsigned char LedId, unsigned long color);//设置彩灯颜色
void rgb_SetRGB(unsigned char LedId, unsigned long red, unsigned long green, unsigned long blue);//设置彩灯颜色
void rgb_SendArray(void);//发送彩灯数据

void WS2812_GPIO_Init(void);

void RGB_LED_Write1(void);
void RGB_LED_Reset(void);

#endif /* __WS2812B_H */
