/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l0xx_hal.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "rtc.h"
#include "gpio.h"
#include "spi.h"

/* USER CODE BEGIN Includes */
#include "usart.h"
#include "hdc1000.h"
#include "app.h"
#include "opt3001.h"
#include "MPL3115.h"
#include "mma8451.h"
#include "lorawan.h"
#include "ili9341.h"
#include "XPT2046.h"
#include "lcd_app.h"
#include "dht11.h"



/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void TIM_init_lcd(uint16_t Value);
void Error_Handler(void);


/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
extern USART_RECEIVETYPE Usart2_RX;
extern USART_RECEIVETYPE Usart1_RX;
extern USART_RECEIVETYPE LPUsart1_RX;
TIM_HandleTypeDef  TimHandle;

uint8_t Light = 70;  //液晶的正常亮度是 70%
uint8_t LCD_EN = 0;  //初始值不带液晶

#define  RED_BAN

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
  //dht11_init ();
  //------------------------判断是否液晶的存在-----------------///
  
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
   /* GPIO Ports Clock Enable */
  
   __HAL_RCC_GPIOD_CLK_ENABLE();

   HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
  
	  /*Configure GPIO pin : PD2 */
   GPIO_InitStruct.Pin = GPIO_PIN_2;
   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pull = GPIO_PULLDOWN;
   HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

   HAL_Delay(20);
  
  if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_2) == 1)
  {
  	LCD_EN = 1;
  }else
  {
  	LCD_EN = 0;
  }

  //------------------------------------------------------------//

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();

  if(LCD_EN ==1)   //如果有液晶，需要进行初始化
  {
  	ILI9341_INITIAL();
	TIM_init_lcd(0);
  }
  //XPT2046_init();
  MX_DMA_Init();
  MX_RTC_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_LPUART1_UART_Init();
  //MX_I2C1_Init();

  /* USER CODE BEGIN 2 */

  //---------------系统初始化补丁--------------------
  Clear_UART2_IT();
  Clear_UART1_IT();
  Clear_LPUART1_IT();
  //-------------------------------------------------

  Hardware_Init();        //外部硬件初始化

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  	User_App();       // 用户应用

	if(LCD_EN ==1)
	{
  		LCD_APP_Mode();
	}
  
  }
  /* USER CODE END 3 */

}


//-------------------------------------Tim_LCD-----------------------------------------

void TIM_init_lcd(uint16_t Value)
{
	/* Compute the prescaler value to have TIMx counter clock equal to 100 KHz */
	  uint32_t uwPrescalerValue = (uint32_t) ((32000000 / 150000) - 1);
	  
	  /*##-1- Configure the TIM peripheral #######################################*/ 
	  /* Set TIMx instance */
	  TimHandle.Instance = TIM6;
	    
	  /* Initialize TIMx peripheral as follow:
	       + Period = 10000 - 1
	       + Prescaler = SystemCoreClock/10000 Note that APB clock = TIMx clock if
	                     APB prescaler = 1.
	       + ClockDivision = 0
	       + Counter direction = Up
	  */
	  TimHandle.Init.Period = 10 - 1;
	  TimHandle.Init.Prescaler = uwPrescalerValue;
	  TimHandle.Init.ClockDivision = 0;
	  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	  if(HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
	  {
	  
	  }
	  
	  /*##-2- Start the TIM Base generation in interrupt mode ####################*/
	  /* Start Channel1 */
	  if(HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
	  {
	    
	  }
	
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
 	static uint16_t num = 0;

	num ++;

	if(num > 0 && num <= Light)
	{
		lLED_ON();
	}
	if(num > Light && num <= 100)
	{
		lLED_OFF();
	}
	if(num > 100)
	{
		num = 0;
	}
	
}




/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  /* 使能PWR外设时钟 */
    __PWR_CLK_ENABLE();
    
    /**Configure the main internal regulator output voltage 
    */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_LPUART1|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_HSI;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

void Hardware_Init(void)
{
	HDC1000_Init();
	OPT3001_Init();
	#ifdef  RED_BAN
	MPL3115_Init(MODE_BAROMETER);
	MMA8451_Init();
	#endif
	LoraNode_GPIO_Init();
	LoraNode_Init_Mode(MODE_CMD);
	//GPS_Init();

	//LoraNode_OTAA_Config();   //用于OTAA方式的参数配置
	//LoraNode_ABP_Config();    //用于ABP方式的参数配置
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  //while(1) 
  //{
  //}
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
