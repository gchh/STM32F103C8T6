/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp/kxcj9/kxcj9.h"
#include "bsp/usart/bsp_debug_usart.h"
#include "bsp/I2C/bsp_i2c.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 写数据到KXCJ9寄存器
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
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
  * 函数功能: 从KXCJ9寄存器读取数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
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
  * 函数功能: 初始化KXCJ9芯片
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
void KXCJ9_Init(void)
{
  int i=0,j=0;
  //在初始化之前要延时一段时间，若没有延时，则断电后再上电数据可能会出错
  for(i=0;i<1000;i++)
  {
    for(j=0;j<1000;j++)
    {
      ;
    }
  }
	KXCJ9_WriteReg(KXCJ9_CTRL_REG1, 0x00);	    //设置为禁用模式
	//KXCJ9_WriteReg(KXCJ9_CTRL_REG1, 0x80);	    //设置为8bit分辨率，+/-2g，并启动
    KXCJ9_WriteReg(KXCJ9_CTRL_REG1, 0xc0);	    //设置为12bit分辨率，+/-2g，并启动
}

/**
  * 函数功能: 读取KXCJ9的ID
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
uint8_t KXCJ9ReadID(void)
{
	unsigned char Re = 0;
    KXCJ9_ReadData(KXCJ9_WHO_AM_I,&Re,1);    //读器件地址
	if(Re != KXCJ9_ID)
	{
		printf("KXCJ9 dectected error!\r\n检测不到KXCJ9模块，请检查模块与开发板的接线");
		return 0;
	}
	else
	{
		printf("KXCJ9 ID = %d\r\n",Re);
		return 1;
	}
		
}

/**
  * 函数功能: 读取KXCJ9的加速度数据，分辨率12bit
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
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
  * 函数功能: 读取KXCJ9的加速度数据，分辨率8bit
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */ 
void KXCJ9ReadAcc8Bit(int8_t *accData)
{
    u8 buf[6];
    KXCJ9_ReadData(KXCJ9_ACC_OUT, buf, 6);
    accData[0] = buf[1];
    accData[1] = buf[3];
    accData[2] = buf[5];
}
