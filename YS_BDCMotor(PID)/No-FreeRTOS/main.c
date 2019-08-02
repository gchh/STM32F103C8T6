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
  * @file   main
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
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
        }
//    #undef FB_USE_GRAPHIC 
#ifdef FB_USE_GRAPHIC 
        FB_Speed =ROUND_TO_INT32(Spd_RPM);
        Transmit_FB( (int32_t *)&FB_Speed);
#else
        //printf("Spd:%d Pulse -- Spd: %.2f r/m \n",Spd_PPS,Spd_RPM);
#endif        
    }

    
    /* 数据反馈周期是50ms,由于电流采集周期大约是 2ms,所以数据反馈周期最好不要低于2ms */
    if ((uwTick % 50) == 0)
    {
        ADC_Resul = AverSum/AverCnt;
        /* 连续采样16次以后,以第17次作为校准偏差值 */
        OffsetCnt_Flag++;
        if(OffsetCnt_Flag >= 16)
        {
            if(OffsetCnt_Flag == 16)
            {
                OffSetHex = ADC_Resul;
            }
            OffsetCnt_Flag = 32;
            ADC_Resul -= OffSetHex;     // 减去偏差值
        }
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
        
        printf("Bus Voltage is out of range!!\n");
        printf("Please Reset the Target!\n");
        while(1);
    }
}
