
/****************************************************************************
 *	@笔者	：	YJH
 *	@日期	：	2025-09-09
 *	@所属	：	杭州星呗科技有限公司
 *****************************************************************************/
#include "main.h"


// 按键初始化
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 启动GPIO时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOC, ENABLE);
    
    // 配置KEY1 (PE10) 输入模式
    GPIO_InitStructure.GPIO_Pin = KEY1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  // 上拉输入，按键未按下时为高电平
    GPIO_Init(KEY1_PORT, &GPIO_InitStructure);
    
    // 配置KEY2 (PC15) 输入模式
    GPIO_InitStructure.GPIO_Pin = KEY2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  // 上拉输入，按键未按下时为高电平
    GPIO_Init(KEY2_PORT, &GPIO_InitStructure);
}

// 按键扫描函数，带简单消抖
uint8_t KEY_Scan(void)
{
   
    if((	GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN) == 0 || 
					GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN) == 0))
    {
        // 延时消抖
        for(int i = 0; i < 0xFFFF; i++);
        
        
        // 检测KEY1是否按下
        if(GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN) == 0)
        {
            return KEY1_PRESSED;
        }
        // 检测KEY2是否按下
        else if(GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN) == 0)
        {
            return KEY2_PRESSED;
        }
    }
    
    return KEY_NONE;  // 无按键按下
}


// 4路红外循迹初始化
/* X1=E0 X2=E2 X3=E3 X4=E4 */
void TRACK_IR4_Init(void)
{
		
		GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(TRTACK_IR4_X1_CLK | TRTACK_IR4_X2_CLK | TRTACK_IR4_X3_CLK | TRTACK_IR4_X4_CLK, ENABLE); /* 使能 舵机 端口时钟 */

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;       
    GPIO_InitStructure.GPIO_Pin = TRTACK_IR4_X1_PIN;         
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;      /*上拉*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
    GPIO_Init(TRTACK_IR4_X1_PORT, &GPIO_InitStructure); /*初始化IO*/

    GPIO_InitStructure.GPIO_Pin = TRTACK_IR4_X2_PIN;
    GPIO_Init(TRTACK_IR4_X2_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = TRTACK_IR4_X3_PIN;
    GPIO_Init(TRTACK_IR4_X3_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = TRTACK_IR4_X4_PIN;
    GPIO_Init(TRTACK_IR4_X4_PORT, &GPIO_InitStructure);
}



/***********************************************
    ultrasonic_sensor_init()
    功能介绍：初始化超声波传感器
    函数参数：无
    返回值：	无
 ***********************************************/
void ultrasonic_sensor_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    // 使能时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);  // 定时器13时钟
    RCC_AHB1PeriphClockCmd(TRIG_RCC, ENABLE);  // GPIOB时钟（修改为GPIOB）

    // 初始化Trig引脚（PB6 - 输出）
    GPIO_InitStructure.GPIO_Pin = TRIG_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         // 推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;       // 无上下拉（输出模式可不用）
    GPIO_Init(TRIG_PORT, &GPIO_InitStructure);
    
    // 初始化Echo引脚（PB7 - 输入）
    GPIO_InitStructure.GPIO_Pin = ECHO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;       // 无上下拉（根据实际情况可改为上拉）
    GPIO_Init(ECHO_PORT, &GPIO_InitStructure);
    
    // 初始化定时器13（用于计时）
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 167;             // 168MHz/168=1MHz（计数周期1us）
    TIM_TimeBaseStructure.TIM_Period = 65535;              // 最大计数65535us（约65ms，对应最大距离≈11米）
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);
    
    // 初始状态Trig置低
    TRIG_LOW();
}

/**
 * @函数描述: 获取距离
 * @return {float}返回距离(cm)，-1表示超出测量范围，-2表示超时错误
 * 距离=高电平时间*声速（340M/S） /2
 */
float sensor_sr_ultrasonic_read(void)
{
    uint16_t time_us = 0;
    uint32_t timeout = 0;
    
    // 发送10us以上的触发脉冲
    TRIG_HIGH();
    Delay_us(20);  // 20us触发脉冲
    TRIG_LOW();
    
    // 等待Echo变为高电平（等待回声开始）
    timeout = 0;
    while (ECHO_STATE() == 0)
    {
        Delay_us(1);
        timeout++;
        if (timeout > 10000)  // 超时10ms（无回声信号）
        {
            return -2.0f;
        }
    }
    
    // 开始计时
    TIM_SetCounter(TIM13, 0);
    TIM_Cmd(TIM13, ENABLE);
    
    // 等待Echo变为低电平（等待回声结束）
    timeout = 0;
    while (ECHO_STATE() == 1)
    {
        Delay_us(1);
        timeout++;
        if (timeout > 60000)  // 超时60ms（超出最大测量范围）
        {
            TIM_Cmd(TIM13, DISABLE);
            return -1.0f;
        }
    }
    
    // 停止计时并读取时间
    TIM_Cmd(TIM13, DISABLE);
    time_us = TIM_GetCounter(TIM13);
    
    // 计算距离（340m/s = 0.034cm/us，往返距离需除以2）
    if (time_us < 60000)  // 小于60ms（约10.2米）
    {
        return (float)time_us * 0.034f / 2.0f;
    }
    else
    {
        return -1.0f;  // 超出测量范围
    }
}

/**
  * @简  述  初始化
  * @参  数  无
  * @返回值  无
  */
void BEEP_Init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//GPIO配置 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 
	
	//配置GPIO  推挽输出模式 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//关闭蜂鸣器
	GPIO_ResetBits(GPIOC,  GPIO_Pin_12);	

}	

/**
 * @函数描述: 蜂鸣器鸣叫时间，单位ms
 * @param {uint16_t} times
 * @return {*}
 */
void beep_on_times(int times, int delay)
{
    int i;
    for (i = 0; i < times; i++)
    {
        BEEP_On();
        Delay_ms(delay);
        BEEP_Off();
        Delay_ms(delay);
    }
}


// LED初始化函数
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIOD时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    // 配置PD14和PD15为输出
    GPIO_InitStructure.GPIO_Pin = LED1_PIN | LED7_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;    // 100MHz速率
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;      // 无上下拉
    GPIO_Init(LED_PORT, &GPIO_InitStructure);
    
    // 默认熄灭（高电平熄灭，可根据硬件修改）
    GPIO_SetBits(LED_PORT, LED1_PIN | LED7_PIN);
}

// LED1点亮
void LED1_On(void)
{
    GPIO_ResetBits(LED_PORT, LED1_PIN);  // 输出低电平点亮
}

// LED1熄灭
void LED1_Off(void)
{
    GPIO_SetBits(LED_PORT, LED1_PIN);    // 输出高电平熄灭
}

// LED1状态翻转
void LED1_Turn(void)
{
    // 读取当前状态并翻转
    if(GPIO_ReadOutputDataBit(LED_PORT, LED1_PIN))
    {
        LED1_On();
    }
    else
    {
        LED1_Off();
    }
}

// LED7点亮
void LED7_On(void)
{
    GPIO_ResetBits(LED_PORT, LED7_PIN);  // 输出低电平点亮
}

// LED7熄灭
void LED7_Off(void)
{
    GPIO_SetBits(LED_PORT, LED7_PIN);    // 输出高电平熄灭
}

// LED7状态翻转
void LED7_Turn(void)
{
    // 读取当前状态并翻转
    if(GPIO_ReadOutputDataBit(LED_PORT, LED7_PIN))
    {
        LED7_On();
    }
    else
    {
        LED7_Off();
    }
}
    

