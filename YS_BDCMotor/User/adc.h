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


//PC1-ADC1_IN11 电压检测
#define ADC_VOLT_GPIO_RCC                 RCC_APB2Periph_GPIOC
#define ADC_VOLT_GPIO                     GPIOC
#define ADC_VOLT_GPIO_PIN                 GPIO_Pin_1
#define ADC_VOLT_CHANNEL                  ADC_Channel_11

#define ADC_OVP_IRQx                      ADC1_2_IRQn
#define ADC_OVP_IRQHandler                ADC1_2_IRQHandler



#define ADC_Base      8//10//9                         // 取2的整数倍作为缓存区大小,得到14bits的ADC值
/* 使用DMA传输数据,采集n个数据点的时间是0.65ms,采样率大约是 1500 KHz */
#define ADC_BUFFER    1024//512                       // 采样数据缓存区


#define VOLT_REF      3.315f//3.25f//3.307f                    // ADC参考电压.理论值是3.3V,通过实际测量得到3.307V
/* 根据驱动板设置放大倍数 和 采样电阻 */
#define GAIN          7.02f                     // 放大倍数
#define SAMPLING_RES  0.02f                     // 采样电阻R020

/** 电压分辨率 =  ADC(Hex) * 3.3 / 2^n * 1000(mV) 单位是mV
  * STM32的ADC分辨率是n = 12bit,4096
  */
#define VOLT_RESOLUTION     ((float)((VOLT_REF/(float)(16384))*(float)1000)) //((float)((VOLT_REF/(float)(4096))*(float)1000)) // ADC 电压分辨率,单位:mV
#define VOLTBUS_RESOLUTION  ((float)( 3.3f/(float)4096) * (80.4f+3.9f) / 3.9f)  //总线电压值分辨率从,分压电阻:80.4kΩ/3.9kΩ
  

/* 总线电压参数相关 */
#define VOLT_MAX                         60.0f // 最大电压值
#define VOLT_MIN                         12.0f

/* 总线分压电阻:3.9 kΩ,80.4 kΩ */
#define VOLT_LIMIT_MAX                   (int32_t)((((VOLT_MAX * 3.9f) / (3.9f+80.4f) ) /3.3f) *4096.0f) // 3445
#define VOLT_LIMIT_MIN                   (int32_t)((((VOLT_MIN * 3.9f) / (3.9f+80.4f) ) /3.3f) *4096.0f) // 689


#define CURRENT_MAX   400.0f // 最大电流值 400 mA

void ADC_CUR_GPIO_Init(void);
void ADC_VOLT_GPIO_Init(void);
void ADC_CUR_Init(void);
void ADC_VOLT_Init(void);
void ADC_DMA_Init(void);
void ADC_Calibration(void);
void ADC_Start_DMA(void);

extern __IO float ADC_CurrentValue;
extern __IO int32_t ADC_Resul;
extern __IO uint32_t OffsetCnt_Flag;
extern __IO  int32_t OffSetHex ;             // 偏差值
extern __IO float ADC_VoltBus;
extern __IO int32_t AverSum;                   // 平均值的累加值
extern __IO int32_t AverCnt;
#endif
