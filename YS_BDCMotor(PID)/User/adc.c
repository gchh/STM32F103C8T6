#include "adc.h"
extern __IO uint32_t uwTick;

#define ADC_ENABLE_TIMEOUT              ((uint32_t) 2)
#define ADC_DISABLE_TIMEOUT             ((uint32_t) 2)
#define ADC_PRECALIBRATION_DELAY_ADCCLOCKCYCLES       ((uint32_t) 2)
#define ADC_STAB_DELAY_US               ((uint32_t) 1)
#define ADC_CALIBRATION_TIMEOUT         ((uint32_t) 10)

// 用于保存转换计算后的数值
__IO float ADC_CurrentValue;
__IO float ADC_VoltBus;
// AD转换结果值
__IO int16_t ADC_ConvValueHex[ADC_BUFFER];   // AD转换结果
__IO int32_t ADC_Resul= 0;
__IO uint32_t OffsetCnt_Flag = 0 ;           // 偏差值的计数器标志
__IO int32_t OffSetHex= 0;             // 偏差值
__IO int32_t AverSum = 0;                   // 平均值的累加值
__IO int32_t AverCnt = 0;                   // 平均值的计数器

void ADC_CUR_GPIO_Init(void)
{
    //定义一个GPIO_InitTypeDef 类型的结构体
    GPIO_InitTypeDef  GPIO_InitStructure;	

    RCC_APB2PeriphClockCmd(ADC_CUR_GPIO_RCC, ENABLE);//使能外设时钟
    
    /*PC0*/
    GPIO_InitStructure.GPIO_Pin = ADC_CUR_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ADC_CUR_GPIO, &GPIO_InitStructure);//调用库函数，初始化GPIO 
}

void ADC_VOLT_GPIO_Init(void)
{
    //定义一个GPIO_InitTypeDef 类型的结构体
    GPIO_InitTypeDef  GPIO_InitStructure;	

    RCC_APB2PeriphClockCmd(ADC_VOLT_GPIO_RCC, ENABLE);//使能外设时钟
    
    /*PC1*/
    GPIO_InitStructure.GPIO_Pin = ADC_VOLT_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
    GPIO_Init(ADC_VOLT_GPIO, &GPIO_InitStructure);//调用库函数，初始化GPIO    
}

void ADC_CUR_Init(void)
{
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
}

void ADC_VOLT_Init(void)
{
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
    
    /* 配置总线电压采集 */
    /* 模拟看门狗配置 */
    ADC_AnalogWatchdogThresholdsConfig(ADCx, VOLT_LIMIT_MAX, VOLT_LIMIT_MIN);
    ADC_AnalogWatchdogSingleChannelConfig(ADCx, ADC_VOLT_CHANNEL);
    ADC_AnalogWatchdogCmd(ADCx, ADC_AnalogWatchdog_SingleRegEnable);
    ADC_ITConfig(ADCx, ADC_IT_AWD, ENABLE);
    
    ADC_RegularChannelConfig(ADCx, ADC_VOLT_CHANNEL, 2, ADC_SampleTime_13Cycles5);

    NVIC_SetPriority(ADC_OVP_IRQx, 0);
    NVIC_EnableIRQ(ADC_OVP_IRQx);
}

void ADC_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructor;
    
    /* DMA时钟使能 */
    RCC_AHBPeriphClockCmd(DMAx_RCC, ENABLE);
    
    DMA_InitStructor.DMA_DIR = DMA_DIR_PeripheralSRC; //DMA方向外设到内存
    DMA_InitStructor.DMA_BufferSize = ADC_BUFFER; //DMA传输数量
    DMA_InitStructor.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructor.DMA_MemoryBaseAddr = (uint32_t)&(ADC_ConvValueHex);
    DMA_InitStructor.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructor.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructor.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructor.DMA_PeripheralBaseAddr = (uint32_t)&(ADCx->DR);
    DMA_InitStructor.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructor.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructor.DMA_Priority = DMA_Priority_High;
    DMA_Init(DMAx_CHANNEL_x, &DMA_InitStructor);
    
    /* 外设中断优先级配置和使能中断 */
    NVIC_SetPriority(ADCx_DMA_IRQx, 1);
    NVIC_EnableIRQ(ADCx_DMA_IRQx);
}

