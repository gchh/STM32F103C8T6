//main clock output
#ifndef __MCO_H
#define __MCO_H

#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"


#define MCO_GPIO_PORT     GPIOA
#define MCO_GPIO_PIN      LL_GPIO_PIN_8
#define MCO_GPIO_CLK_ENABLE()             LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA)

void MCO_config(uint32_t MCOxSource);


#endif
