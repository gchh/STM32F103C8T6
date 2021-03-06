#include "main.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
#define ENCODER     11    // 编码器线数
#define SPEEDRATIO  30    // 电机减速比
#define PPR         (SPEEDRATIO*ENCODER*4) // Pulse/r 每圈可捕获的脉冲数
/* 私有变量 ------------------------------------------------------------------*/
__IO uint8_t  start_flag=0;
__IO uint16_t time_count=0;        // 时间计数，每1ms增加一(与滴定时器频率有关)
__IO int32_t CaptureNumber=0;      // 输入捕获数
uint8_t timer_1ms;

void Key_process(void);

#ifdef UART_CONTROL
typedef struct {
  __IO uint8_t IS_Enable ;  	// 电机旋转状态
  __IO int16_t Duty_Cycles ;    // 电机占空比
}MotorParam_TypeDef;
MotorParam_TypeDef DCMotor_Param;

/**
  * 函数功能: 获取电机转动所需要的参数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void Get_Para()
{
    __IO char ch = 0;
    __IO int Num = 0;

    printf("/*------ 直流有刷电机串口控制例程 -----*/\r\n");
    printf("/*------ 输入电机转动参数:(以回车键结束)*/\r\n");
    if(DCMotor_Param.IS_Enable==0)
    {
        //printf("      Y --> 启动      N --> 停止    \n");
        printf("      Y --> 启动     \r\n");
        scanf("%c",&ch);
        printf("Your input is %c\r\n",ch);
        /* 启动转动 */
        if((ch=='y')||(ch=='Y'))
        {
            DCMotor_Param.IS_Enable = 1;
            printf("Y--> 启动转动 \r\n");
        }
        //else if((ch=='n')||(ch=='N'))
        //{
        //    DCMotor_Param.IS_Enable = 0;
        //    printf("N--> 停止转动 \r\n");
        //    return ;
        //}
        else return;
    }
    else if(DCMotor_Param.IS_Enable==1)
    {
        printf("      N --> 停止    \r\n");
        scanf("%c",&ch);
        printf("Your input is %c\r\n",ch);
        /* 停止转动 */
        if((ch=='n')||(ch=='N'))
        {
            DCMotor_Param.IS_Enable = 0;
            printf("N--> 停止转动 \r\n");
            return ;
        }
        else return;
    }
    
    /* 设置占空比 */
    printf("\n------设置输出占空比 x(-400~400),正负代表转动方向,并以回车键结束 \r\n");
    scanf("%d",&Num);
    printf("Your input is %d\r\n",Num);
    while((Num > (BDCMOTOR_TIM_PERIOD+1) || (Num <- (BDCMOTOR_TIM_PERIOD+1) )))
    {
        printf("\r\n --速度输入错误,请从新输入( -400~400 )\r\n");
        scanf("%d",&Num);
    }
    DCMotor_Param.Duty_Cycles = Num;
    printf(" 占空比设置为:%d/400 \r\n",DCMotor_Param.Duty_Cycles);

    /* 电机参数一览 */
    if(DCMotor_Param.IS_Enable)
    {
        printf(" 电机状态:启动转动 ------ 输出占空比:%.2f%% \r\n\n",
                (float)DCMotor_Param.Duty_Cycles/(float)(BDCMOTOR_TIM_PERIOD+1)*100.0f);
    }
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
    //__IO char ch = 0,buff[100];
    //float ft;
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
#ifdef UART_CONTROL    
    USART_GPIO_Init();
    USART_Config();
#endif

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
  
#ifdef UART_CONTROL     
    //printf("*************************************************************\r\n");
    //printf("*                                                           *\r\n");
    //printf("* Thank you for using The Development Board Of BBDD ! ^_^ *\r\n");
    //printf("*                                                           *\r\n");
    //printf("*************************************************************\r\n");
    printf("^^^^--请使用硬石上位机收发串口数据--^^^^\r\n");
