#include "usart.h"

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

    /* ���ô����жϲ�ʹ�ܣ���Ҫ����UART_Init������ִ���޸Ĳ���Ч */    
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
        if(buff[cnt]==0x0A||cnt>=99)//������ϲŴ�ӡ���������һ����ӡһ��������ɹ��أ���ʧ����
        {
            for(i=0;i<=cnt;i++)
            {
                printf("%c",buff[i]);    //�����ܵ�������ֱ�ӷ��ش�ӡ
            }
            cnt=0;
        }
        else cnt++;
        //c=USART_ReceiveData(USART);
        USART_ClearFlag(USART, USART_FLAG_RXNE);
        //printf("%c",c);    //�����ܵ�������ֱ�ӷ��ش�ӡ
    }
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
