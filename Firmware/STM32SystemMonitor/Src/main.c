/**
  ******************************************************************************
  * @file    USB_Device/HID_Standalone/Src/main.c
  * @author  MCD Application Team
  * @version V1.0.3
  * @date    22-April-2016
  * @brief   USB device HID demo main file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>
#include "usbd_cdc_if.h"
#include "WM.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_ts.h"
#include "ringbuffer.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CURSOR_STEP     5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBD_HandleTypeDef USBD_Device;
uint8_t HID_Buffer[4];
RingBuffer UARTRxRBuffer;
UART_HandleTypeDef UartHandle;
TIM_HandleTypeDef    TimHandle;

/* Prescaler declaration */
uint32_t uwPrescalerValue = 0;
uint8_t RxFlag = 0;
float CPUTemperature=0.0;
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
static void CPU_CACHE_Enable(void);
//static void USBH_UserProcess(USBD_HandleTypeDef *phost, uint8_t id);
//static void CDC_InitApplication(void);
static void USART1_Init(void);
static void TIMER3_Init(void);
static void ToggleGPIO_Init(void);
void TranslateToCPUTemperature(RingBuffer  *inputRingBuff);
extern void CPUTempUpdate(struct systemInfo_s * Info);
extern GUI_CONST_STORAGE GUI_FONT GUI_FontSevenSegment13;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontSevenSegment30;
struct systemInfo_s SystemInfo;
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Enable the CPU Cache */
  CPU_CACHE_Enable();
  
  /* STM32F7xx HAL library initialization:
       - Configure the Flash ART accelerator on ITCM interface
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();
  
  /* Configure the System clock to have a frequency of 216 MHz */
  SystemClock_Config();
  //TIMER3_Init();  
  /* Configure LED1 */
  BSP_LED_Init(LED1);
    
  /* Configure Key button for remote wakeup */
  BSP_PB_Init(BUTTON_TAMPER, BUTTON_MODE_EXTI);
  
  /* Init Device Library */
  USART1_Init();
  RingBuffer_Init(&UARTRxRBuffer);
  WM_MULTIBUF_Enable(1);
  /* Init the STemWin GUI Library */
  BSP_SDRAM_Init(); /* Initializes the SDRAM device */
	__HAL_RCC_BKPSRAM_CLK_ENABLE();
  __HAL_RCC_CRC_CLK_ENABLE(); /* Enable the CRC Module */
	HAL_Delay(100);
	GUI_Init();
	WM_SetCreateFlags(WM_CF_MEMDEV);
	
		CreateSystemMonitor();
	//GUI_DispStringAt("Hello", 100,100);
  /* Run Application (Interrupt mode) */
  while (1)
  {
    GUI_Exec();
		GUI_Delay(100);
		if( RxFlag == 1 )
		{
				RxFlag = 0 ;
				TranslateToCPUTemperature(&UARTRxRBuffer);
			  CPUTempUpdate(&SystemInfo);
		}
  }
}



void TranslateToCPUTemperature(RingBuffer  *inputRingBuff)
{
	
	uint8_t inputString[512] = "";
	char resultString[10] = "";
	char * found;
	int resultCnt=0;
	int length = RingBuffer_GetDataLength(inputRingBuff);
	//Copy ring buffer into inputString
	RingBuffer_Read(inputRingBuff, &inputString[0], length );
	memset(SystemInfo.CPUTemp,0, sizeof(SystemInfo.CPUTemp));
	memset(SystemInfo.GPUTemp,0, sizeof(SystemInfo.GPUTemp));
	//Search "CPU Package :"
	found = strstr((const char*)&inputString[0],(const char *)"CPU Package:");
	if( found != NULL )
	{
		//skip up to ":" and copy up to ","
		found = strstr((const char*)found, (const char *)":");

		while(*(++found)!=',')
		{
			SystemInfo.CPUTemp[resultCnt++] = *found;
		}	
		SystemInfo.CPUTemp[resultCnt++] = ' '; SystemInfo.CPUTemp[resultCnt++] = 'd'; SystemInfo.CPUTemp[resultCnt++] = 'e'; 
		SystemInfo.CPUTemp[resultCnt++] = 'g'; SystemInfo.CPUTemp[resultCnt++] = 'C';
		//SystemInfo.CPUTemp = atof(resultString);
		//clear resultString for next usage
		memset(resultString, 0, sizeof(resultCnt));
	}
	
	//Search "GPU Core :"
	resultCnt = 0;
	found = strstr((const char*)&inputString[0],(const char *)"GPU Core:");
	if( found != NULL )
	{
    //skip up to ":" and copy up to "\n"
		found = strstr((const char*)found, (const char *)":");
		while(*(++found)!='\n')
		{
			SystemInfo.GPUTemp[resultCnt++] = *found;
		}	
		SystemInfo.GPUTemp[resultCnt++] = ' '; SystemInfo.GPUTemp[resultCnt++] = 'd'; SystemInfo.GPUTemp[resultCnt++] = 'e'; 
		SystemInfo.GPUTemp[resultCnt++] = 'g'; SystemInfo.GPUTemp[resultCnt++] = 'C';
		//SystemInfo.GPUTemp = atof(resultString);		
	}
}

