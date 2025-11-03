/****************************************************************************
 *	@笔者	：	YJH
 *	@日期	：	2025-09-09
 *	@所属	：	杭州星呗科技有限公司
 *****************************************************************************/
#include "main.h"

/**************************串口打印相关函数重定义********************************/
/**
 * @简  述  重定义putc函数
 */
int fputc(int ch, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1, (uint8_t)ch);

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
	}

	return ch;
}

/**
 * @简  述  重定义getc函数（USART1）
 */
int fgetc(FILE *f)
{
	/* 等待串口1输入数据 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
	{
	}

	return (int)USART_ReceiveData(USART1);
}


//-----------------------------UART1普通串口-------------------------------------

/**
 * @简  述  UART 串口初始化
 * @参  数  baud： 波特率设置
 * @返回值	 无
 */
// 串口1，自动下载电路
void UART1_Init(uint32_t baud)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 串口 USART1配置 */
	// 打开GPIO和USART部件的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	// USART1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	// USART1 端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USART1参数配置
	USART_InitStructure.USART_BaudRate = baud; // 波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	// 使能 USART， 配置完毕
	USART_Cmd(USART1, ENABLE);

	// 规避第一个字符不能输出的BUG
	//	USART_ClearFlag(USART1, USART_FLAG_TC);

	// 开启串口接收中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 开启相关中断

	// Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		  // 串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  // 子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  // 根据指定的参数初始化VIC寄存器
}


/***********************************************
	功能介绍：	串口1发送字节
	函数参数：	dat 发送的字节
	返回值：		无
 ***********************************************/
void uart1_send_byte(u8 dat)
{
	USART_SendData(USART1, dat);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;
}

/***********************************************
	功能介绍：	串口1发送字符串
	函数参数：	*s 发送的字符串
	返回值：		无
 ***********************************************/
void uart1_send_str(u8 *s)
{
	while (*s)
	{
		uart1_send_byte(*s++);
	}
}

/* QSA */
/**
 * @简  述  DBUART 串口中断服务函数
 * @参  数  无
 * @返回值  无
 */
void USART1_IRQHandler(void)
{
   u8 sbuf_bak;
   static u16 buf_index = 0;
   if (USART_GetFlagStatus(USART1, USART_IT_RXNE) == SET)
   {
       USART_ClearITPendingBit(USART1, USART_IT_RXNE);
       sbuf_bak = USART_ReceiveData(USART1);
       //USART_SendData(USART1, sbuf_bak);
       if (uart1_get_ok)
           return;
       if (sbuf_bak == '<')
       {
           uart1_mode = 4;
           buf_index = 0;
       }
       else if (uart1_mode == 0)
       {
           if (sbuf_bak == '$')
           {
               uart1_mode = 1;
           }
           else if (sbuf_bak == '#')
           {
               uart1_mode = 2;
           }
           else if (sbuf_bak == '{')
           {
               uart1_mode = 3;
           }
           else if (sbuf_bak == '<')
           {
               uart1_mode = 4;
           }
           buf_index = 0;
       }

       uart_receive_buf[buf_index++] = sbuf_bak;

       if ((uart1_mode == 4) && (sbuf_bak == '>'))
       {
           uart_receive_buf[buf_index] = '\0';
           uart1_get_ok = 1;
       }
       else if ((uart1_mode == 1) && (sbuf_bak == '!'))
       {
           uart_receive_buf[buf_index] = '\0';
           uart1_get_ok = 1;
       }
       else if ((uart1_mode == 2) && (sbuf_bak == '!'))
       {
           uart_receive_buf[buf_index] = '\0';
           uart1_get_ok = 1;
       }
       else if ((uart1_mode == 3) && (sbuf_bak == '}'))
       {
           uart_receive_buf[buf_index] = '\0';
           uart1_get_ok = 1;
       }
		
	    if(uart1_get_ok==1)
	    {
			uart_receive_num = 1;
        }
		
       if (buf_index >= 1024)
           buf_index = 0;
   }
}


//-----------------------------UART2普通串口-------------------------------------

/**
 * @简  述  UART   串口初始化
 * @参  数  baud： 波特率设置
 * @返回值	 无
 */

