#include "BDCMotor.h"

void BDCMOTOR_GPIO_Init(void)
{
    //����һ��GPIO_InitTypeDef ���͵Ľṹ��
    GPIO_InitTypeDef  GPIO_InitStructure;	
    
    RCC_APB2PeriphClockCmd(BDCMOTOR_TIM_CH1_GPIO_RCC | BDCMOTOR_TIM_CH1N_GPIO_RCC | RCC_APB2Periph_AFIO, ENABLE);//ʹ������ʱ��
    
    /*PA8*/
    GPIO_InitStructure.GPIO_Pin = BDCMOTOR_TIM_CH1_PIN;//ѡ��Ҫ�õ�GPIO����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //��������ģʽΪ���ù����������ģʽ						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���������ٶ�Ϊ50MHZ         
    GPIO_Init(BDCMOTOR_TIM_CH1_PORT, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO    
    
    /*PB13*/
    GPIO_InitStructure.GPIO_Pin = BDCMOTOR_TIM_CH1N_PIN;//ѡ��Ҫ�õ�GPIO����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //��������ģʽΪ���ù����������ģʽ						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���������ٶ�Ϊ50MHZ         
    GPIO_Init(BDCMOTOR_TIM_CH1N_PORT, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO        
    
    /*PB12*/
    GPIO_InitStructure.GPIO_Pin = SHUTDOWN_PIN;//ѡ��Ҫ�õ�GPIO����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //��������ģʽΪ�������ģʽ						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���������ٶ�Ϊ50MHZ         
    GPIO_Init(SHUTDOWN_PORT, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO    

    SHUTDOWN_OFF;
}

void BDCMOTOR_TIMx_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_BDTRInitTypeDef  sBreakDeadTimeConfig;
    TIM_OCInitTypeDef  TIM_OCInitStructure;    
    /* ������ʱ������ʱ��ʹ�� */
    RCC_APB2PeriphClockCmd(BDCMOTOR_TIM_RCC,ENABLE);//ʹ������TIM1ʱ��

    /* ��ʱ�������������� */
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = BDCMOTOR_TIM_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler = BDCMOTOR_TIM_PRESCALER;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = BDCMOTOR_TIM_REPETITIONCOUNTER;
    TIM_TimeBaseInit(BDCMOTOR_TIMx, &TIM_TimeBaseStructure);
    
    /* ��ʱ��ʱ��Դ���� */
    TIM_InternalClockConfig(BDCMOTOR_TIMx); // ʹ���ڲ�ʱ��Դ
    
    /* ɲ��������ʱ������ */
    sBreakDeadTimeConfig.TIM_OSSRState = TIM_BDTR_OSSR;
    sBreakDeadTimeConfig.TIM_OSSIState = 0;
    sBreakDeadTimeConfig.TIM_LOCKLevel = 0;
    sBreakDeadTimeConfig.TIM_DeadTime = 0x00;
    sBreakDeadTimeConfig.TIM_Break = 0;
    sBreakDeadTimeConfig.TIM_BreakPolarity = 0;
    sBreakDeadTimeConfig.TIM_AutomaticOutput = 0;    
    TIM_BDTRConfig(BDCMOTOR_TIMx, &sBreakDeadTimeConfig);
    
    /* ��ʱ���Ƚ�������� */
    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                  // �Ƚ����ģʽ��PWM1ģʽ
    TIM_OCInitStructure.TIM_Pulse =  0;                                // ռ�ձ�
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;           // �������
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;         // ����ͨ���������
    //TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;           // ����ģʽ
    TIM_OCInitStructure.TIM_OCIdleState = 0;                           // ���е�ƽ
    TIM_OCInitStructure.TIM_OCNIdleState = 0;                          // ����ͨ�����е�ƽ
    //TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    //TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OC1Init(BDCMOTOR_TIMx, &TIM_OCInitStructure); 

    /* TIM1 enable counter */
    TIM_Cmd(BDCMOTOR_TIMx, ENABLE);
    TIM_CCxCmd(BDCMOTOR_TIMx, TIM_Channel_1, TIM_CCx_Enable);
    TIM_CCxNCmd(BDCMOTOR_TIMx, TIM_Channel_1, TIM_CCxN_Disable);  // ֹͣ���
}

/**
  * ��������: ���õ���ٶ�
  * ���뺯��: Duty,�������ռ�ձ�
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void SetMotorSpeed(int16_t Duty)
{
    TIM_SetCompare1(BDCMOTOR_TIMx,Duty);
}

/**
  * ��������: ���õ��ת������
  * ���뺯��: Dir,���ת������
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void SetMotorDir(int16_t Dir)
{
    if(Dir)
    {
        TIM_CCxCmd(BDCMOTOR_TIMx, TIM_Channel_1, TIM_CCx_Enable);
        TIM_CCxNCmd(BDCMOTOR_TIMx, TIM_Channel_1, TIM_CCxN_Disable);  // ֹͣ���      
    }
    else
    {
        TIM_CCxCmd(BDCMOTOR_TIMx, TIM_Channel_1, TIM_CCx_Disable);   // ֹͣ���  
        TIM_CCxNCmd(BDCMOTOR_TIMx, TIM_Channel_1, TIM_CCxN_Enable);  
    }
}


void SHUTDOWN_Toggle(void)
{
    SHUTDOWN_PORT->ODR^=SHUTDOWN_PIN;
}


