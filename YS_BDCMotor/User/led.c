#include "led.h"

 /**
  * @file   LED_GPIO_Config
  * @brief  LED����������
  * @param  ��
  * @retval ��
  */
void LED_GPIO_Config(void)
{	
    //����һ��GPIO_InitTypeDef ���͵Ľṹ��
    GPIO_InitTypeDef  GPIO_InitStructure;	
    RCC_APB2PeriphClockCmd(LED1_GPIO_RCC,ENABLE);//ʹ��GPIO������ʱ��
    /*D1*/
    GPIO_InitStructure.GPIO_Pin =LED1_GPIO_PIN;//ѡ��Ҫ�õ�GPIO����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //��������ģʽΪ�������ģʽ						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���������ٶ�Ϊ50MHZ         
    GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO
}

 /**
  * @file   LED1Toggle
  * @brief  LED1����ת
  * @param  
  * @retval ��
  */
void LED1Toggle(void)
{
    LED1_GPIO_PORT->ODR^=LED1_GPIO_PIN;
}
