/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "oled.h"
#include "font.h"


/**
  * @������       Delay
  * @��  ��       �ӳ�
  * @��  ��       data���ӳ�data
  * @����ֵ       ��
  */
static void Delay(uint32_t data)
{
	while(data--);
}



/**
  * @������       OLED_GPIO_Init
  * @��  ��       OLED���ź����ų�ʼ��
  * @��  ��       ��
  * @����ֵ       ��
  */
static void OLED_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	//����ṹ��
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE, ENABLE);	//ʹ��GPIOAʱ��
	GPIO_InitStructure.GPIO_Pin = OLED_CS_GPIO_PINS | OLED_WR_GPIO_PINS;		//��ʼ�����ڿ���OLED���������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);	//��ʼ��
    GPIO_InitStructure.GPIO_Pin = OLED_RES_GPIO_PINS | OLED_DC_GPIO_PINS;		//��ʼ�����ڿ���OLED���������
    GPIO_Init(GPIOC, &GPIO_InitStructure);	//��ʼ��
    GPIO_InitStructure.GPIO_Pin = OLED_RD_GPIO_PINS;		//��ʼ�����ڿ���OLED���������
    GPIO_Init(GPIOE, &GPIO_InitStructure);	//��ʼ��    

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 
                                | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;		//��ʼ�����ڿ���OLED���ź�����
	GPIO_Init(GPIOE, &GPIO_InitStructure);	//��ʼ��
	
	GPIO_SetBits(GPIOB, OLED_CS_GPIO_PINS | OLED_WR_GPIO_PINS);	//��RES��DC��CS��RD��WR��������
    GPIO_SetBits(GPIOC, OLED_RES_GPIO_PINS | OLED_DC_GPIO_PINS);	//��RES��DC��CS��RD��WR��������
    GPIO_SetBits(GPIOE, OLED_RD_GPIO_PINS);	//��RES��DC��CS��RD��WR��������
}


static void Parallel_Write(unsigned char Data)
{
	unsigned int temp = 0;
	
	GPIOE->ODR = (GPIOE->ODR & 0x00ff) | ((temp | Data) << 8);
}


/**
  * @������       Write_Command
  * @��  ��       ��OLED��������
  * @��  ��       Cmd�����͵�����
  * @����ֵ       ��
  */
void Write_Command(unsigned char Cmd)
{
  OLED_DC_Clr();  //DC���ͣ���������
	SPI_NSS_LOW(); //Ƭѡ���ͣ�ѡͨ����
	OLED_WR_Clr();	//WR���ͣ�ʹ����OLEDд��
  Parallel_Write(Cmd); //��������
	OLED_WR_Set();	//WR���ߣ�������OLEDд��
  SPI_NSS_HIGH(); //Ƭѡ���ߣ��ر�����
  OLED_DC_Set(); //DC���ߣ�����ʱΪ�ߵ�ƽ
}


/**
  * @������       Write_Data
  * @��  ��       ��OLED��������
  * @��  ��       Data�����͵�����
  * @����ֵ       ��
  */
void Write_Data(unsigned char Data)
{
	OLED_DC_Set();  //DC���ߣ���������
	SPI_NSS_LOW(); //Ƭѡ���ͣ�ѡͨ����
	OLED_WR_Clr();	//WR���ͣ�ʹ����OLEDд��
  Parallel_Write(Data); //��������
	OLED_WR_Set();	//WR���ߣ�������OLEDд��
  SPI_NSS_HIGH(); //Ƭѡ���ߣ��ر�����
  OLED_DC_Set(); //DC���ߣ�����ʱΪ�ߵ�ƽ
}


/**
  * @������       Set_Column_Address
  * @��  ��       �����е�ַ
  * @��  ��       Clo_Start������ʼ��ַ��Clo_End���н�����ַ
  * @����ֵ       ��
  */
void Set_Column_Address(unsigned char Clo_Start, unsigned char Clo_End)
{
  Write_Command(0x15);  // �����е�ַ
  Write_Data(Clo_Start);  // ����ʼ��ַ
  Write_Data(Clo_End);  // �н�����ַ
}


/**
  * @������       Set_Row_Address
  * @��  ��       �����е�ַ
  * @��  ��       Row_Start������ʼ��ַ��Row_End���н�����ַ
  * @����ֵ       ��
  */
void Set_Row_Address(unsigned char Row_Start, unsigned char Row_End)
{
  Write_Command(0x75);  // �����е�ַ
  Write_Data(Row_Start);  // ����ʼ��ַ
  Write_Data(Row_End);  // �н�����ַ
}


