//main clock output
#include "mco.h"

void MCO_config(uint32_t MCOxSource)
{
    //PA8: MCO
    /* Enable the MCO Clock */
    MCO_GPIO_CLK_ENABLE();

    /* Configure IO in alternate function output push-pull mode */
    LL_GPIO_SetPinMode(MCO_GPIO_PORT, MCO_GPIO_PIN, LL_GPIO_MODE_ALTERNATE | LL_GPIO_SPEED_FREQ_HIGH);
    /* Reset value is LL_GPIO_OUTPUT_PUSHPULL */
    LL_GPIO_SetPinOutputType(MCO_GPIO_PORT, MCO_GPIO_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    /* Reset value is LL_GPIO_SPEED_FREQ_HIGH */
    LL_GPIO_SetPinSpeed(MCO_GPIO_PORT, MCO_GPIO_PIN, LL_GPIO_SPEED_FREQ_HIGH);

    
    LL_RCC_ConfigMCO(MCOxSource);
}

