#ifndef __BDCMOTOR_H
#define __BDCMOTOR_H

#include "stm32f10x.h"

#define BDCMOTOR_TIMx                       TIM1
#define BDCMOTOR_TIM_RCC                    RCC_APB2Periph_TIM1

#define BDCMOTOR_TIM_CH1_GPIO_RCC           RCC_APB2Periph_GPIOA
#define BDCMOTOR_TIM_CH1_PORT               GPIOA
#define BDCMOTOR_TIM_CH1_PIN      			GPIO_Pin_8

#define BDCMOTOR_TIM_CH1N_GPIO_RCC          RCC_APB2Periph_GPIOB
#define BDCMOTOR_TIM_CH1N_PORT              GPIOB
#define BDCMOTOR_TIM_CH1N_PIN      			GPIO_Pin_13

#define SHUTDOWN_GPIO_RCC                   RCC_APB2Periph_GPIOB
#define SHUTDOWN_PORT                       GPIOB
#define SHUTDOWN_PIN      			        GPIO_Pin_12
#define SHUTDOWN_ON         			    GPIO_WriteBit(SHUTDOWN_PORT,SHUTDOWN_PIN,Bit_SET)//高电平shutdown
#define SHUTDOWN_OFF     			        GPIO_WriteBit(SHUTDOWN_PORT,SHUTDOWN_PIN,Bit_RESET)//低电平正常pwm控制

// 定义定时器预分频，定时器实际时钟频率为：8MHz/（BDCMOTOR_TIMx_PRESCALER+1）
#define BDCMOTOR_TIM_PRESCALER               0    // 实际时钟频率为：8MHz

// 定义定时器周期，PWM频率为：168MHz/（BDCMOTOR_TIMx_PRESCALER+1）/（BDCMOTOR_TIM_PERIOD+1）
#define BDCMOTOR_TIM_PERIOD                  399  // PWM频率为8MHz/(399+1)=20KHz

#define BDCMOTOR_DUTY_ZERO                   (0) // 0%占空比
#define BDCMOTOR_DUTY_FULL                   (BDCMOTOR_TIM_PERIOD-60) // 由于自举电容的存在,无法做到100%占空比

// 定义高级定时器重复计数寄存器值
// 实际PWM频率为：8MHz/（BDCMOTOR_TIMx_PRESCALER+1）/（BDCMOTOR_TIM_PERIOD+1）/（BDCMOTOR_TIM_REPETITIONCOUNTER+1）
#define BDCMOTOR_TIM_REPETITIONCOUNTER       0


void BDCMOTOR_GPIO_Init(void);
void BDCMOTOR_TIMx_Init(void);
void SetMotorSpeed(int16_t Duty);
void SetMotorDir(int16_t Dir);
void SHUTDOWN_Toggle(void);//for test

#endif
