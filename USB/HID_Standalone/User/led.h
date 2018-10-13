#ifndef __LED_H
#define __LED_H

#include "stm32f1xx_hal.h"



#define LED1_PIN                         GPIO_PIN_13
#define LED1_GPIO_PORT                   GPIOC
#define LED1_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOC_CLK_ENABLE()  
#define LED1_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOC_CLK_DISABLE()  


void LED_GPIO_Config(void);
void LED1Toggle(void);
void LED1ON(void);
void LED1OFF(void);
    
#endif
