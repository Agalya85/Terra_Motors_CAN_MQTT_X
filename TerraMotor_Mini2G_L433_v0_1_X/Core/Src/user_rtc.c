/*
  *****************************************************************************
  * @file    internalrtc.c
  * @author  KloudQ Team
  * @version
  * @date
  * @brief   Functions for Accessing Internal RTC
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

*/
#include"stm32l4xx_hal.h"
#include "stm32l4xx_hal_rtc.h"
#include "rtc.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "user_rtc.h"
#include "applicationdefines.h"
#include "system_reset.h"
#include "externs.h"
#include <stdio.h>

extern RTC_TimeTypeDef STime1;
extern RTC_DateTypeDef SDate1;

char gau8Year[5] = {'0'};
char gau8Month[3] = {'0'};
char gau8Date[3] = {'0'};
char gau8Hour[3] = {'0'};
char gau8Minutes[3] = {'0'};
char gau8Seconds[3] = {'0'};
char CorrectRTC = 0;
strTimeElapsedAfterSync strTimeUpdate = {0,0,0,0,0,0,0,0,0,0,0,0};

/* 14-12-2020 . GPS time Sync */
uint32_t u32GPSTimeSyncFlag = FALSE;
uint32_t u32GPSRefTimeHH = 0;
uint32_t u32GPSRefTimeMM = 0;
uint32_t u32GPSRefTimeSS = 0;

uint32_t u32GPSRefDateDD = 0;
uint32_t u32GPSRefDateMM = 0;
uint32_t u32GPSRefDateYY = 0;

volatile uint32_t gu32TimeSyncVariable = 0;
uint32_t  gu32TimeSyncFlag = 0;

char gtime[10]={0};
char gdate[10]={0};
RTC_TimeTypeDef gTime = {0};
RTC_DateTypeDef gDate = {0};


/****************************************************************************
 Function getrtcStamp
 Purpose: Get Time and Date
 Input:	None.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      11-04-18
******************************************************************************/
void getrtcStamp(void)
{
	HAL_RTC_GetTime(&hrtc,&STime1,RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc,&SDate1,RTC_FORMAT_BCD);

	itoa(BCDToDecimal(SDate1.Year),gau8Year,PAYLOAD_DATA_STRING_RADIX);
	itoa(BCDToDecimal(SDate1.Month),gau8Month,PAYLOAD_DATA_STRING_RADIX);
	itoa(BCDToDecimal(SDate1.Date),gau8Date,PAYLOAD_DATA_STRING_RADIX);

	itoa(BCDToDecimal(STime1.Hours),gau8Hour,PAYLOAD_DATA_STRING_RADIX);
	itoa(BCDToDecimal(STime1.Minutes),gau8Minutes,PAYLOAD_DATA_STRING_RADIX);
	itoa(BCDToDecimal(STime1.Seconds),gau8Seconds,PAYLOAD_DATA_STRING_RADIX);
}

/****************************************************************************
 Function DecimalToBCD
 Purpose: Convert Decimal to BCD
 Input:	None.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      13-10-2020			100133
******************************************************************************/
uint32_t DecimalToBCD (uint32_t Decimal)
{
   return (((Decimal/10) << 4) | (Decimal % 10));
}

/****************************************************************************
 Function BCDToDecimal
 Purpose: Convert BCD to Decimal
 Input:	None.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      13-10-2020			100133
******************************************************************************/
uint32_t BCDToDecimal(uint32_t BCD)
{
   return (((BCD >> 4) * 10) + (BCD & 0xF));
}

