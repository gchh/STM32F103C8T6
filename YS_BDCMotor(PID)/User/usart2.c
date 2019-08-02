#include "usart.h"

uint8_t tx_ready, rx_cnt=0, tx_cnt=0, tx_cnt_size, rx_cnt_size;
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
    USART_ITConfig(USART, USART_IT_RXNE, ENABLE);
    //USART_ITConfig(USART, USART_IT_TXE, ENABLE);	    
    USART_Cmd(USART, ENABLE);
    
    tx_ready=1;
    rx_cnt=0;
    tx_cnt=0;    
    rx_cnt_size=FRAME_LENTH;//��������֡����
    tx_cnt_size=FRAME_LENTH;//��������֡����
    
    /* ���ô����жϲ�ʹ�ܣ���Ҫ����UART_Init������ִ���޸Ĳ���Ч */    
    NVIC_SetPriority(USART_IRQn, 0);
    NVIC_EnableIRQ(USART_IRQn); 
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
            if(RxBuf[0] != FRAME_START )  rx_cnt=0;   // ֡ͷ��ȷ
            else rx_cnt++;
        }
        else if(rx_cnt==rx_cnt_size-1)
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

    if(tx_ready==1) // ��������֡
    {
        tx_ready=0;
        tx_cnt=0;
        tx_cnt_size=FRAME_LENTH;
        /* Enable the UART Transmit data register empty Interrupt */
        USART_ITConfig(USART, USART_IT_TXE, ENABLE);
    }
}