void ADC_Calibration(void)
{
    __IO uint32_t wait_loop_index = 0;
    RCC_ClocksTypeDef RCC_Clocks;
    
    //校准
    /* 1. Disable ADC peripheral */
    if(ADCx->CR2 & ADC_CR2_ADON) //ADC已开启转换，校准前先关闭
    {
        ADC_Cmd(ADCx, DISABLE);
        //等待ADC停止
        while( (ADCx->CR2 & ADC_CR2_ADON) != RESET ){}
    }

    //校准前，ADC要停止至少2个ADC时钟周期
    RCC_GetClocksFreq(&RCC_Clocks);
    wait_loop_index = ( (SystemCoreClock / RCC_Clocks.ADCCLK_Frequency) 
                        * ADC_PRECALIBRATION_DELAY_ADCCLOCKCYCLES );        
    while(wait_loop_index != 0)
    {
      wait_loop_index--;
    }
    
    /* 2. Enable the ADC peripheral */
    if( (ADCx->CR2 & ADC_CR2_ADON) == RESET )
    {
        ADC_Cmd(ADCx, ENABLE);
        //等待ADC开启
        while( (ADCx->CR2 & ADC_CR2_ADON) == RESET ){}
    }
    
    /* 3. Resets ADC calibration registers */  
    ADC_ResetCalibration(ADCx);
    while( ADC_GetResetCalibrationStatus(ADCx) == SET ){}
    
    /* 4. Start ADC calibration */
    ADC_StartCalibration(ADCx);
    while( ADC_GetCalibrationStatus(ADCx) == SET ){}
}

void ADC_Start_DMA(void)
{  
    ADC_DMACmd(ADCx, ENABLE);
    
    ADC_SoftwareStartConvCmd(ADCx, ENABLE);
    
    /* Enable the transfer complete interrupt */
    /* Enable the Half transfer complete interrupt */
    /* Enable the transfer Error interrupt */
    //DMA_ITConfig(DMAx_CHANNEL_x, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE, ENABLE);
    DMA_ITConfig(DMAx_CHANNEL_x, DMA_IT_TC, ENABLE);
    
    /* Enable the Peripheral */
    DMA_Cmd(DMAx_CHANNEL_x, ENABLE);    
}

/*
  * 函数功能: 设置AD转换通道的转换顺序为1
  * 输入参数: hadc ADC句柄 , Channel可以是ADC_VOLT_CHANNEL,ADC_CURRENT_CHANNEL
  * 返 回 值: 无
  * 说    明: 无
  */
