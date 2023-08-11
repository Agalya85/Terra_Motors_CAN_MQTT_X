/*
 * gsmSin868.h
 *
 *  Created on: Apr 19, 2021
 *      Author: admin
 */

#ifndef INC_GSMSIN868_H_
#define INC_GSMSIN868_H_

#include "user_error.h"
#define DEFAULT_APN				"airteliot.com"
#define DEFAULT_URL				"http://piaggioapi.remotemonitor.in/api/values/PostStringData"
#define DEFAULT_PHNO			"+918669666703"
#define DEFAULT_UPFREQON		(60)								/* Default Up Freq in ON mode */
#define DEFAULT_UPFREQOFF		(300)								/* Default Up Freq in OFF mode*/

#define GPSTRANGULATION (0)
#define GPSGNSSCHIP		(1)

#define GPSDATATYPERMC  (2)
#define GPSDATATYPERECK (3)

#define GPSTYPE					(GPSGNSSCHIP)
#define GPSDATATYPE				(GPSDATATYPERMC)

extern uint8_t gu8UpdateLoc;

extern char gau8GSM_apn[100];
extern char TIME_STRING[20];

extern uint32_t GSMQueueEmptyFlag;
extern uint32_t GSMQueueFullFlag;
/************************************** GSM Defines **************************************************************/
#define GSM_MAX_RETRY 				(3)
#define GPS_MAX_RETRY 				(5)
#define PAYLOAD_DATA_STRING_RADIX   (10) 		/* 10-> Decimal 2-> Binary 16-> Hex */
#define GSM_RESPONSE_ARRAY_SIZE     (2500)
#define GPS_LOCATION_ARRAY_SIZE     (150)
#define GSM_SIGSTRGTH_ARRAY_SIZE    (10)
#define GPRS_NETWORKIP_ARRAY_SIZE   (35)
#define GSM_TOTAL_COMMANDS		    (30)
#define GSM_HTTP_RESPONSE_CODE_SIZE (8)
#define GSM_PAYLOAD_MAX_SIZE        (1500) //600
#define MAX_HTTP_ATTEMPTS			(10)		/* Max Http retry for illegal response code */
#define GSM_ARRAY_INIT_CHAR  		(0x00)
#define DATA_UNKNOWN				'?'
#define SMS_MAX_MSG_LENGTH			(800)
#define SMS_MOB_NO_LENGTH			(13)
#define GSM_END_OF_SMS_MARKER       (0x1A)
#define KLOUDQ_SIGNATURE            "Tor by Kloudq \r\n"

typedef enum
{
	enmGSMTASK_RESET = 0,			/* Task : Reset Module*/
	enmGSMTASK_INITMODULE,		/* Task : Init Module */
	enmGSMTASK_UPDATELOCATION,	/* Task : Update GPS Location */
	enmGSMTASK_UPLOADDATA,		/* Task : GPRS Data Upload */
	enmGSMTASK_READSMS,			/* Task : Poll for new SMS */
	enmGSMTASK_SENDSMS,			/* Task : Send SMS */
	enmGSMTASK_ISALIVE,			/* Task : HeartBeat*/
	enmGSMTASK_GETDATA,			/* Task : Fetch Data from Server */
	enmGSMTASK_DOWNLOADFOTAFILE, /* Task : Fetch FOTA File from Server */
	enmGSMTASK_IDLE				/* Task : Idle  */
}enmGSMTask;

