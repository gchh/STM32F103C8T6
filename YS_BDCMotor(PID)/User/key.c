#include "key.h"

void KEY_GPIO_Init(void)
{
    //定义一个GPIO_InitTypeDef 类型的结构体
    GPIO_InitTypeDef  GPIO_InitStructure;	
    
    RCC_APB2PeriphClockCmd(KEY1_GPIO_RCC | 
                           KEY2_GPIO_RCC | 
                           KEY3_GPIO_RCC | 
                           KEY4_GPIO_RCC | 
                           KEY5_GPIO_RCC, ENABLE);//使能外设时钟
    
    /* 配置KEY1 GPIO:输入上拉模式 */
    GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;//选择要用的GPIO引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置引脚模式为输入上拉模式					 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;//设置引脚速度为10MHZ         
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);//调用库函数，初始化GPIO    
    /* 配置KEY2 GPIO:输入上拉模式 */
    GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;//选择要用的GPIO引脚
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);//调用库函数，初始化GPIO 
    /* 配置KEY3 GPIO:输入上拉模式 */
    GPIO_InitStructure.GPIO_Pin = KEY3_GPIO_PIN;//选择要用的GPIO引脚
    GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStructure);//调用库函数，初始化GPIO    
    /* 配置KEY4 GPIO:输入上拉模式 */
    GPIO_InitStructure.GPIO_Pin = KEY4_GPIO_PIN;//选择要用的GPIO引脚
    GPIO_Init(KEY4_GPIO_PORT, &GPIO_InitStructure);//调用库函数，初始化GPIO     
    /* 配置KEY5 GPIO:输入上拉模式 */
    GPIO_InitStructure.GPIO_Pin = KEY5_GPIO_PIN;//选择要用的GPIO引脚
    GPIO_Init(KEY5_GPIO_PORT, &GPIO_InitStructure);//调用库函数，初始化GPIO 
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



