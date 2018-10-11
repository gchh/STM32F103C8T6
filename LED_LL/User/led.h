#ifndef __LED_H
#define __LED_H

#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"

#define LED1_PIN                           LL_GPIO_PIN_13
#define LED1_GPIO_PORT                     GPIOC
#define LED1_GPIO_CLK_ENABLE()             LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC)


void LED_GPIO_Config(void);
void LED1Toggle(void);
    
#endif
