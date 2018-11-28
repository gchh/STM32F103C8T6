#include "usart.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  
/**
  * ��������: ������������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void USARTx_GPIO_Init(void)
{
    //����һ��GPIO_InitTypeDef ���͵Ľṹ��
    GPIO_InitTypeDef  GPIO_InitStructure;	
    
    RCC_APB2PeriphClockCmd(USARTx_GPIO_RCC, ENABLE);//ʹ������ʱ��
    
    /*PA9-USART1_TX*/
    GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;//ѡ��Ҫ�õ�GPIO����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //��������ģʽΪ���ù����������ģʽ						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���������ٶ�Ϊ50MHZ         
    GPIO_Init(USARTx_GPIO_PORT, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO    
    
    /*PA10-USART1_RX*/
    GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN;//ѡ��Ҫ�õ�GPIO����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������ģʽΪ��������ģʽ						 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���������ٶ�Ϊ50MHZ         
    GPIO_Init(USARTx_GPIO_PORT, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO            
}

/**
  * ��������: ���ڲ�������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void USARTx_Config(void)
{
    //115200-8-N-1
    USART_InitTypeDef USARTx_InitStructure;  //���崮�ڳ�ʼ���ṹ��
    
    USARTx_InitStructure.USART_BaudRate = USARTx_BaudRate; //������
    USARTx_InitStructure.USART_WordLength = USART_WordLength_8b; //����λ8λ
    USARTx_InitStructure.USART_StopBits = USART_StopBits_1; //ֹͣλ1λ
    USARTx_InitStructure.USART_Parity = USART_Parity_No; //У��λ ��
    USARTx_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��������
    USARTx_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //ʹ�ܽ��պͷ�������
    USART_Init(USARTx, &USARTx_InitStructure); //���ÿ⺯��������USART1
    
    USART_ClearFlag(USARTx,USART_FLAG_TC);
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
    //USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);	    
    USART_Cmd(USARTx, ENABLE);

    /* ���ô����жϲ�ʹ�ܣ���Ҫ����HAL_UART_Init������ִ���޸Ĳ���Ч */    
    NVIC_SetPriority(USARTx_IPQn, 0);
}

void USARTx_IRQHANDLER(void)
{
    uint8_t c;
    if(USART_GetITStatus(USARTx, USART_IT_RXNE) != RESET)
    { 	
        c=USART_ReceiveData(USARTx);
        printf("%c",c);    //�����ܵ�������ֱ�ӷ��ش�ӡ
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
  * ��������: �ض���c�⺯��printf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
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
