#include "Encoder.h"

void ENCODER_GPIO_Init(void)
{
    //����һ��GPIO_InitTypeDef ���͵Ľṹ��
    GPIO_InitTypeDef  GPIO_InitStructure;	
    
    RCC_APB2PeriphClockCmd(ENCODER_TIM_CH1_GPIO_RCC | ENCODER_TIM_CH2_GPIO_RCC, ENABLE);//ʹ������ʱ��
    
    /*PA6: TIM3_CH1*/
    GPIO_InitStructure.GPIO_Pin = ENCODER_TIM_CH1_PIN;//ѡ��Ҫ�õ�GPIO����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //��������ģʽΪ���ù����������ģʽ						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���������ٶ�Ϊ50MHZ         
    GPIO_Init(ENCODER_TIM_CH1_PORT, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO    
    
    /*PA7: TIM3_CH2*/
    GPIO_InitStructure.GPIO_Pin = ENCODER_TIM_CH2_PIN;//ѡ��Ҫ�õ�GPIO����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //��������ģʽΪ��������ģʽ						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���������ٶ�Ϊ50MHZ         
    GPIO_Init(ENCODER_TIM_CH2_PORT, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO      
}

void ENCODER_TIMx_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef sEncoderConfig;
    RCC_APB1PeriphClockCmd(ENCODER_TIM_RCC, ENABLE);//ʹ������ʱ��
    
    /* ��ʱ�������������� */
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler = ENCODER_TIM_PRESCALER;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(ENCODER_TIMx, &TIM_TimeBaseStructure);    
    
}




