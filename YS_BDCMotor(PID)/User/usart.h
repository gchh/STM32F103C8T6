#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include <stdio.h>

//115200-8-N-1
#define USART             	 USART1
#define BaudRate	         115200
#define USART_RCC         	 RCC_APB2Periph_USART1

#define USART_GPIO_RCC       RCC_APB2Periph_GPIOA
#define USART_GPIO_PORT      GPIOA  
#define USART_TX_PIN         GPIO_Pin_9  // out
#define USART_RX_PIN         GPIO_Pin_10 // in  

#define USART_IRQHANDLER                      USART1_IRQHandler
#define USART_IRQn                            USART1_IRQn

void USART_GPIO_Init(void);
void USART_Config(void);


#define USART_IT // π”√÷–∂œ

#endif