// 用户接口，也可直接接树莓派的GPIO
void UART2_Init(uint32_t baud)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 串口USART配置 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	// USART对应引脚复用映射
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

	// USART 端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// USART参数配置
	USART_InitStructure.USART_BaudRate = baud; // 波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	// USART使能
	USART_Cmd(USART2, ENABLE);

	// 开启串口接收中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 开启相关中断

	// USART2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		  // 串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  // 子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  // 根据指定的参数初始化VIC寄存器
}

/***********************************************
	功能介绍：	串口2发送字节
	函数参数：	dat 发送的字节
	返回值：		无
 ***********************************************/
void uart2_send_byte(u8 dat)
{
	USART_SendData(USART2, dat);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
		;
}

/***********************************************
	功能介绍：	串口2发送字符串
	函数参数：	*s 发送的字符串
	返回值：		无
 ***********************************************/
void uart2_send_str(u8 *s)
{
	while (*s)
	{
		uart2_send_byte(*s++);
	}
}

/**
 * @简  述  UART 串口中断服务函数
 * @参  数  无
 * @返回值  无
 */
void USART2_IRQHandler(void)
{
	u8 sbuf_bak;
	static u16 buf_index = 0;

	if (USART_GetFlagStatus(USART2, USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		sbuf_bak = USART_ReceiveData(USART2);
		//USART_SendData(USART1, sbuf_bak);
		if (uart1_get_ok)
			return;
		if (sbuf_bak == '<')
		{
			uart1_mode = 4;
			buf_index = 0;
		}
		else if (uart1_mode == 0)
		{
			if (sbuf_bak == '$')
			{
				uart1_mode = 1;
			}
			else if (sbuf_bak == '#')
			{
				uart1_mode = 2;
			}
			else if (sbuf_bak == '{')
			{
				uart1_mode = 3;
			}
			else if (sbuf_bak == '<')
			{
				uart1_mode = 4;
			}
			buf_index = 0;
		}

		uart_receive_buf[buf_index++] = sbuf_bak;

		if ((uart1_mode == 4) && (sbuf_bak == '>'))
		{
			uart_receive_buf[buf_index] = '\0';
			uart1_get_ok = 1;
		}
		else if ((uart1_mode == 1) && (sbuf_bak == '!'))
		{
			uart_receive_buf[buf_index] = '\0';
			uart1_get_ok = 1;
		}
		else if ((uart1_mode == 2) && (sbuf_bak == '!'))
		{
			uart_receive_buf[buf_index] = '\0';
			uart1_get_ok = 1;
		}
		else if ((uart1_mode == 3) && (sbuf_bak == '}'))
		{
			uart_receive_buf[buf_index] = '\0';
			uart1_get_ok = 1;
		}

		if (buf_index >= 1024)
			buf_index = 0;
	}
}



//-----------------------------UART3总线串口-------------------------------------

/************************************************
函数名称 ： USART3_Send_ArrayU8
功    能 ： 总线舵机的串口设置
参    数 ： pData ---- 字符串
            Length --- 长度
返 回 值 ： 无
*************************************************/
/* QSA */
void USART3_Init(uint32_t baud)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		/* 串口 USART3配置 */
		// 打开GPIO和USART部件的时钟
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

		// USART3对应引脚复用映射
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);

		// USART3 端口配置
		// USART 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //开漏上拉
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		// USART3参数配置
		USART_InitStructure.USART_BaudRate = baud; // 波特率
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART3, &USART_InitStructure);
		USART_HalfDuplexCmd(USART3, ENABLE);  	//注意这个，启动半双工模式

		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;         // 串口3中断通道
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);                           // 根据指定的参数初始化VIC寄存器

		// 开启串口接收中断
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 开启相关中断
		USART_ITConfig(USART3, USART_IT_TXE, DISABLE); /* 禁止串口发送中断 */
		// 使能 USART， 配置完毕
		USART_Cmd(USART3, ENABLE);
}

/***********************************************
	函数名称:	uart3_send_str()
	功能介绍:	串口3发送字符串
	函数参数:	*s 发送的字符串
	返回值:		无
 ***********************************************/
void uart3_send_str(uint8_t *s)
{
	sendOnly(USART3);
	while (*s)
	{
		USART_SendData(USART3, *s++);
		while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
			;
	}
	USART_SendData(USART3, 0xff);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
		;
	readOnly(USART3);
}

