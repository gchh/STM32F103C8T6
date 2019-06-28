#include "adc.h"
extern __IO uint32_t uwTick;

#define ADC_DISABLE_TIMEOUT             ((uint32_t) 2)

void ADC_CUR_GPIO_Init(void)
{
    //����һ��GPIO_InitTypeDef ���͵Ľṹ��
    GPIO_InitTypeDef  GPIO_InitStructure;	

    RCC_APB2PeriphClockCmd(ADC_CUR_GPIO_RCC, ENABLE);//ʹ������ʱ��
    
    /*PC0*/
    GPIO_InitStructure.GPIO_Pin = ADC_CUR_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ADC_CUR_GPIO, &GPIO_InitStructure);//���ÿ⺯������ʼ��GPIO
}

void ADC_CUR_Init(void)
{
    uint32_t tickstart = 0, adc_error = 0;
    ADC_InitTypeDef ADC_InitStructure;
    
    RCC_APB2PeriphClockCmd(ADCx_RCC, ENABLE);//ʹ������ʱ��
    
    //ADC1
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //����ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //����ת��ģʽʹ��
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; //ɨ��ģʽ�ر�
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //�����Ҷ���
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //�������
    ADC_InitStructure.ADC_NbrOfChannel = 1; //ת��ͨ������
    ADC_Init(ADCx, &ADC_InitStructure);
    
    /* ���õ�������ͨ�� */
    ADC_RegularChannelConfig(ADCx, ADC_CURRENT_CHANNEL, 1, ADC_SampleTime_13Cycles5);
    //У׼
    if(ADCx->CR2 & ADC_CR2_ADON) //ADC�ѿ���ת����У׼ǰ�ȹر�
    {
        ADC_Cmd(ADCx, DISABLE);
        tickstart = uwTick;
        //�ȴ�ADCֹͣ
        while( (ADCx->CR2 & ADC_CR2_ADON) != RESET )
        {
            if((uwTick - tickstart) > ADC_DISABLE_TIMEOUT)
            {
                adc_error=1;
            }            
        }
    }
    if(adc_error==0)
    {
        ADC_ResetCalibration(ADCx);
        while( ADC_GetResetCalibrationStatus(ADCx) == SET );
        ADC_StartCalibration(ADCx);
        while( ADC_GetCalibrationStatus(ADCx) == SET );
    }
}