#endif
    while(1)
    {	    
#ifdef UART_CONTROL 
        //printf("/*------ please input one char:(end with Enter)*/\r\n");
        //scanf("%c",&ch); 
        //printf("Your input is %c\r\n",ch);
        //scanf("%s",buff); 
        //printf("Your input is %s\r\n",buff);
        //scanf("%d",&ch); 
        //printf("Your input is %d\r\n",ch);
        //scanf("%f",&ft); 
        //printf("Your input is %f\r\n",ft);
        Get_Para();
        if(DCMotor_Param.IS_Enable)
        {
            /* 设置方向和速度 */
            if(DCMotor_Param.Duty_Cycles>0)
            {
                SetMotorDir(1);
            }
            else
            {
                SetMotorDir(0);
                DCMotor_Param.Duty_Cycles = -DCMotor_Param.Duty_Cycles;
            }
            SetMotorSpeed(DCMotor_Param.Duty_Cycles);
            start_flag = 1;
        }
        else
        {
            /* 关闭通道输出 */
            DCMotor_Param.Duty_Cycles = 0;
            SetMotorSpeed(DCMotor_Param.Duty_Cycles);  
            SetMotorStop();
        }
#else        
        KEY_Scan();
        if(key_id_done==0)Key_process();
        LED1Toggle();
        while(timer_1ms<TIME_BASE_MAIN){};    //将接受到的数据直接返回打印}		//20ms
	    timer_1ms=0;
#endif
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
                if(PWM_Duty < BDCMOTOR_DUTY_FULL) PWM_Duty+=5;
                if(PWM_Duty > BDCMOTOR_DUTY_FULL) PWM_Duty = BDCMOTOR_DUTY_FULL;
                SetMotorSpeed(PWM_Duty);
            }
            break;
        case KEY_SPEED_DOWN:
            key_id_done=1;
            if(BDCMOTOR_state == BDCMOTOR_RUN)
            {
                //if(PWM_Duty >= BDCMOTOR_DUTY_ZERO+5) PWM_Duty-=5;
                //else PWM_Duty = BDCMOTOR_DUTY_ZERO;
                if(PWM_Duty > BDCMOTOR_DUTY_ZERO) PWM_Duty-=5;
                if(PWM_Duty < BDCMOTOR_DUTY_ZERO) PWM_Duty = BDCMOTOR_DUTY_ZERO;
                SetMotorSpeed(PWM_Duty);        
            }
            break;    
        case KEY_TURN_Clockwise:
            key_id_done=1;
            if(BDCMOTOR_state == BDCMOTOR_IDLE) SetMotorDir(0);
            break;   
        case KEY_TURN_Anticlockwise:
            key_id_done=1;
            if(BDCMOTOR_state == BDCMOTOR_IDLE) SetMotorDir(1);
            break;   
        case KEY_SHUTDOWN:
            key_id_done=1;
            if(BDCMOTOR_state == BDCMOTOR_RUN) 
            {
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
    unsigned char str[10];
    __IO float Volt_Result = 0;
    __IO float Volt_Bus = 0;
    
    if(start_flag) // 等待脉冲输出后才开始计时
    {     
        time_count++;         // 每1ms自动增一
        if(time_count==1000)  // 1s
        {
            float Speed = 0;
            CaptureNumber = ( int32_t )TIM_GetCounter(ENCODER_TIMx)+OverflowCount*65536;
            printf("Inputs:%d \r\n",CaptureNumber);
            // 4 : 使用定时器编码器接口捕获AB相的上升沿和下降沿，一个脉冲*4.
            // 11：编码器线数(转速一圈输出脉冲数)
            // 270：电机减数比，内部电机转动圈数与电机输出轴转动圈数比，即减速齿轮比
            Speed = (float)CaptureNumber/PPR;
            printf("电机实际转动速度%0.2f r/s \r\n",Speed);

            Clear_Screen(3);
            Display_String(0, 48, "Speed:", Red);
            sprintf((char *)str, "%0.2f", Speed);
            Display_String(64, 48, str, Green);//
            Display_String(104, 48, "r/s", Blue);
        
            if(Speed==0)start_flag = 0;
            OverflowCount = 0;
            TIM_SetCounter(ENCODER_TIMx, 0);
            time_count=0;
        }
    }
    
    /* 数据反馈周期是50ms,由于电流采集周期大约是 2ms,所以数据反馈周期最好不要低于2ms */
    if ((uwTick % 50) == 0)//((uwTick % 50) == 0&&OffsetCnt_Flag<32 || (uwTick % 500) == 0&&OffsetCnt_Flag>=32)//
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
        //if(Volt_Result<0) Volt_Result = 0;        
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
        
        /* 直接使用串口助手打印电流电压值 */
        if ((uwTick % 1000) == 0)
        {
        ADC_VoltBus = (float)ADC_VoltBus * VOLTBUS_RESOLUTION;
        Volt_Bus = ADC_VoltBus;
        printf("BUSVolt: %.2fV\n", ADC_VoltBus);            
        printf("Volt: %.1f mV -- Curr: %d mA\n",Volt_Result,(int32_t)(ADC_CurrentValue+10));  // +10 是因为驱动板的电流大约是10mA

        Clear_Screen(0);
        Display_String(0, 0, "Current:", Red);
        sprintf((char *)str, "%d", (int32_t)(ADC_CurrentValue+10));
        Display_String(64, 0, str, Green);
        Display_String(112, 0, "mA", Blue);
            
        Clear_Screen(1);
        Display_String(0, 16, "Volt:", Red);
        sprintf((char *)str, "%.1f", Volt_Result);
        Display_String(64, 16, str, Green);
        Display_String(112, 16, "mV", Blue); 
            
        Clear_Screen(2);
        Display_String(0, 32, "BusVolt:", Red);
        sprintf((char *)str, "%.2f", Volt_Bus);
        Display_String(64, 32, str, Green);
        Display_String(120, 32, "V", Blue);             
        }            
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
