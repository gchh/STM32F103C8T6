#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include <stdio.h>

//115200-8-N-1
#define USARTx             	 USART1
#define USARTx_BaudRate	     115200
#define USART_RCC         	 RCC_APB2Periph_USART1

#define USARTx_GPIO_RCC       RCC_APB2Periph_GPIOA
#define USARTx_GPIO_PORT      GPIOA  
#define USARTx_TX_PIN         GPIO_Pin_9  // out
#define USARTx_RX_PIN         GPIO_Pin_10 // in  

#define USARTx_IRQHANDLER     USART1_IRQHandler
#define USARTx_IPQn           USART1_IRQn  


void USARTx_GPIO_Init(void);
void USARTx_Config(void);


#endif
