/*
 * user_can.h
 *
 *  Created on: Apr 20, 2021
 *      Author: admin
 */

#ifndef INC_USER_CAN_H_
#define INC_USER_CAN_H_

#define MAX_CAN_IDS_SUPPORTED (50)
#define CAN_BUFFER_LENGTH     (200)

typedef enum
{
	enmMCUMode_RUN = 0,
	enmMCUMode_SLEEPTRIGGER,
	enmMCUMode_SLEEP
}enmMCUMode;


typedef enum
{
	enmCANQUERY_IDLE = 0,
	enmCANQUERY_UPDATEQUERY,
	enmCANQUERY_SENDQUERY,
	enmCANQUERY_AWAITRESPONSE,
	enmCANQUERY_PASRERESPONSE,
	enmCANQUERY_RESPONSETIMEOUT
}enmCanQueryState;


typedef struct
{
	uint8_t u8CanMsgByte7;
	uint8_t u8CanMsgByte6;
	uint8_t u8CanMsgByte5;
	uint8_t u8CanMsgByte4;
	uint8_t u8CanMsgByte3;
	uint8_t u8CanMsgByte2;
	uint8_t u8CanMsgByte1;
	uint8_t u8CanMsgByte0;
}strCanReceivedMsg;

typedef union{
	uint16_t u16J1939Priority:5;
	uint16_t u16J1939PGN : 16;
	uint16_t u16J1939SA : 8;
	uint32_t u32J1939CommandId;
}unCan1939CommandId;

//typedef enum
//{
//	enmCANQUERY_IDLE = 0,
//	enmCANQUERY_UPDATEQUERY,
//	enmCANQUERY_SENDQUERY,
//	enmCANQUERY_AWAITRESPONSE,
//	enmCANQUERY_PASRERESPONSE,
//	enmCANQUERY_RESPONSETIMEOUT
//}enmCanQueryState;
///*
 /*
 BO_ 337 TEL_SoftwareVersion: 8 TEL
 SG_ TEL_Version : 55|16@0+ (1,0) [0|0] "" Vector__XXX
 SG_ TEL_Date : 39|16@0+ (1,0) [0|0] "" Vector__XXX
 SG_ TEL_Month : 23|16@0+ (1,0) [0|0] "" Vector__XXX
 SG_ TEL_Year : 7|16@0+ (1,0) [0|0] "" Vector__XXX

 * */
typedef union{
	struct{
		//uint32_t u8BGaussPack_1 : 7;		// bit 0 to 7
		uint32_t u32BGaussYear : 16;    // bit 0 to 15
		uint32_t u32BGaussMonth: 16;    // bit 16 to 31
		uint32_t u32BGaussDate: 16; 		// bit 32 to 47
		uint32_t u32BgaussVersion:16; // bit 48 to 63
	};
	uint8_t u8JBgaussCommandId[8];
}unCanBgaussCommandIdSoftwareVersion;

/*
   BO_ 336 TEL_Info: 8 TEL
   SG_ GPRS_Connections : 23|1@0+ (1,0) [0|0] "" Vector__XXX
   SG_ GPS_Connection : 15|1@0+ (1,0) [0|0] "" Vector__XXX
   SG_ TEL_HeartBeat : 7|1@0+ (1,0) [0|1] "" VCU
*/

typedef union{
	struct{
		uint8_t u8BGaussPack_1 : 7;		// bit 0 to 6
		uint32_t u32BGaussHeartBeat: 1;    // bit 7 to 7
		uint32_t u32BGaussPack_2: 7;    // bit 8 to 14
		uint32_t u8BGaussGPSConnection: 1; 		// bit 15 to 15
		uint32_t u32BGaussPack_3: 7;    // bit 16  to 22
		uint32_t u8JBGaussGprsConnection:1; // bit 23 to 23
		};
	uint8_t u32JBgaussCommandId[8];
}unCanBgaussCommandIdTelInfo;

typedef struct
{
	uint8_t u8CanTxMsgByte7;
	uint8_t u8CanTxMsgByte6;
	uint8_t u8CanTxMsgByte5;
	uint8_t u8CanTxMsgByte4;
	uint8_t u8CanTxMsgByte3;
	uint8_t u8CanTxMsgByte2;
	uint8_t u8CanTxMsgByte1;
	uint8_t u8CanTxMsgByte0;
}strCanTransmitMsg;



void canFilterConfig(void);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
uint32_t isCommandIdConfigured(uint32_t canId);
void parseCanMessageQueue(void);
void executeCANQueries(void);
void updateCANQuery(void);
void sendMessageCAN (void);
void executeUpdateCANQuery(void);
#endif /* INC_USER_CAN_H_ */
