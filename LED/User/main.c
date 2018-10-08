#include "stm32f10x.h"
#include "led.h"

//��������
void delay(__IO uint32_t delay_cnt);

/**
  * @file   main
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	/*Configure the SysTick to have interrupt in 1ms time basis*/
	SysTick_Config(72000);//ʹ��HSI=72MHz��Ϊϵͳʱ��
    
    /*��ʼ��LED�˿�*/
    LED_GPIO_Config();
    while(1)
    {	   
        LED1Toggle();
        delay(1000);
    }		
}


extern __IO uint32_t uwTick;
void delay(__IO uint32_t delay_cnt)//delay_cnt in 1ms
{
   uint32_t tickstart = uwTick;
   uint32_t wait = delay_cnt;

   /* Add a period to guarantee minimum wait */
   if (wait < 0xFFFFFFFF)
   {
      wait++;
   }
  
   while((uwTick-tickstart)<wait){}
}
