#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include <stdio.h>

//115200-8-N-1
#define USART             	 USART1
#define BaudRate	         115200
#define USART_RCC         	 RCC_APB2Periph_USART1

#define USART_GPIO_RCC       RCC_APB2Periph_GPIOA
#define USART_GPIO_PORT      GPIOA  
#define USART_TX_PIN         GPIO_Pin_9  // out
#define USART_RX_PIN         GPIO_Pin_10 // in  

#define USART_IRQHANDLER                      USART1_IRQHandler
#define USART_IRQn                            USART1_IRQn



/* ���ݸ�ʽ��ת�� */
typedef union {
  char Ch[4];
  float Float;
  int32_t Int;
}Format_UnionTypedef;

typedef struct {
  __IO uint8_t  Code ;  	
  __IO Format_UnionTypedef data[3];//����֡��3������
}MSG_TypeDef;



// ��λ��PID��������Э����ض���
#define FRAME_LENTH               16    // ָ���
#define FRAME_START               0xAA  // Э��֡��ʼ
#define FRAME_END                 '/'   // Э��֡����
#define FRAME_CHECK_BEGIN          1    // У���뿪ʼ��λ�� RxBuf[1]
#define FRAME_CHECKSUM            14    // У�����λ��   RxBuf[14]
#define FRAME_CHECK_NUM           13    // ��ҪУ����ֽ���
#define FILL_VALUE                0x55  // ���ֵ
#define CODE_SETPID               0x07  // ����PID����
#define CODE_SETTGT               0x08  // ����Ŀ��ֵ
#define CODE_RESET                0x09   // ��λ����
#define CODE_STARTMOTOR           0x0A   // �������



extern __IO uint8_t RxBuf[FRAME_LENTH] ; // ���ջ�����
extern __IO uint8_t TxBuf[FRAME_LENTH] ; // ���ͻ�����
extern MSG_TypeDef Msg;


void USART_GPIO_Init(void);
void USART_Config(void);


#define USART_IT //ʹ���ж�

#endif
