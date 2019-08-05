#include "main.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
#define ENCODER     11    // 编码器线数
#define SPEEDRATIO  30    // 电机减速比
#define PPR         (SPEEDRATIO*ENCODER*4) // Pulse/r 每圈可捕获的脉冲数
/* 私有变量 ------------------------------------------------------------------*/
__IO uint8_t  start_flag=0;        // PID 开始标志
__IO int32_t LastSpd_Pulse= 0;      // 编码器捕获值 Pulse

uint8_t timer_1ms;

void Key_process(void);

/**
  * 函数功能: 系统时钟配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
#ifdef SYSCLk_72MHz
void SystemClock_Config(void)
{
    RCC_ClocksTypeDef RCC_CLOCK;
    
    if(RCC_GetSYSCLKSource()==0x04 || RCC_GetSYSCLKSource()==0x08) //HSE used as system clock / PLL used as system clock
    {
        //切换为HSI作为系统时钟
        if((RCC_GetFlagStatus(RCC_FLAG_HSIRDY)==RESET)) //打开HSI
        {
            RCC_HSICmd(ENABLE);
            while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY)==SET);
        }
        /*------------------------- SYSCLK Configuration ---------------------------*/ 
        RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI); //HSI selected as system clock; SYSCLK = HSI      
        /*-------------------------- HCLK Configuration --------------------------*/
        RCC_HCLKConfig(RCC_SYSCLK_Div1); //AHB clock (HCLK) = SYSCLK;
        while(RCC_GetSYSCLKSource()!=0x00); //等待系统时钟切换为HSI
        /*-------------------------- PCLK1 Configuration ---------------------------*/ 
        RCC_PCLK1Config(RCC_HCLK_Div1); //APB1 clock (PCLK1) = HCLK; 
        /*-------------------------- PCLK2 Configuration ---------------------------*/ 
        RCC_PCLK2Config(RCC_HCLK_Div1); //APB2 clock (PCLK2) = HCLK;      
    }
    if(RCC_GetSYSCLKSource()==0x00) //HSI used as system clock
    {
        if((RCC_GetFlagStatus(RCC_FLAG_HSERDY)==SET)) //如果HSE已打开，先关闭
        {
            RCC_HSEConfig(RCC_HSE_OFF);
            while(RCC_GetFlagStatus(RCC_FLAG_HSERDY)==SET);
        }
        RCC_HSEConfig(RCC_HSE_ON); //打开外部晶振，8MHz
        while(RCC_GetFlagStatus(RCC_FLAG_HSERDY)==RESET); //等待外部时钟稳定
        
        
        if((RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==SET)) //如果PLL已打开，先关闭
        {
            RCC_PLLCmd(DISABLE);
            while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==SET);
        }        
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); //HSE/1作为PLL的输入时钟源；PLL 9倍频输出：PLLCLK = 8MHz * 9 = 72MHz
        RCC_PLLCmd(ENABLE); //打开PLL
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET); //等待PLL稳定 

        
        /*FLASH读取等待时间和SYSCLK的关系：
          0等待周期，当 0 < SYSCLK ≤ 24MHz 
          1等待周期，当 24MHz < SYSCLK ≤ 48MHz 
          2等待周期，当 48MHz < SYSCLK ≤ 72MHz      
        ************************************/
        /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
        FLASH_SetLatency(FLASH_Latency_2);
        /* Check that the new number of wait states is taken into account to access the Flash memory by reading the FLASH_ACR register */
        while((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_Latency_2);
        
        /*------------------------- SYSCLK Configuration ---------------------------*/ 
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //PLL selected as system clock; SYSCLK = PLLCLK
        while(RCC_GetSYSCLKSource()!=0x08); //等待系统时钟切换为PLL        
        /*-------------------------- HCLK Configuration --------------------------*/
        RCC_HCLKConfig(RCC_SYSCLK_Div1); //AHB clock (HCLK) = SYSCLK; HCLK最大72MHz
        /*-------------------------- PCLK1 Configuration ---------------------------*/ 
        RCC_PCLK1Config(RCC_HCLK_Div2); //APB1 clock (PCLK1) = HCLK/2; PCLK1最大36MHzs
        /*-------------------------- PCLK2 Configuration ---------------------------*/ 
        RCC_PCLK2Config(RCC_HCLK_Div1); //APB2 clock (PCLK2) = HCLK; PCLK2最大72MHz        
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
    SystemClock_Config(); //设置系统时钟72MHz，否则8MHz
#endif
    
    //char c[15];
	OLED_Init();	//OLED初始化
	Fill_RAM(0x0000);	//清屏

	/*Configure the SysTick to have interrupt in 1ms time basis*/
	//in the file system_stm3210x.c, configure the SystemCoreClock
        /* #define SYSCLK_FREQ_HSE    HSE_VALUE */
        /* #define SYSCLK_FREQ_24MHz  24000000 */
        /* #define SYSCLK_FREQ_36MHz  36000000 */
        /* #define SYSCLK_FREQ_48MHz  48000000 */
        /* #define SYSCLK_FREQ_56MHz  56000000 */
        /* #define SYSCLK_FREQ_72MHz  72000000 */
    SysTick_Config(SystemCoreClock/1000);
    
    /*初始化LED端口*/
    LED_GPIO_Config();
    
    KEY_GPIO_Init();
    
    /* 串口初始化 */
    USART_GPIO_Init();
    USART_Config();

    /* 编码器初始化及使能编码器模式 */
    ENCODER_GPIO_Init();
    ENCODER_TIMx_Init();
  
    /* 高级控制定时器初始化并配置PWM输出功能 */
    BDCMOTOR_GPIO_Init();
    BDCMOTOR_TIMx_Init();

    /* ADC-DMA 初始化 */
    ADC_CUR_GPIO_Init();
    ADC_VOLT_GPIO_Init();
    ADC_CUR_Init();
    ADC_VOLT_Init();
    ADC_DMA_Init();
    ADC_Calibration();
    /* 启动AD转换并使能DMA传输和中断 */
    ADC_Start_DMA();

    /* PID 参数初始化 */
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
                if(sPID.SetPoint >= MAX_SPEED) // 满速 r/m
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
  * 函数功能: 系统滴答定时器中断回调函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 每发生一次滴答定时器中断进入该回调函数一次
  */
extern __IO uint32_t uwTick;
static __IO uint32_t OverCurCount;          // 过流次数记录
#ifdef SPD_CUR_PID
__IO int32_t tmpPWM_Duty = 0;
#endif
void SYSTICK_Callback(void)
{
    __IO float Volt_Result = 0;
    __IO float Volt_Bus = 0;

    __IO int32_t Spd_Pulse = 0;       // 编码器捕获值 Pulse
    __IO int32_t Spd_PPS   = 0;       // 速度值 Pulse/Sample
    __IO float Spd_RPM     = 0;       // 速度值 r/m
    __IO int32_t FB_Speed  = 0;       // 用于反馈速度值到上位机 

    /* 速度环周期100ms */
    if(uwTick % 100 == 0)
    {
        Spd_Pulse = ( int32_t )TIM_GetCounter(ENCODER_TIMx)+OverflowCount*65536;
        Spd_PPS = Spd_Pulse - LastSpd_Pulse;
        LastSpd_Pulse = Spd_Pulse;
        
        /* 11线编码器,30减速比,一圈脉冲信号是11*30*4 */
        /* 放大10倍计算r/s，放大60则是计算r/m*/
        Spd_RPM = ((((float)Spd_PPS/(float)PPR)*10.0f)*(float)60);
        
        /* 计算PID结果 */
        if(start_flag == 1)
        {
#ifdef SPD_PID          
            PWM_Duty = SpdPIDCalc(Spd_RPM);

            /* 根据速度环的计算结果判断当前运动方向 */
            if(PWM_Duty < 0)
            {
                SetMotorDir(0);
                /* 限制占空比 */
                if(PWM_Duty < -BDCMOTOR_DUTY_FULL)
                    PWM_Duty = -BDCMOTOR_DUTY_FULL;
                /* 直接修改占空比 */
                SetMotorSpeed(-PWM_Duty);                
            }
            else
            {
                SetMotorDir(1);
                /* 限制占空比 */
                if(PWM_Duty > BDCMOTOR_DUTY_FULL)
                    PWM_Duty = BDCMOTOR_DUTY_FULL;
                /* 直接修改占空比 */
                SetMotorSpeed(PWM_Duty);        
            }
#endif
#ifdef SPD_CUR_PID
            tmpPWM_Duty = SpdPIDCalc(Spd_RPM);
      
            /* 根据速度环的计算结果判断当前运动方向 */
            if(tmpPWM_Duty < 0)
            {
                SetMotorDir(0);
                tmpPWM_Duty = -tmpPWM_Duty;
            }
            else
            {
                SetMotorDir(1);
            }
            /* 设定电流环的目标值,电流没有负数 */
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
    /* 电流环周期是40ms,电流单次采集周期大约是 2ms,最好不要低于2ms */
    if(uwTick % 40 == 0)
#else      
    /* 数据反馈周期是50ms,由于电流采集周期大约是 2ms,所以数据反馈周期最好不要低于2ms */
    if ((uwTick % 50) == 0)
#endif
    {
        ADC_Resul = AverSum/AverCnt;
        
        OffsetCnt_Flag++;
#if 0
        /* 连续采样16次以后,以第17次作为校准偏差值 */    
        if(OffsetCnt_Flag >= 16)
        {
            if(OffsetCnt_Flag == 16)
            {
                OffSetHex = ADC_Resul;
            }
            OffsetCnt_Flag = 32;
            ADC_Resul -= OffSetHex;     // 减去偏差值
        }
#else
        /* 连续采样16次以后,以前15次的平均采样值作为校准偏差值 */
        if(OffsetCnt_Flag >= 16)
        {
            if(OffsetCnt_Flag == 16)
            {
                OffSetHex /= (OffsetCnt_Flag-1);
            }
            OffsetCnt_Flag = 32;
            ADC_Resul -= OffSetHex;//减去偏差值
        }
        else OffSetHex += ADC_Resul;        
#endif        
        /* 计算电压值和电流值 */
        Volt_Result = ( (float)( (float)(ADC_Resul) * VOLT_RESOLUTION) );
        ADC_CurrentValue = (float)( (Volt_Result / GAIN) / SAMPLING_RES);
        if(Volt_Result<0) Volt_Result = 0;        
        /* 清空计数 */
        AverCnt = 0;
        AverSum = 0;        

        /* 过流保护 */
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
        /* 计算PID结果 */
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
  * 函数功能: 解析数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 将接收到的数据帧进行解析提取.
  */
PID_TypeDef  *ptr =  &sPID;
void Analyse_Data_Callback()
{
    __IO uint8_t i = 0 ;
    __IO uint8_t *Ptr = &RxBuf[FRAME_CHECK_BEGIN+1];
    __IO char *Str = &Msg.data[0].Ch[0];
    Msg.Code = RxBuf[FRAME_CHECK_BEGIN];         // 第二位是指令码
    /* 利用结构体和数组的内存连续性,使用指针提取数据 */
    for(i=0;i<(FRAME_CHECK_NUM-1);i++)
    {
        *Str++ = *Ptr++ ;
    }
    switch(Msg.Code)
    {
        /* 设置PID参数 */
        case CODE_SETPID:
            ptr->Proportion = Msg.data[0].Float; 
            ptr->Integral   = Msg.data[1].Float;
            ptr->Derivative = Msg.data[2].Float;
            break;
        /* 设置目标值 */
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
  * 函数功能: ADC看门狗中断回调函数
  * 输入参数: ADC句柄
  * 返 回 值: 无
  * 说    明: ADC窗口看门狗,检测到电压过低或者过高的时候就调用这个函数,停止输出.
  */
void ADC_LevelOutOfWindowCallback(void)
{
    /* 使能电机控制引脚 */
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
//        ADC_VoltBus = (float)ADC_VoltBus * VOLTBUS_RESOLUTION;// ADC_VoltBus是在中断响应函数中读取的adc值
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
