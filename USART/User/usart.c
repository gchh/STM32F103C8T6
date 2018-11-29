#include "usart.h"

#define BUFFERSIZE   8
__IO uint8_t Buffer[BUFFERSIZE];

UART_HandleTypeDef husartx;

/**
  * 函数功能: 串口硬件初始化配置
  * 输入参数: huart：串口句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(huart == &husartx)
  {
    /* 使能串口功能引脚GPIO时钟 */
    USARTx_GPIO_ClK_ENABLE();
    /* 串口外设功能GPIO配置 */
    GPIO_InitStruct.Pin = USARTx_Tx_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(USARTx_Tx_GPIO, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = USARTx_Rx_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(USARTx_Rx_GPIO, &GPIO_InitStruct);
  }
}

/**
  * 函数功能: 串口硬件反初始化配置
  * 输入参数: huart：串口句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart == &husartx)
  {
    /* 串口外设时钟禁用 */
    USART_RCC_CLK_DISABLE();
  
    /* 串口外设功能GPIO配置 */
    HAL_GPIO_DeInit(USARTx_Tx_GPIO, USARTx_Tx_GPIO_PIN);
    HAL_GPIO_DeInit(USARTx_Rx_GPIO, USARTx_Rx_GPIO_PIN);
    
    /* 串口中断禁用 */
    HAL_NVIC_DisableIRQ(USARTx_IRQn);
  }
}

/**
  * 函数功能: NVIC配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
static void MX_NVIC_USARTx_Init(void)
{
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USARTx_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USARTx_IRQn);
}

/**
  * 函数功能: 串口参数配置.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void MX_USARTx_Init(void)
{
  /* 串口外设时钟使能 */
  USART_RCC_CLK_ENABLE();
  
  husartx.Instance = USARTx;
  husartx.Init.BaudRate = USARTx_BAUDRATE;
  husartx.Init.WordLength = UART_WORDLENGTH_8B;
  husartx.Init.StopBits = UART_STOPBITS_1;
  husartx.Init.Parity = UART_PARITY_NONE;
  husartx.Init.Mode = UART_MODE_TX_RX;
  husartx.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  husartx.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&husartx);
  
  /* 配置串口中断并使能，需要放在HAL_UART_Init函数后执行修改才有效 */
  MX_NVIC_USARTx_Init();
  HAL_UART_Receive_IT(&husartx,(uint8_t*)Buffer,BUFFERSIZE);
}

/**
  * 函数功能: 串口接收中断回调函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：重新设置接收区,并使能中断
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Process Unlocked */
  __HAL_UNLOCK(&husartx);// 防止在发送数据的时候使能中断失败
  HAL_UART_Receive_IT(&husartx,(uint8_t*)Buffer,BUFFERSIZE);
}


/**
  * 函数功能: 中断处理函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void USARTx_IRQHANDLER()
{
  HAL_UART_IRQHandler(&husartx);
}


/**
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&husartx, (uint8_t *)&ch, 1, 0x00FF);
  return ch;
}

/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：重定向到Buffer,使用中断接收数据到Buffer,然后读取.防止
  * 来不及数据接收导致丢失数据.
  */
int fgetc(FILE * f)
{
  static uint8_t i = 0;
  uint8_t ch = 0;
  while(Buffer[i] == 0)//||Buffer[i] == '\r'||Buffer[i] == '\n')
  {
    //if(Buffer[i] == '\r'||Buffer[i] == '\n') Buffer[i] = 0;
    i++;
    if(i==BUFFERSIZE)
        i = 0;
  }
  ch = Buffer[i];
  Buffer[i] = 0;
  return ch;
}