void SetChannelAsRank1(uint32_t Channel)
{
    if(Channel == ADC_VOLT_CHANNEL)
    {
        /* 配置电压通道 */
        ADC_RegularChannelConfig(ADCx, ADC_VOLT_CHANNEL, 1, ADC_SampleTime_13Cycles5);
        /* 配置电流通道 */
        ADC_RegularChannelConfig(ADCx, ADC_CURRENT_CHANNEL, 2, ADC_SampleTime_13Cycles5);
    }
    else
    {
        /* 配置电流通道 */
        ADC_RegularChannelConfig(ADCx, ADC_CURRENT_CHANNEL, 1, ADC_SampleTime_13Cycles5);
        /* 配置电压通道 */
        ADC_RegularChannelConfig(ADCx, ADC_VOLT_CHANNEL, 2, ADC_SampleTime_13Cycles5);
    }
}
__IO int32_t ADConv;
void ADCx_DMA_IRQx_Handler(void)
{
    __IO uint16_t ConvCnt = 0;
    //__IO int32_t 
    ADConv = 0 ;

    /* Transfer Error Interrupt management ***************************************/
    if( DMA_GetITStatus(DMA1_IT_TE1) != RESET )
    {
        /* Disable the transfer error interrupt */
        DMA_ITConfig(DMAx_CHANNEL_x, DMA_IT_TE, DISABLE);
        /* Clear the transfer error flag */
        DMA_ClearITPendingBit(DMA1_IT_TE1);
    }
    /* Half Transfer Complete Interrupt management ******************************/
    if( DMA_GetITStatus(DMA1_IT_HT1) != RESET )
    {
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if((DMAx_CHANNEL_x->CCR & DMA_CCR1_CIRC) == 0)
        {
            /* Disable the half transfer interrupt */
            DMA_ITConfig(DMAx_CHANNEL_x, DMA_IT_HT, DISABLE);
        }     
        /* Clear the half transfer complete flag */
        DMA_ClearITPendingBit(DMA1_IT_HT1);        
    }
    /* Transfer Complete Interrupt management ***********************************/
    if( DMA_GetITStatus(DMA1_IT_TC1) != RESET )
    {
        if((DMAx_CHANNEL_x->CCR & DMA_CCR1_CIRC) == 0)
        {
            /* Disable the transfer complete interrupt */
            DMA_ITConfig(DMAx_CHANNEL_x, DMA_IT_TC, DISABLE);
        }       
        /* Clear the transfer complete flag */
        DMA_ClearITPendingBit(DMA1_IT_TC1);
        
        /* ADC采集太快,需要先停止DMA再处理数据 */
        ADC_Cmd(ADCx, DISABLE);
        while( (ADCx->CR2 & ADC_CR2_ADON) != RESET );
        ADC_DMACmd(ADCx, DISABLE);
        ADC_SoftwareStartConvCmd(ADCx, DISABLE);
        DMA_Cmd(DMAx_CHANNEL_x, DISABLE);
        while( (DMAx_CHANNEL_x->CCR & DMA_CCR1_EN) != RESET );
        /* 采集总线电压 */
        SetChannelAsRank1(ADC_VOLT_CHANNEL);
        ADC_Cmd(ADCx, ENABLE);
        while( (ADCx->CR2 & ADC_CR2_ADON) == RESET );
        ADC_SoftwareStartConvCmd(ADCx, ENABLE);
        
        
        /* 取平均 */
        for(ConvCnt = 0; ConvCnt < ADC_BUFFER; ConvCnt++)
        {
            ADConv += ((int32_t)ADC_ConvValueHex[ConvCnt]);
        }
        /* 采样数据设置为2的整数倍 */
        ADConv = ADConv>>ADC_Base;
        /* 累加采样结果并记录采样次数*/
        AverSum += ADConv;
        AverCnt++;        
        
        
        /* 读取总线电压值 */
        while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) == RESET);
        ADC_VoltBus = ADCx->DR;
        ADC_ClearFlag(ADCx, ADC_FLAG_EOC);
        
        ADC_Cmd(ADCx, DISABLE);
        while( (ADCx->CR2 & ADC_CR2_ADON) != RESET );
        ADC_SoftwareStartConvCmd(ADCx, DISABLE);
        SetChannelAsRank1(ADC_CURRENT_CHANNEL);
        /* 重新开启DMA 采集电流 */
        ADC_Cmd(ADCx, ENABLE);
        while( (ADCx->CR2 & ADC_CR2_ADON) == RESET );
        ADC_DMACmd(ADCx, ENABLE);
        ADC_SoftwareStartConvCmd(ADCx, ENABLE);
        DMA_Cmd(DMAx_CHANNEL_x, ENABLE);
        while( (DMAx_CHANNEL_x->CCR & DMA_CCR1_EN) == RESET );     
    }
}

extern void ADC_LevelOutOfWindowCallback(void);
void ADC_OVP_IRQHandler(void)
{
    /* 读取总线电压值 */
    //while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) == RESET);
    //if( ADC_GetITStatus(ADCx, ADC_IT_EOC) != RESET)
    //{
        //ADC_VoltBus = ADCx->DR;
    //    ADC_ClearITPendingBit(ADCx, ADC_IT_EOC);
    //}
    /* Check Analog watchdog flag */
    //ADC窗口看门狗,检测到电压过低或者过高的时候,停止输出.  
    if( ADC_GetITStatus(ADCx, ADC_IT_AWD) != RESET)
    {
        ADC_LevelOutOfWindowCallback();
        ADC_ClearITPendingBit(ADCx, ADC_IT_AWD);
    }
}






