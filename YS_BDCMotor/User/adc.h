#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"


/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
// ע�⣺����ADC�ɼ���IO����û�и��ã�����ɼ���ѹ����Ӱ��
/********************ADC����ͨ�������ţ�����**************************/
//PC0-ADC1_IN10 �������
#define ADC_CUR_GPIO_RCC                 RCC_APB2Periph_GPIOC
#define ADC_CUR_GPIO                     GPIOC
#define ADC_CUR_GPIO_PIN                 GPIO_Pin_0

#define ADCx_RCC                         RCC_APB2Periph_ADC1
#define ADCx                             ADC1
#define ADC_CURRENT_CHANNEL              ADC_Channel_10

#define DMAx_RCC                         RCC_AHBPeriph_DMA1
#define DMAx_CHANNEL_x                   DMA1_Channel1
#define ADCx_DMA_IRQx                    DMA1_Channel1_IRQn
#define ADCx_DMA_IRQx_Handler            DMA1_Channel1_IRQHandler


//PC1-ADC1_IN11 ��ѹ���
#define ADC_VOLT_GPIO_RCC                 RCC_APB2Periph_GPIOC
#define ADC_VOLT_GPIO                     GPIOC
#define ADC_VOLT_GPIO_PIN                 GPIO_Pin_1
#define ADC_VOLT_CHANNEL                  ADC_Channel_11

#define ADC_OVP_IRQx                      ADC1_2_IRQn
#define ADC_OVP_IRQHandler                ADC1_2_IRQHandler



#define ADC_Base      10//9                         // ȡ2����������Ϊ��������С,�õ�14bits��ADCֵ
/* ʹ��DMA��������,�ɼ�n�����ݵ��ʱ����0.65ms,�����ʴ�Լ�� 1500 KHz */
#define ADC_BUFFER    1024//512                       // �������ݻ�����


#define VOLT_REF      3.307f                    // ADC�ο���ѹ.����ֵ��3.3V,ͨ��ʵ�ʲ����õ�3.307V
/* �������������÷Ŵ��� �� �������� */
#define GAIN          7.02f                     // �Ŵ���
#define SAMPLING_RES  0.02f                     // ��������R020

/** ��ѹ�ֱ��� =  ADC(Hex) * 3.3 / 2^n * 1000(mV) ��λ��mV
  * STM32��ADC�ֱ�����n = 12bit,4096
  */
#define VOLT_RESOLUTION     ((float)((VOLT_REF/(float)(4096))*(float)1000)) // ADC ��ѹ�ֱ���,��λ:mV
#define VOLTBUS_RESOLUTION  ((float)( 3.3f/(float)4096) * (80.4f+3.9f) / 3.9f)  //���ߵ�ѹֵ�ֱ��ʴ�,��ѹ����:80.4k��/3.9k��
  

/* ���ߵ�ѹ������� */
#define VOLT_MAX                         60.0f // ����ѹֵ
#define VOLT_MIN                         12.0f

/* ���߷�ѹ����:3.9 k��,80.4 k�� */
#define VOLT_LIMIT_MAX                   (int32_t)((((VOLT_MAX * 3.9f) / (3.9f+80.4f) ) /3.3f) *4096.0f) // 3445
#define VOLT_LIMIT_MIN                   (int32_t)((((VOLT_MIN * 3.9f) / (3.9f+80.4f) ) /3.3f) *4096.0f)


void ADC_CUR_GPIO_Init(void);
void ADC_CUR_Init(void);
void ADC_DMA_Init(void);
void ADC_Start_DMA(void);

extern __IO float ADC_CurrentValue;
extern __IO int32_t ADC_Resul;
extern __IO uint32_t OffsetCnt_Flag;
extern __IO  int32_t OffSetHex ;             // ƫ��ֵ


#endif
