/* BGAUSS */
/*
  *****************************************************************************
   * @file    payload.c
  * @author  KloudQ Team
  * @version
  * @date
  * @brief   TorMini Payload utility Functions
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


  Note  : : 59.163.219.179:8014/
  http://10.3.8.62:8014/api/Values/PostStringData

String Sequence should be

1.	Start of frame
2.	Hardware ID
3.	Model No.
4.	UTC date time
5.	Firmware Version

*/

#include "main.h"



/* USER CODE BEGIN Includes */
#include "applicationDefines.h"
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "payload.h"
#include "user_can.h"
#include "gsmSin868.h"
#include "externs.h"
#include "user_error.h"

#define ENABLERTC TRUE

char * gpu8GPSString01 = "1,1,20171020064853.000";
char * gpu8GPSString02 = "192.407,0.00,0.0,1,,1.0,2.3,2.1,,8,11,,,52,,";
char * gpu8NoString = 	  ": 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0";
char * gpu8Norssi = "0,0";
char * gpu8Nohrs = "0,0";
char gpu8ModbusRunHrs[3] = "0,0";
char gau8Lat[10] = {'0'};
char gau8Lon[10] = {'0'};
char gau8EngineRPM[5]="0000";
char gau8EngineTemp[4]="050";
char gau8FuelLevel[2]="0";
char gau8FilterClog[2]="0";
char gau8LoP[2]={'0'};
char gau8FnR[2]={'0'};
char gau8RunHrHH[5]="00000";
char gau8RunHrMM[3]="01";
char gau8TempLocation[80]={'0'};

char voltage[6] = "0.00";

char agpsLastLocationData[150] = "";
char agpsTempLocationData[150] = "";

char * gau8CanPayload[MAX_CAN_IDS_SUPPORTED + 1];

//uint32_t gu32EngineRPM = 0;
//uint32_t gu32EngineTepmerature = 0;
//uint32_t gu32FuelLevel = 0;
//uint32_t gu32FilterClog = 0;
//uint32_t gu32LowOilPressure = 0;
//uint32_t gu32FNRSwitch = 0;
//uint32_t gu32RunHourHH = 404;
//uint32_t gu32RunHourMM = 404;

