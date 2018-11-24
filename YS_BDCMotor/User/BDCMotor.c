#include "BDCMotor.h"

void BDCMOTOR_GPIO_Init(void)
{
    //定义一个GPIO_InitTypeDef 类型的结构体
    GPIO_InitTypeDef  GPIO_InitStructure;	
    
    RCC_APB2PeriphClockCmd(BDCMOTOR_TIM_CH1_GPIO_RCC | BDCMOTOR_TIM_CH1N_GPIO_RCC | RCC_APB2Periph_AFIO, ENABLE);//使能外设时钟
    
    /*PA8*/
    GPIO_InitStructure.GPIO_Pin = BDCMOTOR_TIM_CH1_PIN;//选择要用的GPIO引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //设置引脚模式为复用功能推免输出模式						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置引脚速度为50MHZ         
    GPIO_Init(BDCMOTOR_TIM_CH1_PORT, &GPIO_InitStructure);//调用库函数，初始化GPIO    
    
    /*PB13*/
    GPIO_InitStructure.GPIO_Pin = BDCMOTOR_TIM_CH1N_PIN;//选择要用的GPIO引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //设置引脚模式为复用功能推免输出模式						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置引脚速度为50MHZ         
    GPIO_Init(BDCMOTOR_TIM_CH1N_PORT, &GPIO_InitStructure);//调用库函数，初始化GPIO        
    
    /*PB12*/
    GPIO_InitStructure.GPIO_Pin = SHUTDOWN_PIN;//选择要用的GPIO引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置引脚模式为推免输出模式						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置引脚速度为50MHZ         
    GPIO_Init(SHUTDOWN_PORT, &GPIO_InitStructure);//调用库函数，初始化GPIO    

    SHUTDOWN_OFF;
}

void BDCMOTOR_TIMx_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_BDTRInitTypeDef  sBreakDeadTimeConfig;
    TIM_OCInitTypeDef  TIM_OCInitStructure;    
    /* 基本定时器外设时钟使能 */
    RCC_APB2PeriphClockCmd(BDCMOTOR_TIM_RCC,ENABLE);//使能外设TIM1时钟

    /* 定时器基本环境配置 */
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = BDCMOTOR_TIM_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler = BDCMOTOR_TIM_PRESCALER;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = BDCMOTOR_TIM_REPETITIONCOUNTER;
    TIM_TimeBaseInit(BDCMOTOR_TIMx, &TIM_TimeBaseStructure);
    
    /* 定时器时钟源配置 */
    TIM_InternalClockConfig(BDCMOTOR_TIMx); // 使用内部时钟源
    
    /* 刹车和死区时间配置 */
    sBreakDeadTimeConfig.TIM_OSSRState = TIM_BDTR_OSSR;
    sBreakDeadTimeConfig.TIM_OSSIState = 0;
    sBreakDeadTimeConfig.TIM_LOCKLevel = 0;
    sBreakDeadTimeConfig.TIM_DeadTime = 0x00;
    sBreakDeadTimeConfig.TIM_Break = 0;
    sBreakDeadTimeConfig.TIM_BreakPolarity = 0;
    sBreakDeadTimeConfig.TIM_AutomaticOutput = 0;    
    TIM_BDTRConfig(BDCMOTOR_TIMx, &sBreakDeadTimeConfig);
    
    /* 定时器比较输出配置 */
    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                  // 比较输出模式：PWM1模式
    TIM_OCInitStructure.TIM_Pulse =  0;                                // 占空比
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;           // 输出极性
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;         // 互补通道输出极性
    //TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;           // 快速模式
    TIM_OCInitStructure.TIM_OCIdleState = 0;                           // 空闲电平
    TIM_OCInitStructure.TIM_OCNIdleState = 0;                          // 互补通道空闲电平
    //TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    //TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OC1Init(BDCMOTOR_TIMx, &TIM_OCInitStructure); 
    
    //TIM_OC1PreloadConfig(BDCMOTOR_TIMx, TIM_OCPreload_Enable);
    //TIM_ARRPreloadConfig(BDCMOTOR_TIMx, ENABLE);
    TIM_CtrlPWMOutputs(BDCMOTOR_TIMx, ENABLE);
    
    /* TIM1 enable counter */
    TIM_Cmd(BDCMOTOR_TIMx, ENABLE);
    TIM_CCxCmd(BDCMOTOR_TIMx, TIM_Channel_1, TIM_CCx_Enable);
    TIM_CCxNCmd(BDCMOTOR_TIMx, TIM_Channel_1, TIM_CCxN_Disable);  // 停止输出
}

/**
  * 函数功能: 设置电机速度
  * 输入函数: Duty,输出脉冲占空比
  * 返 回 值: 无
  * 说    明: 无
  */
void SetMotorSpeed(int16_t Duty)
{
    TIM_SetCompare1(BDCMOTOR_TIMx,Duty);
}

/**
  * 函数功能: 设置电机转动方向
  * 输入函数: Dir,电机转动方向
  * 返 回 值: 无
  * 说    明: 无
  */
void SetMotorDir(int16_t Dir)
{
    if(Dir)
    {
        TIM_CCxCmd(BDCMOTOR_TIMx, TIM_Channel_1, TIM_CCx_Enable);
        TIM_CCxNCmd(BDCMOTOR_TIMx, TIM_Channel_1, TIM_CCxN_Disable);  // 停止输出      
    }
    else
    {
        TIM_CCxCmd(BDCMOTOR_TIMx, TIM_Channel_1, TIM_CCx_Disable);   // 停止输出  
        TIM_CCxNCmd(BDCMOTOR_TIMx, TIM_Channel_1, TIM_CCxN_Enable);  
    }
}


void SHUTDOWN_Toggle(void)
{
    SHUTDOWN_PORT->ODR^=SHUTDOWN_PIN;
}


