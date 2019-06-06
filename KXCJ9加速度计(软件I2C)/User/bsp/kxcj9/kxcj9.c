/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "bsp/kxcj9/kxcj9.h"
#include "bsp/usart/bsp_debug_usart.h"
#include "bsp/I2C/bsp_i2c.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

/**
  * ��������: д���ݵ�KXCJ9�Ĵ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */ 
void KXCJ9_WriteReg(u8 reg_add,u8 reg_dat)
{
	i2c_Start();
	i2c_SendByte(KXCJ9_SLAVE_ADDRESS);
	i2c_WaitAck();
	i2c_SendByte(reg_add);
	i2c_WaitAck();
	i2c_SendByte(reg_dat);
	i2c_WaitAck();
	i2c_Stop();
}

/**
  * ��������: ��KXCJ9�Ĵ�����ȡ����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */ 
void KXCJ9_ReadData(u8 reg_add,unsigned char*Read,u8 num)
{
	unsigned char i;
	
	i2c_Start();
	i2c_SendByte(KXCJ9_SLAVE_ADDRESS);
	i2c_WaitAck();
	i2c_SendByte(reg_add);
	i2c_WaitAck();
	
	i2c_Start();
	i2c_SendByte(KXCJ9_SLAVE_ADDRESS+1);
	i2c_WaitAck();
	
	for(i=0;i<(num-1);i++){
		*Read=i2c_ReadByte(1);
		Read++;
	}
	*Read=i2c_ReadByte(0);
	i2c_Stop();
}

/**
  * ��������: ��ʼ��KXCJ9оƬ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */ 
void KXCJ9_Init(void)
{
  int i=0,j=0;
  //�ڳ�ʼ��֮ǰҪ��ʱһ��ʱ�䣬��û����ʱ����ϵ�����ϵ����ݿ��ܻ����
  for(i=0;i<1000;i++)
  {
    for(j=0;j<1000;j++)
    {
      ;
    }
  }
	KXCJ9_WriteReg(KXCJ9_CTRL_REG1, 0x00);	    //����Ϊ����ģʽ
	//KXCJ9_WriteReg(KXCJ9_CTRL_REG1, 0x80);	    //����Ϊ8bit�ֱ��ʣ�+/-2g��������
    KXCJ9_WriteReg(KXCJ9_CTRL_REG1, 0xc0);	    //����Ϊ12bit�ֱ��ʣ�+/-2g��������
}

/**
  * ��������: ��ȡKXCJ9��ID
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */ 
uint8_t KXCJ9ReadID(void)
{
	unsigned char Re = 0;
    KXCJ9_ReadData(KXCJ9_WHO_AM_I,&Re,1);    //��������ַ
	if(Re != KXCJ9_ID)
	{
		printf("KXCJ9 dectected error!\r\n��ⲻ��KXCJ9ģ�飬����ģ���뿪����Ľ���");
		return 0;
	}
	else
	{
		printf("KXCJ9 ID = %d\r\n",Re);
		return 1;
	}
		
}

/**
  * ��������: ��ȡKXCJ9�ļ��ٶ����ݣ��ֱ���12bit
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */ 
void KXCJ9ReadAcc12Bit(short *accData)
{
    u8 buf[6];
    KXCJ9_ReadData(KXCJ9_ACC_OUT, buf, 6);
    accData[0] = (buf[1] << 8) | buf[0];
    accData[1] = (buf[3] << 8) | buf[2];
    accData[2] = (buf[5] << 8) | buf[4];
    accData[0] >>= 4;
    accData[1] >>= 4;
    accData[2] >>= 4;
}

/**
  * ��������: ��ȡKXCJ9�ļ��ٶ����ݣ��ֱ���8bit
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */ 
void KXCJ9ReadAcc8Bit(int8_t *accData)
{
    u8 buf[6];
    KXCJ9_ReadData(KXCJ9_ACC_OUT, buf, 6);
    accData[0] = buf[1];
    accData[1] = buf[3];
    accData[2] = buf[5];
}