/*  串口中断接收处理 */
void USART3_IRQHandler(void)
{
    u8 sbuf_bak;
	static u16 buf_index = 0;
	if (USART_GetFlagStatus(USART3, USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		sbuf_bak = USART_ReceiveData(USART3);
		printf("BUS3 Receive");
		//USART_SendData(USART1, sbuf_bak);
		if (uart1_get_ok)
			return;

		if (uart1_mode == 0)
		{
			if (sbuf_bak == '$')
			{
				uart1_mode = 1;
			}
			else if (sbuf_bak == '#')
			{
				uart1_mode = 2;
			}
			else if (sbuf_bak == '{')
			{
				uart1_mode = 3;
			}
			buf_index = 0;
		}

		uart_receive_buf[buf_index++] = sbuf_bak;


		if ((uart1_mode == 1) && (sbuf_bak == '!'))
		{
			uart_receive_buf[buf_index] = '\0';
			uart1_get_ok = 1;
		}
		else if ((uart1_mode == 2) && (sbuf_bak == '!'))
		{
			uart_receive_buf[buf_index] = '\0';
			uart1_get_ok = 1;
		}
		else if ((uart1_mode == 3) && (sbuf_bak == '}'))
		{
			uart_receive_buf[buf_index] = '\0';
			uart1_get_ok = 1;
		}

		if(uart1_get_ok==1)
		{
			uart_receive_num = 3;
		}

		if (buf_index >= 1024)
			buf_index = 0;
	}
}


//-----------------------------UART4普通串口-------------------------------------

/**
 * @简  述  UART   串口初始化
 * @参  数  baud： 波特率设置
 * @返回值	 无
 */

// 用户接口，也可直接接树莓派的GPIO
void UART4_Init(uint32_t baud)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 串口USART配置 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

	// USART对应引脚复用映射
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);

	// USART 端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// USART参数配置
	USART_InitStructure.USART_BaudRate = baud; // 波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);

	// USART使能
	USART_Cmd(UART4, ENABLE);

	// 开启串口接收中断
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); // 开启相关中断

	// UART4 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;		  // 串口4中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  // 子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  // 根据指定的参数初始化VIC寄存器
}

/***********************************************
	功能介绍：	串口4发送字节
	函数参数：	dat 发送的字节
	返回值：		无
 ***********************************************/
void uart4_send_byte(u8 dat)
{
	USART_SendData(UART4, dat);
	while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET)
		;
}

/***********************************************
	功能介绍：	串口4发送字符串
	函数参数：	*s 发送的字符串
	返回值：		无
 ***********************************************/
void uart4_send_str(u8 *s)
{
	while (*s)
	{
		uart4_send_byte(*s++);
	}
}

/**
 * @简  述  UART 串口中断服务函数
 * @参  数  无
 * @返回值  无
 */
