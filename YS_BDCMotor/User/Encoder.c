#include "Encoder.h"

int16_t OverflowCount = 0;//定时器溢出次数

void ENCODER_GPIO_Init(void)
{
    //定义一个GPIO_InitTypeDef 类型的结构体
    GPIO_InitTypeDef  GPIO_InitStructure;	
    
    RCC_APB2PeriphClockCmd(ENCODER_TIM_CH1_GPIO_RCC | ENCODER_TIM_CH2_GPIO_RCC, ENABLE);//使能外设时钟
    
    /*PA6: TIM3_CH1*/
    GPIO_InitStructure.GPIO_Pin = ENCODER_TIM_CH1_PIN;//选择要用的GPIO引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_AF_PP; //设置引脚模式为浮空输入模式						 
    GPIO_InitStructure.GPIO_Speed = 0;//GPIO_Speed_50MHz;//设置引脚速度为50MHZ----只有引脚为输出时，才需要设置速度
    GPIO_Init(ENCODER_TIM_CH1_PORT, &GPIO_InitStructure);//调用库函数，初始化GPIO    
    
    /*PA7: TIM3_CH2*/
    GPIO_InitStructure.GPIO_Pin = ENCODER_TIM_CH2_PIN;//选择要用的GPIO引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_AF_PP; //设置引脚模式为浮空输入模式						 
    GPIO_InitStructure.GPIO_Speed = 0;//GPIO_Speed_50MHz;//设置引脚速度为50MHZ         
    GPIO_Init(ENCODER_TIM_CH2_PORT, &GPIO_InitStructure);//调用库函数，初始化GPIO      
}

void ENCODER_TIMx_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef sEncoderConfig;
    RCC_APB1PeriphClockCmd(ENCODER_TIM_RCC, ENABLE);//使能外设时钟
    
    /* 定时器基本环境配置 */
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler = ENCODER_TIM_PRESCALER;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(ENCODER_TIMx, &TIM_TimeBaseStructure);    
    
    TIM_SetCounter(ENCODER_TIMx, 0);
    
#if 1  
    TIM_EncoderInterfaceConfig(ENCODER_TIMx, TIM_ENCODERMODE_TIx, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    ENCODER_TIMx->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
#else
    ENCODER_TIMx->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
    
    sEncoderConfig.TIM_Channel = TIM_Channel_1;
    sEncoderConfig.TIM_ICFilter = 0;
    sEncoderConfig.TIM_ICPolarity = TIM_ICPolarity_Rising;
    sEncoderConfig.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    sEncoderConfig.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInit(ENCODER_TIMx, &sEncoderConfig);
    
    sEncoderConfig.TIM_Channel = TIM_Channel_2;
    TIM_ICInit(ENCODER_TIMx, &sEncoderConfig);
#endif
    //TIM_ClearFlag(ENCODER_TIMx, TIM_FLAG_Update); // 清除更新中断标志位
    TIM_ClearITPendingBit(ENCODER_TIMx, TIM_IT_Update);
    TIM_UpdateRequestConfig(ENCODER_TIMx, TIM_UpdateSource_Regular); // 仅允许计数器溢出才产生更新中断
    TIM_ITConfig(ENCODER_TIMx, TIM_IT_Update, ENABLE); // 使能更新中断

    NVIC_SetPriority(ENCODER_TIM_IRQn, 0);
    NVIC_EnableIRQ(ENCODER_TIM_IRQn);     
    
    TIM_Cmd(ENCODER_TIMx, ENABLE);
}


/**
  * 函数功能: 中断服务函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 记录溢出次数
  */
void ENCODER_TIM_IRQHANDLER(void)
{
    /* TIM Update event */
    //if(TIM_GetFlagStatus(ENCODER_TIMx, TIM_FLAG_Update) != RESET)
    if(TIM_GetITStatus(ENCODER_TIMx, TIM_IT_Update) !=RESET)
    {
        //TIM_ClearFlag(ENCODER_TIMx, TIM_FLAG_Update); // 清除更新中断标志位
        TIM_ClearITPendingBit(ENCODER_TIMx, TIM_IT_Update);
        
        if(ENCODER_TIMx->CR1 & TIM_CR1_DIR)
            OverflowCount--;       //向下计数溢出
        else
            OverflowCount++;       //向上计数溢出
    }
}

