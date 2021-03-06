#include "usart.h"

/**
  * 函数功能: 串口引脚配置.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void USART_GPIO_Init(void)
{
    //定义一个GPIO_InitTypeDef 类型的结构体
    GPIO_InitTypeDef  GPIO_InitStructure;	
    
    RCC_APB2PeriphClockCmd(USART_GPIO_RCC, ENABLE);//使能外设时钟
    
    /*PA9-USART1_TX*/
    GPIO_InitStructure.GPIO_Pin = USART_TX_PIN;//选择要用的GPIO引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //设置引脚模式为复用功能推免输出模式						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置引脚速度为50MHZ         
    GPIO_Init(USART_GPIO_PORT, &GPIO_InitStructure);//调用库函数，初始化GPIO    
    
    /*PA10-USART1_RX*/
    GPIO_InitStructure.GPIO_Pin = USART_RX_PIN;//选择要用的GPIO引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //设置引脚模式为浮空输入模式						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置引脚速度为50MHZ         
    GPIO_Init(USART_GPIO_PORT, &GPIO_InitStructure);//调用库函数，初始化GPIO            
}

/**
  * 函数功能: 串口参数配置.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void USART_Config(void)
{
    //115200-8-N-1
    USART_InitTypeDef USART_InitStructure;  //定义串口初始化结构体
    
    RCC_APB2PeriphClockCmd(USART_RCC, ENABLE);//使能外设时钟
    
    USART_InitStructure.USART_BaudRate = BaudRate; //波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位
    USART_InitStructure.USART_Parity = USART_Parity_No; //校验位 无
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //使能接收和发送引脚
    USART_Init(USART, &USART_InitStructure); //调用库函数，配置USART1
    
    USART_ClearFlag(USART,USART_FLAG_TC);
    USART_ITConfig(USART, USART_IT_RXNE, ENABLE);
    //USART_ITConfig(USART, USART_IT_TXE, ENABLE);	    
    USART_Cmd(USART, ENABLE);

    /* 配置串口中断并使能，需要放在UART_Init函数后执行修改才有效 */    
    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_EnableIRQ(USART1_IRQn); 
}

uint8_t buff[100],cnt;
void USART1_IRQHandler(void)
{
    //u8 c;
    uint8_t i;
    if(USART_GetFlagStatus(USART, USART_FLAG_ORE) != RESET)
    {
        //USART_ReceiveData(USART);
        USART_ClearFlag(USART, USART_FLAG_ORE);
    }
    if(USART_GetITStatus(USART, USART_IT_RXNE) != RESET)
    //if(USART_GetFlagStatus(USART, USART_FLAG_RXNE) != RESET)
    {
        //USART_ClearITPendingBit(USART, USART_IT_RXNE);
        
        buff[cnt]=USART_ReceiveData(USART);
        if(buff[cnt]==0x0A||cnt>=99)//接收完毕才打印，否则接收一个打印一个，会造成过载，丢失数据
        {
            for(i=0;i<=cnt;i++)
            {
                printf("%c",buff[i]);    //将接受到的数据直接返回打印
            }
            cnt=0;
        }
        else cnt++;
        //c=USART_ReceiveData(USART);
        USART_ClearFlag(USART, USART_FLAG_RXNE);
        //printf("%c",c);    //将接受到的数据直接返回打印
    }
}

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
    USART_SendData(USART, (uint8_t) ch);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART, USART_FLAG_TC) == RESET)
    {}
    return ch;
}
