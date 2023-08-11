/*
  *****************************************************************************
  * @file    timer.c
  * @author  KloudQ Team
  * @version
  * @date
  * @brief   Timer functions
*******************************************************************************
*/
/******************************************************************************

            Copyright (c) by KloudQ Technologies Limited.

  This software is copyrighted by and is the sole property of KloudQ
  Technologies Limited.
  All rights, title, ownership, or other interests in the software remain the
  property of  KloudQ Technologies Limited. This software may only be used in
  accordance with the corresponding license agreement. Any unauthorized use,
  duplication, transmission, distribution, or disclosure of this software is
  expressly forbidden.

  This Copyright notice may not be removed or modified without prior written
  consent of KloudQ Technologies Limited.

  KloudQ Technologies Limited reserves the right to modify this software
  without notice.

  KloudQ Technologies Limited

*******************************************************************************
*/
#include "main.h"
#include "stm32l4xx_hal.h"
#include "applicationDefines.h"
#include "timer.h"
#include "externs.h"

volatile uint8_t gu8Timer7_test_variable = 0;
volatile uint32_t gu32GPIODelay = 0;
volatile uint32_t gu16GSMCycleTimeout = TEN_MIN;
volatile uint32_t gu32GSMCharacterTimeout = 0;
volatile uint32_t gu32PayloadQueueEnqueue = 0;
volatile uint32_t gu8rtcUpdateTimer = 0;
volatile uint32_t gu32DebugTimer = 0;
volatile uint32_t gu32ADCRestartTimer = 0;
volatile uint32_t gu32GSMDelay = 0;
volatile uint32_t gu32GSMTimeout = 0;
volatile uint32_t gu32ModbusPollDelay = 0;
volatile uint32_t gu32LEDDelay = 0;
volatile uint32_t gu32TestTimerVariable = 0;
volatile uint32_t gu32I2CMemoryOperationTimeout = 0;
volatile uint32_t gu32FotaFileReadTimer = 0;
volatile uint32_t gu32FlashMemoryTimer = 0;

volatile uint32_t gu32ADCPollTimer = 0;
volatile uint32_t gu32ADCOperationTimer = 0;

volatile uint32_t gu32GSMHangTimer = 0;
volatile uint32_t gu32OBDPollTimer;
volatile uint32_t gu32CANQueryPollTimer = 0;
volatile uint32_t gu32CANCommandResponseTimeout = 0;

uint32_t gu32SleepModeTimer = 0;
uint32_t gu32SleepModeTimer1 = 0;
volatile uint32_t gu32GSMPowerDownTimer = 0;

uint32_t gu32CANReqResponseTimeout = 0;
volatile uint32_t gu32ImmobilizerReadTimer = 0;
uint32_t gu32SystemResetTimer = 0;
/******************************************************************************
 Function Name: HAL_TIM_PeriodElapsedCallback
 Purpose: Timer Interrupt Handler
 Input:	TIM_HandleTypeDef timer instance
 Return value: None.


 -------------------------------------------
 Note : Interrupt Timing Calculations
 -------------------------------------------
 Update_Time = 		  (TIMxCLK)
            	--------------------
				((PSC + 1)(ARR + 1))

				PSC -> Prescalar
				ARR -> Autoreload register

--------------------------------------------
--------------------------------------------

 Change History:
 Author           	Date                Remarks
 System Generated  11-04-18
******************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/*HP Timer Interrupt : 50 us*/

	if(htim->Instance == TIM7)
	{
		if(gu32I2CMemoryOperationTimeout)gu32I2CMemoryOperationTimeout--;
//
		if(gu32GSMCharacterTimeout)gu32GSMCharacterTimeout--;
		if(gsmInstance.u32GSMTimer)gsmInstance.u32GSMTimer--;
		if(gsmInstance.u32GSMResponseTimer)gsmInstance.u32GSMResponseTimer--;
		if(gsmInstance.u32GSMHeartbeatTimer)gsmInstance.u32GSMHeartbeatTimer--;
		if(gu32PayloadQueueEnqueue)gu32PayloadQueueEnqueue--;
		if(gu32FotaFileReadTimer)gu32FotaFileReadTimer--;
		if(gu32GSMHangTimer)gu32GSMHangTimer--;
		if(gu16GSMCycleTimeout)gu16GSMCycleTimeout--;

		/* RTC Timer Variables */
		if(gu8rtcUpdateTimer)gu8rtcUpdateTimer--;

		/*LED*/
		if(gu32LEDDelay)gu32LEDDelay--;

		/* CAN */
		if(gu32CANQueryPollTimer)gu32CANQueryPollTimer--;
		if(gu32CANCommandResponseTimeout)gu32CANCommandResponseTimeout--;


		/* ADC */
		if(gu32ADCPollTimer)gu32ADCPollTimer--;
		if(gu32ADCOperationTimer)gu32ADCOperationTimer--;

		/* Sleep mode*/
		if(gu32SleepModeTimer1)gu32SleepModeTimer1--;
		if(gu32GSMPowerDownTimer)gu32GSMPowerDownTimer--;

		//100355
		/*CAN waiting for Response*/
		if(gu32CANReqResponseTimeout)gu32CANReqResponseTimeout--;
		/*Key Connection Stable*/
		if(u32KeyOffStableTimeout)u32KeyOffStableTimeout--;

		/* Immobilization Check Respnse*/
		if(gu32ImmobilizerReadTimer)gu32ImmobilizerReadTimer--;

		/*No CAN DATA*/
		if(gu8NoCANDATATimeOut)gu8NoCANDATATimeOut--;

		/*System reset Timer*/
		if(gu32FotaRquestFlag == FALSE)
		{
			if(gu32SystemResetTimer)gu32SystemResetTimer--;
		}
	} // End of if(htim->Instance == TIM7)
}

/******************************************************************************
 Function Name: HAL_TIM_IC_CaptureCallback
 Purpose: Input Capture Interrupt
 Input:	TIM_HandleTypeDef timer instance
 Return value: None.



 Change History:
 Author           	Date                Remarks
 System Generated  	9-02-19
 100138			   	26-02-19			Falling / Ceiling Logic for RPM
 100138			   	27-02-19			Falling / Ceiling Logic for RPM with 80 base
 100138				28-02-19			Changed falling - ceiling in 2 parts, 0-50
 	 	 	 	 	 	 	 	 	 	and 51-100 as required by Manitou
******************************************************************************/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
}

//******************************* End of File *******************************************************************
