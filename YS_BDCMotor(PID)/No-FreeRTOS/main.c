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
  * ��������: ϵͳʱ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
#ifdef SYSCLk_72MHz
void SystemClock_Config(void)
{
    RCC_ClocksTypeDef RCC_CLOCK;
    
    if(RCC_GetSYSCLKSource()==0x04 || RCC_GetSYSCLKSource()==0x08) //HSE used as system clock / PLL used as system clock
    {
        //�л�ΪHSI��Ϊϵͳʱ��
        if((RCC_GetFlagStatus(RCC_FLAG_HSIRDY)==RESET)) //��HSI
        {
            RCC_HSICmd(ENABLE);
            while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY)==SET);
        }
        /*------------------------- SYSCLK Configuration ---------------------------*/ 
        RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI); //HSI selected as system clock; SYSCLK = HSI      
        /*-------------------------- HCLK Configuration --------------------------*/
        RCC_HCLKConfig(RCC_SYSCLK_Div1); //AHB clock (HCLK) = SYSCLK;
        while(RCC_GetSYSCLKSource()!=0x00); //�ȴ�ϵͳʱ���л�ΪHSI
        /*-------------------------- PCLK1 Configuration ---------------------------*/ 
        RCC_PCLK1Config(RCC_HCLK_Div1); //APB1 clock (PCLK1) = HCLK; 
        /*-------------------------- PCLK2 Configuration ---------------------------*/ 
        RCC_PCLK2Config(RCC_HCLK_Div1); //APB2 clock (PCLK2) = HCLK;      
    }
    if(RCC_GetSYSCLKSource()==0x00) //HSI used as system clock
    {
        if((RCC_GetFlagStatus(RCC_FLAG_HSERDY)==SET)) //���HSE�Ѵ򿪣��ȹر�
        {
            RCC_HSEConfig(RCC_HSE_OFF);
            while(RCC_GetFlagStatus(RCC_FLAG_HSERDY)==SET);
        }
        RCC_HSEConfig(RCC_HSE_ON); //���ⲿ����8MHz
        while(RCC_GetFlagStatus(RCC_FLAG_HSERDY)==RESET); //�ȴ��ⲿʱ���ȶ�
        
        
        if((RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==SET)) //���PLL�Ѵ򿪣��ȹر�
        {
            RCC_PLLCmd(DISABLE);
            while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==SET);
        }        
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); //HSE/1��ΪPLL������ʱ��Դ��PLL 9��Ƶ�����PLLCLK = 8MHz * 9 = 72MHz
        RCC_PLLCmd(ENABLE); //��PLL
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET); //�ȴ�PLL�ȶ� 

        
        /*FLASH��ȡ�ȴ�ʱ���SYSCLK�Ĺ�ϵ��
          0�ȴ����ڣ��� 0 < SYSCLK �� 24MHz 
          1�ȴ����ڣ��� 24MHz < SYSCLK �� 48MHz 
          2�ȴ����ڣ��� 48MHz < SYSCLK �� 72MHz      
        ************************************/
        /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
        FLASH_SetLatency(FLASH_Latency_2);
        /* Check that the new number of wait states is taken into account to access the Flash memory by reading the FLASH_ACR register */
        while((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_Latency_2);
        
        /*------------------------- SYSCLK Configuration ---------------------------*/ 
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //PLL selected as system clock; SYSCLK = PLLCLK
        while(RCC_GetSYSCLKSource()!=0x08); //�ȴ�ϵͳʱ���л�ΪPLL        
        /*-------------------------- HCLK Configuration --------------------------*/
        RCC_HCLKConfig(RCC_SYSCLK_Div1); //AHB clock (HCLK) = SYSCLK; HCLK���72MHz
        /*-------------------------- PCLK1 Configuration ---------------------------*/ 
        RCC_PCLK1Config(RCC_HCLK_Div2); //APB1 clock (PCLK1) = HCLK/2; PCLK1���36MHzs
        /*-------------------------- PCLK2 Configuration ---------------------------*/ 
        RCC_PCLK2Config(RCC_HCLK_Div1); //APB2 clock (PCLK2) = HCLK; PCLK2���72MHz        
    }
    
    RCC_GetClocksFreq(&RCC_CLOCK);
    SystemCoreClock = RCC_CLOCK.SYSCLK_Frequency;
}
#endif
/**
  * @file   main
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
#ifdef SYSCLk_72MHz    
    SystemClock_Config(); //����ϵͳʱ��72MHz������8MHz
#endif
    
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
                sPID.SumError = 0;
#ifdef SPD_CUR_PID
                cPID.LastError = 0;
                cPID.PrevError = 0;
                cPID.SumError = 0;                
#endif                
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
#ifdef SPD_CUR_PID
__IO int32_t tmpPWM_Duty = 0;
#endif
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
#ifdef SPD_PID          
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
#endif
#ifdef SPD_CUR_PID
            tmpPWM_Duty = SpdPIDCalc(Spd_RPM);
      
            /* �����ٶȻ��ļ������жϵ�ǰ�˶����� */
            if(tmpPWM_Duty < 0)
            {
                SetMotorDir(0);
                tmpPWM_Duty = -tmpPWM_Duty;
            }
            else
            {
                SetMotorDir(1);
            }
            /* �趨��������Ŀ��ֵ,����û�и��� */
            if(tmpPWM_Duty >= TARGET_CURRENT)
                tmpPWM_Duty = TARGET_CURRENT;            
