#ifndef __USART_H
#define __USART_H

#include "stm32f4xx.h"
#include "global.h"

//-----------------------------UART1普通串口-------------------------------------
void UART1_Init(uint32_t baud); 
void uart1_send_str(u8 *s);
void uart1_send_byte(u8 dat);

//-----------------------------UART2普通串口-------------------------------------
void UART2_Init(uint32_t baud);
void uart2_send_str(u8 *s);



//-----------------------------UART3总线串口-------------------------------------
#define readOnly(x)	x->CR1 |= 4;	x->CR1 &= 0xFFFFFFF7;		//串口x配置为只读，CR1->RE=1, CR1->TE=0
#define sendOnly(x)	x->CR1 |= 8;	x->CR1 &= 0xFFFFFFFB;		//串口x配置为只写，CR1->RE=0, CR1->TE=1
void USART3_Init(uint32_t baudrate);
void uart3_send_str(uint8_t *s);


//-----------------------------UART4普通串口-------------------------------------
void UART4_Init(uint32_t baud);
void uart4_send_str(u8 *s);


//-----------------------------UART5-SBUS-串口-----------------------------------
void SBUS_Init(void);  //SBUS_Init串口初始化



//-----------------------------UART6总线串口-------------------------------------
void USART6_Init(uint32_t baudrate);
void uart6_send_str(uint8_t *s);

#endif 