typedef enum
{
	/* Module Initialization Commands */
	enmGSMSTATE_AT = 0,
	enmGSMSTATE_ATE0,

	/* GPS Sequence*/
	enmSTATE_GPSGPIOEN,
	enmGSMSTATE_GPSPOWERON,
	enmGSMSTATE_GPSCGNSSEQ,
	enmGSMSTATE_GPSBR,
	enmGSMSTATE_GPSDATAEN,
	/* Testing GPS */
	enmGSMSTATE_GPSCGNSINF,
	/* Temp Fetch Time */
	enmGSMSTATE_CREG,
	enmGSMSTATE_CGACT,
//	enmGSMSTATE_ATCPIN,
//	enmGSMSTATE_ATCSMINS,
	enmGSMSTATE_ATCSQ,

	enmGSMSTATE_GETTIMESTAMP,
//	enmGSMSTATE_CMEE,
	enmGSMSTATE_SAPBR3,
	enmGSMSTATE_SAPBRAPN,
	enmGSMSTATE_SAPBR1,
	enmGSMSTATE_SAPRBIP,

	enmGSMSTATE_CSCS,

	enmGSMSTATE_HTTPINIT,
	enmGSMSTATE_HTTPPARACID,
	enmGSMSTATE_HTTPTIMEOUT,
	enmGSMSTATE_HTTPUA,
	enmGSMSTATE_HTTPSSL_EN,
	enmGSMSTATE_TIMESTAMPEN,
	enmGSMSTATE_TIMESTANPSAVE,

	/*MQTT Commands*/
	enmGSMSTATE_ATSMURL,
	enmGSMSTATE_ATSMCLIENTID,
	enmGSMSTATE_ATSMCONFSESSION,
	enmGSMSTATE_ATSMCONN,
	enmGSMSTATE_ATAMSUB,
	enmGSMSTATE_ATCMPUB, /* MQTT Pub Sub*/


	/* HTTP Sequence */
////	enmGSMSTATE_HTTPINIT,
////	enmGSMSTATE_HTTPPARACID,
//	enmGSMSTATE_HTTPPARAURL,
////	enmGSMSTATE_HTTPTIMEOUT,
////	enmGSMSTATE_HTTPUA,
////	enmGSMSTATE_TIMESTAMPEN,
////	enmGSMSTATE_TIMESTANPSAVE,
//	/*Temp Time fetched at start */
//	//enmGSMSTATE_GETTIMESTAMP,
////	enmGSMSTATE_ATHTTPGETBREAK,
//	enmGSMSTATE_ATHTTPDATACOMMAND,
	enmGSMSTATE_SENDDATA,
//	enmGSMSTATE_HTTPACTION,
//	enmGSMSTATE_HTTPTERM,
	/* GPS Commands */
//	enmGSMSTATE_GPSCGNSINF,
	enmGSMSTATE_GPSPOWEROFF,
	/* Send SMS */
	enmGSMSTATE_CMGF,
	enmGSMSTATE_SMSCMGS,
	enmGSMSTATE_SENDSMS,
	enmGSMSTATE_SMSEOM,
	/* Read SMS */
	enmGSMSTATE_READMODE,
	enmGSMSTATE_READSMS,
	enmGSMSTATE_DELETESMS,
	/*Fota*/
	enmGSMSTATE_ATHTTPGETBREAK,
	enmGSMSTATE_SETFOTAURL,
//	enmGSMSTATE_HTTPSSL_EN,
	enmGSMSTATE_FOTAHTTPACTION,
	enmGSMSTATE_READFILE,
	/*Retore URL*/
	enmGSMSTATE_RESTORE_URL_ATHTTPGETBREAK,
	enmGSMSTATE_RESTORE_URL,
//	enmGSMSTATE_HTTPSSL_DIS,
}enmGSMState;

typedef enum
{
	enmGPSSTATE_DEVPOWER = 0,
	enmGPSSTATE_AT,					/* Task : HeartBeat*/
	enmGPSSTATE_ATE0,				/* Task : Module Initialise*/
	enmGPSSTATE_POWER,
	enmGPSSTATE_CGNSSEQ,
	enmGPSSTATE_CGNSINF,
	enmGPSSTATE_CHECKRESPONSE
}enmGPSState;

typedef enum
{
	enmGSM_SENDCMD = 0,
	enmGSM_CHKRESPONSE
}enmGSMTaskState;

typedef enum
{
	enmGSM_PWRNOTSTARTED = 0,
	enmGSM_PWRSTARTED,
	enmGSM_PWRCOMPLETED

}enmGSMPowerState;

typedef enum
{
	enmGSM_CMDNOTSTARTED = 0,
	enmGSM_CMDSEND,
	enmGSM_CMDINPROCESS,
	enmGSM_CMDSUCCESS,
	enmGSM_CMDRESPONSEERROR,
	enmGSM_CMDTIMEOUT,
}enmGSMCMDState;