/****************************************************************************
 Function updateElapsedTime
 Purpose: Calculates elapsed time after rtc sync (network)
 Input:	None.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      13-10-2020			100133
******************************************************************************/
void updateElapsedTime(uint32_t millisecond)
{
	strTimeUpdate.u32TimeHH = (millisecond / (1000 * 60 * 60)) % 24;
	strTimeUpdate.u32TimeMin = (millisecond / (1000 * 60 )) % 60;
	strTimeUpdate.u32TimeSec = (millisecond / 1000) % 60;
	strTimeUpdate.u32TimeMilliSec = millisecond % 1000;

	strTimeUpdate.u32ActTimeHH = strTimeUpdate.u32RefTimeHH + strTimeUpdate.u32TimeHH;
	strTimeUpdate.u32ActTimeMin = strTimeUpdate.u32RefTimeMin + strTimeUpdate.u32TimeMin;
	strTimeUpdate.u32ActTimeSec = strTimeUpdate.u32RefTimeSec + strTimeUpdate.u32TimeSec;
	strTimeUpdate.u32ActTimeMilliSec = strTimeUpdate.u32RefTimeMilliSec + strTimeUpdate.u32TimeMilliSec;

	if(strTimeUpdate.u32ActTimeSec > 59)
	{
		strTimeUpdate.u32ActTimeMin++;
		strTimeUpdate.u32ActTimeSec = strTimeUpdate.u32ActTimeSec - 60;
		strTimeUpdate.u32TimeSec = 0;
	}

	if(strTimeUpdate.u32ActTimeMin > 59)
	{
		strTimeUpdate.u32ActTimeHH++;
		strTimeUpdate.u32ActTimeMin = strTimeUpdate.u32ActTimeMin - 60;
		 strTimeUpdate.u32TimeMin = 0;
	}

	if(strTimeUpdate.u32ActTimeHH > 23)
	{
		strTimeUpdate.u32ActTimeHH = strTimeUpdate.u32ActTimeHH - 24;
		/* Increment date / month and year also */
		// take system reset so that device syncs the date
		systemReset();
	}

	HAL_RTC_GetTime(&hrtc,&STime1,RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc,&SDate1,RTC_FORMAT_BCD);

	if(BCDToDecimal(STime1.Hours) != strTimeUpdate.u32ActTimeHH)
		STime1.Hours = DecimalToBCD(strTimeUpdate.u32ActTimeHH);

	if(BCDToDecimal(STime1.Minutes) != strTimeUpdate.u32ActTimeMin)
			STime1.Minutes = DecimalToBCD(strTimeUpdate.u32ActTimeMin);

	if(BCDToDecimal(STime1.Seconds) != strTimeUpdate.u32ActTimeSec)
		STime1.Seconds = DecimalToBCD(strTimeUpdate.u32ActTimeSec);

	HAL_RTC_SetTime(&hrtc,&STime1,RTC_FORMAT_BCD);
	HAL_RTC_SetDate(&hrtc,&SDate1,RTC_FORMAT_BCD);
	//backupCurrentRTC();

}

/****************************************************************************
 Function rtcreadbackupreg
 Purpose: Read RTC backup register
 Input:	uint32_t BackupRegister.
 Return value: None.


 Note(s)(if-any) :
 RTC features 20 internal backup registers where you can store anything
 and will be available whole time RTC is active and has power.

 Change History:
 Author           	Date                Remarks
 KloudQ Team      28-4-2021				100133
******************************************************************************/
uint32_t rtcreadbackupreg(uint32_t BackupRegister)
{
    RTC_HandleTypeDef RtcHandle;
    RtcHandle.Instance = RTC;
    return HAL_RTCEx_BKUPRead(&RtcHandle, BackupRegister);
}

/****************************************************************************
 Function rtcwritebackupreg
 Purpose: Write to rtc backup reg
 Input:	uint32_t BackupRegister, uint32_t data.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      28-4-2021			100133
******************************************************************************/
//void rtcwritebackupreg(uint32_t BackupRegister, uint32_t data)
//{
//    RTC_HandleTypeDef RtcHandle;
//    RtcHandle.Instance = RTC;
//    HAL_PWR_EnableBkUpAccess();
//    HAL_RTCEx_BKUPWrite(&RtcHandle, BackupRegister, data);
//    HAL_PWR_DisableBkUpAccess();
//}

