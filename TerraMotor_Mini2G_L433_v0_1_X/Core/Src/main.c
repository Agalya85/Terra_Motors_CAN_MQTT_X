/* USER COD/E BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Version
 *
 * TerraMotor_Mini2G_L433_MQTT_v0_1
 *
 * 
 */


/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "iwdg.h"
#include "lptim.h"
#include "rng.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gsmSin868.h"
#include "timer.h"
#include "externs.h"
#include "queue.h"
#include "user_eeprom.h"
#include "user_adc.h"
#include "user_can.h"
#include "user_rtc.h"
#include "deviceinfo.h"
#include "payload.h"
#include "tim.h"
#include "user_flash.h"
#include <string.h>
#include "user_MqttSubSperator.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint32_t MCU_Id = 23;
uint8_t gu8PowerOnFlag = TRUE;
uint8_t fotaPloadFlag = FALSE;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
//  MX_CAN1_Init();
  MX_I2C1_Init();
  MX_IWDG_Init();
  MX_LPTIM1_Init();
  MX_RTC_Init();
  MX_SPI2_Init();
  MX_TIM7_Init();
  MX_USART1_UART_Init();
  MX_RNG_Init();
  /* USER CODE BEGIN 2 */
//  gu8PowerOnFlag  = 1;
  updateDeviceSignature();
  HAL_TIM_Base_Start_IT(&htim7);
  LL_USART_EnableIT_RXNE(USART1);
  gu32PayloadQueueEnqueue = ONE_MIN;//TEN_MIN;	//
  gu32SleepModeTimer1 = 0;
  gu32SystemResetTimer = THIRTY_MIN;
//  HAL_GPIO_WritePin(LED_ERROR_GPIO_Port,LED_ERROR_Pin,GPIO_PIN_SET);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  if(MCU_Id)
  {
	  canFilterConfig();
	  Diagnostic(enmDiagnostic_CAN_ID);
	  MCU_Id = 0;
  }

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

			  parseCanMessageQueue();

			  if(gu32PayloadQueueEnqueue == 0)
			  {
				#if (DATA_PROCESSING_METHOD == FIFO )
				  	  enqueue(&gsmPayload,(char *)getMachineDataString());
				#elif(DATA_PROCESSING_METHOD == LIFO)
				  	  pushDataToStack(&gstrGMSPayloadLIFO,(char *)getMachineDataString());
 	 	 	 	#endif
				  	  gu32PayloadQueueEnqueue = FIFTEEN_SEC; // /* Upload Frequency */ update given by Mahesh Sir.

 	 	 	 	 #if(ENABLEDEBUG == TRUE)
				  	  enqueue(&debug, getMachineDataString());
				  	  enqueue(&debug, (char *)getDebugString("\r\n"));
				#endif
			  }// end of if(gu32PayloadQueueEnqueue == 0)

		  if(gu32GSMHangTimer == 0)
			  initGSMSIM868();

		  operateGSMSIM868();


		if(gu32LEDDelay == 0)
		{
			HAL_GPIO_WritePin(LED_COMM_GPIO_Port, LED_COMM_Pin, GPIO_PIN_RESET);
			HAL_GPIO_TogglePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin);
			gu32LEDDelay = ONE_SEC;// HUNDRED_MS;// //FIVE_SEC;
		}

		 if((gu32SystemResetTimer == 0) || ((FOTACompleteFlag == TRUE) && (isQueueEmpty(&gsmPayload) == 1)))
		 {
			 while(1){}
		 }
/*
		  if(SUBTriggerFlag == TRUE)
		  {
			  if(strstr(gau8SUBRequest, "7,") != NULL)
			  {
//				  const char *URL = strstr(gau8SUBRequest,"https:");
				  const char *URL = strstr(gau8SUBRequest,"https:");
				  const char *BIN = strstr(gau8SUBRequest,".bin");
				  const size_t mlen = BIN - URL;
				  if((URL != NULL) && (BIN != NULL))
				  {
					  memset(gau8FotaURL,0x00,sizeof(gau8FotaURL));
					  memcpy(gau8FotaURL,URL,mlen+4);

					  Diagnostic(enmDiagnostic_FOTA_REQ_RX);

//					  if((gfInputSupplyVoltage < 11.5)) //|| (gfBatteryVoltage < 2.50))
					  if((gfInputSupplyVoltage > 11.5)) //|| (gfBatteryVoltage < 2.50))
					  {
						  gu32FotaRquestFlag = FALSE;
						  SUBTriggerFlag = FALSE;
	//					  LowVoltgUploadFlag = TRUE;
						  Diagnostic(enmDiagnostic_POWER_SUPPLY_ERROR);
					  }
					  else
					  {
						  gu32FotaRquestFlag = TRUE;
					  }
				  }
				  else
				  {
					  SUBTriggerFlag = FALSE;
				  }
			  }
			  else
			  {
				  SUBTriggerFlag = FALSE;
			  }
			  memset(gau8SUBRequest,0x00,sizeof(gau8SUBRequest));
		  }//end of  if(SUBTriggerFlag == TRUE)
*/
	  	  if(SUBTriggerFlag == TRUE)
	  	  {
	  		SubRevicedString();
	  	  }//end of  if(SUBTriggerFlag == TRUE)
		HAL_IWDG_Refresh(&hiwdg);
  }//end of while(1)

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 12;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RNG|RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.RngClockSelection = RCC_RNGCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 8;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV4;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK|RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

