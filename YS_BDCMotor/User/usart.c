#include "usart.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  
/**
  * 函数功能: 串口引脚配置.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void USARTx_GPIO_Init(void)
{
    //定义一个GPIO_InitTypeDef 类型的结构体
    GPIO_InitTypeDef  GPIO_InitStructure;	
    
    RCC_APB2PeriphClockCmd(USARTx_GPIO_RCC, ENABLE);//使能外设时钟
    
    /*PA9-USART1_TX*/
    GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;//选择要用的GPIO引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //设置引脚模式为复用功能推免输出模式						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置引脚速度为50MHZ         
    GPIO_Init(USARTx_GPIO_PORT, &GPIO_InitStructure);//调用库函数，初始化GPIO    
    
    /*PA10-USART1_RX*/
    GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN;//选择要用的GPIO引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //设置引脚模式为浮空输入模式						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置引脚速度为50MHZ         
    GPIO_Init(USARTx_GPIO_PORT, &GPIO_InitStructure);//调用库函数，初始化GPIO            
}

/**
  * 函数功能: 串口参数配置.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void USARTx_Config(void)
{
    //115200-8-N-1
    USART_InitTypeDef USARTx_InitStructure;  //定义串口初始化结构体
    
    USARTx_InitStructure.USART_BaudRate = USARTx_BaudRate; //波特率
    USARTx_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位
    USARTx_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位
    USARTx_InitStructure.USART_Parity = USART_Parity_No; //校验位 无
    USARTx_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无流控制
    USARTx_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //使能接收和发送引脚
    USART_Init(USARTx, &USARTx_InitStructure); //调用库函数，配置USART1
    
    USART_ClearFlag(USARTx,USART_FLAG_TC);
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
    //USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);	    
    USART_Cmd(USARTx, ENABLE);

    /* 配置串口中断并使能，需要放在HAL_UART_Init函数后执行修改才有效 */    
    NVIC_SetPriority(USARTx_IPQn, 0);
}

void USARTx_IRQHANDLER(void)
{
    uint8_t c;
    if(USART_GetITStatus(USARTx, USART_IT_RXNE) != RESET)
    { 	
        c=USART_ReceiveData(USARTx);
        printf("%c",c);    //将接受到的数据直接返回打印
    }     
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(USARTx, (uint8_t) ch);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
    {}
    return ch;
}
#if 0
/**
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(USARTx, (uint8_t) ch);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
    {}
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
  while(Buffer[i] == 0)
  {
    i++;
    if(i==BUFFERSIZE)
      i = 0;
  }
  ch = Buffer[i];
  Buffer[i] = 0;
  return ch;
}
#endif
