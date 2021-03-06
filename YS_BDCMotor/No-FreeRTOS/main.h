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

extern uint8_t timer_1ms;

#define TIME_BASE_MAIN      20

#define UART_CONTROL //使用串口控制电机运转

void SYSTICK_Callback(void);

#endif
