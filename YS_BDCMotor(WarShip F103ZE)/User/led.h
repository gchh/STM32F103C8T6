#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#if 0
/*������stm32f103c8t6��Сϵͳ�� */
#define LED1_GPIO_RCC               RCC_APB2Periph_GPIOC
#define LED1_GPIO_PORT              GPIOC
#define LED1_GPIO_PIN      			GPIO_Pin_13
#define LED1_ONOFF(x)     			GPIO_WriteBit(LED1_GPIO_PORT,LED1_GPIO_PIN,x)
#define LED1_ON         			LED1_ONOFF(Bit_RESET)//�͵�ƽ����
#define LED1_OFF     			    LED1_ONOFF(Bit_SET)//�ߵ�ƽ����
#else
/*����������ԭ��ս�������� */
#define LED1_GPIO_RCC               RCC_APB2Periph_GPIOB//RCC_APB2Periph_GPIOC
#define LED1_GPIO_PORT              GPIOB//GPIOC
#define LED1_GPIO_PIN      			GPIO_Pin_5//GPIO_Pin_13
#define LED1_ONOFF(x)     			GPIO_WriteBit(LED1_GPIO_PORT,LED1_GPIO_PIN,x)
#define LED1_ON         			LED1_ONOFF(Bit_RESET)//�͵�ƽ����
#define LED1_OFF     			    LED1_ONOFF(Bit_SET)//�ߵ�ƽ����
#endif

void LED_GPIO_Config(void);
void LED1Toggle(void);
#endif
