#include "led.h"

 /**
  * @file   LED_GPIO_Config
  * @brief  LED灯引脚配置
  * @param  无
  * @retval 无
  */
void LED_GPIO_Config(void)
{	
    //定义一个GPIO_InitTypeDef 类型的结构体
    GPIO_InitTypeDef  GPIO_InitStruct;
    
    /* -1- Enable GPIO Clock (to be able to program the configuration registers) */
    LED1_GPIO_CLK_ENABLE();

    /* -2- Configure IO in output push-pull mode to drive external LEDs */
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStruct.Pin = LED1_PIN;
    HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);
}

 /**
  * @file   LED1Toggle
  * @brief  LED1亮灭翻转
  * @param  
  * @retval 无
  */
void LED1Toggle(void)
{
    HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
}

