#include "main.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
#define ENCODER     11    // ����������
#define SPEEDRATIO  30    // ������ٱ�
#define PPR         (SPEEDRATIO*ENCODER*4) // Pulse/r ÿȦ�ɲ����������
/* ˽�б��� ------------------------------------------------------------------*/
__IO uint8_t  start_flag=0;
__IO uint16_t time_count=0;        // ʱ�������ÿ1ms����һ(��ζ�ʱ��Ƶ���й�)
__IO int32_t CaptureNumber=0;      // ���벶����
uint8_t timer_1ms;

__IO float oled_volt = 0, oled_speed = 0;;

void Key_process(void);

#ifdef UART_CONTROL
typedef struct {
  __IO uint8_t IS_Enable ;  	// �����ת״̬
  __IO int16_t Duty_Cycles ;    // ���ռ�ձ�
}MotorParam_TypeDef;
MotorParam_TypeDef DCMotor_Param;

/**
  * ��������: ��ȡ���ת������Ҫ�Ĳ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void Get_Para()
{
    __IO char ch = 0;
    __IO int Num = 0;

    printf("/*------ ֱ����ˢ������ڿ������� -----*/\r\n");
    printf("/*------ ������ת������:(�Իس�������)*/\r\n");
    if(DCMotor_Param.IS_Enable==0)
    {
        //printf("      Y --> ����      N --> ֹͣ    \n");
        printf("      Y --> ����     \r\n");
        scanf("%c",&ch);
        printf("Your input is %c\r\n",ch);
        /* ����ת�� */
        if((ch=='y')||(ch=='Y'))
        {
            DCMotor_Param.IS_Enable = 1;
            printf("Y--> ����ת�� \r\n");
        }
        //else if((ch=='n')||(ch=='N'))
        //{
        //    DCMotor_Param.IS_Enable = 0;
        //    printf("N--> ֹͣת�� \r\n");
        //    return ;
        //}
        else return;
    }
    else if(DCMotor_Param.IS_Enable==1)
    {
        printf("      N --> ֹͣ    \r\n");
        scanf("%c",&ch);
        printf("Your input is %c\r\n",ch);
        /* ֹͣת�� */
        if((ch=='n')||(ch=='N'))
        {
            DCMotor_Param.IS_Enable = 0;
            printf("N--> ֹͣת�� \r\n");
            return ;
        }
        else return;
    }
    
    /* ����ռ�ձ� */
    printf("\n------�������ռ�ձ� x(-400~400),��������ת������,���Իس������� \r\n");
    scanf("%d",&Num);
    printf("Your input is %d\r\n",Num);
    while((Num > (BDCMOTOR_TIM_PERIOD+1) || (Num <- (BDCMOTOR_TIM_PERIOD+1) )))
    {
        printf("\r\n --�ٶ��������,���������( -400~400 )\r\n");
        scanf("%d",&Num);
    }
    DCMotor_Param.Duty_Cycles = Num;
    printf(" ռ�ձ�����Ϊ:%d/400 \r\n",DCMotor_Param.Duty_Cycles);

    /* �������һ�� */
    if(DCMotor_Param.IS_Enable)
    {
        printf(" ���״̬:����ת�� ------ ���ռ�ձ�:%.2f%% \r\n\n",
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
#ifdef UART_CONTROL    
    USART_GPIO_Init();
    USART_Config();
#endif

    /* ��������ʼ����ʹ�ܱ�����ģʽ */
    ENCODER_GPIO_Init();
    ENCODER_TIMx_Init();
  
    /* �߼����ƶ�ʱ����ʼ��������PWM������� */
    BDCMOTOR_GPIO_Init();
    BDCMOTOR_TIMx_Init();

    /* ADC-DMA ��ʼ�� */
    ADC_CUR_GPIO_Init();
    ADC_CUR_Init();
    ADC_DMA_Init();
    /* ����ADת����ʹ��DMA������ж� */
    ADC_Start_DMA();
  
#ifdef UART_CONTROL     
    //printf("*************************************************************\r\n");
    //printf("*                                                           *\r\n");
    //printf("* Thank you for using The Development Board Of BBDD ! ^_^ *\r\n");
    //printf("*                                                           *\r\n");
    //printf("*************************************************************\r\n");
    printf("^^^^--��ʹ��Ӳʯ��λ���շ���������--^^^^\r\n");
#endif
    while(1)
    {	
        Clear_Screen(0);
        Display_String(0, 0, "Current:", Red);
        Display_Num(64,0,(int32_t)(ADC_CurrentValue+10), 4, Green);
        Display_String(96, 0, "mA", Blue);
        Clear_Screen(1);
        Display_String(0, 16, "Volt:", Red);
        Display_Num(64,16,(int32_t)(oled_volt), 4, Green);
        Display_String(96, 16, "mV", Blue);  
        Clear_Screen(2);
        Display_String(0, 32, "Speed:", Red);
        if(oled_speed>=0) Display_Num(64,32,(int32_t)(oled_speed), 4, Green);
        else  Display_Num(64,32,(int32_t)(0-oled_speed), 4, Green);
        Display_String(96, 32, "r/s", Blue);          
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
            /* ���÷�����ٶ� */
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
            /* �ر�ͨ����� */
            DCMotor_Param.Duty_Cycles = 0;
            SetMotorSpeed(DCMotor_Param.Duty_Cycles);  
            SetMotorStop();
        }
#else        
        KEY_Scan();
        if(key_id_done==0)Key_process();
        LED1Toggle();
        while(timer_1ms<TIME_BASE_MAIN){};    //�����ܵ�������ֱ�ӷ��ش�ӡ}		//20ms
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
  * ��������: ϵͳ�δ�ʱ���жϻص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ÿ����һ�εδ�ʱ���жϽ���ûص�����һ��
  */
extern __IO uint32_t uwTick;
void SYSTICK_Callback(void)
{
    __IO float Volt_Result = 0;
    
    if(start_flag) // �ȴ����������ſ�ʼ��ʱ
    {
        time_count++;         // ÿ1ms�Զ���һ
        if(time_count==1000)  // 1s
        {
            float Speed = 0;
            CaptureNumber = ( int32_t )TIM_GetCounter(ENCODER_TIMx)+OverflowCount*65536;
            printf("Inputs:%d \r\n",CaptureNumber);
            // 4 : ʹ�ö�ʱ���������ӿڲ���AB��������غ��½��أ�һ������*4.
            // 11������������(ת��һȦ���������)
            // 270����������ȣ��ڲ����ת��Ȧ�����������ת��Ȧ���ȣ������ٳ��ֱ�
            Speed = (float)CaptureNumber/PPR;
            oled_speed = Speed;
            printf("���ʵ��ת���ٶ�%0.2f r/s \r\n",Speed);
     
            if(Speed==0)start_flag = 0;
            OverflowCount = 0;
            TIM_SetCounter(ENCODER_TIMx, 0);
            time_count=0;
        }
    }
    
    /* ���ݷ���������50ms,���ڵ����ɼ����ڴ�Լ�� 2ms,�������ݷ���������ò�Ҫ����2ms */
    if((uwTick % 50) == 0)
    {
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
        oled_volt = Volt_Result;
        ADC_CurrentValue = (float)( (Volt_Result / GAIN) / SAMPLING_RES);
        /* ֱ��ʹ�ô������ִ�ӡ������ѹֵ */
        //printf("Volt: %.1f mV -- Curr: %d mA\n",Volt_Result,(int32_t)(ADC_CurrentValue+10));  // +10 ����Ϊ������ĵ�����Լ��10mA
    }    
}