#endif
        }
//    #undef FB_USE_GRAPHIC 
#ifdef FB_USE_GRAPHIC 
        FB_Speed =ROUND_TO_INT32(Spd_RPM);
        Transmit_FB( (int32_t *)&FB_Speed);
#else
        //printf("Spd:%d Pulse -- Spd: %.2f r/m \n",Spd_PPS,Spd_RPM);
#endif        
    }

#ifdef SPD_CUR_PID
    /* ������������40ms,�������βɼ����ڴ�Լ�� 2ms,��ò�Ҫ����2ms */
    if(uwTick % 40 == 0)
#else      
    /* ���ݷ���������50ms,���ڵ����ɼ����ڴ�Լ�� 2ms,�������ݷ���������ò�Ҫ����2ms */
    if ((uwTick % 50) == 0)
#endif
    {
        ADC_Resul = AverSum/AverCnt;
        
        OffsetCnt_Flag++;
#if 0
        /* ��������16���Ժ�,�Ե�17����ΪУ׼ƫ��ֵ */    
        if(OffsetCnt_Flag >= 16)
        {
            if(OffsetCnt_Flag == 16)
            {
                OffSetHex = ADC_Resul;
            }
            OffsetCnt_Flag = 32;
            ADC_Resul -= OffSetHex;     // ��ȥƫ��ֵ
        }
#else
        /* ��������16���Ժ�,��ǰ15�ε�ƽ������ֵ��ΪУ׼ƫ��ֵ */
        if(OffsetCnt_Flag >= 16)
        {
            if(OffsetCnt_Flag == 16)
            {
                OffSetHex /= (OffsetCnt_Flag-1);
            }
            OffsetCnt_Flag = 32;
            ADC_Resul -= OffSetHex;//��ȥƫ��ֵ
        }
        else OffSetHex += ADC_Resul;        
#endif        
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
#ifdef FB_USE_GRAPHIC 
                    Clear_Screen(0);
                    Display_String(0, 0, "Over Current!", Red);

                    Clear_Screen(1);
                    Display_String(0, 16, "Please reset!", Red);             
#else                    
                    printf("Over Current %.2f \n",ADC_CurrentValue);
                    printf("Please reset the target!!\n");
#endif                    
                    SHUTDOWN_ON;
                    SetMotorStop();
                    PWM_Duty = 0;
                    OverCurCount = 0;
                    while(1);
                }
            }
            else OverCurCount = 0;
        }

#ifdef SPD_CUR_PID
        /* ����PID��� */
        if(start_flag == 1)
        {  
            cPID.SetPoint = tmpPWM_Duty ;
            PWM_Duty = CurPIDCalc( (int32_t)ADC_CurrentValue);
            if(PWM_Duty >= BDCMOTOR_DUTY_FULL)
                PWM_Duty = BDCMOTOR_DUTY_FULL;
            if(PWM_Duty <=0)
                PWM_Duty = 0;   
            SetMotorSpeed(PWM_Duty);
        }        
#endif        
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
#ifdef FB_USE_GRAPHIC 
        Clear_Screen(0);
        Display_String(0, 0, "Bus Voltage is", Red);

        Clear_Screen(1);
        Display_String(0, 16, "out of range!!", Red);        

        Clear_Screen(2);
        Display_String(0, 32, "Please reset!", Red);        
#else         
        printf("Bus Voltage is out of range!!\n");
        printf("Please Reset the Target!\n");
#endif      
        while(1);
    }
}
