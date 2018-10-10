//main clock output
#include "mco.h"

void MCO_GPIO_config(void)
{
    //PA8: MCO
    //����һ��GPIO_InitTypeDef ���͵Ľṹ��
    GPIO_InitTypeDef  GPIO_InitStructure;	
    RCC_APB2PeriphClockCmd(MCO_GPIO_RCC,ENABLE);//ʹ��GPIO������ʱ��
    /*D1*/
    GPIO_InitStructure.GPIO_Pin =MCO_GPIO_PIN;//ѡ��Ҫ�õ�GPIO����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //��������ģʽΪ�����������ģʽ						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���������ٶ�Ϊ50MHZ         
    GPIO_Init(MCO_GPIO_PORT, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO    
}


void MCO_SelectClockSource(uint8_t RCC_MCO)
{
    RCC_MCOConfig(RCC_MCO);
}