uint32_t gu32LastGPSFlag = 0;
_Bool FOTACompleteFlag = FALSE;
extern strctGSM gsmInstance;
/******************************************************************************
 Function Name: getMachineDataString
 Purpose: Generate System Payload
 Input:	None
 Return value: (char *)

	Note(s)(if any)
 	*,TorMini,Intg 1.0,20/5/28 12:23:69,05406193121196442369524317,
 	*: 1,0,19800106000027.000,,,,0.00,0.0,0,,,,,,0,0,,,,,,0,0,0,0,
 	*4030201,0,0,1040404,2020202,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 	*0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,,21,0,#

 Change History:
 Author           	Date                Remarks
 KloudQ Team       07-02-19
 Kloudq Team	   30-03-2020			Malloc Error countered.(CANPayloadErr)
 Kloudq Team	   28-05-2020			Added long long int to string (CAN)
 Kloudq Team	   28-10-2020			Time stamp sequence made DD/MM/YY
******************************************************************************/
uint32_t gu32CanIdsLoopCounter = 1;
uint32_t gu32CanPayloadLoopCounter = 0;
uint32_t gu32StringLength = 0;
_Bool gbitMcuReset = 1;
char * gpsState = "";
char gau8GPSLatchState[2]={'0'};
char * getMachineDataString(void)
{
	//updateSystemParameters();

	uint16_t payload_length;
	char * systemPayload = malloc(sizeof(char) * GSM_PAYLOAD_MAX_SIZE);
	//getrtcStamp();
	/* Hold Last know GPS */
	gpsState = strstr(gsmInstance.agpsLocationData,": 1,1,");
	if(gpsState != NULL)
	{
		/*GPS available */
		gu32LastGPSFlag = 0;
//		memset(agpsLastLocationData,0x00,sizeof(agpsLastLocationData));
//		memcpy(agpsLastLocationData,gsmInstance.agpsLocationData,strlen(gsmInstance.agpsLocationData));
		memset(agpsTempLocationData,0x00,sizeof(agpsTempLocationData));
		memcpy(agpsTempLocationData,gsmInstance.agpsLocationData,strlen(gsmInstance.agpsLocationData));
//		SperateGPSString();
	}
	else
	{
		gu32LastGPSFlag = 1;
	}


	if(systemPayload != NULL)
	{
		/*
			1.	Start of frame
			2.	Hardware ID
			3.	Model No.
			4.	UTC date time
			5.	Firmware Version
		*/
		memset(systemPayload,0x00,sizeof(char) * GSM_PAYLOAD_MAX_SIZE);
		/* Start of Frame */
		strcpy((char *)systemPayload,(char * )START_OF_FRAME);
		strcat((char *)systemPayload,(char * )",");

		/* Device UUID */
		strcat((char *)systemPayload,(char * )dinfo);
		strcat((char *)systemPayload,(char * )",");

		/* Model Number */
		strcat((char *)systemPayload,(char * )MODEL_NUMBER);
		strcat((char *)systemPayload,(char * )",");

		PaylodTime(systemPayload);

		/* Firmware Version*/
		strcat((char *)systemPayload,(char * )FIRMWARE_VER);
		strcat((char *)systemPayload,(char * )",");
		
		strcat((char *)systemPayload,(char * )BOOT_REGION);
		strcat((char *)systemPayload,(char * )",");
		/* GPS Data */

		//strcat((char *)systemPayload,(char * )gsmInstance.agpsLocationData);
		if(strlen(agpsTempLocationData) > 0)
			strcat((char *)systemPayload,(char *)agpsTempLocationData);
		else
			strcat((char *)systemPayload,(char *)gpu8NoString);

		strcat((char *)systemPayload,",");

		/* CAN Data */

		updateCanPayload(1, systemPayload );
		updateCanPayload(2, systemPayload );
		updateCanPayload(3, systemPayload );
		updateCanPayload(4, systemPayload );
		updateCanPayload(5, systemPayload );
		updateCanPayload(6, systemPayload );
		updateCanPayload(7, systemPayload );
		updateCanPayload(8, systemPayload );
		updateCanPayload(9, systemPayload );
//		updateCanPayload(10, systemPayload );
//		updateCanPayload(11, systemPayload );
//		updateCanPayload(12, systemPayload );
//		updateCanPayload(13, systemPayload );
//		updateCanPayload(14, systemPayload );
//		updateCanPayload(15, systemPayload );
//		updateCanPayload(16, systemPayload );
//		updateCanPayload(17, systemPayload );
//		updateCanPayload(18, systemPayload );
//		updateCanPayload(19, systemPayload );
//		updateCanPayload(20, systemPayload );
//		updateCanPayload(21, systemPayload );
//		updateCanPayload(22, systemPayload );
//		updateCanPayload(23, systemPayload );
//		updateCanPayload(24, systemPayload );
//		updateCanPayload(25, systemPayload );
//		updateCanPayload(26, systemPayload );
//		updateCanPayload(27, systemPayload );
//		updateCanPayload(28, systemPayload );
//		updateCanPayload(29, systemPayload );
//		updateCanPayload(30, systemPayload );
//		updateCanPayload(31, systemPayload );
//		updateCanPayload(32, systemPayload );
//		updateCanPayload(33, systemPayload );
//		updateCanPayload(34, systemPayload );
//		updateCanPayload(35, systemPayload );

//		strcat((char *)systemPayload,(char * )",");
		/* RSSI */
		if(strlen(gsmInstance.agsmSignalStrength ) > 0)
			strcat((char *)systemPayload,(char * )gsmInstance.agsmSignalStrength);
		else
			strcat((char *)systemPayload,gpu8Norssi);

		strcat((char *)systemPayload,(char * )",");

		if(gu32LastGPSFlag == 1)
			strcat((char *)systemPayload,"1");
		else
			strcat((char *)systemPayload,"0");

		strcat((char *)systemPayload,(char * )",");

		/* End of Frame */
		strcat((char *)systemPayload,(char * )END_OF_FRAME);
		gu32CanIdsLoopCounter = 1;

		/* MCU Reset identification logic */
		if(gbitMcuReset == 1)
		{
			gbitMcuReset = 0;
			payload_length = strlen((char *)systemPayload);
			systemPayload[payload_length - 3] = '1';
		}

		return systemPayload;
	}
	else
		return NULL; // malloc Error . Memory Allocation Failure
}