static void ToggleGPIO_Init(void)
{
	  GPIO_InitTypeDef  gpio_init_structure;

	  __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    gpio_init_structure.Pin = GPIO_PIN_7;
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  
    HAL_GPIO_Init(GPIOC, &gpio_init_structure);
    
    /* By default, turn off LED */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
}

static void TIMER3_Init(void)
{
 /*##-1- Configure the TIM peripheral #######################################*/
  /* -----------------------------------------------------------------------
    In this example TIM3 input clock (TIM3CLK)  is set to APB1 clock (PCLK1) x2,
    since APB1 prescaler is equal to 4.
      TIM3CLK = PCLK1*2
      PCLK1 = HCLK/4
      => TIM3CLK = HCLK/2 = SystemCoreClock/2
    To get TIM3 counter clock at 10 KHz, the Prescaler is computed as follows:
    Prescaler = (TIM3CLK / TIM3 counter clock) - 1
    Prescaler = ((SystemCoreClock/2) /10 KHz) - 1

    Note:
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f7xx.c file.
     Each time the core clock (HCLK) changes, user had to update SystemCoreClock
     variable value. Otherwise, any configuration based on this variable will be incorrect.
     This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetSysClockFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
  ----------------------------------------------------------------------- */

  /* Compute the prescaler value to have TIMx counter clock equal to 10000000 Hz */
  uwPrescalerValue = (uint32_t)((SystemCoreClock / 2) / 10000000) - 1;

  /* Set TIMx instance */
  TimHandle.Instance = TIMx;

  /* Initialize TIMx peripheral as follows:
       + Period = 10000 - 1
       + Prescaler = ((SystemCoreClock / 2)/10000000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Init.Period            = 10000 - 1;
  TimHandle.Init.Prescaler         = uwPrescalerValue;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;

  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Start the TIM Base generation in interrupt mode ####################*/
  /* Start Channel1 */
  if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
  {
    /* Starting Error */
    Error_Handler();
  }	
}

static void USART1_Init(void)
{
	//static uint8_t tempBuffer[2] ;
	/*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance        = USARTx;

  UartHandle.Init.BaudRate   = 115200;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
  UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT; 
  if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    Error_Handler();
  }	
  SET_BIT(USART1->CR1, USART_CR1_PEIE);

  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  SET_BIT(USART1->CR3, USART_CR3_EIE);

  /* Enable the UART Data Register not empty Interrupt */
  SET_BIT(USART1->CR1, USART_CR1_RXNEIE);

	//HAL_UART_Receive_IT(&UartHandle, &tempBuffer[0], 1);
}

#if 0
/**
  * @brief  CDC application Init.
  * @param  None
  * @retval None
  */
static void CDC_InitApplication(void)
{
  /* Configure Key Button */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
      
  /* Configure LED1 */
  BSP_LED_Init(LED1);
  
  /* Initialize the LCD */
  //BSP_LCD_Init();
  
  /* LCD Layer Initialization */
  //BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS); 
  
  /* Select the LCD Layer */
  //BSP_LCD_SelectLayer(1);
  
  /* Enable the display */
  //BSP_LCD_DisplayOn();
  
  /* Initialize the LCD Log module */
  //LCD_LOG_Init();
  
#ifdef USE_USB_HS 
  //LCD_LOG_SetHeader((uint8_t *)" USB OTG HS CDC Host");
#else
  //LCD_LOG_SetHeader((uint8_t *)" USB OTG FS CDC Host");
#endif
  
  //LCD_UsrLog("USB Host library started.\n"); 
  
  /* Start CDC Interface */
  USBH_UsrLog("Starting CDC Demo");
  
  Menu_Init();
}

/**
  * @brief  User Process
  * @param  phost: Host Handle
  * @param  id: Host Library user message ID
  * @retval None
  */
static void USBH_UserProcess(USBD_HandleTypeDef *phost, uint8_t id)
{
  switch(id)
  { 
//  case HOST_USER_SELECT_CONFIGURATION:
//    break;
//    
//  case HOST_USER_DISCONNECTION:
//    Appli_state = APPLICATION_DISCONNECT;
//    break;
//    
//  case HOST_USER_CLASS_ACTIVE:
//    Appli_state = APPLICATION_READY;
//    break;
//    
//  case HOST_USER_CONNECTION:
//    Appli_state = APPLICATION_START;
//    break;

  default:
    break; 
  }
}
#endif
/**
  * @brief This function provides accurate delay (in milliseconds) based 
  *        on SysTick counter flag.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */

void HAL_Delay(__IO uint32_t Delay)
{
  while(Delay) 
  {
    if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) 
    {
      Delay--;
    }
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;  
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Activate the OverDrive to reach the 216 Mhz Frequency */
  if(HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
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
  /* User may add here some code to deal with this error */
  while(1)
  {
  }
}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
