//main clock output
#ifndef __MCO_H
#define __MCO_H


#include "stm32f1xx_hal.h"


#define MCO_GPIO_RCC      RCC_APB2Periph_GPIOA
#define MCO_GPIO_PORT     GPIOA
#define MCO_GPIO_PIN      GPIO_Pin_8


void MCO_config(uint32_t RCC_MCOSource, uint32_t RCC_MCODiv);


#endif