/****************************************************************************
 Function backupCurrentRTC
 Purpose: Write to rtc backup reg
 Input:	None
 Return value: None.


 Note(s)(if-any) :
 Stores rtc data in backup registers

 Change History:
 Author           	Date                Remarks
 KloudQ Team      28-4-2021			100133
******************************************************************************/
//void backupCurrentRTC(void)
//{
//	uint32_t hrs = BCDToDecimal(STime1.Hours);
//	uint32_t min = BCDToDecimal(STime1.Minutes);
//	uint32_t sec = BCDToDecimal(STime1.Seconds);
//
//	uint32_t date = BCDToDecimal(SDate1.Date);
//	uint32_t month = BCDToDecimal(SDate1.Month);
//	uint32_t yr = BCDToDecimal(SDate1.Year);
//	/* Update Backup Registers */
//	HAL_PWR_EnableBkUpAccess();
//	HAL_RTCEx_BKUPWrite(&hrtc , RTC_BKP_DR0 ,hrs);
//	HAL_RTCEx_BKUPWrite(&hrtc , RTC_BKP_DR1 ,min);
//	HAL_RTCEx_BKUPWrite(&hrtc , RTC_BKP_DR2 ,sec);
//	HAL_RTCEx_BKUPWrite(&hrtc , RTC_BKP_DR3 ,date);
//	HAL_RTCEx_BKUPWrite(&hrtc , RTC_BKP_DR4 ,month);
//	HAL_RTCEx_BKUPWrite(&hrtc , RTC_BKP_DR5 ,yr);
//	HAL_PWR_DisableBkUpAccess();
//}

/****************************************************************************
 Function backupCurrentRTC
 Purpose: Write to rtc backup reg
 Input:	None
 Return value: None.


 Note(s)(if-any) :
 Stores rtc data in backup registers

 Change History:
 Author           	Date                Remarks
 KloudQ Team      28-4-2021			100133
******************************************************************************/
//void readrtcbackupdata(void)
//{
//	uint32_t hrs = DecimalToBCD(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR0));
//	uint32_t min = DecimalToBCD(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1));
//	uint32_t sec = DecimalToBCD(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR2));
//
//	uint32_t date = DecimalToBCD(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR3));
//	uint32_t month = DecimalToBCD(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR4));
//	uint32_t yr = DecimalToBCD(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR5));
//
//	STime1.Hours = hrs;
//	STime1.Minutes =  min;
//	STime1.Seconds = sec;
//
//	SDate1.Date =  date;
//	SDate1.Month = month;
//	SDate1.Year =  yr;
//
//	/*Update RTC with fetched data */
////	HAL_RTC_SetTime(&hrtc,&STime1,RTC_FORMAT_BCD);
////	HAL_RTC_SetDate(&hrtc,&SDate1,RTC_FORMAT_BCD);
//}

