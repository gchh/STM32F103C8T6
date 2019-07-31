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
#define SHUTDOWN_ON         			    GPIO_WriteBit(SHUTDOWN_PORT,SHUTDOWN_PIN,Bit_SET)//�ߵ�ƽshutdown
#define SHUTDOWN_OFF     			        GPIO_WriteBit(SHUTDOWN_PORT,SHUTDOWN_PIN,Bit_RESET)//�͵�ƽ����pwm����

// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��8MHz/��BDCMOTOR_TIMx_PRESCALER+1��
#define BDCMOTOR_TIM_PRESCALER               0    // ʵ��ʱ��Ƶ��Ϊ��8MHz

// ���嶨ʱ�����ڣ�PWMƵ��Ϊ��8MHz/��BDCMOTOR_TIMx_PRESCALER+1��/��BDCMOTOR_TIM_PERIOD+1��
#define BDCMOTOR_TIM_PERIOD                  399  // PWMƵ��Ϊ8MHz/(399+1)=20KHz

#define BDCMOTOR_DUTY_ZERO                   (0) // 0%ռ�ձ�
#define BDCMOTOR_DUTY_FULL                   (BDCMOTOR_TIM_PERIOD-4) // �����Ծٵ��ݵĴ���,�޷�����100%ռ�ձ�

// ����߼���ʱ���ظ������Ĵ���ֵ
// ʵ��PWMƵ��Ϊ��8MHz/��BDCMOTOR_TIMx_PRESCALER+1��/��BDCMOTOR_TIM_PERIOD+1��/��BDCMOTOR_TIM_REPETITIONCOUNTER+1��
#define BDCMOTOR_TIM_REPETITIONCOUNTER       0

#define BDCMOTOR_IDLE     0
#define BDCMOTOR_RUN      1

void BDCMOTOR_GPIO_Init(void);
void BDCMOTOR_TIMx_Init(void);
void SetMotorSpeed(int16_t Duty);
void SetMotorDir(int16_t Dir);
void SHUTDOWN_Toggle(void);//for test
void SetMotorStop(void);

extern __IO int16_t PWM_Duty;
extern uint8_t BDCMOTOR_state;;

#endif