typedef struct
{
	char agsmSMSRecipient[SMS_MOB_NO_LENGTH];								/* 10 or 13 Digit Mobile Number */
	char agsmSMSMessageBody[SMS_MAX_MSG_LENGTH];							/* Stores Message to send */
	uint8_t u8NewMessage;													/* Message State 0 : New 1 : Old */
}strctSMS;

typedef struct
{
	char agpsLocationData[GPS_LOCATION_ARRAY_SIZE];         				/* Stores GPS Data */
	char agsmSignalStrength[GSM_SIGSTRGTH_ARRAY_SIZE];						/* Stores Signal Strength */
	char agsmNetworkIP[GPRS_NETWORKIP_ARRAY_SIZE];							/* Stores Network IP after connecting to gprs n/w*/
	char agsmCommandResponse[GSM_TOTAL_COMMANDS][GSM_TOTAL_COMMANDS];		/* Stores Command Response Bug : 18-01-2019 Timer does not work when GSM_RESPONSE_ARRAY_SIZE is changed*/
	char agsmHTTPRequestStatus[GSM_HTTP_RESPONSE_CODE_SIZE]	;				/* Stores HTTP Response code */
	char u32GSMHttpResponseCode[3];
	volatile char as8GSM_Response_Buff[GSM_RESPONSE_ARRAY_SIZE];			/* Stores Response Received from module */

	uint8_t u8gsmSIMReadyStatus;											/* Tracks Sim Ready Status */
	uint8_t u8gsmRegistrationStatus;										/* Tracks Sim Registration */
	uint8_t u8gsmRetryCount;   												/* Defines Max retry attempts for each command */
	uint8_t u8isConnected;													/* Registered to network */
	uint8_t u8HTTPInitStatus;												/* HTTP Status */
	uint8_t u8AttemptFota;
	uint8_t u8IncrementGsmState;
	uint8_t u8IllegalHttpResponseCounter;

	uint16_t u8LastHttpResponseCode;

	volatile uint32_t u32GSMTimer;											/* GSM Delay */
	uint32_t u32ONPayloadUploadFreq;										/* Data upload freq : System ON*/
	uint32_t u32OFFPayloadUploadFreq;										/* Data upload freq : System OFF*/
	uint32_t u32GSMHeartbeatTimer;
	uint32_t u32FotaFileSizeBytes;

	volatile uint32_t u32GSMResponseTimer;											/* GSM Response Delay */
	volatile uint32_t u8GSM_Response_Character_Counter;								/* Tracks received characters */
	volatile enmGSMTaskState enmGSMCommandResponseState;								/* GSM State response vs command*/
	volatile enmGSMCMDState enmGSMCommandState;										/* Command Tracker */
	volatile enmGSMTask enmcurrentTask;												/* Tracks On Going Task*/
	enmGSMState enmGSMCommand;												/* Tracks GSM Command Number */
	enmGSMPowerState enmGSMPwrState;										/*Tracks PWRKEY Operations*/
	strctSMS strSystemSMS;
}strctGSM;

typedef struct
{
	char * atCommand;														/* AT Command to Send */
	const char * atCommandResponse;											/* Expected Command Response*/
	uint32_t msTimeOut;														/* Response Time out in ms*/
	strctGSM * GSMInstance;

}strctGSMStateTable;


/*GSM Function Prototypes*/
void GSM_CharReception_Callback(void);
void gsmCommandsFSM(void);
void sendGSMCommand();
void updateHttpDataLength();
void initGSMSIM868();
void operateGSMSIM868();
strctGSM * getGSMInstance();
void sendSystemConfigurationSMS(void);
void updatePhoneNumber(void);
void strreplace(char s[], char chr, char repl_chr);
void extractLocationData(char * dataString);
void syncrtcwithNetworkTime(void);
void restoreHTTPURLforData(void);
void initHTTPURLforFOTA(void);
void initHTTPURLforFOTA_V2(char *ptrFileName);
uint32_t updateHTTPReadLength(uint32_t fotaFileSizeBytes);
void initHTTPURLforImmobilizer(void);
void verifyImmobilizerCommand(void);
//void Diagnostic(void);
void Diagnostic(enmDiagnosticStatus DiagnosticStatus);
void BootUpdateFailed();
/**************************************End of GSM Defines *****************************************************/
#endif /* INC_GSMSIN868_H_ */
