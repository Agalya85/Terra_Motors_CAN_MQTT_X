/*
 * externs.h
 *
 *  Created on: Apr 19, 2021
 *      Author: admin
 */

#ifndef INC_EXTERNS_H_
#define INC_EXTERNS_H_

#include "queue.h"
#include "gsmSin868.h"
#include "user_eeprom.h"
#include "user_rtc.h"
#include "applicationdefines.h"
#include "user_can.h"

extern strctGSM gsmInstance;

extern RTC_TimeTypeDef STime1;
extern RTC_DateTypeDef SDate1;
extern RTC_HandleTypeDef hrtc;
extern I2C_HandleTypeDef hi2c1;
extern CAN_HandleTypeDef hcan1;
extern CAN_TxHeaderTypeDef   TxHeaderImmobilizer;

extern RTC_TimeTypeDef GTime1;
extern RTC_DateTypeDef GDate1;

extern RTC_TimeTypeDef gTime ;
extern RTC_DateTypeDef gDate ;

extern uint32_t gu32GSMPowerStateFlag;


extern RTC_DateTypeDef lastKnownSDate1;

extern CAN_TxHeaderTypeDef   TxHeader;
extern CAN_RxHeaderTypeDef   RxHeader;
extern uint32_t              TxMailbox;
extern uint8_t               TxData[8];


//uint32_t gu32ImmobilizerCommandID ;
//uint8_t gau32ImmobilizerDataOn[8];
//uint8_t gau32ImmobilizerDataOff[8];

extern strctGSM gsmInstance;
extern strctQUEUE gsmPayload;
extern strctMemoryLayout strI2cEeprom;
extern strTimeElapsedAfterSync strTimeUpdate;

extern uint64_t gu64CanMessageArray[MAX_CAN_IDS_SUPPORTED];
extern strCanReceivedMsg unCanReceivedMsgs[CAN_BUFFER_LENGTH];

extern volatile uint32_t gu32GSMHangTimer;
extern volatile uint32_t u8GSMCharRcv;
extern char dinfo[50];

extern uint8_t gu8OperateSystemStatus;

extern volatile uint32_t gu32GSMCharacterTimeout;
extern volatile uint32_t gu32FotaFileReadTimer;
extern uint32_t  gu32TimeSyncFlag;
extern volatile uint32_t gu32I2CMemoryOperationTimeout;

extern char gau8Year[5];
extern char gau8Month[3];
extern char gau8Date[3];
extern char gau8Hour[3];
extern char gau8Minutes[3];
extern char gau8Seconds[3];

extern uint32_t urlLength;

extern char gu8NewURL[150];
extern char gau8GSM_url[150];
extern char gau8GSM_smsto[15];

extern uint8_t gu32MemoryOperation;
extern volatile uint8_t gu8MemoryCycleComplete;
extern uint8_t gu8RestoreSystemSettings;
extern volatile uint32_t gu32PayloadQueueEnqueue;
extern volatile uint32_t gu8rtcUpdateTimer;
extern volatile uint32_t gu32TimeSyncVariable;
extern volatile uint32_t gu32LEDDelay;
extern uint8_t gu8SignatureReadFlag;
extern volatile uint32_t u32DefautParamWriteStatus;

extern uint32_t gu32ImmobilizerCommand;
extern uint32_t gu32CurrImmobilizerState;
extern uint8_t  gu8PreImmobilizerState;
extern uint32_t gu32ImmobilizerActivatedFlag;
extern uint32_t u32GPSTimeSyncFlag;
extern volatile uint32_t gu32ImmobilizerReadTimer;
/*CAN*/
extern volatile uint32_t gu32CANQueryPollTimer;
extern volatile uint32_t gu32CANCommandResponseTimeout;

extern uint32_t gu32CanQueryCounter;

extern uint64_t gu64CanMessageArray[MAX_CAN_IDS_SUPPORTED];

extern unCanBgaussCommandIdSoftwareVersion unSoftwaredetails;
extern unCanBgaussCommandIdTelInfo unTelInfo;


/*MCU Mode*/
extern enmMCUMode gu32MCUCurrentMode;
extern enmMCUMode gu32MCUCurrentWorkingMode;
extern volatile uint32_t gu32KeyDataFlag;
extern uint8_t gua8KeyData[8];
extern uint32_t gu32SleepModeTimer;
extern uint32_t gu32SleepModeTimer1;
extern uint32_t gu32MCUModeExitFlag;
extern uint32_t gu32InputSupplySwitchedAlert;

extern uint32_t gu32InitSleepModeFlag;

//100355
extern uint8_t u8ExSleepModeFlag;
extern uint32_t gu32CANReqResponseTimeout;
extern uint8_t gu8CANRxResponseFlag;
extern uint8_t u8WakeupCmdFlag;
extern enmCanQueryState canCurrentState;
extern uint8_t RxData[8];
extern uint8_t gua8KeyData[8];
extern uint8_t u8PreRxData;
extern uint8_t u8PreTxData;
extern strCanTransmitMsg unCanTransmitMsgs;
extern uint8_t u8DefaultEncryptFailCnt;
extern uint8_t u8GenSeedFlag;
extern uint8_t updateCANQueryState;
extern uint16_t u16Tel_VCU_Key;
extern uint16_t u16VCU_Tel_Key;
extern uint8_t u8keyVerifyErr;
extern uint8_t KeyVerifyPassFlag;
extern uint8_t u8EncryptionStatusFlag;

extern uint32_t gu8NoCANDATATimeOut;
extern uint8_t u8keyConnState;
extern uint8_t u8keyConnStateFlag;
extern uint32_t u32KeyOffStableTimeout;
extern uint32_t gu32ImmobilizerCommand;
extern uint8_t u8LOCKFlag;
extern uint8_t u8UNLOCKFlag;

extern uint32_t gu32FotaInitTime;
extern uint32_t u32FotaChunckLength;
extern char gau8SUBRequest[150];
extern uint8_t SUBTriggerFlag;
extern uint32_t gu32FotaRquestFlag;
extern uint32_t MCU_Id;
extern char gau8FotaURL[100];

extern uint8_t gu8PowerOnFlag;
extern uint8_t NoOfCANID;
extern uint8_t fotaPloadFlag;
extern uint32_t gu32CanConfigurationArray[(MAX_CAN_IDS_SUPPORTED + 1)];

/*FOTA*/
extern _Bool FOTACompleteFlag;
extern char gu32FotaFileSizeinBytes[8];

extern uint32_t gu32SystemResetTimer;

extern float gfInputSupplyVoltage;
extern float gfBatteryVoltage;
//extern  volatile uint32_t gu32TimeSyncVariable;
//extern uint32_t  gu32TimeSyncFlag;
#endif /* INC_EXTERNS_H_ */
