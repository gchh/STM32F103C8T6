#include "stm32f10x.h"
#include "led.h"
#include "BDCMotor.h"

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
	//in the file system_stm3210x.c, configure the SystemCoreClock
        /* #define SYSCLK_FREQ_HSE    HSE_VALUE */
        /* #define SYSCLK_FREQ_24MHz  24000000 */ 
        /* #define SYSCLK_FREQ_36MHz  36000000 */
        /* #define SYSCLK_FREQ_48MHz  48000000 */
        /* #define SYSCLK_FREQ_56MHz  56000000 */
        /* #define SYSCLK_FREQ_72MHz  72000000 */    
    SysTick_Config(SystemCoreClock/1000);
    
    /*��ʼ��LED�˿�*/
    LED_GPIO_Config();
    
    /* �߼����ƶ�ʱ����ʼ��������PWM������� */
    BDCMOTOR_GPIO_Init();
    BDCMOTOR_TIMx_Init();
    SetMotorSpeed(200);
    
    while(1)
    {	   
        LED1Toggle();
        delay(500);
        //SHUTDOWN_Toggle();
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