void UART4_IRQHandler(void)
{
	u8 sbuf_bak;
	static u16 buf_index = 0;
	if (USART_GetFlagStatus(UART4, USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(UART4, USART_IT_RXNE);
		sbuf_bak = USART_ReceiveData(UART4);
		// USART_SendData(USART1, sbuf_bak);
		if (uart1_get_ok)
			return;
		if (sbuf_bak == '<')
		{
			uart1_mode = 4;
			buf_index = 0;
		}
		else if (uart1_mode == 0)
		{
			if (sbuf_bak == '$')
			{
				uart1_mode = 1;
			}
			else if (sbuf_bak == '#')
			{
				uart1_mode = 2;
			}
			else if (sbuf_bak == '{')
			{
				uart1_mode = 3;
			}
			else if (sbuf_bak == '<')
			{
				uart1_mode = 4;
			}
			buf_index = 0;
		}

		uart_receive_buf[buf_index++] = sbuf_bak;

		if ((uart1_mode == 4) && (sbuf_bak == '>'))
		{
			uart_receive_buf[buf_index] = '\0';
			uart1_get_ok = 1;
		}
		else if ((uart1_mode == 1) && (sbuf_bak == '!'))
		{
			uart_receive_buf[buf_index] = '\0';
			uart1_get_ok = 1;
		}
		else if ((uart1_mode == 2) && (sbuf_bak == '!'))
		{
			uart_receive_buf[buf_index] = '\0';
			uart1_get_ok = 1;
		}
		else if ((uart1_mode == 3) && (sbuf_bak == '}'))
		{
			uart_receive_buf[buf_index] = '\0';
			uart1_get_ok = 1;
		}

		if (buf_index >= 1024)
			buf_index = 0;
	}
}


//-----------------------------UART5-SBUS-串口-------------------------------------

#include "robot.h"

static uint8_t  uart_sbus_rx_con=0;       //接收计数器
static uint8_t  uart_sbus_rx_buf[40];     //接收缓冲，数据内容小于等于32Byte
static uint16_t sbus_buf[10];             //通道数据

/**
  * @简  述  SBUS串口初始化
  * @参  数  无
  * @返回值	 无
  */
void SBUS_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	/* 串口USART配置 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
	
	//USART对应引脚复用映射
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2, GPIO_AF_UART5); 

	//USART 端口配置	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOD,&GPIO_InitStructure); 

	//USART参数配置
	USART_InitStructure.USART_BaudRate = 100000;    //波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_2;
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx;
	USART_Init(UART5, &USART_InitStructure);
	
    //USART3 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器	
	
	//开启串口接收中断
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启相关中断
	USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
	
		//USART使能
	USART_Cmd(UART5, ENABLE); 
	
}

/**
  * @简  述  UART 串口中断服务函数
  * @参  数  无 
  * @返回值  无
  */
void UART5_IRQHandler(void)
{
	uint8_t Res;
	
	//接收中断
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  
	{
		Res =USART_ReceiveData(UART5);	
		
		uart_sbus_rx_buf[uart_sbus_rx_con++] = Res;
		
		USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	}
	
    //总线空闲终端
	else if(USART_GetITStatus(UART5, USART_IT_IDLE) != RESET)
	{
		//接收完成，恢复初始状态
		uart_sbus_rx_con = 0;
		printf("sbus : %s\r\n",uart_sbus_rx_buf);
		//转换到对应通道
		sbus_buf[1] = ((int16_t)uart_sbus_rx_buf[ 1] >> 0 | ((int16_t)uart_sbus_rx_buf[ 2] << 8 )) & 0x07FF;
		sbus_buf[2] = ((int16_t)uart_sbus_rx_buf[ 2] >> 3 | ((int16_t)uart_sbus_rx_buf[ 3] << 5 )) & 0x07FF;
		sbus_buf[3] = ((int16_t)uart_sbus_rx_buf[ 3] >> 6 | ((int16_t)uart_sbus_rx_buf[ 4] << 2 )  | (int16_t)uart_sbus_rx_buf[ 5] << 10 ) & 0x07FF;
		sbus_buf[4] = ((int16_t)uart_sbus_rx_buf[ 5] >> 1 | ((int16_t)uart_sbus_rx_buf[ 6] << 7 )) & 0x07FF;
		sbus_buf[5] = ((int16_t)uart_sbus_rx_buf[ 6] >> 4 | ((int16_t)uart_sbus_rx_buf[ 7] << 4 )) & 0x07FF;
		sbus_buf[6] = ((int16_t)uart_sbus_rx_buf[ 7] >> 7 | ((int16_t)uart_sbus_rx_buf[ 8] << 1 )  | (int16_t)uart_sbus_rx_buf[9] <<  9 ) & 0x07FF;
		sbus_buf[7] = ((int16_t)uart_sbus_rx_buf[9] >> 2 | ((int16_t)uart_sbus_rx_buf[10] << 6 )) & 0x07FF;
		sbus_buf[8] = ((int16_t)uart_sbus_rx_buf[10] >> 5 | ((int16_t)uart_sbus_rx_buf[11] << 3 )) & 0x07FF;
		
		//控制逻辑，通道5拨杆到下方生效
		if(sbus_buf[5] > 1500)
		{
			//运动控制
			Vel.TG_IX = (int16_t)(  1.0 * (sbus_buf[2] - 992));
			Vel.TG_IY = (int16_t)( -0.8 * (sbus_buf[1] - 992));
			
			//如果是阿克曼机器人
			#if (ROBOT_TYPE == ROBOT_AKM)
				ax_akm_angle = (int16_t)( -0.6 * (sbus_buf[4] - 992));
			#else
				Vel.TG_IW = (int16_t)( -5.0 * (sbus_buf[4] - 992));
			#endif
		}
		
		USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
	}
}

//-----------------------------UART6总线串口-------------------------------------

/************************************************
函数名称 ： USART6_Send_ArrayU8
功    能 ： 串口6的数据发送函数
参    数 ： pData ---- 数据数组
            Length --- 数据长度
返 回 值 ： 无
*************************************************/
/* QSA */
void USART6_Init(uint32_t baud)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		/* 串口 USART6配置 */
		// 打开GPIO和USART部件的时钟
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  // USART6使用GPIOC
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); // USART6位于APB2总线

		// USART6对应引脚复用映射 (PC6->TX, PC7->RX)
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);  // TX引脚
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);  // RX引脚

		// USART6 端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  // 同时配置TX和RX
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  // 开漏模式
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		// USART6参数配置
		USART_InitStructure.USART_BaudRate = baud;  // 波特率由参数指定
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;  // 8位数据位
		USART_InitStructure.USART_StopBits = USART_StopBits_1;       // 1位停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;          // 无奇偶校验
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // 无硬件流控
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  // 同时使能收发
		USART_Init(USART6, &USART_InitStructure);
		USART_HalfDuplexCmd(USART6, ENABLE);  // 启用半双工模式

		// 配置中断优先级
		NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;          // USART6中断通道
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // 抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;         // 子优先级，与USART3不同
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // 使能中断通道
		NVIC_Init(&NVIC_InitStructure);

		// 开启串口接收中断
		USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);  // 使能接收中断
		USART_ITConfig(USART6, USART_IT_TXE, DISABLE);  // 禁止发送中断
		USART_Cmd(USART6, ENABLE);  // 使能USART6
}

