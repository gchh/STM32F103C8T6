#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"


/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
// 注意：用作ADC采集的IO必须没有复用，否则采集电压会有影响
/********************ADC输入通道（引脚）配置**************************/
//PC0-ADC1_IN10 电流检测
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



#define VOLT_REF      3.307f                    // ADC参考电压.理论值是3.3V,通过实际测量得到3.307V
/* 根据驱动板设置放大倍数 和 采样电阻 */
#define GAIN          7.02f                     // 放大倍数
#define SAMPLING_RES  0.02f                     // 采样电阻R020

/** 电压分辨率 =  ADC(Hex) * 3.3 / 2^n * 1000(mV) 单位是mV
  * STM32的ADC分辨率是n = 12bit,4096
  */
#define VOLT_RESOLUTION     ((float)((VOLT_REF/(float)(4096))*(float)1000)) // ADC 电压分辨率,单位:mV
#define VOLTBUS_RESOLUTION  ((float)( 3.3f/(float)4096) * (80.4f+3.9f) / 3.9f)  //总线电压值分辨率从,分压电阻:80.4kΩ/3.9kΩ
  



void ADC_CUR_GPIO_Init(void);
void ADC_CUR_Init(void);













#endif
