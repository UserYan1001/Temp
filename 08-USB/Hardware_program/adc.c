/****************************************************************************
 *	@笔者	：	YJH
 *	@日期	：	2025-09-09
 *	@所属	：	杭州星呗科技有限公司
 *****************************************************************************/
#include "adc.h"

// 校准系数定义
#define ADC_REVISE         1.0f    // ADC校准系数
#define BATTERY_VOLTAGE_RATIO 11.0f // 电池电压分压比

/**
 * @brief  初始化电池电压测量通道(PC0 - ADC2_CH10)
 * @param  无
 * @retval 无
 */
void BatteryVoltage_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    // 使能ADC2和GPIOC时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

    // 配置PC0为模拟输入（电池电压测量）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 复位ADC2外设
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2, DISABLE);

    // ADC通用配置
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
    ADC_CommonInit(&ADC_CommonInitStructure);

    // 配置ADC2参数
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC2, &ADC_InitStructure);

    // 配置ADC2通道（PC0对应ADC_Channel_10）
    ADC_RegularChannelConfig(ADC2, ADC_Channel_10, 1, ADC_SampleTime_480Cycles);

    // 使能ADC2
    ADC_Cmd(ADC2, ENABLE);
}

/**
 * @brief  获取电池电压原始AD值
 * @param  无
 * @retval 12位AD转换结果
 */
uint16_t BatteryVoltage_GetRawAD(void)
{
    // 启动ADC2转换
    ADC_SoftwareStartConv(ADC2);
    
    // 等待转换完成
    while (!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC));
    
    // 返回转换结果
    return ADC_GetConversionValue(ADC2);
}

/**
 * @brief  获取电池电压值（放大100倍，用于整数处理）
 * @param  无
 * @retval 电压值 * 100 (单位：V)
 */
uint16_t BatteryVoltage_GetVol_X100(void)
{
    uint16_t adValue = BatteryVoltage_GetRawAD();
    // 计算电压（分压比例、参考电压3.3V，12位ADC最大值4095）
    return (uint16_t)((3.3f * BATTERY_VOLTAGE_RATIO * ADC_REVISE * adValue) / 4095.0f * 100.0f);
}

/**
 * @brief  初始化电机电流测量通道
 *         PA2(ADC1_CH2)、PA3(ADC1_CH3)、PA4(ADC1_CH4)、PC1(ADC1_CH11)
 * @param  无
 * @retval 无
 */
void MotorCurrent_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    // 使能ADC1和GPIOA、GPIOC时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // 配置GPIOA引脚为模拟输入（电机电流测量）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置GPIOC引脚为模拟输入（电机电流测量）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 复位ADC1外设
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE);

    // ADC通用配置
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
    ADC_CommonInit(&ADC_CommonInitStructure);

    // 配置ADC1参数
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // 使能ADC1
    ADC_Cmd(ADC1, ENABLE);
}

/**
 * @brief  通用函数：获取指定ADC1通道的原始AD值
 * @param  channel: ADC通道号
 * @retval 12位AD转换结果
 */
static uint16_t MotorCurrent_GetRawValue(uint8_t channel)
{
    // 配置指定的ADC通道
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_480Cycles);
    
    // 启动ADC转换
    ADC_SoftwareStartConv(ADC1);
    
    // 等待转换完成
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    
    // 返回转换结果
    return ADC_GetConversionValue(ADC1);
}

/**
 * @brief  获取PA2通道电机电流原始AD值
 * @param  无
 * @retval 12位AD转换结果
 */
uint16_t MotorCurrent_PA2_GetRawAD(void)
{
    return MotorCurrent_GetRawValue(ADC_Channel_2);
}

/**
 * @brief  获取PA2通道电机电流值（放大100倍）
 * @param  无
 * @retval 电流值 * 100 (单位：A)
 */
uint16_t MotorCurrent_PA2_GetCurrent_X100(void)
{
    uint16_t adValue = MotorCurrent_PA2_GetRawAD();
    // 根据电流传感器特性转换AD值为电流值
    return (uint16_t)((3.3f * adValue / 4095.0f) * ADC_REVISE);
}

/**
 * @brief  获取PA3通道电机电流原始AD值
 * @param  无
 * @retval 12位AD转换结果
 */
uint16_t MotorCurrent_PA3_GetRawAD(void)
{
    return MotorCurrent_GetRawValue(ADC_Channel_3);
}

/**
 * @brief  获取PA3通道电机电流值（放大100倍）
 * @param  无
 * @retval 电流值 * 100 (单位：A)
 */
uint16_t MotorCurrent_PA3_GetCurrent_X100(void)
{
    uint16_t adValue = MotorCurrent_PA3_GetRawAD();
    return (uint16_t)((3.3f * adValue / 4095.0f) * ADC_REVISE);
}

/**
 * @brief  获取PA4通道电机电流原始AD值
 * @param  无
 * @retval 12位AD转换结果
 */
uint16_t MotorCurrent_PA4_GetRawAD(void)
{
    return MotorCurrent_GetRawValue(ADC_Channel_4);
}

/**
 * @brief  获取PA4通道电机电流值（放大100倍）
 * @param  无
 * @retval 电流值 * 100 (单位：A)
 */
uint16_t MotorCurrent_PA4_GetCurrent_X100(void)
{
    uint16_t adValue = MotorCurrent_PA4_GetRawAD();
    return (uint16_t)((3.3f * adValue / 4095.0f) * ADC_REVISE);
}

/**
 * @brief  获取PC1通道电机电流原始AD值
 * @param  无
 * @retval 12位AD转换结果
 */
uint16_t MotorCurrent_PC1_GetRawAD(void)
{
    return MotorCurrent_GetRawValue(ADC_Channel_11);
}

/**
 * @brief  获取PC1通道电机电流值（放大100倍）
 * @param  无
 * @retval 电流值 * 100 (单位：A)
 */
uint16_t MotorCurrent_PC1_GetCurrent_X100(void)
{
    uint16_t adValue = MotorCurrent_PC1_GetRawAD();
    return (uint16_t)((3.3f * adValue / 4095.0f) * ADC_REVISE);
}
