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



/* 数据格式化转换 */
typedef union {
  char Ch[4];
  float Float;
  int32_t Int;
}Format_UnionTypedef;

typedef struct {
  __IO uint8_t  Code ;  	
  __IO Format_UnionTypedef data[3];//数据帧有3个参数
}MSG_TypeDef;



// 上位机PID调试助手协议相关定义
#define FRAME_LENTH               16    // 指令长度
#define FRAME_START               0xAA  // 协议帧开始
#define FRAME_END                 '/'   // 协议帧结束
#define FRAME_CHECK_BEGIN          1    // 校验码开始的位置 RxBuf[1]
#define FRAME_CHECKSUM            14    // 校验码的位置   RxBuf[14]
#define FRAME_CHECK_NUM           13    // 需要校验的字节数
#define FILL_VALUE                0x55  // 填充值
#define CODE_SETPID               0x07  // 设置PID参数
#define CODE_SETTGT               0x08  // 设置目标值
#define CODE_RESET                0x09   // 复位重启
#define CODE_STARTMOTOR           0x0A   // 启动电机



extern __IO uint8_t RxBuf[FRAME_LENTH] ; // 接收缓存区
extern __IO uint8_t TxBuf[FRAME_LENTH] ; // 发送缓存区
extern MSG_TypeDef Msg;


void USART_GPIO_Init(void);
void USART_Config(void);
void Transmit_FB( __IO int32_t *Feedback);

#define USART_IT //使用中断

#endif
