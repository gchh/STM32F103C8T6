/* 包含头文件 ----------------------------------------------------------------*/
#include "oled.h"
#include "font.h"


/**
  * @函数名       Delay
  * @功  能       延迟
  * @参  数       data：延迟data
  * @返回值       无
  */
static void Delay(uint32_t data)
{
	while(data--);
}



/**
  * @函数名       OLED_GPIO_Init
  * @功  能       OLED的信号引脚初始化
  * @参  数       无
  * @返回值       无
  */
static void OLED_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	//定义结构体
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE, ENABLE);	//使能GPIOA时钟
	GPIO_InitStructure.GPIO_Pin = OLED_CS_GPIO_PINS | OLED_WR_GPIO_PINS;		//初始化用于控制OLED的相关引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);	//初始化
    GPIO_InitStructure.GPIO_Pin = OLED_RES_GPIO_PINS | OLED_DC_GPIO_PINS;		//初始化用于控制OLED的相关引脚
    GPIO_Init(GPIOC, &GPIO_InitStructure);	//初始化
    GPIO_InitStructure.GPIO_Pin = OLED_RD_GPIO_PINS;		//初始化用于控制OLED的相关引脚
    GPIO_Init(GPIOE, &GPIO_InitStructure);	//初始化    

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 
                                | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;		//初始化用于控制OLED的信号引脚
	GPIO_Init(GPIOE, &GPIO_InitStructure);	//初始化
	
	GPIO_SetBits(GPIOB, OLED_CS_GPIO_PINS | OLED_WR_GPIO_PINS);	//将RES、DC、CS、RD、WR引脚拉高
    GPIO_SetBits(GPIOC, OLED_RES_GPIO_PINS | OLED_DC_GPIO_PINS);	//将RES、DC、CS、RD、WR引脚拉高
    GPIO_SetBits(GPIOE, OLED_RD_GPIO_PINS);	//将RES、DC、CS、RD、WR引脚拉高
}


static void Parallel_Write(unsigned char Data)
{
	unsigned int temp = 0;
	
	GPIOE->ODR = (GPIOE->ODR & 0x00ff) | ((temp | Data) << 8);
}


/**
  * @函数名       Write_Command
  * @功  能       往OLED发送命令
  * @参  数       Cmd：发送的命令
  * @返回值       无
  */
void Write_Command(unsigned char Cmd)
{
  OLED_DC_Clr();  //DC拉低，发送命令
	SPI_NSS_LOW(); //片选拉低，选通器件
	OLED_WR_Clr();	//WR拉低，使能往OLED写入
  Parallel_Write(Cmd); //发送数据
	OLED_WR_Set();	//WR拉高，禁能往OLED写入
  SPI_NSS_HIGH(); //片选拉高，关闭器件
  OLED_DC_Set(); //DC拉高，空闲时为高电平
}


/**
  * @函数名       Write_Data
  * @功  能       往OLED发送数据
  * @参  数       Data：发送的数据
  * @返回值       无
  */
void Write_Data(unsigned char Data)
{
	OLED_DC_Set();  //DC拉高，发送数据
	SPI_NSS_LOW(); //片选拉低，选通器件
	OLED_WR_Clr();	//WR拉低，使能往OLED写入
  Parallel_Write(Data); //发送数据
	OLED_WR_Set();	//WR拉高，禁能往OLED写入
  SPI_NSS_HIGH(); //片选拉高，关闭器件
  OLED_DC_Set(); //DC拉高，空闲时为高电平
}


/**
  * @函数名       Set_Column_Address
  * @功  能       设置列地址
  * @参  数       Clo_Start：列起始地址；Clo_End：列结束地址
  * @返回值       无
  */
void Set_Column_Address(unsigned char Clo_Start, unsigned char Clo_End)
{
  Write_Command(0x15);  // 设置列地址
  Write_Data(Clo_Start);  // 列起始地址
  Write_Data(Clo_End);  // 列结束地址
}


/**
  * @函数名       Set_Row_Address
  * @功  能       设置行地址
  * @参  数       Row_Start：行起始地址；Row_End：行结束地址
  * @返回值       无
  */