/***********************************************
	函数名称:	uart6_send_str()
	功能介绍:	串口6发送字符串
	函数参数:	*s 发送的字符串指针
	返回值:		无
 ***********************************************/
void uart6_send_str(uint8_t *s)
{
	sendOnly(USART6);  // 切换到发送模式
	while (*s)  // 循环发送每个字符直到字符串结束
	{
		USART_SendData(USART6, *s++);  // 发送当前字符并移动指针
		while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);  // 等待发送完成
	}
	USART_SendData(USART6, 0xff);  // 发送结束标志
	while (USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);  // 等待发送完成
	readOnly(USART6);  // 切换回接收模式
}

/*  串口6中断接收处理函数 */
void USART6_IRQHandler(void)
{
    u8 sbuf_bak;
	static u16 buf6_index = 0;  // 使用独立的缓冲区索引，避免与其他串口冲突
	
	if (USART_GetFlagStatus(USART6, USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART6, USART_IT_RXNE);  // 清除中断标志
		sbuf_bak = USART_ReceiveData(USART6);  // 读取接收的数据
		printf("BUS6 Receive");
		// 如果已经接收到完整数据，则忽略新数据
		if (uart1_get_ok)
			return;

		// 根据起始字符设置接收模式
		if (uart1_mode == 0)
		{
			if (sbuf_bak == '$')
			{
				uart1_mode = 1;
			}
			else if (sbuf_bak == '#')
			{
				uart1_mode = 2;
			}
			else if (sbuf_bak == '{')
			{
				uart1_mode = 3;
			}
			buf6_index = 0;  // 重置缓冲区索引
		}

		// 将接收的数据存入缓冲区
		uart_receive_buf[buf6_index++] = sbuf_bak;

		// 检查是否接收到结束标志
		if ((uart1_mode == 1) && (sbuf_bak == '!'))
		{
			uart_receive_buf[buf6_index] = '\0';  // 添加字符串结束符
			uart1_get_ok = 1;  // 标记接收完成
		}
		else if ((uart1_mode == 2) && (sbuf_bak == '!'))
		{
			uart_receive_buf[buf6_index] = '\0';
			uart1_get_ok = 1;
		}
		else if ((uart1_mode == 3) && (sbuf_bak == '}'))
		{
			uart_receive_buf[buf6_index] = '\0';
			uart1_get_ok = 1;
		}

		// 标记串口6接收到数据
		if(uart1_get_ok == 1)
		{
			uart_receive_num = 6;  // 用6标识是串口6的数据
		}

		// 防止缓冲区溢出
		if (buf6_index >= 1024)
			buf6_index = 0;
	}
}
