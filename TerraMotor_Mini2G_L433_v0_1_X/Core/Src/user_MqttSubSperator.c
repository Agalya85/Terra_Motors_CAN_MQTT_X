/*
 * user_MqttSubSperator.c
 *
 *  Created on: Jun 8, 2023
 *      Author: admin
 */


#include "user_MqttSubSperator.h"

void SubRevicedString()
{//$,054061957514975180815242966,2,3,D1,1,@
 //$,054061957514975180815242966,7,"url",@$,054061957514975180815242966,1,7,"url",@
	uint16_t key = 0;
	char gau8TempSUBRequest[150] = {'0'};
	char *token = NULL;
	token = strtok(gau8SUBRequest,",");//$
	token = strtok(NULL,",");//HW
	token = strtok(NULL,",");//2way

	if(atoi(token) == 2) //2 All 2 way code
	{
		token = strtok(NULL,",");//Operation
		key = atoi(token);
	}
	else
	{

		key = atoi(token);
	}

	switch (key)
	{
		case enmSUBSTRINGINDETIFY_FOTA:
		{
			token = strtok(NULL,",");//Data
			memset(gau8TempSUBRequest,0x00,sizeof(gau8TempSUBRequest));
			memcpy(gau8TempSUBRequest, token, strlen(strtok(token,",")));

			const char *URL = strstr(gau8TempSUBRequest,"http");
			const char *BIN = strstr(gau8TempSUBRequest,".bin");
			const size_t mlen = BIN - URL;
//			gu32FotaRquestFlag = TRUE;
			if((URL != NULL) && (BIN != NULL))
			{
				memset(gau8FotaURL,0x00,sizeof(gau8FotaURL));
				memcpy(gau8FotaURL,URL,mlen+4);
				Diagnostic(enmDiagnostic_FOTA_REQ_RX);
				gu32FotaRquestFlag = TRUE;
				SUBTriggerFlag = FALSE;
			}
			else
			{
				memset(gau8FotaURL,0x00,sizeof(gau8FotaURL));
				memcpy(gau8FotaURL,gau8TempSUBRequest,strlen(gau8TempSUBRequest));
//			  Diagnostic(enmDiagnostic_FOTA_REQ_RX_ERROR);
			  SUBTriggerFlag = FALSE;
			}
		}
			break;

		case enmSUBSTRINGINDETIFY_DO:
//			token = strtok(NULL,",");
//			token = strtok(NULL,",");
//			key = atoi(token);
//			Diagnostic(enmDiagnostic_IMOBI_REQ_RX);
//			if(key == 1)
//			{
//				Current_Immobilization_State = TRUE;
//				if(Previous_Immobilization_State != Current_Immobilization_State)
//				{
//					WriteImobilizedStateToEEPROM(Current_Immobilization_State);
//					HAL_GPIO_WritePin(DO_uC_GPIO_Port, DO_uC_Pin, GPIO_PIN_SET);
//					Previous_Immobilization_State = Current_Immobilization_State;
//					Diagnostic(enmDiagnostic_IMOBI_SUCCESS_OK);
//				}
//				SUBTriggerFlag = FALSE;
//			}
//			else if(key == 0)
//			{
//				Current_Immobilization_State = FALSE;
////				Diagnostic(enmDiagnostic_IMOBI_REQ_RX);
//				if(Previous_Immobilization_State != Current_Immobilization_State)
//				{
//					WriteImobilizedStateToEEPROM(Current_Immobilization_State);
//					HAL_GPIO_WritePin(DO_uC_GPIO_Port, DO_uC_Pin, GPIO_PIN_RESET);
//					Previous_Immobilization_State = Current_Immobilization_State;
//					Diagnostic(enmDiagnostic_IMOBI_SUCCESS_OK);
//				}
//				SUBTriggerFlag = FALSE;
//			}
//			else
//			{
////				Current_Immobilization_State = key;
////				Diagnostic(enmDiagnostic_IMOBI_REQ_RX);
//			}

			break;
		default:
			SUBTriggerFlag = FALSE;
			break;
	}


	memset(gau8SUBRequest,0x00,sizeof(gau8SUBRequest));
}