void Set_Row_Address(unsigned char Row_Start, unsigned char Row_End)
{
  Write_Command(0x75);  // 设置行地址
  Write_Data(Row_Start);  // 行起始地址
  Write_Data(Row_End);  // 行结束地址
}


/**
  * @函数名       Set_Write_RAM
  * @功  能       使能MCU写RAM
  * @参  数       无
  * @返回值       无
  */
void Set_Write_RAM()
{
  Write_Command(0x5C);  //使能MCU写RAM
}


/**
  * @函数名       Set_Read_RAM
  * @功  能       使能MCU读RAM
  * @参  数       无
  * @返回值       无
  */
void Set_Read_RAM()
{
  Write_Command(0x5D);  //使能MCU读RAM
}


/**
  * @函数名       Fill_RAM
  * @功  能       填充全屏（刷屏）
  * @参  数       Colour_RGB：填充颜色：RRRRRGGGGGGBBBBB
  * @返回值       无
  */
void Fill_RAM(int Colour_RGB)
{
  unsigned char i,j;  //定义变量

  Set_Column_Address(0x00,0x7F);  //设置列地址0~127
  Set_Row_Address(0x00,0x7F);   //设置行地址0~127
  Set_Write_RAM();  //使能MCU写RAM

  for(i=0;i<128;i++)  //填充128列*128行
  {
    for(j=0;j<128;j++)
    {
      Write_Data(Colour_RGB >> 8);  //写入填充颜色高字节
      Write_Data(Colour_RGB); //写入填充颜色低字节
    }
  }
}


/**
  * @函数名       Clear_Screen
  * @功  能       指定某一行清屏（一共8行：0~7）
  * @参  数       Var_Row：第Var_Row行
  * @返回值       无
  */
void Clear_Screen(unsigned char Var_Row)
{
	unsigned int i;

	Set_Column_Address(0x00, 0x7F);	//设置列地址0~127
	Set_Row_Address(Var_Row*16, (Var_Row*16 + 15));		//设置行地址
	Set_Write_RAM();	//使能MCU写RAM

	for(i = 0; i < 2048; i++)	//把这一行清屏，点数量 = 16*128 = 2048
	{
		Write_Data(0x00);
		Write_Data(0x00);
	}
}


/**
  * @函数名       Draw_Point
  * @功  能       画点
  * @参  数       x：列起始地址；y：行起始地址；Colour_RGB：填充颜色：RRRRRGGGGGGBBBBB
  * @返回值       无
  */
void Draw_Point(unsigned char x, unsigned char y, int Colour_RGB)
{
  Write_Command(0x15);  // 设置列地址
  Write_Data(x);    

  Write_Command(0x75);  // 设置行地址
  Write_Data(y);    

  Set_Write_RAM();  //使能MCU写RAM

  Write_Data(Colour_RGB >> 8); //写入填充颜色高字节
  Write_Data(Colour_RGB);  //写入填充颜色低字节
}


/**
  * @函数名       display_char
  * @功  能       指定坐标画一个字符（逐行式），字符大小固定为8*16
  * @参  数       x：列起始地址；y：行起始地址；Var_Char：写入的字符；Colour_RGB：填充颜色：RRRRRGGGGGGBBBBB
  * @返回值       无
  */
void Display_Char(unsigned char x, unsigned char y, unsigned char Var_Char, int Colour_RGB)
{
  unsigned char i, j; //定义变量
  unsigned char Var_Temp, x_Temp; //定义变量

  x_Temp = x; //获取x起始坐标
  for (i = 0; i < 16; i++)
  {
    Var_Temp = ACSII_Font[Var_Char - 0x20][i];  //获取字符在数组的偏移量
    for ( j = 0; j < 8; j++)
    {
      if (Var_Temp & 0x80)  //先画最高位的点，为1则画对应的颜色
        Draw_Point(x, y, Colour_RGB);
      //else
        //Draw_Point(x, y, 0x0000); //为0则黑色（都不亮），可作为背景色

      Var_Temp <<= 1; //移位，准备画下一个点
      x++;  //x坐标加1

      if((x - x_Temp) == 8)
      { 
        x = x_Temp; //x坐标回到起始位置
        y++;  //y坐标加1
        break;  //退出当前循环
      }
    }
  }
}