/****************************************************************************
 Function syncRTCGPS
 Purpose: Sync local rtc / time with gps UTC time
 Input:	None.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      14-12-2020			100133
******************************************************************************/
void syncRTCGPS(void)
{
	u32GPSRefTimeHH = ((gsmInstance.agpsLocationData[14] - '0') * 10 )
											+ ((gsmInstance.agpsLocationData[15] - '0'));

	u32GPSRefTimeMM = ((gsmInstance.agpsLocationData[16] - '0') * 10 )
							+ ((gsmInstance.agpsLocationData[17] - '0'));

	u32GPSRefTimeSS = ((gsmInstance.agpsLocationData[18] - '0') * 10 )
							+ ((gsmInstance.agpsLocationData[19] - '0'));

	u32GPSRefDateDD = ((gsmInstance.agpsLocationData[12] - '0') * 10 )
							+ ((gsmInstance.agpsLocationData[13] - '0'));

	u32GPSRefDateMM = ((gsmInstance.agpsLocationData[10] - '0') * 10 )
							+ ((gsmInstance.agpsLocationData[11] - '0'));

	u32GPSRefDateYY = ((gsmInstance.agpsLocationData[6] - '0') * 1000 )
						+ ((gsmInstance.agpsLocationData[7] - '0') * 100)
						+ ((gsmInstance.agpsLocationData[8] - '0') * 10 )
						+ ((gsmInstance.agpsLocationData[9] - '0'));

	/* UTC to Local Time Conversion */
	u32GPSRefTimeHH += 5;
	u32GPSRefTimeMM += 30;

	if(u32GPSRefTimeSS > 59)
	{
		u32GPSRefTimeMM++;
		u32GPSRefTimeSS = u32GPSRefTimeSS - 60;
	}
	if(u32GPSRefTimeMM > 59)
	{
		u32GPSRefTimeHH++;
		u32GPSRefTimeMM = u32GPSRefTimeMM - 60;

	}
	if(u32GPSRefTimeHH > 23)
	{
		u32GPSRefTimeHH = u32GPSRefTimeHH - 24;
	}

	/*Check Null data */

//	HAL_RTC_GetTime(&hrtc, &STime1, RTC_FORMAT_BCD);
//	HAL_RTC_GetDate(&hrtc, &SDate1, RTC_FORMAT_BCD);

	if(BCDToDecimal(STime1.Hours) != u32GPSRefTimeHH)
		STime1.Hours = DecimalToBCD(u32GPSRefTimeHH);

	if(BCDToDecimal(STime1.Minutes) != u32GPSRefTimeMM)
		STime1.Minutes = DecimalToBCD(u32GPSRefTimeMM);

	if(BCDToDecimal(STime1.Seconds) != u32GPSRefTimeSS)
		STime1.Seconds = DecimalToBCD(u32GPSRefTimeSS);

//	HAL_RTC_SetTime(&hrtc, &STime1, RTC_FORMAT_BCD);
//	HAL_RTC_SetDate(&hrtc, &SDate1, RTC_FORMAT_BCD);
}

void Get_Time_Date(void)
{
//	RTC_TimeTypeDef gTime = {0};
//	RTC_DateTypeDef gDate = {0};

	HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BCD);

	HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BCD);

//	sprintf((char*)gtime,"%02d:%02d:%02d",gTime.Hours,gTime.Minutes,gTime.Seconds);
//	sprintf((char*)gdate,"%02d-%02d-%02d",gDate.Date,gDate.Month,(2000+(gDate.Year)));

}

/***************************************************************************
Function: RTCWriteBackupReg
Purpose: To store data in backup register of RTC(like Internal EEPROM).
Input: 	1) uint32_t BackupRegister : address of register(e.g RTC_BKP_DR31)
		2) uint32_t data (actual data)

Return value: None
****************************************************************************/
//void RTCWriteBackupReg(uint32_t BackupRegister, uint32_t data)
//{
//	RTC_HandleTypeDef hrtc;
////  	RtcHandle.Instance = RTC;
//	hrtc.Instance = RTC;
//
//  	HAL_PWR_EnableBkUpAccess();
//  	HAL_RTCEx_BKUPWrite(&hrtc, BackupRegister, data);
//  	HAL_PWR_DisableBkUpAccess();
//}

/***************************************************************************
Function: RTCReadBackupReg
Purpose: To Read data from backup register of RTC(like Internal EEPROM).
Input: 1) uint32_t BackupRegister : address of register(e.g RTC_BKP_DR31)


Return value: Data
****************************************************************************/
//uint32_t RTCReadBackupReg(uint32_t BackupRegister)
//{
//	RTC_HandleTypeDef hrtc;
////	RtcHandle.Instance = RTC;
//	hrtc.Instance = RTC;
//	return HAL_RTCEx_BKUPRead(&hrtc, BackupRegister);
//}


//******************************* End of File *******************************************************************
