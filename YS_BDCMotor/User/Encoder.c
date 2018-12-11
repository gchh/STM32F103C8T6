#include "Encoder.h"

void ENCODER_GPIO_Init(void)
{
    //定义一个GPIO_InitTypeDef 类型的结构体
    GPIO_InitTypeDef  GPIO_InitStructure;	
    
    RCC_APB2PeriphClockCmd(ENCODER_TIM_CH1_GPIO_RCC | ENCODER_TIM_CH2_GPIO_RCC, ENABLE);//使能外设时钟
    
    /*PA6: TIM3_CH1*/
    GPIO_InitStructure.GPIO_Pin = ENCODER_TIM_CH1_PIN;//选择要用的GPIO引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //设置引脚模式为复用功能推免输出模式						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置引脚速度为50MHZ         
    GPIO_Init(ENCODER_TIM_CH1_PORT, &GPIO_InitStructure);//调用库函数，初始化GPIO    
    
    /*PA7: TIM3_CH2*/
    GPIO_InitStructure.GPIO_Pin = ENCODER_TIM_CH2_PIN;//选择要用的GPIO引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //设置引脚模式为浮空输入模式						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置引脚速度为50MHZ         
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
    
}