/**
  * @函数名       Display_String
  * @功  能       显示字符串
  * @参  数       x：列起始地址；y：行起始地址；*chr：显示的字符串；Colour_RGB：填充颜色：RRRRRGGGGGGBBBBB
  * @返回值       无
  */
void Display_String(unsigned char x, unsigned char y, unsigned char *chr, int Colour_RGB)
{
  unsigned char i = 0;  //定义变量

  while(chr[i] != '\0') //判断是否结束
  {
    Display_Char(x, y, chr[i], Colour_RGB); //显示字符
    x += 8; //x坐标加8（一个字符x方向占8个点，Y方向占16个点）
    i++;  //下一个字符
  }
}


/**
  * @函数名       Display_Chinese
  * @功  能       指定坐标画一个字符（逐行式），字符大小固定为16*16
  * @参  数       x：列起始地址；y：行起始地址；Var_Char：汉字在数组的下标；Colour_RGB：填充颜色：RRRRRGGGGGGBBBBB
  * @返回值       无
  */
void Display_Chinese(unsigned char x, unsigned char y, unsigned char Var_Char, int Colour_RGB)
{
  unsigned char i, j; //定义变量
  unsigned char Var_Temp, x_Temp; //定义变量

  x_Temp = x; //获取x起始坐标
  for(i = 0; i < 32; i++) //16*16 = 32*8；一次只能写入一字节，也就是8位，所以采用8*32的画法
  {
    Var_Temp = Chinese_Font[Var_Char][i]; //获取汉字在数组的偏移量
    for(j = 0; j < 8; j++)
    {
      if(Var_Temp & 0x80)   //先画最高位的点，为1则画对应的颜色
        Draw_Point(x, y, Colour_RGB);
      //else
        //Draw_Point(x, y, 0x0000); //为0则黑色（都不亮），可作为背景色
      Var_Temp <<= 1; //移位，准备画下一个点
      x++;  //x坐标加1

      if((x - x_Temp) == 16)  //画完一列
      {
        x = x_Temp; //x坐标回到起始位置
        y++;  //y坐标加1
        break;  //退出当前循环
      }
    }
  }
}


/**
  * @函数名       Num_Pow
  * @功  能       m^n幂运算
  * @参  数       m：底数；n：指数
  * @返回值       无
  */
uint32_t Num_Pow(unsigned char m,unsigned char n)
{
  uint32_t result=1; //定义变量

  while(n--)result*=m;  //m^n
  return result;  //返回结果
}


/**
  * @函数名       Display_Num
  * @功  能       显示数字
  * @参  数       x：列起始地址；y：行起始地址；Var_Num：显示的数字；Num_Len：数字长度；Colour_RGB：填充颜色：RRRRRGGGGGGBBBBB
  * @返回值       无
  */
void Display_Num(unsigned char x,unsigned char y,int Var_Num, unsigned char Num_Len, int Colour_RGB)
{
  unsigned char i;  //定义变量
  unsigned char Var_Temp;   //定义变量
  unsigned char Var_Show = 0; //定义变量

  for(i = 0; i < Num_Len; i++)  //显示数字
  {
    Var_Temp  =  (Var_Num / Num_Pow(10, Num_Len-i-1)) % 10;  //获取最高位（数字显示是一位一位取出来再显示）
    if(Var_Show == 0 && i<(Num_Len - 1))  //判断首位数字是否为0
    {
      if(Var_Temp == 0) 
      {
        Display_Char(x+8*i, y, ' ', Colour_RGB);  //首位数据为0则不显示
        continue; //继续循环
      }
      else
        Var_Show = 1; //后面的数字正常显示
    }

    Display_Char(x+8*i, y, Var_Temp + '0', Colour_RGB); //显示数字
  }
}


/**
  * @函数名       Display_Pattern
  * @功  能       显示图片
  * @参  数       *Pattern_Data：图片数组；Start_x：x方向起点；End_x：x方向终点；Start_y：y方向起点；End_y：y方向终点
  * @返回值       无
  */
