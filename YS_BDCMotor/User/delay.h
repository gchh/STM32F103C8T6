#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

void delay(__IO uint32_t delay_cnt);

void delay_init();
void delay_us(u32 nus);
void delay_ms(u16 nms);

#endif
