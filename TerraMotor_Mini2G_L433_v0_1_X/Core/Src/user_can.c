

#include "main.h"
#include "stm32l4xx_hal.h"
#include "user_can.h"
#include "externs.h"
#include "applicationdefines.h"
#include "error_handler.h"
#include "string.h"
#include "timer.h"
#include "lptim.h"
#include "user_rtc.h"
#include "user_flash.h"

#define NORMAL 0
#define SILENT 1
/*To be used for request response model*/
#define CAN_QUERY_COUNT (8)

CAN_TxHeaderTypeDef   TxHeaderImmobilizer;

CAN_TxHeaderTypeDef   TxHeader;
CAN_RxHeaderTypeDef   RxHeader;
CAN_FilterTypeDef  sFilterConfig;

/* For Immobilizer*/

uint32_t              TxMailbox;
uint8_t               TxData[8];

uint32_t gu32CANQueryCommandResponseReceivedFlag = 0;
enmCanQueryState canCurrentState = enmCANQUERY_IDLE;
uint32_t gu32CanQueryCounter = 0;

uint32_t gu8NoCANDATATimeOut = 0;

uint32_t gu32CurrImmobilizerState = 0;
uint8_t gu8PreImmobilizerState = 0;

uint8_t u8LOCKFlag = FALSE;
uint8_t u8UNLOCKFlag = FALSE;