/******************************************************************************
 Function Name: convertIntergertoString
 Purpose: Convert integert to String
 Input:	uint32_t value
 Return value: (char *)

	Note(s)(if any)
	Generic Function

 Change History:
 Author           	Date                Remarks
 KloudQ Team       07-02-19
******************************************************************************/
void updateSystemParameters(void)
{
	//itoa((uwFrequency),gau8EngineRPM,PAYLOAD_DATA_STRING_RADIX);
	//itoa(gu32FilterClog,gau8FilterClog,PAYLOAD_DATA_STRING_RADIX);
	//itoa(gu32LowOilPressure,gau8LoP,PAYLOAD_DATA_STRING_RADIX);
	//itoa(gu32FNRSwitch,gau8FnR,PAYLOAD_DATA_STRING_RADIX);
	/* Already in ASCII */
	//itoa(gu32RunHourHH,gau8RunHrHH,PAYLOAD_DATA_STRING_RADIX);
	//itoa(gu32RunHourMM,gau8RunHrMM,PAYLOAD_DATA_STRING_RADIX);

}

void updateCanPayload(uint32_t data ,char * systemPayload )
{
	char temp[10] = "";
	uint8_t CANTempData = 0;
	if(gu64CanMessageArray[data] != 0 )
	{
		strcat((char *)systemPayload,"^");
		strcat((char *)systemPayload,"0x");
		itoa(gu32CanConfigurationArray[data] ,temp,CAN_PAYLOADSTRING_RADIX);
		strcat((char *)systemPayload,temp);
		strcat((char *)systemPayload,",");
		CANTempData = (((uint64_t)gu64CanMessageArray[data] >> 56) & 255);
		sprintf(temp,"%02X",CANTempData);
        CANTempData = 0;
//		itoa((((uint64_t)gu64CanMessageArray[data] >> 56) & 255),temp,CAN_PAYLOADSTRING_RADIX);
		strcat((char *)systemPayload,temp);
		strcat((char *)systemPayload,",");
		CANTempData = (((uint64_t)gu64CanMessageArray[data] >> 48) & 255);
		sprintf(temp,"%02X",CANTempData);
		CANTempData = 0;
//		itoa((((uint64_t)gu64CanMessageArray[data] >> 48) & 255),temp,CAN_PAYLOADSTRING_RADIX);
		strcat((char *)systemPayload,temp);
		strcat((char *)systemPayload,",");
		CANTempData = (((uint64_t)gu64CanMessageArray[data] >> 40) & 255);
		sprintf(temp,"%02X",CANTempData);
		CANTempData = 0;
//		itoa((((uint64_t)gu64CanMessageArray[data] >> 40) & 255),temp,CAN_PAYLOADSTRING_RADIX);
		strcat((char *)systemPayload,temp);
		strcat((char *)systemPayload,",");
		CANTempData = (((uint64_t)gu64CanMessageArray[data] >> 32) & 255);
		sprintf(temp,"%02X",CANTempData);
		CANTempData = 0;
//		itoa((((uint64_t)gu64CanMessageArray[data] >> 32) & 255),temp,CAN_PAYLOADSTRING_RADIX);
		strcat((char *)systemPayload,temp);
		strcat((char *)systemPayload,",");
//		itoa((((uint64_t)gu64CanMessageArray[data] >> 24) & 255),temp,CAN_PAYLOADSTRING_RADIX);
		CANTempData = (((uint64_t)gu64CanMessageArray[data] >> 24) & 255);
		sprintf(temp,"%02X",CANTempData);
		CANTempData = 0;
		strcat((char *)systemPayload,temp);
		strcat((char *)systemPayload,",");
//		itoa((((uint64_t)gu64CanMessageArray[data] >> 16) & 255),temp,CAN_PAYLOADSTRING_RADIX);
		CANTempData = (((uint64_t)gu64CanMessageArray[data] >> 16) & 255);
		sprintf(temp,"%02X",CANTempData);
		CANTempData = 0;
		strcat((char *)systemPayload,temp);
		strcat((char *)systemPayload,",");
//		itoa(((uint64_t)(gu64CanMessageArray[data] >> 8) & 255),temp,CAN_PAYLOADSTRING_RADIX);
		CANTempData = (((uint64_t)gu64CanMessageArray[data] >> 8) & 255);
		sprintf(temp,"%02X",CANTempData);
		CANTempData = 0;
		strcat((char *)systemPayload,temp);
		strcat((char *)systemPayload,",");
//		itoa(((uint64_t)(gu64CanMessageArray[data]) & 255),temp,CAN_PAYLOADSTRING_RADIX); //255
		CANTempData = (((uint64_t)gu64CanMessageArray[data] ) & 255);
		sprintf(temp,"%02X",CANTempData);
		CANTempData = 0;
		strcat((char *)systemPayload,temp);
		strcat((char *)systemPayload,",");

		/*Clear Payload Array*/
		/* Removed after discussion with team that we should keep previous data in case of CAN communication not available */
		gu64CanMessageArray[data] = 0;
	}
	else if(gu64CanMessageArray[data] == 0 )
	{
		/* Added constant string in place of multiple strcat - 18/02/2021 Milind Vaze*/
		strcat((char *)systemPayload,"^");
		strcat((char *)systemPayload,"0x");
		itoa(gu32CanConfigurationArray[data] ,temp,CAN_PAYLOADSTRING_RADIX);
		strcat((char *)systemPayload,temp);
		strcat((char *)systemPayload,",0,0,0,0,0,0,0,0,");//",0,0,0,0,0,0,0,0,"
	}
}

