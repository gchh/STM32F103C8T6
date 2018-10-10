#include "stm32f10x.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

//��������
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
    /*��ʼ��LED�˿�*/
    LED_GPIO_Config();
    
    xTaskCreate( led_Serve, 
                 (char const*)"led", 
                 LED_STACK_SIZE, 
                 NULL,
                 1, 
                 &LedTaskHandle);  
                 
	//����������
	vTaskStartScheduler();  
                 
    while(1){}
}


void led_Serve(void* pvParam)
{
    while(1)
    {
        LED1Toggle();
        vTaskDelay(500);
    }
}