uint32_t gu32ImmobilizerCommandID = 0x801F4F5;
uint8_t gau32ImmobilizerDataOn[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
uint8_t gau32ImmobilizerDataOff[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


//uint32_t gu32CanQueryArray[15] ={ 0x18900140,0x18910140,0x18920140,0x18930140,0x18940140,
//								  0x18950140,0x18960140,0x18970140,0x18980140
//								};

uint32_t gu32CanQueryArray[15] ={ 336,337,1024,0,0,0,0,0,0	};

/* Configuration Array for Can Peripheral
 * Prerequisite : [0] - Will / Should always contain ONLY CAN BaudRate / Bit Rate !
 * CommandIds   : [1 to 50] Sequential list of CAN Command Ids to be captured .
 * Defaults / Example : { 500, 1, 2, 3, 0x3AD,0x1FF,6,7,0x7FF,9  ,0x3AB};
 *                        BR ,Id,Id,Id, Id  , Id  ,Id , Id  ,Id , Id
 * */


//Phase3

#define CAN_MODE  (NORMAL)//(SILENT)
uint32_t gu32CanConfigurationArray[(MAX_CAN_IDS_SUPPORTED + 1)] = { 500,
																	0x15257B01,0x1801A1F0,0x1801A1FE,0x1801A1FF,0x1801A201,
																	0x1801A1FC,0x14520902,0x18530902,0x1806B4C4,
																	2,2,2,2,2,
																	2,2,2,2,2,
																	2,2,2,2,2,
																	2,2,2,2,2,
//																	2,2,2,2,2,0x14520902x,0x18530902x,
//																	2,2,2,2,2,
//																	2,2,2,2,2,
//																	2,2,2,2,2,
															 	   };








uint8_t NoOfCANID = 35;



uint8_t               TxData[8];
uint8_t               RxData[8];
uint32_t              TxMailbox;
uint32_t u32CanIdsReceived[CAN_BUFFER_LENGTH] = {'0'};
uint32_t u32CanRxMsgLoopCounter = 0;
uint32_t canTestVariable = 0;
uint32_t u32IdSearchFlag = FALSE;
uint64_t gu64CanMessageArray[MAX_CAN_IDS_SUPPORTED] = {0};
uint64_t u64CanMessageReceived[CAN_BUFFER_LENGTH] = {0};

volatile uint32_t gu32ProcessCanMessage = 0;

strCanReceivedMsg unCanReceivedMsgs[CAN_BUFFER_LENGTH];


/*J1939*/
unCan1939CommandId unCanIdsReceived[CAN_BUFFER_LENGTH] = {0};

unCanBgaussCommandIdSoftwareVersion unSoftwaredetails;
unCanBgaussCommandIdTelInfo unTelInfo;

/* Bgauss mode exit response */
uint32_t gu32MCUModeExitFlag = 0; // 0 - Run Mode , 1- mode switch alert
volatile uint32_t gu32KeyDataFlag = 0;
uint32_t gu32InitSleepModeFlag = 0;


uint8_t gua8KeyData[8];

//100355
uint8_t u8ExSleepModeFlag = 1;
uint8_t u8WakeupCmdFlag = 0;
uint8_t u8PreRxData = 0;
uint8_t u8PreTxData = 0;
strCanTransmitMsg unCanTransmitMsgs;
uint8_t updateCANQueryState = 99;
uint8_t u8keyVerifyErr = 0;
uint8_t KeyVerifyPassFlag = 0;
uint8_t VerifyDoneFlag = 0;
uint8_t u8EncryptionStatusFlag = 0;

uint8_t u8keyConnState = 99;
uint8_t u8keyConnStateFlag = 99;
uint32_t u32KeyOffStableTimeout = 0;

/****************************************************************************
 Function: canFilterConfig
 Purpose: Init CAN peripheral with filter configuration
 Input: None.

 Return value: None

 Refer Link for timing calculations :
 http://www.bittiming.can-wiki.info/

 Clock = 48 Mhz (Refer Clock Configuration in CubeMX for details)

 Bit Rate    Pre-scaler  time quanta  Seg 1  Seg 2   Sample Point
 kbps

 1000			3			16			13	   2	    87.5
 500			6			16			13     2		87.5
 250			12			16			13     2    	87.5
 125			24			16			13     2		87.5
 100			30			16			13     2		87.5
 83.33			36			16			13     2		87.5
 50				60			16			13     2		87.5
 20				150			16			13     2		87.5
 10				300			16			13     2		87.5


 Note(s)(if-any) :

 Change History:
 Author            	Date                Remarks
 KloudQ Team        22/03/2020			initial Definitions
 kloudq				27/03/2020			Bit Calculation Added
 kloudq				20/04/2021			Added support for STM32L433 MCU
******************************************************************************/
void canFilterConfig(void)
{
	/*##-1- Configure the CAN peripheral #######################################*/
	/*##-1- Configure the CAN peripheral #######################################*/
	hcan1.Instance = CAN1;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.AutoBusOff = ENABLE;
	hcan1.Init.AutoWakeUp = ENABLE;
	hcan1.Init.AutoRetransmission = ENABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TransmitFifoPriority = ENABLE;

#if(CAN_MODE == NORMAL)
	hcan1.Init.Mode = CAN_MODE_NORMAL;
#elif(CAN_MODE == SILENT)
	hcan1.Init.Mode = CAN_MODE_SILENT;
#endif
	/* Seg 1 and Seg 2 are kept constant for all bit rates .
	 * Only Prescaler value will change . Refer table in notes */
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;

	switch(gu32CanConfigurationArray[0])
	{
		case 1000:
			hcan1.Init.Prescaler = 3;
		break;
		case 500:
			hcan1.Init.Prescaler = 6;
		break;
		case 250:
			hcan1.Init.Prescaler = 12;
		break;
		case 125:
			hcan1.Init.Prescaler = 24;
		break;
		case 100:
			hcan1.Init.Prescaler = 30;
		break;
		case 83:
			hcan1.Init.Prescaler = 36;
		break;
		case 50:
			hcan1.Init.Prescaler = 60;
		break;
		case 20:
			hcan1.Init.Prescaler = 150;
		break;
		case 10:
			hcan1.Init.Prescaler = 300;
		break;
		default:
		/* Illegal BaudRate Configured . Use Default 500 Kbps */
			hcan1.Init.Prescaler = 6;
		break;
	}

	if (HAL_CAN_Init(&hcan1) != HAL_OK)
		assertError(enmTORERRORS_CAN1_INIT,enmERRORSTATE_ACTIVE);
	else
		 assertError(enmTORERRORS_CAN1_INIT,enmERRORSTATE_NOERROR);

	/*##-2- Configure the CAN Filter ###########################################*/
	  sFilterConfig.FilterBank = 0;
	  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	  sFilterConfig.FilterIdHigh = 0x0000;
	  sFilterConfig.FilterIdLow = 0x0000;
	  sFilterConfig.FilterMaskIdHigh = 0x0000;
	  sFilterConfig.FilterMaskIdLow = 0x0000;
	  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	  sFilterConfig.FilterActivation = ENABLE;
	  sFilterConfig.SlaveStartFilterBank = 14;

	if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
		assertError(enmTORERRORS_CAN1_CONFIGFILTER,enmERRORSTATE_ACTIVE);
	else
		assertError(enmTORERRORS_CAN1_CONFIGFILTER,enmERRORSTATE_NOERROR);

	/*##-3- Start the CAN peripheral ###########################################*/
	  if (HAL_CAN_Start(&hcan1) != HAL_OK)
	  {
	    /* Start Error */
	    Error_Handler();
	  }

	/*##-4- Activate CAN RX notification #######################################*/
	  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	  {
		/* Notification Error */
		  assertError(enmTORERRORS_CAN1_CONFIGFILTER,enmERRORSTATE_ACTIVE);
	  }
//	  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK)
//	  {
//		/* Notification Error */
//		  assertError(enmTORERRORS_CAN1_CONFIGFILTER,enmERRORSTATE_ACTIVE);
//	  }

	  /*##-5- Configure Transmission process #####################################*/
	TxHeader.StdId = 0x321;
	TxHeader.ExtId = 0x01;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_EXT;
	TxHeader.DLC = 2;
	TxHeader.TransmitGlobalTime = ENABLE;

	unSoftwaredetails.u32BGaussMonth = 7;// 5(dec) converted to Hex
	unSoftwaredetails.u32BGaussYear = 22; // 22(dec) converted to Hex
	unSoftwaredetails.u32BGaussDate = 13; // 13(dec) converted to Hex
	unSoftwaredetails.u32BgaussVersion = 30001;// 3.0.x16(dec) to Hex

	unTelInfo.u32BGaussHeartBeat = 1;
	gua8KeyData[0] = 0x01;


	memset(u32CanIdsReceived,0x00,sizeof(u32CanIdsReceived));
	memset(unCanReceivedMsgs,0x00,sizeof(unCanReceivedMsgs));

}


/****************************************************************************
 Function: HAL_CAN_RxCpltCallback
 Purpose:  Rx complete callback in non blocking mode
 Input:	   CanHandle: pointer to a CAN_HandleTypeDef structure that contains
           the configuration information for the specified CAN.

 Return value: None
 Note(s)(if-any) :


 Change History:
 Author            	Date                Remarks
 KloudQ Team        20/04/21			initial code
******************************************************************************/
extern uint32_t gu32GSMPowerStateFlag;
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	/* LED Only for testing/ Indication . Can be removed in production if not required  */

	/* Get RX message . Exit Sleep Mode */
//	memset(TxData,0x00,sizeof(TxData));
//	gu32CANQueryCommandResponseReceivedFlag = TRUE;
	HAL_GPIO_TogglePin(LED_COMM_GPIO_Port, LED_COMM_Pin);
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
	{
		/* Reception Error */
		Error_Handler();
	}

	gu8NoCANDATATimeOut = THREE_SEC;

	/* Parse the incoming data only if array location is available
		 * Added on 3/3/21 - For payload overwrite issue */
	if(u32CanIdsReceived[u32CanRxMsgLoopCounter] == 0)
	{
		if(RxHeader.IDE == CAN_ID_EXT)
		{
			u32CanIdsReceived[u32CanRxMsgLoopCounter] = RxHeader.ExtId;
			unCanIdsReceived[u32CanRxMsgLoopCounter].u32J1939CommandId = RxHeader.ExtId;
		}
		else if(RxHeader.IDE == CAN_ID_STD)
		{
			u32CanIdsReceived[u32CanRxMsgLoopCounter] = RxHeader.StdId;
			unCanIdsReceived[u32CanRxMsgLoopCounter].u32J1939CommandId = RxHeader.StdId;
		}

		//Motorola format
		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte0 = (RxData[7]);
		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte1 = (RxData[6]);
		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte2 = (RxData[5]);
		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte3 = (RxData[4]);
		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte4 = (RxData[3]);
		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte5 = (RxData[2]);
		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte6 = (RxData[1]);
		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte7 = (RxData[0]);
		u32CanRxMsgLoopCounter++;
	}

	
	if(u32CanRxMsgLoopCounter >= CAN_BUFFER_LENGTH)
		u32CanRxMsgLoopCounter = 0;

}

/****************************************************************************
 Function: HAL_CAN_ErrorCallback
 Purpose:  CAN Error
 Input:	   hcan: pointer to a CAN_HandleTypeDef structure that contains
           the configuration information for the specified CAN.

 Return value: None
 Note(s)(if-any) :


 Change History:
 Author            	Date                Remarks
 KloudQ Team        20/04/21			initial code
******************************************************************************/
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	canTestVariable = hcan->ErrorCode;
	HAL_CAN_DeInit(&hcan1);
	canFilterConfig();
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{

}
/******************************************************************************
 Function Name: isCommandIdConfigured
 Purpose: Checks if received Id is configured for CAN
 Input:	uint32_t canId

 Return value: uint32_t u32PositioninConfigArray - Id Position in configuration Array
												 - 0 If Id not configured
 Notes:
 How to use this function for Testing

uint32_t idIndex = 0;
	do
	{
		idIndex = isCommandIdConfigured(0x0803FF00);
	}while(u32IdSearchFlag != 2);

	u32IdSearchFlag = 0;

 Change History:

 Author           	Date                Remarks
 KTL   				18-5-2020			Initial Draft . Tested All conditions
******************************************************************************/
uint32_t isCommandIdConfigured(uint32_t canId)
{
	static uint32_t LoopCounter = 0;
	static uint32_t u32PositioninConfigArray = 0;

	if(LoopCounter == 0)
	{
		u32IdSearchFlag = 1;
		u32PositioninConfigArray = 0;
	}

	if(u32IdSearchFlag == 1)
	{
		if(gu32CanConfigurationArray[LoopCounter] == canId)
		{
			/*
			 * If Received CanID is found in configuration Array
			 * then parse the frame else ignore .
			 */
			u32PositioninConfigArray = LoopCounter;
			u32IdSearchFlag = 2;
			LoopCounter = 0;
		}
		else
		{
			LoopCounter++;
			if (LoopCounter == MAX_CAN_IDS_SUPPORTED)
			{
				LoopCounter = 0;
				u32IdSearchFlag = 2;
			}
		}
	}
	return u32PositioninConfigArray;
}

/******************************************************************************
 Function Name: parseCanMessageQueue
 Purpose: Parse CAN Message . If command ID is configured store the message
 Input:	None
 Return value: None

 Notes:

 Change History:

 Author           	Date                Remarks
 KTL   				19-5-2020			Initial Draft . Tested All conditions
 KTL				27-5-2020			Convert to Array Logic . Tested
******************************************************************************/
uint32_t temp = 0;
uint32_t gu32CanIdParserCounter = 0;
void parseCanMessageQueue(void)
{
	static uint32_t u32CanMsgID = 0;
	static uint32_t u32ParserState = 0;
	static uint32_t u32IdStatus = 0;

	if(u32CanIdsReceived[gu32CanIdParserCounter] != 0)
	{
		if(u32ParserState == 0)
		{
			/* Message Available. Parse The Message */
			u32CanMsgID = u32CanIdsReceived[gu32CanIdParserCounter];
			u32ParserState = 1;
		}
		else if(u32ParserState == 1)
		{
			/* In Process */
			if(u32IdSearchFlag == 2)
			{
				/* Search Process Completed */
				if(u32IdStatus != 0)
				{

					gu64CanMessageArray[u32IdStatus] =  ((uint64_t)unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte7 << 56)|
														((uint64_t)unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte6 << 48)|
														((uint64_t)unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte5 << 40)|
														((uint64_t)unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte4 << 32)|
														((uint64_t)unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte3 << 24)|
														((uint64_t)unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte2 << 16)|
														((uint64_t)unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte1 << 8) |
														((uint64_t)unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte0);
					if(gu64CanMessageArray[u32IdStatus] == 0)
					{
						gu64CanMessageArray[u32IdStatus] = 0;
					}

					// Reset Array Value for new Message
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte7 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte6 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte5 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte4 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte3 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte2 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte1 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte0 = 0;
					u32CanIdsReceived[gu32CanIdParserCounter] = 0;
					u32IdStatus = 0;
				}
				else if(u32IdStatus == 0)
				{
					/* Command Id is not Configured . Discard the Message*/
//					gu64CanMessageArray[gu32CanIdParserCounter] = 0;
					//u64CanMessageReceived[gu32CanIdParserCounter] = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte7 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte6 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte5 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte4 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte3 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte2 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte1 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte0 = 0;
					u32CanIdsReceived[gu32CanIdParserCounter] = 0;
				}
				u32IdSearchFlag = 0;
				u32ParserState = 0;
				u32CanMsgID = 0;
				gu32CanIdParserCounter++;
			}
			else
				u32IdStatus = isCommandIdConfigured(u32CanMsgID);
		}
	}
	else
	{
		gu32CanIdParserCounter++;
	}
	if(gu32CanIdParserCounter >= CAN_BUFFER_LENGTH)
		gu32CanIdParserCounter = 0;
}