/**
  * @������       Set_Write_RAM
  * @��  ��       ʹ��MCUдRAM
  * @��  ��       ��
  * @����ֵ       ��
  */
void Set_Write_RAM()
{
  Write_Command(0x5C);  //ʹ��MCUдRAM
}


/**
  * @������       Set_Read_RAM
  * @��  ��       ʹ��MCU��RAM
  * @��  ��       ��
  * @����ֵ       ��
  */
void Set_Read_RAM()
{
  Write_Command(0x5D);  //ʹ��MCU��RAM
}


/**
  * @������       Fill_RAM
  * @��  ��       ���ȫ����ˢ����
  * @��  ��       Colour_RGB�������ɫ��RRRRRGGGGGGBBBBB
  * @����ֵ       ��
  */
void Fill_RAM(int Colour_RGB)
{
  unsigned char i,j;  //�������

  Set_Column_Address(0x00,0x7F);  //�����е�ַ0~127
  Set_Row_Address(0x00,0x7F);   //�����е�ַ0~127
  Set_Write_RAM();  //ʹ��MCUдRAM

  for(i=0;i<128;i++)  //���128��*128��
  {
    for(j=0;j<128;j++)
    {
      Write_Data(Colour_RGB >> 8);  //д�������ɫ���ֽ�
      Write_Data(Colour_RGB); //д�������ɫ���ֽ�
    }
  }
}


/**
  * @������       Clear_Screen
  * @��  ��       ָ��ĳһ��������һ��8�У�0~7��
  * @��  ��       Var_Row����Var_Row��
  * @����ֵ       ��
  */
void Clear_Screen(unsigned char Var_Row)
{
	unsigned int i;

	Set_Column_Address(0x00, 0x7F);	//�����е�ַ0~127
	Set_Row_Address(Var_Row*16, (Var_Row*16 + 15));		//�����е�ַ
	Set_Write_RAM();	//ʹ��MCUдRAM

	for(i = 0; i < 2048; i++)	//����һ�������������� = 16*128 = 2048
	{
		Write_Data(0x00);
		Write_Data(0x00);
	}
}


/**
  * @������       Draw_Point
  * @��  ��       ����
  * @��  ��       x������ʼ��ַ��y������ʼ��ַ��Colour_RGB�������ɫ��RRRRRGGGGGGBBBBB
  * @����ֵ       ��
  */
void Draw_Point(unsigned char x, unsigned char y, int Colour_RGB)
{
  Write_Command(0x15);  // �����е�ַ
  Write_Data(x);    

  Write_Command(0x75);  // �����е�ַ
  Write_Data(y);    

  Set_Write_RAM();  //ʹ��MCUдRAM

  Write_Data(Colour_RGB >> 8); //д�������ɫ���ֽ�
  Write_Data(Colour_RGB);  //д�������ɫ���ֽ�
}


/**
  * @������       display_char
  * @��  ��       ָ�����껭һ���ַ�������ʽ�����ַ���С�̶�Ϊ8*16
  * @��  ��       x������ʼ��ַ��y������ʼ��ַ��Var_Char��д����ַ���Colour_RGB�������ɫ��RRRRRGGGGGGBBBBB
  * @����ֵ       ��
  */
void Display_Char(unsigned char x, unsigned char y, unsigned char Var_Char, int Colour_RGB)
{
  unsigned char i, j; //�������
  unsigned char Var_Temp, x_Temp; //�������

  x_Temp = x; //��ȡx��ʼ����
  for (i = 0; i < 16; i++)
  {
    Var_Temp = ACSII_Font[Var_Char - 0x20][i];  //��ȡ�ַ��������ƫ����
    for ( j = 0; j < 8; j++)
    {
      if (Var_Temp & 0x80)  //�Ȼ����λ�ĵ㣬Ϊ1�򻭶�Ӧ����ɫ
        Draw_Point(x, y, Colour_RGB);
      //else
        //Draw_Point(x, y, 0x0000); //Ϊ0���ɫ����������������Ϊ����ɫ

      Var_Temp <<= 1; //��λ��׼������һ����
      x++;  //x�����1

      if((x - x_Temp) == 8)
      { 
        x = x_Temp; //x����ص���ʼλ��
        y++;  //y�����1
        break;  //�˳���ǰѭ��
      }
    }
  }
}


/**
  * @������       Display_String
  * @��  ��       ��ʾ�ַ���
  * @��  ��       x������ʼ��ַ��y������ʼ��ַ��*chr����ʾ���ַ�����Colour_RGB�������ɫ��RRRRRGGGGGGBBBBB
  * @����ֵ       ��
  */
