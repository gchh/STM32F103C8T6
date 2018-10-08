#include "stm32f10x.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

//函数声明
void led_Serve(void* pvParam);

#define LED_STACK_SIZE  10
TaskHandle_t LedTaskHandle = NULL;

/**
  * @file   main
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    /*初始化LED端口*/
    LED_GPIO_Config();
    
    xTaskCreate( led_Serve, 
                 (char const*)"led", 
                 LED_STACK_SIZE, 
                 NULL,
                 1, 
                 &LedTaskHandle);  
                 
	//启动调度器
	vTaskStartScheduler();  
                 
    while(1){}
}


void led_Serve(void* pvParam)
{
    while(1)
    {
        LED1Toggle();
        vTaskDelay(1000);
    }
}
