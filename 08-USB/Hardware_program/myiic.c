/****************************************************************************
 *	@笔者	：	YJH
 *	@日期	：	2025-09-09
 *	@所属	：	杭州星呗科技有限公司
 *****************************************************************************/
#include "stm32f4xx.h"
#include "sys.h"
#include "myiic.h"
//// 引脚方向设置
//#define SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	//PC3输入模式
//#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} //PC3输出模式

//// IO操作函数	 
//#define IIC_SCL    PBout(8) //SCL - PC5
//#define IIC_SDA    PBout(9) //SDA - PC3
//#define READ_SDA   PBin(9)  //输入SDA - PC3

//// 初始化IIC
//void IIC_Init(void)
//{			
//  GPIO_InitTypeDef  GPIO_InitStructure;

//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // 使能GPIOC时钟

//  // GPIOC5(SCL), GPIOC3(SDA)初始化设置
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;         // 普通输出模式
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;        // 开漏输出
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;    // 100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          // 上拉
//  GPIO_Init(GPIOB, &GPIO_InitStructure);                // 初始化GPIOC
//  // 初始化IIC总线电平
//  IIC_SCL = 1;
//  IIC_SDA = 1;
//}

// 引脚方向设置
#define SDA_IN()  {GPIOC->MODER&=~(3<<(3*2));GPIOC->MODER|=0<<3*2;}	//PC3输入模式
#define SDA_OUT() {GPIOC->MODER&=~(3<<(3*2));GPIOC->MODER|=1<<3*2;} //PC3输出模式

// IO操作函数	 
#define IIC_SCL    PCout(5) //SCL - PC5
#define IIC_SDA    PCout(3) //SDA - PC3
#define READ_SDA   PCin(3)  //输入SDA - PC3

// 初始化IIC
void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // 使能GPIOC时钟

  // GPIOC5(SCL), GPIOC3(SDA)初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;         // 普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;        // 开漏输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;    // 100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          // 上拉
  GPIO_Init(GPIOC, &GPIO_InitStructure);                // 初始化GPIOC
  // 初始化IIC总线电平
  IIC_SCL = 1;
  IIC_SDA = 1;
}

// 产生IIC起始信号
void IIC_Start(void)
{
  SDA_OUT();     // sda线输出
  IIC_SDA = 1;	  
  IIC_SCL = 1;
  Delay_us(5);
  IIC_SDA = 0;   // START: 当CLK为高时，DATA从高变低
  Delay_us(5);
  IIC_SCL = 0;   // 钳住I2C总线，准备发送或接收数据 
}	  

// 产生IIC停止信号
void IIC_Stop(void)
{
  SDA_OUT();     // sda线输出
  IIC_SCL = 0;
  IIC_SDA = 0;   // STOP: 当CLK为高时，DATA从低变高
  Delay_us(5);
  IIC_SCL = 1; 
  IIC_SDA = 1;   // 发送I2C总线结束信号
  Delay_us(5);							   	
}

// 等待应答信号到来
// 返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
  u8 ucErrTime = 0;
  SDA_IN();      // SDA设置为输入  
  IIC_SDA = 1;
  Delay_us(5);	   
  IIC_SCL = 1;
  Delay_us(5);	 
  while(READ_SDA)
  {
    ucErrTime++;
    if(ucErrTime > 250)
    {
      IIC_Stop();
      return 1;
    }
  }
  IIC_SCL = 0;   // 时钟输出0 	   
  return 0;  
} 

// 产生ACK应答
void IIC_Ack(void)
{
  IIC_SCL = 0;
  SDA_OUT();
  IIC_SDA = 0;
  Delay_us(5);
  IIC_SCL = 1;
  Delay_us(5);
  IIC_SCL = 0;
}

// 不产生ACK应答		    
void IIC_NAck(void)
{
  IIC_SCL = 0;
  SDA_OUT();
  IIC_SDA = 1;
  Delay_us(5);
  IIC_SCL = 1;
  Delay_us(5);
  IIC_SCL = 0;
}					 				     

// IIC发送一个字节
void IIC_Send_Byte(u8 txd)
{                        
  u8 t;   
  SDA_OUT(); 	    
  IIC_SCL = 0;   // 拉低时钟开始数据传输
  for(t = 0; t < 8; t++)
  {              
    IIC_SDA = (txd & 0x80) >> 7;
    txd <<= 1; 	  
    Delay_us(5);
    IIC_SCL = 1;
    Delay_us(5); 
    IIC_SCL = 0;	
  }	 
} 	    

// 读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
  unsigned char i, receive = 0;
  SDA_IN();      // SDA设置为输入
  for(i = 0; i < 8; i++)
  {
    IIC_SCL = 0; 
    Delay_us(5);
    IIC_SCL = 1;
    receive <<= 1;
    if(READ_SDA) receive++;   
    Delay_us(5); 
  }					 
  if (!ack)
    IIC_NAck();  // 发送nACK
  else
    IIC_Ack();   // 发送ACK   
  return receive;
}

