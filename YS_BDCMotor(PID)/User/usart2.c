#include "usart.h"

__IO uint8_t RxBuf[FRAME_LENTH] ; // ���ջ�����
__IO uint8_t TxBuf[FRAME_LENTH] ; // ���ͻ�����
MSG_TypeDef Msg;

extern void Analyse_Data_Callback(void);
void (*ptr_Fun_)(void) = Analyse_Data_Callback ;//����ָ��  ptr_Fun_ = Analyse_Data_Callback;

/**
  * ��������: ������������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void USART_GPIO_Init(void)
{
    //����һ��GPIO_InitTypeDef ���͵Ľṹ��
    GPIO_InitTypeDef  GPIO_InitStructure;	
    
    RCC_APB2PeriphClockCmd(USART_GPIO_RCC, ENABLE);//ʹ������ʱ��
    
    /*PA9-USART1_TX*/
    GPIO_InitStructure.GPIO_Pin = USART_TX_PIN;//ѡ��Ҫ�õ�GPIO����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //��������ģʽΪ���ù����������ģʽ						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���������ٶ�Ϊ50MHZ         
    GPIO_Init(USART_GPIO_PORT, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO    
    
    /*PA10-USART1_RX*/
    GPIO_InitStructure.GPIO_Pin = USART_RX_PIN;//ѡ��Ҫ�õ�GPIO����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������ģʽΪ��������ģʽ						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���������ٶ�Ϊ50MHZ         
    GPIO_Init(USART_GPIO_PORT, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO            
}

/**
  * ��������: ���ڲ�������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void USART_Config(void)
{
    //115200-8-N-1
    USART_InitTypeDef USART_InitStructure;  //���崮�ڳ�ʼ���ṹ��
    
    RCC_APB2PeriphClockCmd(USART_RCC, ENABLE);//ʹ������ʱ��
    
    USART_InitStructure.USART_BaudRate = BaudRate; //������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //����λ8λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //ֹͣλ1λ
    USART_InitStructure.USART_Parity = USART_Parity_No; //У��λ ��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //ʹ�ܽ��պͷ�������
    USART_Init(USART, &USART_InitStructure); //���ÿ⺯��������USART1
    
    USART_ClearFlag(USART,USART_FLAG_TC);
#ifdef USART_IT
    USART_ITConfig(USART, USART_IT_RXNE, ENABLE);
#endif
    //USART_ITConfig(USART, USART_IT_TXE, ENABLE);	    
    USART_Cmd(USART, ENABLE);

#ifdef USART_IT
    /* ���ô����жϲ�ʹ�ܣ���Ҫ����UART_Init������ִ���޸Ĳ���Ч */    
    NVIC_SetPriority(USART_IRQn, 0);
    NVIC_EnableIRQ(USART_IRQn); 
#endif
}


/**
  * ��������: �ض���c�⺯��printf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
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
  * ��������: �ض���c�⺯��getchar,scanf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    �����ض���Buffer,ʹ���жϽ������ݵ�Buffer,Ȼ���ȡ.��ֹ
  * ���������ݽ��յ��¶�ʧ����.
  */
int fgetc(FILE * f)
{
    uint8_t ch = 0;
#ifdef USART_IT
    static uint8_t i = 0;
    while(RxBuf[i] == 0) //|| Buffer[i] == '\r' || Buffer[i] == '\n')
    {
        i++;
        if(i==FRAME_LENTH)
            i = 0;
    }
    ch = RxBuf[i];
    RxBuf[i] = 0;
#else
    if(USART_ReceiveData(USART) == '\r')ch = USART_ReceiveData(USART);//USART_ClearFlag(USART, USART_FLAG_RXNE);//��Ҫ������Yʱ��������볬��2����Y�ַ�������ʱ��������Y������ղ������Y���յ�����'\r'���������Ϳ��ԡ�
    if(USART_GetFlagStatus(USART, USART_FLAG_ORE) != RESET)
    {
        USART_ClearFlag(USART, USART_FLAG_ORE);
    }
    while(USART_GetFlagStatus(USART, USART_FLAG_RXNE) == RESET);
    ch = USART_ReceiveData(USART);
#endif  
    return ch;
}


/**
  * ��������: ��������
  * �������: Ptr:������У��͵�������ʼ��ַ , Num:��������ֽ���
  * �� �� ֵ: ����õ���У���
  * ˵    ��: ��������
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

#ifdef USART_IT
uint8_t rx_cnt=0;
void USART_IRQHANDLER(void)
{
    if(USART_GetFlagStatus(USART, USART_FLAG_ORE) != RESET)
    {
        USART_ClearFlag(USART, USART_FLAG_ORE);
    }
    if(USART_GetITStatus(USART, USART_IT_RXNE) != RESET)
    //if(USART_GetFlagStatus(USART, USART_FLAG_RXNE) != RESET)
    {
        //USART_ClearITPendingBit(USART, USART_IT_RXNE);
        USART_ClearFlag(USART, USART_FLAG_RXNE);
        
        if(rx_cnt >= FRAME_LENTH) rx_cnt=0;
        RxBuf[rx_cnt]=USART_ReceiveData(USART);
        
        if(rx_cnt==0)
        {
            if(RxBuf[0] != FRAME_START )  rx_cnt=0;   // ֡ͷ��ȷ
            else rx_cnt++;
        }
        else if(rx_cnt==FRAME_LENTH-1)
        {
            if(RxBuf[rx_cnt] == FRAME_END ) // ֡β��ȷ
            { 
                /* �ж�У���� */
                if(CheckSum((uint8_t*)&RxBuf[FRAME_CHECK_BEGIN],FRAME_CHECK_NUM) != RxBuf[FRAME_CHECKSUM] )
                {
                    Msg.Code = NULL;
                    rx_cnt=0;
                }
                else
                {
                    /* ��������֡ */
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
            if(rx_cnt < FRAME_LENTH) rx_cnt++;
            else rx_cnt=0;
        }
        //HAL_UART_Receive_IT(huart,(uint8_t *)&RxBuf,FRAME_LENTH); // ����ʹ�ܽ����ж�        
    }
    
}
#endif


/**
  * ��������: ���ͷ���ֵ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ������ֵ���͵�����
  */
void Transmit_FB( __IO int32_t *Feedback)
{
  uint8_t i = 0;
  for(i=0;i<FRAME_LENTH;i++)
  {
    TxBuf[i] = FILL_VALUE;  // ������� 0x55
  }
  
  Msg.data[0].Int = *Feedback;//����ֵ �ٶ�
  
  TxBuf[0] = FRAME_START;   // ֡ͷ
  TxBuf[1] = 0x80|CODE_SETTGT; // ָ����
  TxBuf[2] = Msg.data[0].Ch[0];
  TxBuf[3] = Msg.data[0].Ch[1];
  TxBuf[4] = Msg.data[0].Ch[2];
  TxBuf[5] = Msg.data[0].Ch[3];
  
  TxBuf[FRAME_CHECKSUM] = CheckSum((uint8_t*)&TxBuf[FRAME_CHECK_BEGIN],FRAME_CHECK_NUM);  // ����У���
  TxBuf[FRAME_LENTH-1] = FRAME_END;   // ����֡β

  USART_SendData(USART, uint16_t Data);
  USART_ITConfig(USART, USART_IT_TXE, ENABLE);
  HAL_UART_Transmit_IT(&husartx,(uint8_t *)&TxBuf,FRAME_LENTH); // ��������֡
}