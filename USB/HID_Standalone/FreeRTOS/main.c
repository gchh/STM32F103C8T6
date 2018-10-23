#include "stm32f1xx_hal.h"
#include "led.h"
#include "mco.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_hid.h"
#include "FreeRTOS.h"
#include "task.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define CURSOR_STEP     5
USBD_HandleTypeDef USBD_Device;
uint8_t HID_Buffer[4];

#define LED_STACK_SIZE  10
TaskHandle_t LedTaskHandle = NULL;

#define USB_HID_STACK_SIZE  10
TaskHandle_t UsbHidTaskHandle = NULL;


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void Error_Handler(void);
static void GetPointerData(uint8_t *pbuf);
void led_Serve(void* pvParam);
void Usb_HID_Serve(void* pvParam);

/* Private functions ---------------------------------------------------------*/
/*
    //Enable USB Wake-up interrupt
    HAL_NVIC_SetPriority(USBWakeUp_IRQn, 0, 0);
    //Set USB Interrupt priority
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 5, 0);
    TICK_INT_PRIORITY的优先级不能低于USB_LP_CAN1_RX0_IRQn，即其值要小于5，程序才正确执行。
*/
//#define  TICK_INT_PRIORITY            0x00U//0x0FU /*!< tick interrupt priority */

/* Make PendSV and SysTick the lowest priority interrupts. */
//	portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
//	portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;

//要注意上面的中断优先级问题！
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    /* This sample code shows how to use GPIO HAL API to toggle LED2 IO
    in an infinite loop. */

    /* STM32F103xB HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
    */
    HAL_Init();

    /* Configure the system clock to 64 MHz */
    SystemClock_Config();
    
    LED_GPIO_Config();
    
    MCO_config(RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_1);
    
    SystemCoreClock=HAL_RCC_GetSysClockFreq();
    
    /* Init Device Library */
    USBD_Init(&USBD_Device, &HID_Desc, 0);

    /* Register the HID class */
    USBD_RegisterClass(&USBD_Device, USBD_HID_CLASS);

    /* Start Device Process */
    USBD_Start(&USBD_Device);
    

//    xTaskCreate( led_Serve, 
//                 (char const*)"led", 
//                 LED_STACK_SIZE, 
//                 NULL,
//                 0, 
//                 &LedTaskHandle);  

    xTaskCreate( Usb_HID_Serve, 
                 (char const*)"USB HID", 
                 USB_HID_STACK_SIZE, 
                 NULL,
                 0, 
                 &UsbHidTaskHandle);  
                 
	//启动调度器
	vTaskStartScheduler();  
                 
    while(1){}
}

void led_Serve(void* pvParam)
{
    while(1)
    {
        LED1Toggle();
        /* Insert delay 100 ms */
        vTaskDelay(100);
    }
}

void Usb_HID_Serve(void* pvParam)
{
    while(1)
    {
        LED1Toggle();
        /* Insert delay 100 ms */
        vTaskDelay(100);//HAL_Delay(100);
        GetPointerData(HID_Buffer);
        USBD_HID_SendReport(&USBD_Device, HID_Buffer, 4);
    }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 1
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  RCC_PeriphCLKInitTypeDef rccperiphclkinit = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
    
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Start Conversation Error */
    Error_Handler(); 
  }
  
  /* USB clock selection */
  rccperiphclkinit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  rccperiphclkinit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  HAL_RCCEx_PeriphCLKConfig(&rccperiphclkinit);
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
  clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Start Conversation Error */
    Error_Handler(); 
  }
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  LED1ON();
  while (1)
  {
  }
}

/**
  * @brief  Gets Pointer Data.
  * @param  pbuf: Pointer to report
  * @retval None
  */
static void GetPointerData(uint8_t *pbuf)
{
  static int8_t cnt = 0;
  int8_t  x = 0, y = 0 ;
  
  if(cnt++ > 0)
  {
    x = CURSOR_STEP;
    y = CURSOR_STEP;
  }
  else
  {
    x = -CURSOR_STEP;
    y = -CURSOR_STEP;
  }
  
  pbuf[0] = 0;
  pbuf[1] = x;
  pbuf[2] = y;
  pbuf[3] = 0;
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
