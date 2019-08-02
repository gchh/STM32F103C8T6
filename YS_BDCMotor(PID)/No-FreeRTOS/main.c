#include "main.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
#define ENCODER     11    // ����������
#define SPEEDRATIO  30    // ������ٱ�
#define PPR         (SPEEDRATIO*ENCODER*4) // Pulse/r ÿȦ�ɲ����������
/* ˽�б��� ------------------------------------------------------------------*/
__IO uint8_t  start_flag=0;        // PID ��ʼ��־
__IO int32_t LastSpd_Pulse= 0;      // ����������ֵ Pulse

uint8_t timer_1ms;

void Key_process(void);

/**
  * @file   main
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    //char c[15];
	OLED_Init();	//OLED��ʼ��
	Fill_RAM(0x0000);	//����

	/*Configure the SysTick to have interrupt in 1ms time basis*/
	//in the file system_stm3210x.c, configure the SystemCoreClock
        /* #define SYSCLK_FREQ_HSE    HSE_VALUE */
        /* #define SYSCLK_FREQ_24MHz  24000000 */
        /* #define SYSCLK_FREQ_36MHz  36000000 */
        /* #define SYSCLK_FREQ_48MHz  48000000 */
        /* #define SYSCLK_FREQ_56MHz  56000000 */
        /* #define SYSCLK_FREQ_72MHz  72000000 */
    SysTick_Config(SystemCoreClock/1000);
    
    /*��ʼ��LED�˿�*/
    LED_GPIO_Config();
    
    KEY_GPIO_Init();
    
    /* ���ڳ�ʼ�� */
    USART_GPIO_Init();
    USART_Config();

    /* ��������ʼ����ʹ�ܱ�����ģʽ */
    ENCODER_GPIO_Init();
    ENCODER_TIMx_Init();
  
    /* �߼����ƶ�ʱ����ʼ��������PWM������� */
    BDCMOTOR_GPIO_Init();
    BDCMOTOR_TIMx_Init();

    /* ADC-DMA ��ʼ�� */
    ADC_CUR_GPIO_Init();
    ADC_VOLT_GPIO_Init();
    ADC_CUR_Init();
    ADC_VOLT_Init();
    ADC_DMA_Init();
    ADC_Calibration();
    /* ����ADת����ʹ��DMA������ж� */
    ADC_Start_DMA();

    /* PID ������ʼ�� */
    PID_ParamInit();    
    
    //printf("Input: \n");
    while(1)
    {
    //scanf("%s", c);
    //printf("Input is %s\n",c);        
        KEY_Scan();
        if(key_id_done==0)Key_process();
        LED1Toggle();
        while(timer_1ms<TIME_BASE_MAIN){}; 	//20ms
	    timer_1ms=0;
    }
}

void Key_process(void)
{
    switch(key_id)
    {
        case KEY_SPEED_UP:
            key_id_done=1;
            if(BDCMOTOR_state == BDCMOTOR_RUN)
            {
                sPID.SetPoint += 2;
                if(sPID.SetPoint >= MAX_SPEED) // ���� r/m
                    sPID.SetPoint = MAX_SPEED;
            }
            break;
        case KEY_SPEED_DOWN:
            key_id_done=1;
            if(BDCMOTOR_state == BDCMOTOR_RUN)
            {
                sPID.SetPoint -= 2;
                if(sPID.SetPoint <=-MAX_SPEED)
                    sPID.SetPoint = -MAX_SPEED;
            }
            break;    
        case KEY_START:
            key_id_done=1;
            if(BDCMOTOR_state == BDCMOTOR_IDLE) 
            {
                if(sPID.SetPoint > 0)
                    SetMotorDir(1);
                else SetMotorDir(0);
                start_flag = 1;
            }
            break;
        case KEY_STOP:
            key_id_done=1;
            if(BDCMOTOR_state == BDCMOTOR_RUN)
            {
                start_flag = 0;
                sPID.PrevError  = 0;
                sPID.LastError = 0;
                PWM_Duty=0;
                SetMotorSpeed(PWM_Duty);
                SetMotorStop();
            }
            break;
    }
}



/**
  * ��������: ϵͳ�δ�ʱ���жϻص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ÿ����һ�εδ�ʱ���жϽ���ûص�����һ��
  */
