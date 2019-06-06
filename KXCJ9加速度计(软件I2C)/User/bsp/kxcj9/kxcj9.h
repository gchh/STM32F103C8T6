#ifndef __KXCJ9_H__
#define __KXCJ9_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include <stm32f10x.h>

/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
#define KXCJ9_SLAVE_ADDRESS      0x1c  //kxcj9��������ַ 
#define KXCJ9_WHO_AM_I           0x0F
#define KXCJ9_ACC_OUT            0x06  //kxcj9���ٶ����ݼĴ�����ַ
#define KXCJ9_CTRL_REG1          0x1B  //���ƼĴ���1


#define KXCJ9_ID     0x0A  //


/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
void KXCJ9ReadAcc12Bit(short *accData);
void KXCJ9ReadAcc8Bit(int8_t *accData);
void KXCJ9_Init(void);
uint8_t KXCJ9ReadID(void);
void KXCJ9_ReadData(u8 reg_add,unsigned char*Read,u8 num);
void KXCJ9_WriteReg(u8 reg_add,u8 reg_dat);

#endif  //__KXCJ9_H__


