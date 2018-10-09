#include "stm32f10x.h"
#include "led.h"
#include "mco.h"

//函数声明
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
	//in the file system_stm3210x.c, configure the SystemCoreClock
        /* #define SYSCLK_FREQ_HSE    HSE_VALUE */
        /* #define SYSCLK_FREQ_24MHz  24000000 */ 
        /* #define SYSCLK_FREQ_36MHz  36000000 */
        /* #define SYSCLK_FREQ_48MHz  48000000 */
        /* #define SYSCLK_FREQ_56MHz  56000000 */
        /* #define SYSCLK_FREQ_72MHz  72000000 */    
    SysTick_Config(SystemCoreClock/1000);
    
    MCO_GPIO_config();
    //在系统时钟作为输出至MCO引脚时，请保证输出时钟频率不超过50MHz (I/O口最高频率)
    MCO_SelectClockSource(RCC_MCO_SYSCLK);
    
    /*初始化LED端口*/
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
