#ifndef __KXCJ9_H__
#define __KXCJ9_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include <stm32f10x.h>

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define KXCJ9_SLAVE_ADDRESS      0x1c  //kxcj9器件读地址 
#define KXCJ9_WHO_AM_I           0x0F
#define KXCJ9_ACC_OUT            0x06  //kxcj9加速度数据寄存器地址
#define KXCJ9_CTRL_REG1          0x1B  //控制寄存器1


#define KXCJ9_ID     0x0A  //


/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
void KXCJ9ReadAcc12Bit(short *accData);
void KXCJ9ReadAcc8Bit(int8_t *accData);
void KXCJ9_Init(void);
uint8_t KXCJ9ReadID(void);
void KXCJ9_ReadData(u8 reg_add,unsigned char*Read,u8 num);
void KXCJ9_WriteReg(u8 reg_add,u8 reg_dat);

#endif  //__KXCJ9_H__