void Display_String(unsigned char x, unsigned char y, unsigned char *chr, int Colour_RGB)
{
  unsigned char i = 0;  //�������

  while(chr[i] != '\0') //�ж��Ƿ����
  {
    Display_Char(x, y, chr[i], Colour_RGB); //��ʾ�ַ�
    x += 8; //x�����8��һ���ַ�x����ռ8���㣬Y����ռ16���㣩
    i++;  //��һ���ַ�
  }
}


/**
  * @������       Display_Chinese
  * @��  ��       ָ�����껭һ���ַ�������ʽ�����ַ���С�̶�Ϊ16*16
  * @��  ��       x������ʼ��ַ��y������ʼ��ַ��Var_Char��������������±ꣻColour_RGB�������ɫ��RRRRRGGGGGGBBBBB
  * @����ֵ       ��
  */
void Display_Chinese(unsigned char x, unsigned char y, unsigned char Var_Char, int Colour_RGB)
{
  unsigned char i, j; //�������
  unsigned char Var_Temp, x_Temp; //�������

  x_Temp = x; //��ȡx��ʼ����
  for(i = 0; i < 32; i++) //16*16 = 32*8��һ��ֻ��д��һ�ֽڣ�Ҳ����8λ�����Բ���8*32�Ļ���
  {
    Var_Temp = Chinese_Font[Var_Char][i]; //��ȡ�����������ƫ����
    for(j = 0; j < 8; j++)
    {
      if(Var_Temp & 0x80)   //�Ȼ����λ�ĵ㣬Ϊ1�򻭶�Ӧ����ɫ
        Draw_Point(x, y, Colour_RGB);
      //else
        //Draw_Point(x, y, 0x0000); //Ϊ0���ɫ����������������Ϊ����ɫ
      Var_Temp <<= 1; //��λ��׼������һ����
      x++;  //x�����1

      if((x - x_Temp) == 16)  //����һ��
      {
        x = x_Temp; //x����ص���ʼλ��
        y++;  //y�����1
        break;  //�˳���ǰѭ��
      }
    }
  }
}


/**
  * @������       Num_Pow
  * @��  ��       m^n������
  * @��  ��       m��������n��ָ��
  * @����ֵ       ��
  */
uint32_t Num_Pow(unsigned char m,unsigned char n)
{
  uint32_t result=1; //�������

  while(n--)result*=m;  //m^n
  return result;  //���ؽ��
}


/**
  * @������       Display_Num
  * @��  ��       ��ʾ����
  * @��  ��       x������ʼ��ַ��y������ʼ��ַ��Var_Num����ʾ�����֣�Num_Len�����ֳ��ȣ�Colour_RGB�������ɫ��RRRRRGGGGGGBBBBB
  * @����ֵ       ��
  */
void Display_Num(unsigned char x,unsigned char y,int Var_Num, unsigned char Num_Len, int Colour_RGB)
{
  unsigned char i;  //�������
  unsigned char Var_Temp;   //�������
  unsigned char Var_Show = 0; //�������

  for(i = 0; i < Num_Len; i++)  //��ʾ����
  {
    Var_Temp  =  (Var_Num / Num_Pow(10, Num_Len-i-1)) % 10;  //��ȡ���λ��������ʾ��һλһλȡ��������ʾ��
    if(Var_Show == 0 && i<(Num_Len - 1))  //�ж���λ�����Ƿ�Ϊ0
    {
      if(Var_Temp == 0) 
      {
        Display_Char(x+8*i, y, ' ', Colour_RGB);  //��λ����Ϊ0����ʾ
        continue; //����ѭ��
      }
      else
        Var_Show = 1; //���������������ʾ
    }

    Display_Char(x+8*i, y, Var_Temp + '0', Colour_RGB); //��ʾ����
  }
}


/**
  * @������       Display_Pattern
  * @��  ��       ��ʾͼƬ
  * @��  ��       *Pattern_Data��ͼƬ���飻Start_x��x������㣻End_x��x�����յ㣻Start_y��y������㣻End_y��y�����յ�
  * @����ֵ       ��
  */
