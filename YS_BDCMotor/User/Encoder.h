#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f10x.h"

//PA6: TIM3_CH1  PA7: TIM3_CH2
#define ENCODER_TIMx                        TIM3
#define ENCODER_TIM_RCC                     RCC_APB1Periph_TIM3

#define ENCODER_TIM_CH1_GPIO_RCC            RCC_APB2Periph_GPIOA
#define ENCODER_TIM_CH1_PORT                GPIOA
#define ENCODER_TIM_CH1_PIN      			GPIO_Pin_6

#define ENCODER_TIM_CH2_GPIO_RCC            RCC_APB2Periph_GPIOA
#define ENCODER_TIM_CH2_PORT                GPIOA
#define ENCODER_TIM_CH2_PIN      			GPIO_Pin_7

#define TIM_ENCODERMODE_TIx                 TIM_EncoderMode_TI12

#define ENCODER_TIM_IRQn                    TIM3_IRQn
#define ENCODER_TIM_IRQHANDLER              TIM3_IRQHandler

// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��8MHz/��ENCODER_TIMx_PRESCALER+1��
#define ENCODER_TIM_PRESCALER               0  

// ���嶨ʱ�����ڣ�����ʱ����ʼ������ENCODER_TIMx_PERIODֵʱ���¶�ʱ�������ɶ�Ӧ�¼����ж�
#define ENCODER_TIM_PERIOD                  0xFFFF


#endif
