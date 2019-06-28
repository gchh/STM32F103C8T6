#include "adc.h"
extern __IO uint32_t uwTick;

#define ADC_ENABLE_TIMEOUT              ((uint32_t) 2)
#define ADC_DISABLE_TIMEOUT             ((uint32_t) 2)
#define ADC_PRECALIBRATION_DELAY_ADCCLOCKCYCLES       ((uint32_t) 2)
#define ADC_STAB_DELAY_US               ((uint32_t) 1)
#define ADC_CALIBRATION_TIMEOUT         ((uint32_t) 10)

// AD转换结果值
__IO int16_t ADC_ConvValueHex[ADC_BUFFER];   // AD转换结果

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
    __IO uint32_t wait_loop_index = 0;
    ADC_InitTypeDef ADC_InitStructure;
    RCC_ClocksTypeDef RCC_Clocks;
    
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
    /* 1. Disable ADC peripheral */
    if(ADCx->CR2 & ADC_CR2_ADON) //ADC已开启转换，校准前先关闭
    {
        ADC_Cmd(ADCx, DISABLE);
        tickstart = uwTick;
        //等待ADC停止
        while( (ADCx->CR2 & ADC_CR2_ADON) != RESET )
        {
            if((uwTick - tickstart) > ADC_DISABLE_TIMEOUT)
            {
                adc_error=0X01;
            }            
        }
    }
    if(adc_error==0)
    {
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
            //等待ADC稳定
            wait_loop_index = (ADC_STAB_DELAY_US * (SystemCoreClock / 1000000));
            while(wait_loop_index != 0)
            {
              wait_loop_index--;
            }            
            tickstart = uwTick;
            //等待ADC开启
            while( (ADCx->CR2 & ADC_CR2_ADON) == RESET )
            {
                if((uwTick - tickstart) > ADC_ENABLE_TIMEOUT)
                {
                    adc_error=0X02;
                }
            }
        }
        
        /* 3. Resets ADC calibration registers */  
        ADC_ResetCalibration(ADCx);
        tickstart = uwTick;
        while( ADC_GetResetCalibrationStatus(ADCx) == SET )
        {
            if((uwTick - tickstart) > ADC_CALIBRATION_TIMEOUT)
            {
                adc_error=0X04;
            }            
        }
        
        /* 4. Start ADC calibration */
        ADC_StartCalibration(ADCx);
        tickstart = uwTick;
        while( ADC_GetCalibrationStatus(ADCx) == SET )
        {
            if((uwTick - tickstart) > ADC_CALIBRATION_TIMEOUT)
            {
                adc_error=0X08;
            }             
        }
    }
}

void ADC_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructor;
    
    /* DMA时钟使能 */
    RCC_AHBPeriphClockCmd(DMAx_RCC, ENABLE);
    
    DMA_InitStructor.DMA_DIR = DMA_DIR_PeripheralSRC; //DMA方向外设到内存
    DMA_InitStructor.DMA_BufferSize = ADC_BUFFER; //DMA传输数量
    DMA_InitStructor.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructor.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvValueHex;
    DMA_InitStructor.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructor.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructor.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructor.DMA_PeripheralBaseAddr = ADCx->DR;
    DMA_InitStructor.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructor.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructor.DMA_Priority = DMA_Priority_High;
    DMA_Init(DMAx_CHANNEL_x, &DMA_InitStructor);
    
    /* 外设中断优先级配置和使能中断 */
    NVIC_SetPriority(ADCx_DMA_IRQx, 0);
    NVIC_EnableIRQ(ADCx_DMA_IRQx);
}

void ADC_Start_DMA(void)
{
    /* Enable ADC DMA mode */
    ADC_DMACmd(ADCx, ENABLE);
    
    /* Enable the transfer complete interrupt */
    /* Enable the Half transfer complete interrupt */
    /* Enable the transfer Error interrupt */
    DMA_ITConfig(DMAx_CHANNEL_x, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE, ENABLE);
    
    /* Enable the Peripheral */
    DMA_Cmd(DMAx_CHANNEL_x, ENABLE);    
}


void ADCx_DMA_IRQx_Handler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

  /* USER CODE END DMA2_Stream0_IRQn 0 */

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
    }
    
  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

  /* USER CODE END DMA2_Stream0_IRQn 1 */
}








