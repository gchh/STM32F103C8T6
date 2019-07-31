#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

/*适用于stm32f103c8t6最小系统板 */
#define LED1_GPIO_RCC               RCC_APB2Periph_GPIOC
#define LED1_GPIO_PORT              GPIOC
#define LED1_GPIO_PIN      			GPIO_Pin_13
#define LED1_ONOFF(x)     			GPIO_WriteBit(LED1_GPIO_PORT,LED1_GPIO_PIN,x)
#define LED1_ON         			LED1_ONOFF(Bit_RESET)//低电平灯亮
#define LED1_OFF     			    LED1_ONOFF(Bit_SET)//高电平灯灭


void LED_GPIO_Config(void);
void LED1Toggle(void);
#endif
