#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f10x.h"

#include "delay.h"
#include "led.h"
#include "BDCMotor.h"
#include "key.h"
#include "usart.h"
#include "Encoder.h"
#include "adc.h"
#include "oled.h"
#include "pid.h"

extern uint8_t timer_1ms;


#define TIME_BASE_MAIN      20

#define FB_USE_GRAPHIC              // 使用图像曲线作为反馈

void SYSTICK_Callback(void);

#endif