void SperateGPSString(void)
{
	uint8_t LoopCount = 0, CopyCount = 0;
	memset(agpsLastLocationData,0x00,sizeof(agpsLastLocationData));
	if(strlen(agpsTempLocationData)<35)
	{
		return;
	}
	/*discard : 1*/
	for(LoopCount = 0;agpsTempLocationData[LoopCount]!= ',';LoopCount++)
	{
		/*Do Nothing*/
	}
	/*discard  1*/
	LoopCount++;
	for(;agpsTempLocationData[LoopCount]!= ',';LoopCount++)
	{
		/*Do Nothing*/
	}
	/*run till '.' after date time*/
//	agpsLastLocationData[CopyCount++] = ',';
	LoopCount++;
	for(;agpsTempLocationData[LoopCount]!= '.';LoopCount++)
	{
		agpsLastLocationData[CopyCount++] = agpsTempLocationData[LoopCount];
	}
	/*after '.' remove all zero*/
	for(;agpsTempLocationData[LoopCount]!= ',';LoopCount++)
	{
		/*Do Nothing*/
	}
	agpsLastLocationData[CopyCount++] = ',';
	LoopCount++;
	/*Long*/
	for(;agpsTempLocationData[LoopCount]!= ',';LoopCount++)
	{
		agpsLastLocationData[CopyCount++] = agpsTempLocationData[LoopCount];
	}
	agpsLastLocationData[CopyCount++] = ',';
	LoopCount++;
	/*LAT*/
	for(;agpsTempLocationData[LoopCount]!= ',';LoopCount++)
	{
		agpsLastLocationData[CopyCount++] = agpsTempLocationData[LoopCount];
	}
}

