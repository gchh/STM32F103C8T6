/******************************************************************************
1.5寸彩色OLED
软件：8080(8bit)
接口：	OLED			STM32
			GND-------GND
			VCC-------3V3
			D0--------PE8
			D1--------PE9
			D2--------PE10
			D3--------PE11
			D4--------PE12
			D5--------PE13
			D6--------PE14
			D7--------PE15
			RES#------PC4
			D/C#------PC5
			CS#-------PB0
			E/RD#-----PE7
			R/W#------PB1
			
******************************************************************************/ 
/* 定义防止递归包含 ----------------------------------------------------------*/
#ifndef __OLED__H
#define __OLED__H

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f10x.h"


/* 宏定义 --------------------------------------------------------------------*/

#define		Black			0x0000		//黑色（不亮）
#define		Navy			0x000F		//深蓝色
#define		Dgreen		0x03E0		//深绿色
#define		Dcyan			0x03EF		//深青色
#define		Maroon		0x7800		//深红色
#define		Purple		0x780F		//紫色
#define		Olive			0x7BE0		//橄榄绿
#define		Lgray			0xC618		//灰白色
#define		Dgray			0x7BEF		//深灰色
#define		Blue			0x001F		//蓝色
#define		Green			0x07E0		//绿色
#define		Cyan			0x07FF		//青色
#define		Red				0xF800		//红色
#define		Magenta		0xF81F		//品红
#define		Yellow		0xFFE0		//黄色
#define		White			0xFFFF		//白色


#define Max_Column	127
#define Max_Row		127


#define OLED_RES_GPIO_PORT      (GPIOC)		//RES#引脚
#define OLED_RES_GPIO_PINS      (GPIO_Pin_4)

#define OLED_DC_GPIO_PORT       (GPIOC)		//D/C#引脚
#define OLED_DC_GPIO_PINS       (GPIO_Pin_5)

#define OLED_CS_GPIO_PORT      	(GPIOB)		//CS#引脚
#define OLED_CS_GPIO_PINS      	(GPIO_Pin_0)

#define OLED_RD_GPIO_PORT      	(GPIOE)		//E/RD#引脚
#define OLED_RD_GPIO_PINS      	(GPIO_Pin_7)

#define OLED_WR_GPIO_PORT      	(GPIOB)		//R/W#引脚
#define OLED_WR_GPIO_PINS      	(GPIO_Pin_1)

#define OLED_RES_Set()          GPIO_SetBits(OLED_RES_GPIO_PORT, OLED_RES_GPIO_PINS)
#define OLED_RES_Clr()          GPIO_ResetBits(OLED_RES_GPIO_PORT, OLED_RES_GPIO_PINS)

#define OLED_DC_Set()           GPIO_SetBits(OLED_DC_GPIO_PORT, OLED_DC_GPIO_PINS)
#define OLED_DC_Clr()           GPIO_ResetBits(OLED_DC_GPIO_PORT, OLED_DC_GPIO_PINS)

#define	SPI_NSS_HIGH()					GPIO_SetBits(OLED_CS_GPIO_PORT, OLED_CS_GPIO_PINS)
#define	SPI_NSS_LOW()						GPIO_ResetBits(OLED_CS_GPIO_PORT, OLED_CS_GPIO_PINS)

#define OLED_RD_Set()            GPIO_SetBits(OLED_RD_GPIO_PORT, OLED_RD_GPIO_PINS)
#define OLED_RD_Clr()            GPIO_ResetBits(OLED_RD_GPIO_PORT, OLED_RD_GPIO_PINS)

#define OLED_WR_Set()           GPIO_SetBits(OLED_WR_GPIO_PORT, OLED_WR_GPIO_PINS)
#define OLED_WR_Clr()           GPIO_ResetBits(OLED_WR_GPIO_PORT, OLED_WR_GPIO_PINS)




/* 函数声明-------------------------------------------------------------------*/
void Fill_RAM(int Colour_RGB);
void Clear_Screen(unsigned char Var_Row);
void Display_Char(unsigned char x, unsigned char y, unsigned char Var_Char, int Colour_RGB);
void Display_String(unsigned char x, unsigned char y, unsigned char *chr, int Colour_RGB);
void Display_Chinese(unsigned char x, unsigned char y, unsigned char Var_Char, int Colour_RGB);
void Display_Num(unsigned char x,unsigned char y,int Var_Num, unsigned char Num_Len, int Colour_RGB);
void Display_Pattern(unsigned char *Pattern_Data, unsigned char Start_x, unsigned char End_x, unsigned char Start_y, unsigned char End_y);
void OLED_Init(void);


#endif

/************************ Copyright (C)2017 LGG. All Rights Reserved *****END OF FILE****/