void Display_Pattern(unsigned char *Pattern_Data, unsigned char Start_x, unsigned char End_x, unsigned char Start_y, unsigned char End_y)
{
	unsigned char *Data_Pointer;	//定义指针变量
	unsigned char i, j;		//定义变量

	Data_Pointer = Pattern_Data;	//获取数组首地址
	Set_Column_Address(Start_x, End_x);	//设置列范围
	Set_Row_Address(Start_y, End_y);	//设置行范围
	Set_Write_RAM();	//设置MCU写RAM

	for(i = 0; i < (End_y - Start_y + 1); i++)	//x方向(End_y - Start_y + 1)
	{
		for(j = 0; j < (End_x - Start_x + 1); j++)	//y方向(End_y - Start_y + 1)
		{
			Write_Data(*Data_Pointer);	//写入数据（一个点需要写入16位数据：RRRRRGGGGGGBBBBB）
			Data_Pointer++;
			Write_Data(*Data_Pointer);
			Data_Pointer++;
		}
	}
}


/**
  * @函数名       Set_Gray_Scale_Table
  * @功  能       灰阶表设置
  * @参  数       无
  * @返回值       无
  */
void Set_Gray_Scale_Table(void)
{
  Write_Command(0xB8);
  Write_Data(0x02);     // Gray Scale Level 1
  Write_Data(0x03);     // Gray Scale Level 2
  Write_Data(0x04);     // Gray Scale Level 3
  Write_Data(0x05);     // Gray Scale Level 4
  Write_Data(0x06);     // Gray Scale Level 5
  Write_Data(0x07);     // Gray Scale Level 6
  Write_Data(0x08);     // Gray Scale Level 7
  Write_Data(0x09);     // Gray Scale Level 8
  Write_Data(0x0A);     // Gray Scale Level 9
  Write_Data(0x0B);     // Gray Scale Level 10
  Write_Data(0x0C);     // Gray Scale Level 11
  Write_Data(0x0D);     // Gray Scale Level 12
  Write_Data(0x0E);     // Gray Scale Level 13
  Write_Data(0x0F);     // Gray Scale Level 14
  Write_Data(0x10);     // Gray Scale Level 15
  Write_Data(0x11);     // Gray Scale Level 16
  Write_Data(0x12);     // Gray Scale Level 17
  Write_Data(0x13);     // Gray Scale Level 18
  Write_Data(0x15);     // Gray Scale Level 19
  Write_Data(0x17);     // Gray Scale Level 20
  Write_Data(0x19);     // Gray Scale Level 21
  Write_Data(0x1B);     // Gray Scale Level 22
  Write_Data(0x1D);     // Gray Scale Level 23
  Write_Data(0x1F);     // Gray Scale Level 24
  Write_Data(0x21);     // Gray Scale Level 25
  Write_Data(0x23);     // Gray Scale Level 26
  Write_Data(0x25);     // Gray Scale Level 27
  Write_Data(0x27);     // Gray Scale Level 28
  Write_Data(0x2A);     // Gray Scale Level 29
  Write_Data(0x2D);     // Gray Scale Level 30
  Write_Data(0x30);     // Gray Scale Level 31
  Write_Data(0x33);     // Gray Scale Level 32
  Write_Data(0x36);     // Gray Scale Level 33
  Write_Data(0x39);     // Gray Scale Level 34
  Write_Data(0x3C);     // Gray Scale Level 35
  Write_Data(0x3F);     // Gray Scale Level 36
  Write_Data(0x42);     // Gray Scale Level 37
  Write_Data(0x45);     // Gray Scale Level 38
  Write_Data(0x48);     // Gray Scale Level 39
  Write_Data(0x4C);     // Gray Scale Level 40
  Write_Data(0x50);     // Gray Scale Level 41
  Write_Data(0x54);     // Gray Scale Level 42
  Write_Data(0x58);     // Gray Scale Level 43
  Write_Data(0x5C);     // Gray Scale Level 44
  Write_Data(0x60);     // Gray Scale Level 45
  Write_Data(0x64);     // Gray Scale Level 46
  Write_Data(0x68);     // Gray Scale Level 47
  Write_Data(0x6C);     // Gray Scale Level 48
  Write_Data(0x70);     // Gray Scale Level 49
  Write_Data(0x74);     // Gray Scale Level 50
  Write_Data(0x78);     // Gray Scale Level 51
  Write_Data(0x7D);     // Gray Scale Level 52
  Write_Data(0x82);     // Gray Scale Level 53
  Write_Data(0x87);     // Gray Scale Level 54
  Write_Data(0x8C);     // Gray Scale Level 55
  Write_Data(0x91);     // Gray Scale Level 56
  Write_Data(0x96);     // Gray Scale Level 57
  Write_Data(0x9B);     // Gray Scale Level 58
  Write_Data(0xA0);     // Gray Scale Level 59
  Write_Data(0xA5);     // Gray Scale Level 60
  Write_Data(0xAA);     // Gray Scale Level 61
  Write_Data(0xAF);     // Gray Scale Level 62
  Write_Data(0xB4);     // Gray Scale Level 63
}



