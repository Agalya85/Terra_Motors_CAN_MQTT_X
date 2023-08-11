/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */
RTC_TimeTypeDef STime1;
RTC_DateTypeDef SDate1;

RTC_TimeTypeDef GTime1;
RTC_DateTypeDef GDate1;

RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef sDate = {0};
RTC_AlarmTypeDef sAlarm = {0};
RTC_TamperTypeDef sTamper = {0};

#include "externs.h"
#include "user_rtc.h"
#include "timer.h"

#include <string.h>
/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;
uint32_t mtime = 0;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x30;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS
                              |RTC_ALARMMASK_SECONDS;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }


//  /* USER CODE BEGIN RTC_Init 2 */
//  sTamper.NoErase = RTC_TAMPER_ERASE_BACKUP_DISABLE; // No erase on Pwr Down
//  if (HAL_RTCEx_SetTamper(&hrtc, &sTamper) != HAL_OK)
//  {
//	  Error_Handler();
//  }
  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();

    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

#define RTC_ALARM_MIN 30

void Set_Alarm(void)
{
 /** Enable the Alarm A
  */

  sAlarm.AlarmTime.Hours = 0x0;
  Get_Time_Date();
  if((RTC_Bcd2ToByte(gTime.Minutes) + RTC_ALARM_MIN) >= 60)
  {
	  mtime =  (RTC_Bcd2ToByte(gTime.Minutes) + RTC_ALARM_MIN) - 60;
  }
  else
  {
	  mtime = RTC_Bcd2ToByte(gTime.Minutes) + RTC_ALARM_MIN;
  }
  sAlarm.AlarmTime.Minutes = RTC_ByteToBcd2(mtime);//0x30;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS
							  |RTC_ALARMMASK_SECONDS;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
	Error_Handler();
  }
}

/**
  * @brief  Alarm callback
  * @param  hrtc : RTC handle
  * @retval None
  *
  *  Akshay Jagtap
  * on 11/11/2021
  * Purpose : after sleep time completed , On ID 0x152 cont. counter was incrementing for next sleep timer i.e 2 min.
  * Added if condition for "gu32MCUModeExitFlag" , "u8ExSleepModeFlag" and "gu32CanQueryCounter".
  * RTC Alarm is Deactivated as it was get in callback function for 4 -5 time,
  * as we have activated it again in setAlarm.
  */

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Turn LED2 on: Alarm generation */
//	unCanTransmitMsgs.u8CanTxMsgByte0 = WAKE_UP;
	unCanTransmitMsgs.u8CanTxMsgByte0 = 0;
	unCanTransmitMsgs.u8CanTxMsgByte1 = 0;
	unCanTransmitMsgs.u8CanTxMsgByte2 = 0;
	unCanTransmitMsgs.u8CanTxMsgByte3 = 0;
	unCanTransmitMsgs.u8CanTxMsgByte4 = 0;
	unCanTransmitMsgs.u8CanTxMsgByte5 = 0;
	unCanTransmitMsgs.u8CanTxMsgByte6 = 0;
	unCanTransmitMsgs.u8CanTxMsgByte7 = 0;
	//unCanTransmitMsgs.u8CanTxMsgByte7 = WAKE_UP;
	HAL_CAN_AbortTxRequest(&hcan1, TxMailbox);
//	gu32MCUCurrentWorkingMode = enmMCUMode_RUN;
	canCurrentState = enmCANQUERY_IDLE;
	memset(gua8KeyData,0x00,sizeof(gua8KeyData));


//	gu32GSMPowerStateFlag = 0;

	if(gu32MCUModeExitFlag == 0)
		gu32MCUModeExitFlag = 1;

//	if(u8ExSleepModeFlag == 1)
//			u8ExSleepModeFlag = 0;

	if(gu32CanQueryCounter != 0)
			gu32CanQueryCounter = 0;

		gu32KeyDataFlag = 1;
//	 gu8CANRxResponseFlag = 0;
	 updateCANQueryState = 0;
	// u8GenSeedFlag = 0;
	// u8PreTxData = WAKE_UP;
	 u8keyVerifyErr = 0;
	gu32CANReqResponseTimeout =TEN_SEC;// THIRTY_SEC;
	HAL_RTC_DeactivateAlarm(hrtc,RTC_ALARM_A);

}
/* USER CODE END 1 */
