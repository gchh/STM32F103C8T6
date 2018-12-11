#include "usart.h"

#define BUFFERSIZE   8
__IO uint8_t Buffer[BUFFERSIZE];

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

#ifdef USART_IT
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

        Buffer[0]=USART_ReceiveData(USART);
        
        USART_ClearFlag(USART, USART_FLAG_RXNE);
    }
}
#endif

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
    while(Buffer[i] == 0) //|| Buffer[i] == '\r' || Buffer[i] == '\n')
    {
        i++;
        if(i==BUFFERSIZE)
            i = 0;
    }
    ch = Buffer[i];
    Buffer[i] = 0;
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