void Display_Pattern(unsigned char *Pattern_Data, unsigned char Start_x, unsigned char End_x, unsigned char Start_y, unsigned char End_y)
{
	unsigned char *Data_Pointer;	//����ָ�����
	unsigned char i, j;		//�������

	Data_Pointer = Pattern_Data;	//��ȡ�����׵�ַ
	Set_Column_Address(Start_x, End_x);	//�����з�Χ
	Set_Row_Address(Start_y, End_y);	//�����з�Χ
	Set_Write_RAM();	//����MCUдRAM

	for(i = 0; i < (End_y - Start_y + 1); i++)	//x����(End_y - Start_y + 1)
	{
		for(j = 0; j < (End_x - Start_x + 1); j++)	//y����(End_y - Start_y + 1)
		{
			Write_Data(*Data_Pointer);	//д�����ݣ�һ������Ҫд��16λ���ݣ�RRRRRGGGGGGBBBBB��
			Data_Pointer++;
			Write_Data(*Data_Pointer);
			Data_Pointer++;
		}
	}
}


/**
  * @������       Set_Gray_Scale_Table
  * @��  ��       �ҽױ�����
  * @��  ��       ��
  * @����ֵ       ��
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
  * @������       OLED_Init
  * @��  ��       OLED��ʼ��
  * @��  ��       ��
  * @����ֵ       ��
  */
void OLED_Init(void)
{
  OLED_GPIO_Init();     //��ʼ��RES��DC��CS����

  OLED_RES_Set();
  Delay(100);
  OLED_RES_Clr();
  Delay(200); //�ӳ٣����ڵ�Ƭ���ϵ��ʼ����OLED�죬���Ա�������ӳ٣��ȴ�OLED�ϵ��ʼ�����
  OLED_RES_Set();
  Delay(200);
    
  Write_Command(0xFD);  // ����OLED����IC��SSD1351������������
  Write_Data(0x12);

  Write_Command(0xFD);  //���� ���A2,B1,B3,BB,BE Ϊ�ɷ���״̬
  Write_Data(0xB1);

  Write_Command(0xAE);  //����ʾ������˯��ģʽ��

  Write_Command(0xB3);  //������ʾʱ�ӷ�Ƶ&�ڲ�����Ƶ��
  Write_Data(0xF1);   //ʱ��2��Ƶ������Ƶ�����

  Write_Command(0xCA);  //���� MUX Ratio
  Write_Data(0x7F);   //128

  Write_Command(0xA2);  //������ʾƫ��
  Write_Data(0x00);   //��ƫ��

  Write_Command(0xA1);  //������ʾ·��
  Write_Data(0x00);   //0x00

  Write_Command(0xA0);  //������ӳ���ʽ
  Write_Data(0x74);   //ˮƽ��ַ����ģʽ���е�ַ0ӳ�䵽SEG0����ɫ���ݴ���˳��D[15:0] = [RRRRR:GGGGGG:BBBBB]��ɨ�跽��COM127~COM0��ʹ����ż���롢��ɫģʽ65K

  Write_Command(0xB5);  //GPIO0&GPIO1����
  Write_Data(0x00);   //GPIO0&GPIO1��Ϊ���迹��Ӳ����·��������û�����ӣ�

  Write_Command(0xAB);  //����ѡ��
  Write_Data(0x01);   //ʹ��VDD�ڲ���ѹ����ѡ��8λ���нӿ�

  Write_Command(0xB4);  //����SEG��ѹ
  Write_Data(0xA0);   //ʹ���ⲿVSL
  Write_Data(0xB5);
  Write_Data(0x55);

  Write_Command(0xC1);  //������ɫA,B,C�ĶԱȵ���
  Write_Data(0xC8);   //A-->Red
  Write_Data(0x80);   //B-->Green
  Write_Data(0xC8);   //C-->Blue

  Write_Command(0xC7);  //���Աȶȵ�������
  Write_Data(0x0f);   //0x0f

  Set_Gray_Scale_Table(); // ���ûҶȱ�������

  Write_Command(0xB1);  //���ý׶�1����λ�׶Σ�&�׶�2��Ԥ��磩������
  Write_Data(0x32);   //�׶�1Ϊ5��ʱ�����ڡ��׶�2Ϊ3��ʱ������

  Write_Command(0xBB);  //����Ԥ����ѹ
  Write_Data(0x17);   //0.5*VCC

  Write_Command(0xB6);  //����SecondԤ������
  Write_Data(0x01);   //1��ʱ�����ڣ�1DCLKS��

  Write_Command(0xBE);  //����VCOMH��ѹ
  Write_Data(0x05);   //0.82*VCC

  Write_Command(0xA6);  //������ʾģʽ������ģʽ

  Fill_RAM(0x0000);  //����

  Write_Command(0xAF);  //����ʾ
}




/************************ Copyright (C)2017 LGG. All Rights Reserved *****END OF FILE****/


