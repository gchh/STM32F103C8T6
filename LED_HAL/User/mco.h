//main clock output
#ifndef __MCO_H
#define __MCO_H


#include "stm32f10x.h"


#define MCO_GPIO_RCC      RCC_APB2Periph_GPIOA
#define MCO_GPIO_PORT     GPIOA
#define MCO_GPIO_PIN      GPIO_Pin_8


void MCO_GPIO_config(void);
void MCO_SelectClockSource(uint8_t RCC_MCO);


#endif