char * getDiagDataString(enmDiagnosticStatus DiagnosticStatus)
{

	char * systemPayload = malloc(sizeof(char) * GSM_PAYLOAD_MAX_SIZE);
	if(systemPayload != NULL)
	{
		/*
			1.	Start of frame
			2.	Hardware ID
			3.	Model No.
			4.	UTC date time
			5.	Firmware Version
		*/
		memset(systemPayload,0x00,sizeof(char) * GSM_PAYLOAD_MAX_SIZE);
		/* Start of Frame */
		strcpy((char *)systemPayload,(char * )START_OF_FRAME);
		strcat((char *)systemPayload,(char * )",");

		/* Device UUID */
		strcat((char *)systemPayload,(char * )dinfo);
		strcat((char *)systemPayload,(char * )",");

		strcat((char *)systemPayload,"Diagnostic");
		strcat((char *)systemPayload,(char * )",");

		strcat((char *)systemPayload,(char * )MODEL_NUMBER);
		strcat((char *)systemPayload,(char * )",");

		PaylodTime(systemPayload);

		/* Firmware Version*/
		strcat((char *)systemPayload,(char * )FIRMWARE_VER);
		strcat((char *)systemPayload,(char * )",");
		/*Boot Loction*/
		strcat((char *)systemPayload,(char * )BOOT_REGION);
		strcat((char *)systemPayload,(char * )",");

		DiagnosticString(systemPayload,DiagnosticStatus);
		if(gu8PowerOnFlag == TRUE)
		{
			strcat((char *)systemPayload,"CAN ID's : ");
			CANIDPrase(systemPayload);
		}

		if(fotaPloadFlag == TRUE)
		{
			strcat((char *)systemPayload,"FR");
			strcat((char *)systemPayload,(char * )",");
			strcat((char *)systemPayload,(char *)gau8FotaURL);
			strcat((char *)systemPayload,(char * )",");
			fotaPloadFlag = FALSE;
		}

		/* End of Frame */
		strcat((char *)systemPayload,(char * )END_OF_FRAME);
		gu32CanIdsLoopCounter = 1;

		if(gu8PowerOnFlag == TRUE)
			gu8PowerOnFlag = FALSE;

		 MCU_Id = 0;
		return systemPayload;
	}
	else
		return NULL; // malloc Error . Memory Allocation Failure
}

void DiagnosticString(char * systemPayload, enmDiagnosticStatus DiagnosticStatus)
{
	char temp[20] = "";
	switch(DiagnosticStatus)
	{
		case enmDiagnostic_FOTA_SUCCESS_OK:
			strcat((char *)systemPayload,"FS");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,CAN_PAYLOADSTRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			FOTACompleteFlag = TRUE;
			break;

		case enmDiagnostic_CAN_ID:
			strcat((char *)systemPayload,"CAN ID's : ");
			CANIDPrase(systemPayload);
			break;

		case enmDiagnostic_HW_ID_MISMATCH_ERROR: // Device HW ID Mismatch
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,CAN_PAYLOADSTRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_FOTA_REQ_RX:
		strcat((char *)systemPayload,"FR");
		strcat((char *)systemPayload,(char * )",");
		strcat((char *)systemPayload,(char *)gau8FotaURL);
		strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_POWER_SUPPLY_ERROR:
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,CAN_PAYLOADSTRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			strcat((char *)systemPayload,"Input Supply : ");
			updateInputVoltage(gfInputSupplyVoltage);
			strcat((char *)systemPayload,(char * )voltage);
			strcat((char *)systemPayload,(char * )",");
			strcat((char *)systemPayload,"Internal Battery Supply : ");
			updateInputVoltage(gfBatteryVoltage);
			strcat((char *)systemPayload,(char * )voltage);
			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_RX_FILE_SIZE_ERROR: //Received File Size is Greater than (FLASH_SIZE-4k)/2
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,CAN_PAYLOADSTRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",Received File Size : ");
			strcat((char *)systemPayload,gu32FotaFileSizeinBytes);
			strcat((char *)systemPayload,(char * )" Bytes,");
			break;

		case enmDiagnostic_FILE_DONWLOAD_ERROR:
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,CAN_PAYLOADSTRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			strcat((char *)systemPayload,(char * )gsmInstance.u32GSMHttpResponseCode);
			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_FLASH_ERRASE_ERROR:
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,CAN_PAYLOADSTRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_BOOT_REGION_SELECTION_ERROR:
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,CAN_PAYLOADSTRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_MEM_WR_ERROR:
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,CAN_PAYLOADSTRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
		break;

		case enmDiagnostic_MEM_WR_CHK_SUM_ERROR: //MEM sector written, but downloaded DATA and Written data in MEM Mismatch
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,CAN_PAYLOADSTRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_BOOT_REGION_JUMP_ERROR:
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,CAN_PAYLOADSTRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_IDLE:
			strcat((char *)systemPayload,"IDLE");
			strcat((char *)systemPayload,(char * )",");
			break;

		default:
			strcat((char *)systemPayload,"IDLE");
			strcat((char *)systemPayload,(char * )",");
			break;
	}//end of switch(DiagnosticStatus)

}//end of void DiagnosticString(char * systemPayload,enmDiagnosticStatus DiagnosticStatus)
void CANIDPrase(char * systemPayload)
{
	uint8_t CANIDloop = 1;

	char temp[10] = "";
	for(CANIDloop=1; CANIDloop <= NoOfCANID; CANIDloop++)
	{
		itoa(gu32CanConfigurationArray[CANIDloop],temp,CAN_PAYLOADSTRING_RADIX);
		strcat((char *)systemPayload,temp);
		strcat((char *)systemPayload,",");
	}
}