/**
  * @函数名       OLED_Init
  * @功  能       OLED初始化
  * @参  数       无
  * @返回值       无
  */
void OLED_Init(void)
{
  OLED_GPIO_Init();     //初始化RES、DC与CS引脚

  OLED_RES_Set();
  Delay(100);
  OLED_RES_Clr();
  Delay(200); //延迟，由于单片机上电初始化比OLED快，所以必须加上延迟，等待OLED上电初始化完成
  OLED_RES_Set();
  Delay(200);
    
  Write_Command(0xFD);  // 解锁OLED驱动IC（SSD1351）的命令输入
  Write_Data(0x12);

  Write_Command(0xFD);  //设置 命令：A2,B1,B3,BB,BE 为可访问状态
  Write_Data(0xB1);

  Write_Command(0xAE);  //关显示（进入睡眠模式）

  Write_Command(0xB3);  //设置显示时钟分频&内部振荡器频率
  Write_Data(0xF1);   //时钟2分频，振荡器频率最高

  Write_Command(0xCA);  //设置 MUX Ratio
  Write_Data(0x7F);   //128

  Write_Command(0xA2);  //设置显示偏移
  Write_Data(0x00);   //无偏移

  Write_Command(0xA1);  //设置显示路线
  Write_Data(0x00);   //0x00

  Write_Command(0xA0);  //设置重映射格式
  Write_Data(0x74);   //水平地址增长模式、列地址0映射到SEG0、颜色数据传输顺序D[15:0] = [RRRRR:GGGGGG:BBBBB]、扫描方向：COM127~COM0、使能奇偶分离、颜色模式65K

  Write_Command(0xB5);  //GPIO0&GPIO1设置
  Write_Data(0x00);   //GPIO0&GPIO1设为高阻抗（硬件电路中两引脚没有连接）

  Write_Command(0xAB);  //功能选择
  Write_Data(0x01);   //使能VDD内部稳压器、选择8位并行接口

  Write_Command(0xB4);  //设置SEG低压
  Write_Data(0xA0);   //使能外部VSL
  Write_Data(0xB5);
  Write_Data(0x55);

  Write_Command(0xC1);  //设置颜色A,B,C的对比电流
  Write_Data(0xC8);   //A-->Red
  Write_Data(0x80);   //B-->Green
  Write_Data(0xC8);   //C-->Blue

  Write_Command(0xC7);  //主对比度电流控制
  Write_Data(0x0f);   //0x0f

  Set_Gray_Scale_Table(); // 设置灰度表脉冲宽度

  Write_Command(0xB1);  //设置阶段1（复位阶段）&阶段2（预充电）的周期
  Write_Data(0x32);   //阶段1为5个时钟周期、阶段2为3个时钟周期

  Write_Command(0xBB);  //设置预充电电压
  Write_Data(0x17);   //0.5*VCC

  Write_Command(0xB6);  //设置Second预充周期
  Write_Data(0x01);   //1个时钟周期（1DCLKS）

  Write_Command(0xBE);  //设置VCOMH电压
  Write_Data(0x05);   //0.82*VCC

  Write_Command(0xA6);  //设置显示模式：正常模式

  Fill_RAM(0x0000);  //清屏

  Write_Command(0xAF);  //开显示
}




/************************ Copyright (C)2017 LGG. All Rights Reserved *****END OF FILE****/


