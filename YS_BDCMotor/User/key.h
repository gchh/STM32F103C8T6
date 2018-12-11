#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define KEY1_GPIO_RCC               RCC_APB2Periph_GPIOB
#define KEY1_GPIO_PORT              GPIOB
#define KEY1_GPIO_PIN      		    GPIO_Pin_11

#define KEY2_GPIO_RCC               RCC_APB2Periph_GPIOB
#define KEY2_GPIO_PORT              GPIOB
#define KEY2_GPIO_PIN      		    GPIO_Pin_10

#define KEY3_GPIO_RCC               RCC_APB2Periph_GPIOB
#define KEY3_GPIO_PORT              GPIOB
#define KEY3_GPIO_PIN      		    GPIO_Pin_1

#define KEY4_GPIO_RCC               RCC_APB2Periph_GPIOB
#define KEY4_GPIO_PORT              GPIOB
#define KEY4_GPIO_PIN      		    GPIO_Pin_0

#define KEY5_GPIO_RCC               RCC_APB2Periph_GPIOB
#define KEY5_GPIO_PORT              GPIOB
#define KEY5_GPIO_PIN      		    GPIO_Pin_9

#define KEY_NONE                    0
#define KEY_SPEED_UP                1
#define KEY_SPEED_DOWN              2
#define KEY_TURN_Clockwise          3
#define KEY_TURN_Anticlockwise      4
#define KEY_SHUTDOWN                5

extern uint8_t key_id,key_id_done;

void KEY_GPIO_Init(void);
void KEY_Scan(void);

#endif