void PaylodTime(char * systemPayload)
{
	char tempArray[15];
#if(ENABLERTC == TRUE)
//		 Time Stamp		// added by 100229 26-03-2020
		memset((char *) tempArray, 0x00, sizeof(tempArray));
		Get_Time_Date();
  	   // HAL_RTC_GetTime(&hrtc, &STime1, RTC_FORMAT_BIN);
	//	HAL_RTC_GetDate(&hrtc, &SDate1, RTC_FORMAT_BIN);

		/*tempArray[0] = ((gDate.Year / 10) + 0x30);
		tempArray[1] = ((gDate.Year % 10) + 0x30);
		tempArray[2] = ((gDate.Month / 10) + 0x30);
		tempArray[3] = ((gDate.Month % 10) + 0x30);
		tempArray[4] = ((gDate.Date / 10) + 0x30);
		tempArray[5] = ((gDate.Date % 10) + 0x30);*/

		tempArray[0] = ((RTC_Bcd2ToByte(gDate.Year) / 10) + 0x30);
		tempArray[1] = ((RTC_Bcd2ToByte(gDate.Year) % 10) + 0x30);
		tempArray[2] = ((RTC_Bcd2ToByte(gDate.Month) / 10) + 0x30);
		tempArray[3] = ((RTC_Bcd2ToByte(gDate.Month) % 10) + 0x30);
		tempArray[4] = ((RTC_Bcd2ToByte(gDate.Date) / 10) + 0x30);
		tempArray[5] = ((RTC_Bcd2ToByte(gDate.Date) % 10) + 0x30);
		tempArray[6] = ' ';

		/*tempArray[7] = ((gTime.Hours / 10) + 0x30);
		tempArray[8] = ((gTime.Hours % 10) + 0x30);
		tempArray[9] = ((gTime.Minutes / 10) + 0x30);
		tempArray[10] = ((gTime.Minutes % 10) + 0x30);
		tempArray[11] = ((gTime.Seconds / 10) + 0x30);
		tempArray[12] = ((gTime.Seconds % 10) + 0x30);*/

		tempArray[7] = ((RTC_Bcd2ToByte(gTime.Hours) / 10) + 0x30);
		tempArray[8] = ((RTC_Bcd2ToByte(gTime.Hours) % 10) + 0x30);
		tempArray[9] = ((RTC_Bcd2ToByte(gTime.Minutes) / 10) + 0x30);
		tempArray[10] = ((RTC_Bcd2ToByte(gTime.Minutes) % 10) + 0x30);
		tempArray[11] = ((RTC_Bcd2ToByte(gTime.Seconds) / 10) + 0x30);
		tempArray[12] = ((RTC_Bcd2ToByte(gTime.Seconds) % 10) + 0x30);
		tempArray[13] = '\0';
		strcat((char *)systemPayload,(char * )tempArray);
		strcat((char *)systemPayload,(char * )",");
#endif
}
void updateInputVoltage(float Volt)
{
//	sprintf(gcSystemSupplyVoltage,"%.2f",gfInputSupplyVoltage);
	uint8_t Integer = 0;
	float Frac =0;
	uint16_t ftoi = 0;
	if(Volt > 0)
	{
		Integer = Volt;
		Frac = (Volt - (int)Volt);
		ftoi = Frac*100;

		if(Integer > 10)
		{
			voltage[0] = (Integer/10)+48;
			voltage[1] = (Integer%10)+48;
		}
		else
		{
			voltage[0] = '0';
			voltage[1] = Integer+48;
		}

		voltage[2] = '.' ;

		if(ftoi > 10)
		{
			voltage[3] = (ftoi/10)+48;
			voltage[4] = (ftoi%10)+48;
		}
		else
		{
			voltage[3] = '0';
			voltage[4] = ftoi+48;
		}
	}
	else
	{
		strcpy(voltage,"0.00");
	}

	//ftoa[0] = "1";

}
//******************************* End of File *******************************************************************
