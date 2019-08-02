#include "usart.h"

uint8_t tx_ready, rx_cnt=0, tx_cnt=0, tx_cnt_size, rx_cnt_size;
__IO uint8_t RxBuf[FRAME_LENTH] ; // 接收缓存区
__IO uint8_t TxBuf[FRAME_LENTH] ; // 发送缓存区
MSG_TypeDef Msg;

extern void Analyse_Data_Callback(void);
void (*ptr_Fun_)(void) = Analyse_Data_Callback ;//函数指针  ptr_Fun_ = Analyse_Data_Callback;

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
    
    tx_ready=1;
    rx_cnt=0;
    tx_cnt=0;    
    rx_cnt_size=FRAME_LENTH;//接收数据帧长度
    tx_cnt_size=FRAME_LENTH;//发送数据帧长度
    
    /* 配置串口中断并使能，需要放在UART_Init函数后执行修改才有效 */    
    NVIC_SetPriority(USART_IRQn, 0);
    NVIC_EnableIRQ(USART_IRQn); 
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
   
    if(i==0)
    {
        rx_cnt=0;
        rx_cnt_size=1;
    }
    while(RxBuf[i] == 0)
    {
        i++;
        if(i==rx_cnt_size)
        {
            i = 0;
            rx_cnt=0;
            rx_cnt_size=1;
        }
    }
    ch = RxBuf[i];
    RxBuf[i] = 0; 
    
    return ch;
}


/**
  * 函数功能: 计算检验和
  * 输入参数: Ptr:待计算校验和的数据起始地址 , Num:待计算的字节数
  * 返 回 值: 计算得到的校验和
  * 说    明: 计算检验和
  */
uint8_t CheckSum(uint8_t *Ptr,uint8_t Num )
{
  uint8_t Sum = 0;
  while(Num--)
  {
    Sum += *Ptr;
    Ptr++;
  }
  return Sum;
}

void USART_IRQHANDLER(void)
{
    if(USART_GetFlagStatus(USART, USART_FLAG_ORE) != RESET)
    {
        USART_ClearFlag(USART, USART_FLAG_ORE);
        RxBuf[rx_cnt]=USART_ReceiveData(USART);
    }
    if(USART_GetITStatus(USART, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART, USART_IT_RXNE);
        //USART_ClearFlag(USART, USART_FLAG_RXNE);
        
        if(rx_cnt >= rx_cnt_size) rx_cnt=0;
        RxBuf[rx_cnt]=USART_ReceiveData(USART);
        
        if(rx_cnt==0)
        {
            if(RxBuf[0] != FRAME_START )  rx_cnt=0;   // 帧头正确
            else rx_cnt++;
        }
        else if(rx_cnt==rx_cnt_size-1)
        {
            if(RxBuf[rx_cnt] == FRAME_END ) // 帧尾正确
            { 
                /* 判断校验码 */
                if(CheckSum((uint8_t*)&RxBuf[FRAME_CHECK_BEGIN],FRAME_CHECK_NUM) != RxBuf[FRAME_CHECKSUM] )
                {
                    Msg.Code = NULL;
                    rx_cnt=0;
                }
                else
                {
                    /* 解析数据帧 */
                    if(ptr_Fun_ == NULL)
                        rx_cnt=0;
                    else 
                        ptr_Fun_();
                    rx_cnt=0;
                }
            }
        }
        else 
        {
            if(rx_cnt < rx_cnt_size) rx_cnt++;
            else rx_cnt=0;
        }
    }
    if(USART_GetITStatus(USART, USART_IT_TXE) != RESET)
    {
        USART_ClearITPendingBit(USART, USART_IT_TXE);
       
        if(tx_cnt<tx_cnt_size) 
        {
            USART_SendData(USART, TxBuf[tx_cnt]);
            tx_cnt++;
        }
        else 
        {
            tx_cnt=0;
            /* Disable the UART Transmit Complete Interrupt */
            USART_ITConfig(USART, USART_IT_TXE, DISABLE);
            /* Enable the UART Transmit Complete Interrupt */   
            USART_ITConfig(USART, USART_IT_TC, ENABLE);
        }
    }
    if(USART_GetITStatus(USART, USART_IT_TC) != RESET)
    {
        USART_ClearITPendingBit(USART, USART_IT_TC);
        /* Disable the UART Transmit Complete Interrupt */ 
        USART_ITConfig(USART, USART_IT_TC, ENABLE);
        tx_ready=1;
    }
}


/**
  * 函数功能: 发送反馈值
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 将反馈值发送到串口
  */
void Transmit_FB( __IO int32_t *Feedback)
{
    uint8_t i = 0;
    for(i=0;i<FRAME_LENTH;i++)
    {
        TxBuf[i] = FILL_VALUE;  // 参数填充 0x55
    }
  
    Msg.data[0].Int = *Feedback;//反馈值 速度
  
    TxBuf[0] = FRAME_START;   // 帧头
    TxBuf[1] = 0x80|CODE_SETTGT; // 指令码
    TxBuf[2] = Msg.data[0].Ch[0];
    TxBuf[3] = Msg.data[0].Ch[1];
    TxBuf[4] = Msg.data[0].Ch[2];
    TxBuf[5] = Msg.data[0].Ch[3];
  
    TxBuf[FRAME_CHECKSUM] = CheckSum((uint8_t*)&TxBuf[FRAME_CHECK_BEGIN],FRAME_CHECK_NUM);  // 计算校验和
    TxBuf[FRAME_LENTH-1] = FRAME_END;   // 加入帧尾

    if(tx_ready==1) // 发送数据帧
    {
        tx_ready=0;
        tx_cnt=0;
        tx_cnt_size=FRAME_LENTH;
        /* Enable the UART Transmit data register empty Interrupt */
        USART_ITConfig(USART, USART_IT_TXE, ENABLE);
    }
}
