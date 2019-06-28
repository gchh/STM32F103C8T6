#include "adc.h"
extern __IO uint32_t uwTick;

#define ADC_DISABLE_TIMEOUT             ((uint32_t) 2)

void ADC_CUR_GPIO_Init(void)
{
    //定义一个GPIO_InitTypeDef 类型的结构体
    GPIO_InitTypeDef  GPIO_InitStructure;	

    RCC_APB2PeriphClockCmd(ADC_CUR_GPIO_RCC, ENABLE);//使能外设时钟
    
    /*PC0*/
    GPIO_InitStructure.GPIO_Pin = ADC_CUR_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ADC_CUR_GPIO, &GPIO_InitStructure);//调用库函数，初始化GPIO
}

void ADC_CUR_Init(void)
{
    uint32_t tickstart = 0, adc_error = 0;
    ADC_InitTypeDef ADC_InitStructure;
    
    RCC_APB2PeriphClockCmd(ADCx_RCC, ENABLE);//使能外设时钟
    
    //ADC1
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //独立模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //连续转换模式使能
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; //扫描模式关闭
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //数据右对齐
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //软件触发
    ADC_InitStructure.ADC_NbrOfChannel = 1; //转换通道数量
    ADC_Init(ADCx, &ADC_InitStructure);
    
    /* 配置电流采样通道 */
    ADC_RegularChannelConfig(ADCx, ADC_CURRENT_CHANNEL, 1, ADC_SampleTime_13Cycles5);
    //校准
    if(ADCx->CR2 & ADC_CR2_ADON) //ADC已开启转换，校准前先关闭
    {
        ADC_Cmd(ADCx, DISABLE);
        tickstart = uwTick;
        //等待ADC停止
        while( (ADCx->CR2 & ADC_CR2_ADON) != RESET )
        {
            if((uwTick - tickstart) > ADC_DISABLE_TIMEOUT)
            {
                adc_error=1;
            }            
        }
    }
    if(adc_error==0)
    {
        ADC_ResetCalibration(ADCx);
        while( ADC_GetResetCalibrationStatus(ADCx) == SET );
        ADC_StartCalibration(ADCx);
        while( ADC_GetCalibrationStatus(ADCx) == SET );
    }
}