extern __IO uint32_t uwTick;
static __IO uint32_t OverCurCount;          // ����������¼
void SYSTICK_Callback(void)
{
    __IO float Volt_Result = 0;
    __IO float Volt_Bus = 0;

    __IO int32_t Spd_Pulse = 0;       // ����������ֵ Pulse
    __IO int32_t Spd_PPS   = 0;       // �ٶ�ֵ Pulse/Sample
    __IO float Spd_RPM     = 0;       // �ٶ�ֵ r/m
    __IO int32_t FB_Speed  = 0;       // ���ڷ����ٶ�ֵ����λ�� 

    
    /* �ٶȻ�����100ms */
    if(uwTick % 100 == 0)
    {
        Spd_Pulse = ( int32_t )TIM_GetCounter(ENCODER_TIMx)+OverflowCount*65536;
        Spd_PPS = Spd_Pulse - LastSpd_Pulse;
        LastSpd_Pulse = Spd_Pulse;
        
        /* 11�߱�����,30���ٱ�,һȦ�����ź���11*30*4 */
        /* �Ŵ�10������r/s���Ŵ�60���Ǽ���r/m*/
        Spd_RPM = ((((float)Spd_PPS/(float)PPR)*10.0f)*(float)60);
        
        /* ����PID��� */
        if(start_flag == 1)
        {
            PWM_Duty = SpdPIDCalc(Spd_RPM);

            /* �����ٶȻ��ļ������жϵ�ǰ�˶����� */
            if(PWM_Duty < 0)
            {
                SetMotorDir(0);
                /* ����ռ�ձ� */
                if(PWM_Duty < -BDCMOTOR_DUTY_FULL)
                    PWM_Duty = -BDCMOTOR_DUTY_FULL;
                /* ֱ���޸�ռ�ձ� */
                SetMotorSpeed(-PWM_Duty);                
            }
            else
            {
                SetMotorDir(1);
                /* ����ռ�ձ� */
                if(PWM_Duty > BDCMOTOR_DUTY_FULL)
                    PWM_Duty = BDCMOTOR_DUTY_FULL;
                /* ֱ���޸�ռ�ձ� */
                SetMotorSpeed(PWM_Duty);        
            }
        }
//    #undef FB_USE_GRAPHIC 
#ifdef FB_USE_GRAPHIC 
        FB_Speed =ROUND_TO_INT32(Spd_RPM);
        Transmit_FB( (int32_t *)&FB_Speed);
#else
        //printf("Spd:%d Pulse -- Spd: %.2f r/m \n",Spd_PPS,Spd_RPM);
#endif        
    }

    
    /* ���ݷ���������50ms,���ڵ����ɼ����ڴ�Լ�� 2ms,�������ݷ���������ò�Ҫ����2ms */
    if ((uwTick % 50) == 0)
    {
        ADC_Resul = AverSum/AverCnt;
        /* ��������16���Ժ�,�Ե�17����ΪУ׼ƫ��ֵ */
        OffsetCnt_Flag++;
        if(OffsetCnt_Flag >= 16)
        {
            if(OffsetCnt_Flag == 16)
            {
                OffSetHex = ADC_Resul;
            }
            OffsetCnt_Flag = 32;
            ADC_Resul -= OffSetHex;     // ��ȥƫ��ֵ
        }
        /* �����ѹֵ�͵���ֵ */
        Volt_Result = ( (float)( (float)(ADC_Resul) * VOLT_RESOLUTION) );
        ADC_CurrentValue = (float)( (Volt_Result / GAIN) / SAMPLING_RES);
        if(Volt_Result<0) Volt_Result = 0;        
        /* ��ռ��� */
        AverCnt = 0;
        AverSum = 0;        

        /* �������� */
        if(OffsetCnt_Flag >= 32 )
        {
            if(ADC_CurrentValue >= CURRENT_MAX )
            {
                OverCurCount++;
                if(OverCurCount >= 5)
                {
                    printf("Over Current %.2f \n",ADC_CurrentValue);
                    printf("Please reset the target!!\n");
                    SHUTDOWN_ON;
                    SetMotorStop();
                    PWM_Duty = 0;
                    OverCurCount = 0;
                    while(1);
                }
            }
            else OverCurCount = 0;
        }     
    }    
}

/**
  * ��������: ��������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: �����յ�������֡���н�����ȡ.
  */
PID_TypeDef  *ptr =  &sPID;
void Analyse_Data_Callback()
{
    __IO uint8_t i = 0 ;
    __IO uint8_t *Ptr = &RxBuf[FRAME_CHECK_BEGIN+1];
    __IO char *Str = &Msg.data[0].Ch[0];
    Msg.Code = RxBuf[FRAME_CHECK_BEGIN];         // �ڶ�λ��ָ����
    /* ���ýṹ���������ڴ�������,ʹ��ָ����ȡ���� */
    for(i=0;i<(FRAME_CHECK_NUM-1);i++)
    {
        *Str++ = *Ptr++ ;
    }
    switch(Msg.Code)
    {
        /* ����PID���� */
        case CODE_SETPID:
            ptr->Proportion = Msg.data[0].Float; 
            ptr->Integral   = Msg.data[1].Float;
            ptr->Derivative = Msg.data[2].Float;
            break;
        /* ����Ŀ��ֵ */
        case CODE_SETTGT:
            ptr->SetPoint = Msg.data[0].Int;
            break;
        case CODE_RESET:
            /* System Reset */
            NVIC_SystemReset(); 
            break;
        case CODE_STARTMOTOR:
            if(BDCMOTOR_state == BDCMOTOR_IDLE) 
            {
                if(sPID.SetPoint > 0)
                    SetMotorDir(1);
                else SetMotorDir(0);
                start_flag = 1;
            }
            break;
        default:break;
    }
} 

/**
  * ��������: ADC���Ź��жϻص�����
  * �������: ADC���
  * �� �� ֵ: ��
  * ˵    ��: ADC���ڿ��Ź�,��⵽��ѹ���ͻ��߹��ߵ�ʱ��͵����������,ֹͣ���.
  */
void ADC_LevelOutOfWindowCallback(void)
{
    /* ʹ�ܵ���������� */
    static uint8_t i = 0;
    i++;
    if(ADC_VoltBus > VOLT_LIMIT_MIN  && ADC_VoltBus < VOLT_LIMIT_MAX)
    {
        i = 0 ;
    }
    else if(i>=6)
    {
        SHUTDOWN_ON;
        SetMotorStop();
        PWM_Duty = 0;
//        ADC_VoltBus = (float)ADC_VoltBus * VOLTBUS_RESOLUTION;// ADC_VoltBus�����ж���Ӧ�����ж�ȡ��adcֵ
        
        printf("Bus Voltage is out of range!!\n");
        printf("Please Reset the Target!\n");
        while(1);
    }
}
