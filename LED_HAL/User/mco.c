//main clock output
#include "mco.h"

void MCO_config(uint32_t RCC_MCOSource, uint32_t RCC_MCODiv)
{
    //PA8: MCO
    HAL_RCC_MCOConfig(RCC_MCO, RCC_MCOSource, RCC_MCODiv);
}

