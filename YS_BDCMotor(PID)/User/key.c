#include "key.h"

void KEY_GPIO_Init(void)
{
    //����һ��GPIO_InitTypeDef ���͵Ľṹ��
    GPIO_InitTypeDef  GPIO_InitStructure;	
    
    RCC_APB2PeriphClockCmd(KEY1_GPIO_RCC | 
                           KEY2_GPIO_RCC | 
                           KEY3_GPIO_RCC | 
                           KEY4_GPIO_RCC | 
                           KEY5_GPIO_RCC, ENABLE);//ʹ������ʱ��
    
    /* ����KEY1 GPIO:��������ģʽ */
    GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;//ѡ��Ҫ�õ�GPIO����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //��������ģʽΪ��������ģʽ					 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;//���������ٶ�Ϊ10MHZ         
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO    
    /* ����KEY2 GPIO:��������ģʽ */
    GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;//ѡ��Ҫ�õ�GPIO����
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO 
    /* ����KEY3 GPIO:��������ģʽ */
    GPIO_InitStructure.GPIO_Pin = KEY3_GPIO_PIN;//ѡ��Ҫ�õ�GPIO����
    GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO    
    /* ����KEY4 GPIO:��������ģʽ */
    GPIO_InitStructure.GPIO_Pin = KEY4_GPIO_PIN;//ѡ��Ҫ�õ�GPIO����
    GPIO_Init(KEY4_GPIO_PORT, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO     
    /* ����KEY5 GPIO:��������ģʽ */
    GPIO_InitStructure.GPIO_Pin = KEY5_GPIO_PIN;//ѡ��Ҫ�õ�GPIO����
    GPIO_Init(KEY5_GPIO_PORT, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO 
}

uint8_t key_press_timer,key_relse_timer,key_input,key_input_last,key_id,key_id_done;
void KEY_Scan(void)
{
	key_input_last=key_input;
	key_input=0;

    if(GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN)==Bit_RESET)
        key_input = KEY_SPEED_UP;
    if(GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_GPIO_PIN)==Bit_RESET)
        key_input = KEY_SPEED_DOWN;
    if(GPIO_ReadInputDataBit(KEY3_GPIO_PORT, KEY3_GPIO_PIN)==Bit_RESET)
        key_input = KEY_START;
    if(GPIO_ReadInputDataBit(KEY4_GPIO_PORT, KEY4_GPIO_PIN)==Bit_RESET)
        key_input = KEY_STOP;
    //if(GPIO_ReadInputDataBit(KEY5_GPIO_PORT, KEY5_GPIO_PIN)==Bit_RESET)
    //    key_input = KEY_SHUTDOWN;
    
	if(key_input>0&&key_input_last==key_input)
	{
		key_press_timer++;
		key_relse_timer=0;
	    if(key_press_timer>3)
	    {
            key_press_timer=100;	   	   	   	
            key_id=key_input;	 	   		
        }	   
	}   
    else
	{
		key_press_timer=0;
		key_relse_timer++;
        if(key_relse_timer>6)
        {
            key_relse_timer=100; 
            key_id_done=0;
            key_id=KEY_NONE;  
        }	   
	}      
}



