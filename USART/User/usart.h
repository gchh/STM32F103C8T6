#ifndef __USART_H
#define __USART_H

#include "stm32f1xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define USARTx                       USART1
#define USARTx_BAUDRATE              115200
#define USART_RCC_CLK_ENABLE()       __HAL_RCC_USART1_CLK_ENABLE()
#define USART_RCC_CLK_DISABLE()      __HAL_RCC_USART1_CLK_DISABLE()

#define USARTx_GPIO_ClK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_Tx_GPIO_PIN           GPIO_PIN_9
#define USARTx_Tx_GPIO               GPIOA
#define USARTx_Rx_GPIO_PIN           GPIO_PIN_10   
#define USARTx_Rx_GPIO               GPIOA

#define USARTx_IRQHANDLER            USART1_IRQHandler
#define USARTx_IRQn                  USART1_IRQn

/* 函数声明 ------------------------------------------------------------------*/
void MX_USARTx_Init(void);

#endif
