//main clock output
#include "mco.h"

void MCO_GPIO_config(void)
{
    //PA8: MCO
    //定义一个GPIO_InitTypeDef 类型的结构体
    GPIO_InitTypeDef  GPIO_InitStructure;	
    RCC_APB2PeriphClockCmd(MCO_GPIO_RCC,ENABLE);//使能GPIO的外设时钟
    /*D1*/
    GPIO_InitStructure.GPIO_Pin =MCO_GPIO_PIN;//选择要用的GPIO引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //设置引脚模式为复用推免输出模式						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置引脚速度为50MHZ         
    GPIO_Init(MCO_GPIO_PORT, &GPIO_InitStructure);//调用库函数，初始化GPIO    
}


void MCO_SelectClockSource(uint8_t RCC_MCO)
{
    RCC_MCOConfig(RCC_MCO);
}

