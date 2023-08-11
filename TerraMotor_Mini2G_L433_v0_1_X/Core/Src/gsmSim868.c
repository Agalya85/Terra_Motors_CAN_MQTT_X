/*
  *****************************************************************************
  * @file    GSM.c
  * @author  KloudQ Team
  * @version
  * @date
  * @brief   GSM HTTP / SMS / GPS and utility Functions
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

  Notes : (if any)

	IoT Error Test Webservice :
	"http://59.163.219.179:8025/api/Values/PostStringData";
*/

#include "main.h"
#include "stm32l433xx.h"
#include "applicationdefines.h"
#include <string.h>
#include <stdlib.h>
#include "gsmSin868.h"
#include "timer.h"
#include "externs.h"
#include "queue.h"
#include "user_eeprom.h"
#include "user_flash.h"
#include "payload.h"
#include "user_error.h"


/* SMS Commands that will execute independent of EEPROM memory  */
//static const char smsCmdGetConfig[]      	= "*TORV1CMDGTCONFIG#";			/* SMS Command for Get Config  */
////static const char smsCmdRqFota[]         	= "*TORV1CMDRQFOTA#";			/* SMS Command to Request FOTA */
//static const char smsCmdRqFota[]         	= "*TORV1CMDRQFOTA$";			/* SMS Command to Request FOTA */
//static const char smsCmdRqReset[]        	= "*TORV1CMDRQRST#";			/* SMS Command to Request Reset */
//
///* SMS Commands that will only execute if EEPROM memory is enabled */
//static const char smsCmdSetUrl[] 			= "*TORV1CMDSTURL,";			/* SMS Command to Set Server URL */
//static const char smsCmdSetUploadFreqOn[]   = "*TORV1CMDSTUPFREQON,";		/* SMS Command to Set ON Upload Freq */
//static const char smsCmdSetUploadFreqOff[]  = "*TORV1CMDSTUPFREQOFF,";		/* SMS Command to Set OFF Upload Freq */
//static const char smsCmdSetAPN[]         	= "*TORV1CMDSTNWAPN,"	;		/* SMS Command to Set Server APN */
//static const char smsCmdUpdMobNo[]      	= "*TORV1CMDNWMOBNO,";			/* SMS Command for Get Config  */

static char gau8GPS_POWERON[]="AT+CGNSPWR=1\r\n";  							/* Turn on GNSS */
static char gau8GPS_POWEROFF[]="AT+CGNSPWR=0\r\n";							/* Turn OFF GNSS */

/* GPS Commands Supported : RMC / GGA - Dead reckoning*/
#if(GPSDATATYPE == GPSDATATYPERMC)
static char gau8GPS_CGNSSEQ[]="AT+CGNSSEQ=\"RMC\"\r\n";  					/* Time, date, position, course and speed data */
#elif(GPSDATATYPE == GPSDATATYPERECK)
static char gau8GPS_CGNSSEQ[]="AT+CGNSSEQ=\"GGA\"\r\n";  					/* Time, date, position, course and speed data */

#endif

static char gau8GPS_UARTEN[]="AT+CGNSTST=0\r\n";      						/* Send out the GPS data */
static char gau8GPS_SETBR[]="AT+CGNSIPR=115200\r\n";						    /* Set GPS BaudRate */
//static char gau8GPS_GPIOEN[]="AT+CLBSCFG=1,3,\"lbs-simcom.com:3002\"\r\n";
static char gau8GPS_GPIOEN[] = "AT+CGPIO=0,57,1,1\r\n";						/* Enable GPIO 01 for GPS */ //

/* GPS Type Supported : Triangulation or Onchip */
#if(GPSTYPE == GPSTRANGULATION)
static char gau8GPS_CGNSINF[]="AT+CLBS=1,1\r\n";//"AT+CIPGSMLOC=1,1\r\n";						/* Trangulation :Get GPS Data */
#elif (GPSTYPE == GPSGNSSCHIP)
static char gau8GPS_CGNSINF[]="AT+CGNSINF\r\n";								/* GNSS Chip : Get GPS Data */
#endif

uint8_t gu8HttpPayloadFlag = 0;												/* Update HTTP Payload Flag */
uint8_t gu8CheckSMS = FALSE;
/* Used for send data gsm command */
static char * gu8GSMDataTimeout=",120000\r\n\0"; 							/* Data input : Max time in ms */

/* GPRS/GSM Commands */
static char gau8GSM_AT[]="AT\r\n";												/* Module Attention */
static char gau8GSM_ATE0[]="ATE0\r\n";    										/* Echo Off */
//static char gau8GSM_ATCPIN[]="AT+CPIN?\r\n";									/* Is SIM Ready */
//static char gau8GSM_CSMINS[]="AT+CSMINS?\r\n";    							/* Check if SIMCARD is present */
static char gau8GSM_CSQ[]="AT+CSQ\r\n";										/* Query Signal Strength */
static char gau8GSM_ATCREG[]="AT+CREG?\r\n"; 								/* Registration */
//static char gau8GSM_ATCMEE[]="AT+CMEE=1\r\n"; 								/* Enable numeric error codes  */
static char gau8GSM_ATCGACT[]="AT+CGACT?\r\n";								/* Context Activate or Deactivate */
static char gau8GSM_ATSAPRB3[]="AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n"; 		/* Set Content Type to GPRS */
static char gau8GSM_ATSAPRB1[]="AT+SAPBR=1,1\r\n"; 							/* Enable GPRS */
static char gau8GSM_ATSAPRBAPN[100]="AT+SAPBR=3,1,\"APN\",";					/* Set APN */
static char gau8GSM_ATSAPRBIP[]="AT+SAPBR=2,1\r\n"; 							/* Check if Valid IP Address is Received After device is connected to the network */
static char gau8GSM_ATCMGF[]="AT+CMGF=1\r\n";									/* SMS Text Mode */
static char gau8GSM_ATHTTPINIT[]="AT+HTTPINIT\r\n"; 							/* Init HTTP */
static char gau8GSM_ATHTTPPARACID[]="AT+HTTPPARA=\"CID\",1\r\n";				/* HTTP Parameter */
static char gau8GSM_ATHTTPPARAURL[150]="AT+HTTPPARA=\"URL\",";					/* HTTP Parameter : URL */
static char gau8GSM_ATHTTPPARATIMEOUT[]="AT+HTTPPARA=\"TIMEOUT\",1000\r\n"; 	/* HTTP Parameter : HTTP Timeout 1000 Sec */
static char gau8GSM_ATHTTPDATA[30]="AT+HTTPDATA=";								/* Set HTTP Data Length and UART Timeout */
static char gau8GSM_ATHTTPDATACOMMAND[30];										/* HTTP Data Command */
char gau8GSM_ATAPN[180] = {'0'};												/* Network APN */
char gau8GSM_ATURL[180] = {'0'};												/* Server URL */
static char gau8GSM_SMSRecepient[180] = {'0'};									/* User Phone Number */
//static char gau8GSM_ATHTTPACTION[17]="AT+HTTPACTION=1\r\n";	 					/* Send Data over HTTP  0:GET method 1:POST 2:HEAD method */
static char gau8GSM_ATHTTPACTIONFOTA[17]="AT+HTTPACTION=0\r\n";	 				/* Send Data over HTTP  0:GET method 1:POST 2:HEAD method */
//static char gau8GSM_ATHTTPTERN[13]="AT+HTTPTERM\r\n";							/* Terminiate HTTP */
static char gau8GSM_ATCMGL[]="AT+CMGL=\"REC UNREAD\"\r\n";					/* Display All Unread SMS */
static char gau8GSM_ATCMGD[]="AT+CMGDA=\"DEL ALL\"\r\n";						/* Delete All Messages */
static char gau8GSM_ATCMGS[26]="AT+CMGS=";										/* Send SMS */
static char gau8GSM_ATCSCS[]="AT+CSCS=\"GSM\"\r\n";							/* TE Character Set */
static char gau8GSM_SMS_EOM[2]={0x1A};											/* End of SMS Character (ctrl+Z) */
static char gau8GSM_ATHTTPPARAUA[]="AT+HTTPPARA=\"UA\",\"Tor by Kloudq\"\r\n";/* User Agent Signature */
static char gau8GSM_ATHTTPSSL_EN[] = "AT+HTTPSSL=1\r\n"; /*SSL certification 1 : Enable*/
//static char gau8GSM_ATHTTPSSL_DIS[] = "AT+HTTPSSL=0\r\n"; /*SSL certification 0 : disable*/
static char gau8GSM_ATCLTS[]="AT+CLTS=1\r\n";									/* Enable auto network time sync  */
static char gau8GSM_ATW[]="AT&W\r\n";											/* Save the setting */
static char gau8GSM_ATCCLK[]="AT+CCLK?\r\n";									/* Query Time */
static char gau8GSM_ATHTTPREAD[25]={'0'};										/* @Fota : Read File / Server URL Response . TODO: Array Length according to data */
static char gauGSM_ATHTTPBREAK[30]="AT+HTTPPARA=\"BREAK\",0\r\n";			   /* Byte Limit for every Read */

char gau8GSM_smsto[15]="+918669666703";
//char gau8GSM_url[150] = "http://pocapitest.kloudqapps.net/api/values/PostStringData";
char gau8GSM_url[150] = "http://Bgaussapi.remotemonitor.in/api/Values/PostStringData";
char gau8GSM_apn[100] = "airteliot.com";	// airtel									/* Holds Network APN */
//char gau8GSM_apn[100] = "M2MISAFE";	// voda		TODO								/* Holds Network APN */
char gau8GSM_TimeStamp[25]={'0'};												/* Stores Network Time Stamp */
//char gu8NewURL[150] = "http://pocapitest.kloudqapps.net/api/values/PostStringData";
char gu8NewURL[150] = "http://Bgaussapi.remotemonitor.in/api/Values/PostStringData";
//char gu8NewURL[150] = "http://59.163.219.179:8025/api/Values/PostStringData";		/* POC Admin test */
/* Testing URL @ Ajit Aher */
//const char gau8FotaURL[100] = "http://59.163.219.178:81/fota/Terex/TOR_Mini_4G.bin";
//const char gau8FotaURL[100] = "http://59.163.219.178:81/fota/Terex/PB8_LED_Blink_20_6.bin";
char gau8FotaURL[100] =""; /*"http://20.198.65.195/fota/Terex/BGauss_30CAN.bin";*/
//const char gau8FotaURL[100] ="http://20.198.65.195/fota/Terex/PB8_LED_Blink_30_6.bin";

//const char gau8ImmobilizerURL[100] ="http://59.163.219.178:81/bgauss/api/Set_Point_Http/DataRecive";
const char gau8ImmobilizerURL[100] ="https://bgauss.remotemonitor.in/api/Set_Point_Http/DataRecive";

const char gau8FotaURLNEW[100] = "http://59.163.219.178:81/fota/Terex/";

char gau8FotaData[2000]={'0'};
//char gau8Immobilizer[100]={'0'};
uint32_t gu32ImmobilizerCommand = 0;
char gau8ImmobilizerCommandID[40] = "";
char gau8ImmobilizerCommand[3] = "";
uint32_t gu32ImmobilizerActivatedFlag = 0;

uint32_t testVar = 0;															/* Dummy : Test Variable */
uint8_t gu8SendSMS = FALSE;	 													/* Send SMS Flag */
uint8_t gu8ReadSMS = FALSE;														/* Read SMS Flag */
uint32_t u8LastHttpResponseCode = 0;											/* Last Http Response Code */

uint32_t gu32NWRTCYear = 0;
uint32_t gu32NWRTCMnt = 0;
uint32_t gu32NWRTCDay = 0;

uint32_t gu32NWRTCHH = 0;
uint32_t gu32NWRTCYMM = 0;
uint32_t gu32NWRTCSS = 0;

uint32_t gu32AttemptFota = FALSE;
char gu32FotaFileSizeinBytes[8] = {'0'};

uint32_t u32FOTAFileBaseAddress = 0;
uint32_t u32FotaFileChunkCounter = 0;
uint32_t u32FotaFileRemainingBytes = 0;
uint32_t u32FotaFileStatus = 0;
uint32_t u32FotaFileReadComplete = 0;
uint32_t u32MemoryWriteCycle = FALSE;
uint32_t u32FotaFileSizeInBytes = 0;
uint32_t u32FotaChunckLength = 0;
uint32_t u32MemoryEraseStatus = 0;
uint32_t u32FlashMemoryWriteStatus = 0;
char buffer[6] = {'0'};
char bufferRemBytes[6] = {'0'};

uint32_t gu32RTCTestFlag = 0;

/* Global GSM Instance */
strctGSM gsmInstance;
strctQUEUE gsmPayload;

uint32_t gu32ModuleInitComplete = 0;
char gau8GPS_LocationDetails[30];
char gau8GPS_TimeDetails[30];
_Bool gbitPowerOnTimeSet = 1;
uint8_t gu8SendDataCounter = 5;
char gau8SUBRequest[150] = {'0'};
uint8_t SUBTriggerFlag = FALSE;
uint32_t gu32FotaRquestFlag = FALSE;

/* MQTT Commands
 * AT+SMCONF= <MQTTParamTag>,<MQTTParamValue>
 * <MQTTParamTag> MQTT parameters
	"CID" bearer identifier
	"URL" MQTT server address
	"Serve: tcpPort"
	"Server": the domain name or IP address
	"TcpPort": The default port is 1883
	"CLIENTID" Client identifier, the default is empty
	"KEEPALIVE" keepAlive set time, in seconds, default 60, range: 60
	to 180
	"CLEANSS" Clean session identifier, the default is 0, the range of (0-
	1)
	"USERNAME" User name, default is empty
	"PASSWORD" Password, the default is empty
	"TIMEOUT" MQTT maximum response time
	>,
 * */
//char gau8GSM_ATCGDATA[] = "AT+CGDATA=\"PPP\",1\r\n";

//char gau8GSM_ATSMURL[] = "AT+SMCONF=\"URL\",\"103.139.69.92:1883\"\r\n";			/* KTL MQTT broker */
//char gau8GSM_ATSMURL[] = "AT+SMCONF=\"URL\",\"65.0.42.230:8883\"\r\n";				/* TTL MQTT broker */
//char gau8GSM_ATSMURL[] = "AT+SMCONF=\"URL\",\"kmqtt.centralindia.azurecontainer.io:1883\"\r\n";				/* KTL Azure MQTT broker */

//char gau8GSM_ATSMURL[] = "AT+SMCONF=\"URL\",\"broker.hivemq.com\":\"1883\"\r\n";			/* hivemq broker */
//char gau8GSM_ATSMURL[] = "AT+SMCONF=\"URL\",\"test.mosquitto.org:1883\"\r\n";			/* server tcp port */

char gau8GSM_ATSMCID[] = "AT+SMCONF=\"CID\",1\r\n";										/* CID */

char gau8GSM_ATSMCONFSESSION[] = "AT+SMCONF=\"CLEANSS\",1\r\n";							/* Session Clean TRUE */
char gau8GSM_ATSMCONFUSRNAME[] = "AT+SMCONF=\"USERNAME\",\"KTL_user_100229\"\r\n";		/* Username String Kloudq_ */
char gau8GSM_ATSMCONFPSQWORD[] = "AT+SMCONF=\"PASSWORD\",123\r\n";						/* Password String */
//char gau8GSM_ATSMCONFTIMEOUT[] = "AT+SMCONF=\"KEEPALIVE\",60\r\n";						/* Keep alive string */
char gau8GSM_ATSMCONFTIMEOUT[] = "AT+SMCONF=\"TIMEOUT\",120\r\n";						/* Timeout string */
char gau8GSM_ATSMCONFTOUTCK[] = "AT+SMCONF?\r\n";	//"AT+SMCONN=?\r\n";	//
char gau8GSM_ATSMCONN[] = "AT+SMCONN\r\n";												/* MQTT connection */

char gau8GSM_MQTTPayload[1500] ="";
char gau8MQTT_SubMsg[100];


char gau8GSM_MQTTCLIENTID[] = "AT+SMCONF=\"CLIENTID\",\"";					/* Client ID */
char gau8GSM_ATSMCLIENTID[100] = "0";

//char gau8GSM_ATSMURL[] = "AT+SMCONF=\"URL\",\"npdmqtt.remotemonitor.in:1883\"\r\n";				/* NPD broker */
//char gauGSM_ATAMSUB[] = "AT+SMSUB=\"NPD/MQTT_v1/Config\",1\r\n";						/* Subscribe to news MQTT */
//char gau8GSM_ATCMPUB[] = "AT+SMPUB=";													/* MQTT news release */
//char gau8MQTT_PubTopicPayload[] =    "NPD/MQTT_v1/Payload";							/* MQTT Payload Publish Topic */
//char gau8MQTT_PubTopicGeo[] =        "NPD/MQTT_v1/Geo";									/* MQTT Geo Publish Topic */
//char gau8MQTT_PubTopicDiagnostic[] = "NPD/MQTT_v1/Diagnostic";							/* MQTT Diagnostic Publish Topic */
//char gau8MQTT_PubTopicBuffer[] =     "NPD/MQTT_v1/Buffer";								/* MQTT Buffer/History data Publish Topic */



char gau8GSM_ATSMURL[] = "AT+SMCONF=\"URL\",\"terramqtt.remotemonitor.in:1883\"\r\n";				/* Terra Motor broker */
char gauGSM_ATAMSUB[] = "AT+SMSUB=\"TERRA/MQTT_v1/Config\",1\r\n";						/* Subscribe to news MQTT */
char gau8GSM_ATCMPUB[] = "AT+SMPUB=";													/* MQTT news release */
char gau8MQTT_PubTopicPayload[] =    "TERRA/MQTT_v1/Payload";							/* MQTT Payload Publish Topic */
char gau8MQTT_PubTopicGeo[] =        "TERRA/MQTT_v1/Geo";									/* MQTT Geo Publish Topic */
char gau8MQTT_PubTopicDiagnostic[] = "TERRA/MQTT_v1/Diagnostic";							/* MQTT Diagnostic Publish Topic */
char gau8MQTT_PubTopicBuffer[] =     "TERRA/MQTT_v1/Buffer";								/* MQTT Buffer/History data Publish Topic */




/* Simcom Operation States */
strctGSMStateTable gsmStateTableArray[50]=
{
	{gau8GSM_AT,"OK\r\n",ONE_SEC,&gsmInstance},//0
	{gau8GSM_ATE0,"OK\r\n",ONE_SEC,&gsmInstance},//1

	/* GPS Init */
	{gau8GPS_GPIOEN,"",ONE_SEC,&gsmInstance},//2
	{gau8GPS_POWERON,"",ONE_SEC,&gsmInstance},//3
	{gau8GPS_CGNSSEQ,"",ONE_SEC,&gsmInstance},//4
	{gau8GPS_SETBR,"",ONE_SEC,&gsmInstance},//5
	{gau8GPS_UARTEN,"",TWO_SEC,&gsmInstance},//6
	/* GPS Location Commands:e */
#if(GPSTYPE == GPSTRANGULATION)
//	{gau8GPS_CGNSINF,"+CLBS:",TEN_SEC,&gsmInstance},
#elif( GPSTYPE == GPSGNSSCHIP)
//	{gau8GPS_CGNSINF,"+CGNSINF:",ONE_MIN,&gsmInstance},//7
	{gau8GPS_CGNSINF,"+CGNSINF:",TEN_SEC,&gsmInstance},//7
#endif

	{gau8GSM_ATCREG,"OK\r\n",ONE_SEC,&gsmInstance},//11
	{gau8GSM_ATCGACT,"+CGACT: 1",ONE_SEC,&gsmInstance},//14
//	{gau8GSM_ATCPIN,"+CPIN: READY",ONE_SEC,&gsmInstance},//8
//	{gau8GSM_CSMINS,"+CSMINS: 0,1",ONE_SEC,&gsmInstance},//9
	{gau8GSM_CSQ,"+CSQ:",ONE_SEC,&gsmInstance},//10

	{gau8GSM_ATCCLK,"+CCLK:",ONE_SEC,&gsmInstance},//12
//	{gau8GSM_ATCMEE,"OK\r\n",ONE_SEC,&gsmInstance},//13

	{gau8GSM_ATSAPRB3,"OK\r\n",ONE_SEC,&gsmInstance},//15
	{gau8GSM_ATAPN,"OK\r\n",ONE_SEC,&gsmInstance},//15
	{gau8GSM_ATSAPRB1,"OK\r\n",THIRTY_SEC,&gsmInstance},//14	//TEN_SEC
	{gau8GSM_ATSAPRBIP,"+SAPBR: 1,1",TEN_SEC,&gsmInstance},//16
//	{gau8GPS_CGNSINF,"+CLBS:",TEN_SEC,&gsmInstance},

	/* SMS Init */
	{gau8GSM_ATCSCS,"OK\r\n",ONE_SEC,&gsmInstance},//17

	{gau8GSM_ATHTTPINIT,"OK\r\n",ONE_SEC,&gsmInstance},//18
	{gau8GSM_ATHTTPPARACID,"OK\r\n",ONE_SEC,&gsmInstance},//19
	{gau8GSM_ATHTTPPARATIMEOUT,"OK\r\n",FOUR_SEC,&gsmInstance},//21
	{gau8GSM_ATHTTPPARAUA,"OK\r\n",TWO_SEC,&gsmInstance},//22
	{gau8GSM_ATHTTPSSL_EN,"OK\r\n",FIVE_SEC,&gsmInstance},
	{gau8GSM_ATCLTS,"OK\r\n",ONE_SEC,&gsmInstance},//23
	{gau8GSM_ATW,"OK\r\n",TWO_SEC,&gsmInstance},//24


	/*MQTT Commands*/
	{gau8GSM_ATSMURL,"OK",ONE_MIN,&gsmInstance},	//ONE_SEC
	{gau8GSM_ATSMCLIENTID,"OK",ONE_MIN,&gsmInstance},	//ONE_SEC  gau8GSM_ATSMCLIENTID
	{gau8GSM_ATSMCONFSESSION,"OK",ONE_MIN,&gsmInstance},	//TWENTY_SEC
	{gau8GSM_ATSMCONN,"OK",ONE_MIN,&gsmInstance},	//TWENTY_SEC
	{gauGSM_ATAMSUB,"+SMSUB:",TWENTY_SEC,&gsmInstance},
	{gau8GSM_ATCMPUB,"+SMPUB:",TWENTY_SEC,&gsmInstance}, /* MQTT SUB PUB */


	/* HTTP Commands : */
//	{gau8GSM_ATHTTPINIT,"OK\r\n",ONE_SEC,&gsmInstance},//18
//	{gau8GSM_ATHTTPPARACID,"OK\r\n",ONE_SEC,&gsmInstance},//19
	{gau8GSM_ATURL,"OK\r\n",ONE_SEC,&gsmInstance},//20
//	{gau8GSM_ATHTTPPARATIMEOUT,"OK\r\n",FOUR_SEC,&gsmInstance},//21
//	{gau8GSM_ATHTTPPARAUA,"OK\r\n",TWO_SEC,&gsmInstance},//22
//	{gau8GSM_ATCLTS,"OK\r\n",ONE_SEC,&gsmInstance},//23
//	{gau8GSM_ATW,"OK\r\n",TWO_SEC,&gsmInstance},//24
//	{gau8GSM_ATCCLK,"+CCLK:",ONE_SEC,&gsmInstance},
//	{gauGSM_ATHTTPBREAK,"OK\r\n",ONE_SEC,&gsmInstance},
//	{gau8GSM_ATHTTPDATACOMMAND,"DOWNLOAD",TEN_SEC,&gsmInstance},//25
//	{"","OK\r\n",TEN_SEC,&gsmInstance},//26	//FIVE_SEC
//	{gau8GSM_ATHTTPACTION,"+HTTPACTION:",ONE_MIN,&gsmInstance},//27	//TEN_SEC THIRTY_SEC
//	{gau8GSM_ATHTTPTERN,"OK\r\n",TEN_SEC,&gsmInstance},//28

	{gau8GPS_POWEROFF,"",TWO_SEC,&gsmInstance},//29 						/* Skipped in Logic .Not Required */
	/* Send SMS */
	{gau8GSM_ATCMGF,"OK\r\n",ONE_SEC,&gsmInstance},//30
	{gau8GSM_SMSRecepient,">",TEN_SEC,&gsmInstance},//31
	{gsmInstance.strSystemSMS.agsmSMSMessageBody,"",TWO_SEC,&gsmInstance},//32
	{gau8GSM_SMS_EOM,"+CMGS:",TWENTY_SEC,&gsmInstance},//33
	/* Read All Unread SMS */
	{gau8GSM_ATCMGF,"OK\r\n",ONE_SEC,&gsmInstance},//34
	{gau8GSM_ATCMGL,"OK\r\n",TWO_SEC,&gsmInstance},//35
	{gau8GSM_ATCMGD,"OK",THIRTY_SEC,&gsmInstance},//36
	/* Fota Commands */
	{gauGSM_ATHTTPBREAK,"OK\r\n",ONE_SEC,&gsmInstance},//37
	{gau8GSM_ATURL,"OK",ONE_SEC,&gsmInstance},//38 //gau8GSM_ATHTTPPARAURL
//	{gau8GSM_ATHTTPSSL_EN,"OK\r\n",FIVE_SEC,&gsmInstance},
	{gau8GSM_ATHTTPACTIONFOTA,"+HTTPACTION:",ONE_MIN,&gsmInstance},//39		//TWENTY_SEC,
	{gau8GSM_ATHTTPREAD,"+HTTPREAD:",TWENTY_SEC,&gsmInstance},//40
	/*Restore URL*/
	{gauGSM_ATHTTPBREAK,"OK\r\n",ONE_SEC,&gsmInstance},//41
	{gau8GSM_ATURL,"OK",ONE_SEC,&gsmInstance},//42 //gau8GSM_ATHTTPPARAURL
//	{gau8GSM_ATHTTPSSL_DIS,"OK\r\n",FIVE_SEC,&gsmInstance},
};


/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/
/******************************************************************************
* Function : initGSMSIM868()
*//**
* \b Description:
*
* This function is used to Initialise GSM Structure used for SIM868 Module
*
* PRE-CONDITION: Initialise UART with with the module is interfaced
*
* POST-CONDITION: Initialized GSM structure
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	initGSMSIM868();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void initGSMSIM868(void)
{
	/* Init Buffers */
	memset(gsmInstance.agpsLocationData, GSM_ARRAY_INIT_CHAR, (GPS_LOCATION_ARRAY_SIZE * sizeof(char)));
	memset(gsmInstance.agsmNetworkIP, GSM_ARRAY_INIT_CHAR, (GPRS_NETWORKIP_ARRAY_SIZE * sizeof(char)));
	memset(gsmInstance.agsmSignalStrength, GSM_ARRAY_INIT_CHAR, (GSM_SIGSTRGTH_ARRAY_SIZE * sizeof(char)));
	memset((char *)gsmInstance.as8GSM_Response_Buff,GSM_ARRAY_INIT_CHAR,(GSM_RESPONSE_ARRAY_SIZE * sizeof(char)));
	memset(gsmInstance.agsmCommandResponse, DATA_UNKNOWN, sizeof(gsmInstance.agsmCommandResponse[0][0])
		   * GSM_TOTAL_COMMANDS * GSM_TOTAL_COMMANDS);
	memset(gsmInstance.agsmHTTPRequestStatus,DATA_UNKNOWN,GSM_HTTP_RESPONSE_CODE_SIZE * sizeof(char));
	memset(gsmInstance.strSystemSMS.agsmSMSRecipient,DATA_UNKNOWN,SMS_MOB_NO_LENGTH * sizeof(char));
	memset(gsmInstance.strSystemSMS.agsmSMSMessageBody,0x00,SMS_MAX_MSG_LENGTH * sizeof(char));
	memset(gsmInstance.u32GSMHttpResponseCode,0x00,sizeof(char) * 3);
	memset(gau8GSM_ATAPN, 0x00, (180 * sizeof(char)));
	memset(gau8GSM_ATURL, 0x00, (180 * sizeof(char)));
	memset(gau8GSM_SMSRecepient, 0x00, ( 180 * sizeof(char)));

	unTelInfo.u8JBGaussGprsConnection = 0;
	strcat((char *)gau8GSM_SMSRecepient,(char *)gau8GSM_ATCMGS);
	strcat((char *)gau8GSM_SMSRecepient,(char *)"\"");
	strcat((char *)gau8GSM_SMSRecepient,(char *)gau8GSM_smsto);
	strcat((char *)gau8GSM_SMSRecepient,(char *)"\"");
	strcat((char *)gau8GSM_ATAPN,(char *)gau8GSM_ATSAPRBAPN);
	strcat((char *)gau8GSM_ATAPN,(char *)"\"");
	strcat((char *)gau8GSM_ATAPN,(char *)gau8GSM_apn);
	strcat((char *)gau8GSM_ATAPN,(char *)"\"");
	strcat((char *)gau8GSM_ATURL,(char *)gau8GSM_ATHTTPPARAURL);
	strcat((char *)gau8GSM_ATURL,(char *)"\"");
	strcat((char *)gau8GSM_ATURL,(char *)gau8GSM_url);
	strcat((char *)gau8GSM_ATURL,(char *)"\"");
	strcat(gau8GSM_ATAPN,"\r\n");
	strcat(gau8GSM_ATURL,"\r\n");
	strcat(gau8GSM_SMSRecepient,"\r\n");

	memset(gau8GSM_ATSMCLIENTID,0x00,sizeof(gau8GSM_ATSMCLIENTID));
	strcpy(gau8GSM_ATSMCLIENTID,gau8GSM_MQTTCLIENTID);
	strcat(gau8GSM_ATSMCLIENTID,(char * )dinfo);
//	strcat(gau8GSM_ATSMCLIENTID,(char * )"revati-100386");
	strcat(gau8GSM_ATSMCLIENTID,"\"\r\n");


	gsmInstance.strSystemSMS.u8NewMessage = FALSE;
	gsmInstance.enmcurrentTask = enmGSMTASK_RESET;
	gsmInstance.enmGSMPwrState = enmGSM_PWRNOTSTARTED;
	gsmInstance.u8isConnected = FALSE;
	gsmInstance.u8GSM_Response_Character_Counter = 0;
	gsmInstance.u8gsmRegistrationStatus = FALSE;
	gsmInstance.u8gsmSIMReadyStatus = FALSE;
	gsmInstance.u8gsmRetryCount = GSM_MAX_RETRY;
	gsmInstance.u8AttemptFota = FALSE;
	gsmInstance.u32GSMTimer = ONE_SEC;
	gu32GSMHangTimer = THREE_MIN;
	gsmInstance.u32GSMHeartbeatTimer = 0;
	gsmInstance.u8IllegalHttpResponseCounter = 0;
	gsmInstance.enmGSMCommandResponseState = enmGSM_SENDCMD;
	gsmInstance.enmGSMCommand = enmGSMSTATE_ATE0;
	gsmInstance.enmGSMCommandState = enmGSM_CMDSEND;

	gu32FotaRquestFlag = FALSE;
	unTelInfo.u32JBgaussCommandId[7] = gu32FotaRquestFlag;

	/* Potential Hang Issue due to this variable */
	gu32ModuleInitComplete = 0;
	gu8SendDataCounter = 5;
}

/******************************************************************************
* Function : operateGSMSIM868()
*//**
* \b Description:
*
* This function is used to Operate GSM module interfaced with MCU over configured UART
*
* PRE-CONDITION: Initialise UART with with the module is interfaced .
*
* POST-CONDITION: Operates Module for HTTP data Upload and GPS co-ordinate fetching
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	initGSMSIM868();
* 	operateGSMSIM868();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* <tr><td> 01/09/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> HTTP Timeout added (16 Mins) </td></tr>
* <tr><td> 01/02/2020 </td><td> 0.0.1            </td><td> HL100133 </td><td> default case handle </td></tr>
* <tr><td> 24/03/2020 </td><td> 0.0.1            </td><td> HL100133 </td><td> LIFO/FIFO Support Added </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void operateGSMSIM868()
{
	if(gsmInstance.u32GSMTimer != 0)
		return;

	if(gsmInstance.enmcurrentTask > 9)
	{
		gsmInstance.enmcurrentTask = 0;
	}

	/* Operate Current Task */
	switch(gsmInstance.enmcurrentTask)
	{
		case enmGSMTASK_RESET:

			if(gsmInstance.enmGSMPwrState == enmGSM_PWRNOTSTARTED)
			{
				HAL_GPIO_WritePin(GSM_PWR_KEY_GPIO_Port,GSM_PWR_KEY_Pin,GPIO_PIN_SET);
//				gsmInstance.u32GSMTimer =  FIVE_SEC;
				gsmInstance.u32GSMTimer =  TEN_SEC;
				gsmInstance.enmGSMPwrState = enmGSM_PWRSTARTED;
			}
			else if(gsmInstance.enmGSMPwrState == enmGSM_PWRSTARTED)
			{
				HAL_GPIO_WritePin(GSM_PWR_KEY_GPIO_Port,GSM_PWR_KEY_Pin,GPIO_PIN_RESET);
				gsmInstance.u32GSMTimer = FOUR_SEC;
				gsmInstance.enmcurrentTask = enmGSMTASK_INITMODULE;
				gsmInstance.enmGSMPwrState = enmGSM_PWRCOMPLETED;
			}
			else
			{
				if((gsmInstance.enmGSMPwrState == enmGSM_PWRCOMPLETED) && (gsmInstance.enmcurrentTask == enmGSMTASK_RESET))
				{
					gsmInstance.enmGSMPwrState = enmGSM_PWRNOTSTARTED;
				}
			}

			if(gsmInstance.enmGSMPwrState > 2)
			{
				gsmInstance.enmGSMPwrState = enmGSM_PWRNOTSTARTED;
			}
			break;


		case enmGSMTASK_INITMODULE:
			/* Initialize Module : ATE0 to HTTP URL */
			if(gsmInstance.enmGSMCommandState == enmGSM_CMDTIMEOUT)
			{
				/* Reset Module */
				initGSMSIM868();
			}
			else
			{
				/* Send Command and Check Response */
//				if(gsmInstance.enmGSMCommand == enmGSMSTATE_ATHTTPDATACOMMAND)
//				{
//					if(gu8CheckSMS == TRUE)
//					{
//						gsmInstance.enmcurrentTask = enmGSMTASK_READSMS;
//						gsmInstance.enmGSMCommand = enmGSMSTATE_READMODE;
//						gu8ReadSMS = TRUE;
//						gu8CheckSMS = FALSE;
//					}
//					else
//					{
//						/* Module is initialized ,Check Received SMS */
//						/* 31-3-2020 : Altered for testing FOTA File Read */
//						gsmInstance.enmGSMCommand = enmGSMSTATE_GPSCGNSINF;
//						gsmInstance.enmcurrentTask = enmGSMTASK_UPDATELOCATION;
//
////						initHTTPURLforFOTA();
////						gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPGETBREAK;		//enmGSMSTATE_SETFOTAURL;
////						gsmInstance.enmcurrentTask = enmGSMTASK_DOWNLOADFOTAFILE;
//					}
				if(gsmInstance.enmGSMCommand == enmGSMSTATE_ATCMPUB)
				{
					if(gu8CheckSMS == TRUE)
					{
						gsmInstance.enmGSMCommand = enmGSMSTATE_GPSCGNSINF;
						gsmInstance.enmcurrentTask = enmGSMTASK_UPDATELOCATION;
						gu8ReadSMS = TRUE;
						gu8CheckSMS = FALSE;
					}
					else
					{
						/* Module is initialized ,Check Received SMS */
						/* 31-3-2020 : Altered for testing FOTA File Read */
						gsmInstance.enmGSMCommand = enmGSMSTATE_GPSCGNSINF;
						gsmInstance.enmcurrentTask = enmGSMTASK_UPDATELOCATION;

//						initHTTPURLforFOTA();
//						gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPGETBREAK;		//enmGSMSTATE_SETFOTAURL;
//						gsmInstance.enmcurrentTask = enmGSMTASK_DOWNLOADFOTAFILE;
					}
				}
				else
				{
					sendGSMCommand(gsmInstance);
				}
			}
			break;

		case enmGSMTASK_ISALIVE:
//			if(gsmInstance.u32GSMHeartbeatTimer == 0)
//			{
				sendGSMCommand(gsmInstance);
//			}
			break;

		case enmGSMTASK_READSMS:
			if(gu8ReadSMS == TRUE)
				sendGSMCommand(gsmInstance);
			else
			{
				gsmInstance.enmGSMCommand = enmGSMSTATE_GPSCGNSINF;
				gsmInstance.enmcurrentTask = enmGSMTASK_UPDATELOCATION;
				gsmInstance.u32GSMTimer = TWO_SEC;
			}

			break;

		case enmGSMTASK_SENDSMS:
			if(gu8SendSMS == TRUE)
				sendGSMCommand(gsmInstance);
			else
			{
				gsmInstance.enmGSMCommand = enmGSMSTATE_GPSCGNSINF;
				gsmInstance.enmcurrentTask = enmGSMTASK_UPDATELOCATION;
				gsmInstance.u32GSMTimer = TWO_SEC;
			}
			break;

		case enmGSMTASK_UPDATELOCATION:
			/*Every 30 Sec */
			sendGSMCommand(gsmInstance);
			break;

		case enmGSMTASK_UPLOADDATA:
		{
			/* Send Data from the Queue with upload Time Interval */
			if((isQueueEmpty(&gsmPayload) == FALSE )&& (gsmPayload.data[gsmPayload.tail] != NULL))
			{
//				if((gu8HttpPayloadFlag == 0) && (gsmInstance.enmGSMCommand == enmGSMSTATE_ATHTTPDATACOMMAND))
//				{
//					/* Update Payload Length once Every Cycle */
//					updateHttpDataLength();
//					gu8HttpPayloadFlag = 1;
//				}
//				else if((gu8HttpPayloadFlag == 1) && (gsmInstance.enmGSMCommand == enmGSMSTATE_SENDDATA))
//				{
//					/* Flag to update new data length */
//					gu8HttpPayloadFlag = 0;
//				}
//				else
//				{
//					sendGSMCommand(gsmInstance);
//				}
//				if((gu8HttpPayloadFlag == 0) && (gsmInstance.enmGSMCommand == enmGSMSTATE_ATHTTPDATACOMMAND))
				if((gu8HttpPayloadFlag == 0) && (gsmInstance.enmGSMCommand == enmGSMSTATE_ATCMPUB))
				{
					/* Update Payload Length once Every Cycle */
					updateHttpDataLength();
					gu8HttpPayloadFlag = 1;
				}
				else if((gu8HttpPayloadFlag == 0) && (gsmInstance.enmGSMCommand == enmGSMSTATE_ATCMPUB))
				{
					updateHttpDataLength();
					gu8HttpPayloadFlag = 1;
				}
				else if((gu8HttpPayloadFlag == 1) && (gsmInstance.enmGSMCommand == enmGSMSTATE_SENDDATA))
				{
					/* Flag to update new data length */
					gu8HttpPayloadFlag = 0;
				}
				else
				{
					sendGSMCommand(gsmInstance);
				}
			}
			else
			{
				gsmInstance.enmGSMCommand = enmGSMSTATE_GPSCGNSINF;
				gsmInstance.enmcurrentTask = enmGSMTASK_UPDATELOCATION;
			}
		}
			break;

		case enmGSMTASK_GETDATA:
			/* Provision for Two Way communication with Server : ex Modbus . Not Implemented
			 * In this Firmware */
//				sendGSMCommand(gsmInstance);
			initGSMSIM868();
			break;

		case enmGSMTASK_DOWNLOADFOTAFILE:
				sendGSMCommand(gsmInstance);
			break;

		case enmGSMTASK_IDLE:
			gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
//			gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
			gsmInstance.enmGSMCommand = enmGSMSTATE_ATCMPUB;
			break;

		default:
			/* Log illegal State Error */
			initGSMSIM868();
			break;
	}
	gu32GSMHangTimer = THREE_MIN;
}

/******************************************************************************
* Function : sendGSMCommand()
*//**
* \b Description:
*
* This function is used to Send commands to  GSM module interfaced with MCU over
* configured UART . Called from operateGSMSIM868();
*
* PRE-CONDITION: Initialise UART with with the module is interfaced .
*
* POST-CONDITION: Send commands to operate module after valid response from module.
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	sendGSMCommand();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* <tr><td> 01/09/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> HTTP Timeout added (16 Mins) </td></tr>
* <tr><td> 19/12/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Bug :IP Array is getting
* 																			  corrupt with OK line 460 : Solved </td></tr>
* <tr><td> 27/12/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Signal Strength Cut Copy bug -Solved </td></tr>
* <tr><td> 27/12/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Update : When Web Service is not
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	   responding with 200
																					   maintain counter and reset .
																					    MAX_HTTP_ATTEMPTS = 10 </td></tr>
  <tr><td> 27/12/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Update : SMS Parsing Added </td></tr>
  <tr><td> 26/03/2020 </td><td> 0.0.1            </td><td> HL100133 </td><td> Restructured Case 200 for HTTP Action
																			  Memory Support Added for SMS .
																			  New Command for update mobile no added
																			  LIFO/FIFO Support Added </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
char bufferChunkBytes[6] = {'0'};
void sendGSMCommand()
{
	static char * command;
	static uint32_t u8CharacterCounter;
	static uint32_t u8LoopCounter ;

	switch (gsmInstance.enmGSMCommandResponseState)
	{
		case enmGSM_SENDCMD:
			if(gsmInstance.enmGSMCommandState == enmGSM_CMDSEND)
			{
				u8LoopCounter = 0;
				if(gsmInstance.enmGSMCommand == enmGSMSTATE_SENDDATA)
				{
#if (DATA_PROCESSING_METHOD == FIFO )
					command = gsmPayload.data[gsmPayload.tail];
#elif  (DATA_PROCESSING_METHOD == LIFO)
					command = gstrGMSPayloadLIFO.au8PayloadStack[gstrGMSPayloadLIFO.u32Stacktop];
#endif
				}
				else if(gsmInstance.enmGSMCommand == enmGSMSTATE_ATCMPUB)
				{
					command = gau8GSM_MQTTPayload;
				}
				else
					command = gsmStateTableArray[gsmInstance.enmGSMCommand].atCommand;

				u8CharacterCounter = strlen((const char *)command);
				LL_USART_TransmitData8(USART1,command[u8LoopCounter++]);
				gsmInstance.enmGSMCommandState = enmGSM_CMDINPROCESS;

			}
			else if(gsmInstance.enmGSMCommandState == enmGSM_CMDINPROCESS)
			{
				if(!LL_USART_IsActiveFlag_TXE(USART1))
				{
					/*Do Nothing . Wait For Previous Character Transmission */
				}
				else
				{
					if(u8LoopCounter < (u8CharacterCounter))
					{
						LL_USART_TransmitData8(USART1,command[u8LoopCounter++]);
					}
					else
					{
						u8LoopCounter = 0;
						gu32GSMCharacterTimeout = FIVEHUNDRED_MS;
						u8CharacterCounter = 0;
						gsmInstance.enmGSMCommandResponseState = enmGSM_CHKRESPONSE;
						gsmInstance.u32GSMResponseTimer = gsmStateTableArray[gsmInstance.enmGSMCommand].msTimeOut;

//						if((gsmInstance.enmGSMCommand == enmGSMSTATE_FOTAHTTPACTION) || (gsmInstance.enmGSMCommand == enmGSMSTATE_HTTPACTION))
						if(gsmInstance.enmGSMCommand == enmGSMSTATE_FOTAHTTPACTION)

//							gu32FotaFileReadTimer = FIFTEEN_SEC;
							gu32FotaFileReadTimer = THREE_SEC;
						else
							gu32FotaFileReadTimer = 0;

						if(gsmInstance.enmGSMCommand == enmGSMSTATE_SENDSMS)
							u8GSMCharRcv=1;
					}
				}
			}
			else
			{
				initGSMSIM868();
			}

			break;

			/*if(gsmInstance.enmGSMCommand == enmGSMSTATE_FOTAHTTPACTION )*/
		case enmGSM_CHKRESPONSE:
			if((gu32GSMCharacterTimeout == 0) && (gsmInstance.u32GSMResponseTimer != 0) && (u8GSMCharRcv == 1) && (gu32FotaFileReadTimer == 0))
			{
				const char *SOF = strstr((const char *)gsmInstance.as8GSM_Response_Buff, "$,");
				if(SOF != NULL)
				{	/* SOF Found */
					const char *EOF = strstr((const char *)gsmInstance.as8GSM_Response_Buff, ",@");
					if(EOF != NULL)
					{
						if(strstr((const char *)gsmInstance.as8GSM_Response_Buff,(const char *)dinfo) != NULL)
						{
							const size_t mlen = EOF - SOF;
							memset(gau8SUBRequest,0x00,sizeof(gau8SUBRequest));
							memcpy(gau8SUBRequest,SOF-1, (mlen+3));
							SUBTriggerFlag = TRUE;
						}
						else
							SUBTriggerFlag = FALSE;
					}
					else
						SUBTriggerFlag = FALSE;
				}
				else
					SUBTriggerFlag = FALSE;


//				if(strSUB !=NULL)
//				{
//					memset(gau8SUBRequest,0x00,sizeof(gau8SUBRequest));
//					memcpy(gau8SUBRequest, strSUB-2, ((strstr((const char *)gsmInstance.as8GSM_Response_Buff,"$,")) - (strstr((const char *)gsmInstance.as8GSM_Response_Buff,",@")));
//
//					if(strstr(gau8SUBRequest, "$,") != NULL)
//					{	/* SOF Found */
//						if(strstr(gau8FotaData, ",@") != NULL)
//						{/*EOF Found*/
//							SUBTriggerFlag = TRUE;
//						}
//						else
//							SUBTriggerFlag = FALSE;
//					}
//					else
//							SUBTriggerFlag = FALSE;
//				}
				/* Parse Response */
				if(strstr((const char *)gsmInstance.as8GSM_Response_Buff
						,(const char *)gsmStateTableArray[gsmInstance.enmGSMCommand].atCommandResponse) != NULL)
				{
					/*Required Response Received */
					switch(gsmInstance.enmGSMCommand)
					{

						case enmGSMSTATE_AT:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_ATE0:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

//						case enmGSMSTATE_ATCPIN:
//							gsmInstance.u8IncrementGsmState = TRUE;
//							break;
//
//						case enmGSMSTATE_ATCSMINS:
//							gsmInstance.u8IncrementGsmState = TRUE;
//							break;

						case enmGSMSTATE_ATCSQ:
							{
							/* Store Signal Strength . Bug 19012019 Cut Copy-Solved	 */
								char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
									,(const char *)"+CSQ");
								memcpy(gsmInstance.agsmSignalStrength, &strLoc[6], strlen(strtok(strLoc,"\r")));
								if(gsmInstance.enmcurrentTask == enmGSMTASK_ISALIVE)
								{
//									gsmInstance.u32GSMHeartbeatTimer = ONE_MIN;
//									gsmInstance.u32GSMTimer = FIVE_SEC;
									gsmInstance.u32GSMTimer = ONE_SEC;
									gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
//									gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
									gsmInstance.enmGSMCommand = enmGSMSTATE_ATCMPUB;
									gsmInstance.u8IncrementGsmState = FALSE;
								}
								else
									gsmInstance.u8IncrementGsmState = TRUE;
							}
							break;

						case enmGSMSTATE_CREG:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

//						case enmGSMSTATE_CMEE:
//							gsmInstance.u8IncrementGsmState = TRUE;
//							break;

						case enmGSMSTATE_CGACT:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_SAPBR3:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_SAPBR1:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_SAPBRAPN:
//							memcpy(gsmInstance.agsmCommandResponse[gsmInstance.enmGSMCommand],
//									(char *)&gsmInstance.as8GSM_Response_Buff, sizeof(gsmInstance.as8GSM_Response_Buff));
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_SAPRBIP:
							{
								char * strLocIp = strstr((const char *)gsmInstance.as8GSM_Response_Buff
									,(const char *)"+SAPBR: 1,1");
								memcpy(gsmInstance.agsmNetworkIP, &strLocIp[13], strlen(strtok(strLocIp,"\"")));
								gsmInstance.u8IncrementGsmState = TRUE;
							}
							break;

						case enmGSMSTATE_CMGF:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_CSCS:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;


						case enmSTATE_GPSGPIOEN:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_GPSPOWERON:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_GPSCGNSSEQ:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_GPSBR:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_GPSDATAEN:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_HTTPINIT:
							gsmInstance.u8IncrementGsmState = TRUE;
							unTelInfo.u8JBGaussGprsConnection = 1;
							break;

						case enmGSMSTATE_HTTPPARACID:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;
//
//						case enmGSMSTATE_HTTPPARAURL:
//							gsmInstance.u8HTTPInitStatus = TRUE;
//							gsmInstance.u8IncrementGsmState = TRUE;
//							break;

						case enmGSMSTATE_HTTPTIMEOUT:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_HTTPUA:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_HTTPSSL_EN:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_TIMESTAMPEN:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_TIMESTANPSAVE:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_GETTIMESTAMP:
							/* Parse and update local RTC . Add Flag check for update
							 * +CCLK: "18/06/21,12:00:21+22" */
							{
								char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
												,(const char *)"+CCLK:");
								memset(gau8GSM_TimeStamp,0x00,(sizeof(char ) * strlen(gau8GSM_TimeStamp)));
								memcpy( gau8GSM_TimeStamp, &strLoc[8], strlen(strtok(&strLoc[8],"\0")));
								syncrtcwithNetworkTime();
								gsmInstance.u8IncrementGsmState = TRUE;
							}
							break;

						case enmGSMSTATE_ATHTTPGETBREAK:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

//						case enmGSMSTATE_ATHTTPDATACOMMAND:
////							memcpy(gsmInstance.agsmCommandResponse[gsmInstance.enmGSMCommand],
////									(char *)&gsmInstance.as8GSM_Response_Buff, sizeof(gsmInstance.as8GSM_Response_Buff));
//							gsmInstance.u8IncrementGsmState = TRUE;
//							break;

						case enmGSMSTATE_SENDDATA:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;


						case enmGSMSTATE_ATSMURL:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;
						case enmGSMSTATE_ATSMCLIENTID:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_ATSMCONFSESSION:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_ATSMCONN:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_ATAMSUB:
							/* + SMSUB: <packet_id>, <status>
							 * <Packet_id> Feed Back message id
							<Status>Subscribe to return to the state news
							0: Success
							1: Timeout
							2: Other error */
							{
								char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff,(const char *)":");
								if(strLoc[4] == '0')	/* Subscribe successful */
								{
									gsmInstance.u8IncrementGsmState = TRUE;
//									gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
//									gsmInstance.enmGSMCommand = enmGSMSTATE_ATCMPUB;
								}
								else
								{
									/* TODO */
								}
							}
//							gsmInstance.u8IncrementGsmState = FALSE;
//							gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
//							gsmInstance.enmGSMCommand = enmGSMSTATE_ATCMPUB;
							break;


						case enmGSMSTATE_ATCMPUB:
							/* + SMPUB: <packet_id>, <status>
							 * <Packet_id> announced return id
								<Status> Announced the return status
								0: Success
								1: Timeout
								2: Other
								error
							 * */
							{
								char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
																		,(const char *)":");
								char *strLoc2 = strstr((const char *)strLoc,(const char *)",");
								if(strLoc2[1] == '0') /* Packet succesfully published */
								{
#if (DATA_PROCESSING_METHOD == FIFO )
										dequeue(&gsmPayload);
#elif (DATA_PROCESSING_METHOD == LIFO)
									popDataFromStack(&gstrGMSPayloadLIFO);
#endif
									gu8HttpPayloadFlag = 0;
									gsmInstance.enmGSMCommand = enmGSMSTATE_ATCMPUB;
									gsmInstance.u8IncrementGsmState = FALSE;
									u8LastHttpResponseCode = atoi(gsmInstance.u32GSMHttpResponseCode);
								}
								else
								{
									gsmInstance.enmGSMCommand = enmGSMSTATE_ATCMPUB;
									gsmInstance.u8IncrementGsmState = FALSE;
								}
							}
							gu8HttpPayloadFlag = 0;
							break;

//						case enmGSMSTATE_HTTPACTION:
//
//							/* Data Uploaded Successfully . Upload Next data packet */
//							/* Process HTTP Response code */
//							{
//							char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
//																			,(const char *)": ");
//							memcpy(gsmInstance.u32GSMHttpResponseCode, (char *)&strLoc[4], 3);
//							gu8SendDataCounter = 5;
//							/* Parse HTTP Response Code */
//							switch(atoi(gsmInstance.u32GSMHttpResponseCode))
//							{
//								case 200:
//								/* HTTP Request Successful . Send Next Packet */
//
//#if (DATA_PROCESSING_METHOD == FIFO )
//									dequeue(&gsmPayload);
//#elif (DATA_PROCESSING_METHOD == LIFO)
//									popDataFromStack(&gstrGMSPayloadLIFO);
//#endif
//
//								gsmInstance.u32GSMTimer = TWO_SEC; // Replace with Upload Frequency
//								gu8HttpPayloadFlag = 0;
////								gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
//								gsmInstance.enmGSMCommand = enmGSMSTATE_ATCMPUB;
//								gsmInstance.u8IncrementGsmState = FALSE;
//								u8LastHttpResponseCode = atoi(gsmInstance.u32GSMHttpResponseCode);
//								break;
//
//
//								default:
//									/* Log and Change State : Tested CSQ and DATA Upload in loop
//									 * When Service is not responding with 200 maintain counter and reset*/
//									u8LastHttpResponseCode = atoi(gsmInstance.u32GSMHttpResponseCode);
//									gsmInstance.u8IllegalHttpResponseCounter++;
//									if(gsmInstance.u8IllegalHttpResponseCounter >= MAX_HTTP_ATTEMPTS)
//									{
//										/* Log and Reset the modem */
//										initGSMSIM868();
//										gu8CheckSMS = TRUE;
//										break;
//									}
//									u8LastHttpResponseCode = atoi(gsmInstance.u32GSMHttpResponseCode);
////									gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
//									gsmInstance.enmGSMCommand = enmGSMSTATE_ATCMPUB;
//									gsmInstance.u32GSMResponseTimer = 0;
//									gsmInstance.u8IncrementGsmState = FALSE;
//
//								break;
//								}
//							}
//							break;

//						case enmGSMSTATE_HTTPTERM:
//							/* Not Implemented */
//							break;

						case enmGSMSTATE_GPSCGNSINF:
							{
								//char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
//										,(const char *)": 0");
								char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
																		,(const char *)":");
								if(strLoc != NULL)
								{
									/*Changed on 11 02 2018 . Garbage Location Logged*/
									memset(gsmInstance.agpsLocationData,0x00, strlen(gsmInstance.agpsLocationData));
									memcpy(gsmInstance.agpsLocationData, strLoc, strlen(strtok(strLoc,"\r\n")));

								}
								if(gu32ModuleInitComplete == 1)
								{
									if((gu32FotaRquestFlag == TRUE) && (isQueueEmpty(&gsmPayload) == 1))
									{
										initHTTPURLforFOTA();
//										gu8PowerOnFlag = 0;
//										gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPGETBREAK;
										gsmInstance.enmGSMCommand = enmGSMSTATE_SETFOTAURL;
										gsmInstance.enmcurrentTask = enmGSMTASK_DOWNLOADFOTAFILE;
//										gu32FotaRquestFlag = FALSE;
										SUBTriggerFlag = FALSE;
									}
									else
									{
										gsmInstance.enmGSMCommand = enmGSMSTATE_ATCSQ;
										gsmInstance.enmcurrentTask = enmGSMTASK_ISALIVE;
									}

									gsmInstance.u8IncrementGsmState = FALSE;
								}
								else if(gu32ModuleInitComplete == 0)
								{
									gsmInstance.u8IncrementGsmState = TRUE;
									gu32ModuleInitComplete = 1;
								}

							}
							break;

						case enmGSMSTATE_GPSPOWEROFF:
							/* Not Required */
							break;

						case enmGSMSTATE_SENDSMS:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_SMSCMGS:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_SMSEOM:
//							gsmInstance.u32GSMHeartbeatTimer = FIVE_SEC;
//							gsmInstance.u32GSMTimer = ONE_SEC;
//							gsmInstance.enmGSMCommand = enmGSMSTATE_ATCSQ;
//							gsmInstance.enmcurrentTask = enmGSMTASK_ISALIVE;
//							gsmInstance.u8IncrementGsmState = FALSE;
//							gu8SendSMS = FALSE;
							break;

						case enmGSMSTATE_READMODE:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_READSMS:
							/* Check for all applicable commands */
//							if(strlen((const char *)gsmInstance.as8GSM_Response_Buff) > 10)
//							{
//								/* Check for Get Configuration Request */
//								if(strstr((const char *)gsmInstance.as8GSM_Response_Buff
//								,(const char *)smsCmdGetConfig) != NULL)
//								{
//									/* Update phone number and set send SMS Flag */
//									updatePhoneNumber();
//								}
//
//								/* Check for Set URL Request */
//								if(strstr((const char *)gsmInstance.as8GSM_Response_Buff
//								,(const char *)smsCmdSetUrl) != NULL)
//								{
//									/*Extract URL , update url array , reset the GSM module*/
//									char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
//											,(const char *)smsCmdSetUrl);
//									/* Clear Previous URL array and copy new URL */
//									memset(gu8NewURL,0x00,sizeof(gu8NewURL));
//									memcpy( gu8NewURL, &strLoc[13], strlen(strtok(&strLoc[13],"#")));
//									strI2cEeprom.u32WrServerURLLength = strlen(gu8NewURL);
//									writeParametertoMemory(I2C_MEM_URLSTR);
//									writeParametertoMemory(I2C_MEM_URLSTRLENGTH);
//									updatePhoneNumber();
//									initGSMSIM868();
//								}
//								/* Check for Set Upload Frequency Request : System Status - ON */
//								if(strstr((const char *)gsmInstance.as8GSM_Response_Buff
//								,(const char *)smsCmdSetUploadFreqOn) != NULL)
//								{
//									/*Extract Frequency*/
//									char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
//															,(const char *)smsCmdSetUploadFreqOn);
//
//									/* update upload frequency  */
//									memcpy(strI2cEeprom.pu8Wr8UploadOnFreq,&strLoc[18], strlen(strtok(&strLoc[18],"#")));
//									writeParametertoMemory(I2C_MEM_ONFREQ);
//									updatePhoneNumber();
//								}
//								/* Check for Set Upload Frequency Request : System Status - OFF */
//								if(strstr((const char *)gsmInstance.as8GSM_Response_Buff
//								,(const char *)smsCmdSetUploadFreqOff) != NULL)
//								{
//									/*Extract Frequency*/
//									char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
//															,(const char *)smsCmdSetUploadFreqOff);
//
//									/* update upload frequency  */
//									memcpy(strI2cEeprom.pu8WrUploadOffFreq,&strLoc[19], strlen(strtok(&strLoc[19],"#")));
//									writeParametertoMemory(I2C_MEM_OFFFREQ);
//									updatePhoneNumber();
//									initGSMSIM868();
//								}
//								/* Check for Set APN Request */
//								if(strstr((const char *)gsmInstance.as8GSM_Response_Buff
//								,(const char *)smsCmdSetAPN) != NULL)
//								{
//									/*Extract APN , update url array , reset the GSM module*/
//									char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
//															,(const char *)smsCmdSetAPN);
//
//									/* Clear Previous APN array and copy new URL */
//									memset(gau8GSM_apn,0x00,sizeof(char) * 100);
//									memcpy( gau8GSM_apn, &strLoc[17], strlen(strtok(&strLoc[17],"#")));
//									strI2cEeprom.u32WrNetworkAPNLength = strlen(gau8GSM_apn);
//									writeParametertoMemory(I2C_MEM_APNSTRLENGTH);
//									writeParametertoMemory(I2C_MEM_APNSTR);
//									updatePhoneNumber();
//									initGSMSIM868();
//								}
//								/* Check for Update Mobile Number Request */
//								if(strstr((const char *)gsmInstance.as8GSM_Response_Buff
//								,(const char *)smsCmdUpdMobNo) != NULL)
//								{
//									/* Update phone number and set send SMS Flag */
//									/*Extract Mobile No reset the GSM module*/
//									char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
//															,(const char *)smsCmdUpdMobNo);
//
//									/* Clear Previous Mobile Number array and copy new Mobile No */
//									memset(strI2cEeprom.pu8WrMobileNumber,0x00,sizeof(strI2cEeprom.pu8WrMobileNumber));
//									memcpy( strI2cEeprom.pu8WrMobileNumber, &strLoc[15], strlen(strtok(&strLoc[15],"#")));
//									strI2cEeprom.u32WrMobileNumberLength = sizeof(strI2cEeprom.pu8WrMobileNumber);
//									writeParametertoMemory(I2C_MEM_MOBILELENGTH);
//									writeParametertoMemory(I2C_MEM_MOBILENOSTR);
//								}
//
//								/* Check for FOTA Request */
//								if(strstr((const char *)gsmInstance.as8GSM_Response_Buff
//								,(const char *)smsCmdRqFota) != NULL)
//								{
////									/* Attempt FOTA */
////                                    /* File name extracting from MSG*/
////                                    char u8filename[100], u8size, u8strlen;
////                                    char * ptrEndStr;
////                                    char * ptrStr = strstr((const char *)gsmInstance.as8GSM_Response_Buff
////                                            ,(const char *)"$");
////                                    memset(u8filename, 0x0, sizeof(u8filename));
////                                    ++ptrStr;
////                                    strcpy(u8filename, (char *)ptrStr);
////                                    ptrEndStr = strchr(u8filename, '#');
////                                    u8size = (uint8_t)(ptrEndStr - u8filename);
////                                    u8strlen = strlen(u8filename);
////                                    for(; u8size < u8strlen; ++u8size)
////                                        u8filename[(uint8_t)u8size] = 0;
////
////                                    /* Attempt FOTA */
////                                    initHTTPURLforFOTA_V2(u8filename);
////                                    gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPGETBREAK;        //enmGSMSTATE_SETFOTAURL;
////                                    gsmInstance.enmcurrentTask = enmGSMTASK_DOWNLOADFOTAFILE;
//
//								}
//								/* Check for Device Reset Request */
//								if(strstr((const char *)gsmInstance.as8GSM_Response_Buff
//								,(const char *)smsCmdRqReset) != NULL)
//								{
//									/* Device Will be reset by Internal WatchDog */
//									HAL_Delay(50000);
//								}
//								gu8ReadSMS = FALSE;
//							}
//							if(gu8ReadSMS ==  FALSE)
//							{
//								/* SMS is Available and parsed so delete all msgs */
//								gsmInstance.u8IncrementGsmState = TRUE;
//								gu8ReadSMS = TRUE;
//							}
//							else
//							{
//								/* Heart Beat or Pending Tasks / Send SMS if required / If
//								 * System config is updated or asked */
//								gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
////								gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
//								gsmInstance.enmGSMCommand = enmGSMSTATE_ATCMPUB;
//								gsmInstance.u8IncrementGsmState = FALSE;
//							}
							break;

						case enmGSMSTATE_DELETESMS:
							sendSystemConfigurationSMS();
							gsmInstance.enmcurrentTask = enmGSMTASK_SENDSMS;
							gsmInstance.enmGSMCommand = enmGSMSTATE_CMGF;
							memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR, (GSM_RESPONSE_ARRAY_SIZE));
							gsmInstance.u8IncrementGsmState = FALSE;
							break;

							/* Fota */
						case enmGSMSTATE_SETFOTAURL:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_FOTAHTTPACTION:
							/* Check if HTTPACTION:0,200,Bytes
							 * HTTPACTION:0,206,Total Bytes for partial content
							 * gu32FotaFileSizeinBytes[8]
							 * */
						{
							char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
																,(const char *)"200,");
							if(strLoc != NULL)
							{

								memset(gu32FotaFileSizeinBytes,0x00, strlen(gu32FotaFileSizeinBytes));
								memcpy(gu32FotaFileSizeinBytes, &strLoc[4], strlen(strtok(&strLoc[4],"\r\n")));

								/*Extract Size bytes */
								u32FotaFileSizeInBytes = strlen(gu32FotaFileSizeinBytes);
								gsmInstance.u32FotaFileSizeBytes = atoi(gu32FotaFileSizeinBytes);
								/*
								 *
								 */
								if(gsmInstance.u32FotaFileSizeBytes <= (61000))
								{
									/* File Size is Valid */
									gu32AttemptFota = TRUE;
									HAL_GPIO_WritePin(GPIOB, LED_COMM_Pin, GPIO_PIN_SET);
									if(gsmInstance.u32FotaFileSizeBytes != 0)
										updateHTTPReadLength(gsmInstance.u32FotaFileSizeBytes);
									else
									{
										/*Error File Size is not valid */
											Diagnostic(enmDiagnostic_RX_FILE_SIZE_ERROR);
									}
								}
								else
								{
									/* Abort Fota / Raise Error / Continue with regular operation */
									gsmInstance.u8IncrementGsmState = FALSE;
                                    Diagnostic(enmDiagnostic_FILE_DONWLOAD_ERROR);
									/* Raise size error and change state back to HTTP Upload */
									gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
//									gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
									gsmInstance.enmGSMCommand = enmGSMSTATE_ATCMPUB;
								}
								gsmInstance.u8IncrementGsmState = TRUE;
							}
							else
							{
								uint8_t cnt = 0;
								while(cnt<3)
								{
									HAL_GPIO_TogglePin(GPIOB,LED_COMM_Pin);
									HAL_Delay(1000);
									cnt++;
								}
								HAL_GPIO_WritePin(GPIOB,LED_COMM_Pin,GPIO_PIN_RESET);
								/* Response Does not contain 200 . Read Failed . Issue Alart / Failure */
								Diagnostic(enmDiagnostic_FILE_DONWLOAD_ERROR);
								gu32FotaRquestFlag = FALSE;
								unTelInfo.u32JBgaussCommandId[7] = gu32FotaRquestFlag;
								restoreHTTPURLforData();
								gsmInstance.u8IncrementGsmState = FALSE;
								//gu32AttemptFota = FALSE;
								gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
//								gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
								gsmInstance.enmGSMCommand = enmGSMSTATE_ATCMPUB;
							}
						}
							break;

							case enmGSMSTATE_READFILE:
								/* Read and Parse Received response and file
								 * +HTTPREAD: 800\r\n\r*/
//								memset(gau8FotaData,0x00,sizeof(gau8FotaData));
//								memcpy(gau8FotaData,(const char *)&gsmInstance.as8GSM_Response_Buff[16],50);
//								restoreHTTPURLforData();
//								gsmInstance.enmcurrentTask = enmGSMTASK_DOWNLOADFOTAFILE;
//								gsmInstance.enmGSMCommand = enmGSMSTATE_RESTORE_URL_ATHTTPGETBREAK;
//								gsmInstance.u8IncrementGsmState = FALSE;
//								gu32ImmobilizerCommand = 1;
								if(gu32AttemptFota == TRUE)
								{
									/* Read File Chucks and write to Internal flash */

									if(u32FotaFileReadComplete == 0)
									{
										memset(gau8FotaData,0x00,sizeof(gau8FotaData));
										memcpy(gau8FotaData,(const char *)&gsmInstance.as8GSM_Response_Buff[19],2000);

										/* More chuncks available */
										if(u32MemoryWriteCycle == FALSE)
										{
											/* Erase the Sector */
											FLASH_If_Init();
											if(FLASH_If_Erase(0,getNewFirmwareStorageLocation()) != FLASHIF_ERASE_OK)
//											if(FLASH_If_Erase(0,enmBOOTSEGMENT_Y_JUMP) != FLASHIF_ERASE_OK)
											{
												/* Error Erasing Flash */
												Diagnostic(enmDiagnostic_FLASH_ERRASE_ERROR);
												BootUpdateFailed();
											}
											/* Write Received Chucnk to memory */
											u32FlashMemoryWriteStatus = WriteDatatoFlash((uint8_t *)gau8FotaData,1);
//											u32FlashMemoryWriteStatus = WriteDatatoFlash(getNewFirmwareStorageLocation(),(uint8_t *)gau8FotaData);

											if(u32FlashMemoryWriteStatus == SUCCESS)
											{
												/* Memory Block Write Complete */
												u32MemoryWriteCycle = TRUE;
												updateHTTPReadLength(gsmInstance.u32FotaFileSizeBytes);
												memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR,(GSM_RESPONSE_ARRAY_SIZE * sizeof(uint8_t)));
											}
											else
											{
												/*Memory Write Failed . Raise Error and Back to HTTP Upload */
												Diagnostic(enmDiagnostic_MEM_WR_ERROR);
												BootUpdateFailed();
											}
										}
										else
										{
											/* Write Next Chunck to Internal Flash */
											u32FlashMemoryWriteStatus = WriteDatatoFlash((uint8_t *)gau8FotaData,0);
//											u32FlashMemoryWriteStatus = WriteDatatoFlash(getNewFirmwareStorageLocation(),(uint8_t *)gau8FotaData);
											if(u32FlashMemoryWriteStatus == SUCCESS)
											{
												/* Memory Block Write Complete */
												updateHTTPReadLength(gsmInstance.u32FotaFileSizeBytes);
											}
											else
											{
												/*Memory Write Failed . Raise Error and Back to HTTP Upload */
												Diagnostic(enmDiagnostic_MEM_WR_ERROR);
												BootUpdateFailed();
											}
										}
										/* Read the file */
										gsmInstance.u8IncrementGsmState = FALSE;
									}
									else if(u32FotaFileReadComplete == 1)
									{
										char *ptr = strstr((const char *)&gsmInstance.as8GSM_Response_Buff,"+HTTPREAD:");
										char *ptr2 = strstr(ptr,"\n");
										memset(gau8FotaData,0x00,sizeof(gau8FotaData));
//										memcpy(gau8FotaData,(const char *)&gsmInstance.as8GSM_Response_Buff[18],gsmInstance.u32FotaFileSizeBytes);
										memcpy(gau8FotaData,(ptr2+1),gsmInstance.u32FotaFileSizeBytes);

										if(u32MemoryWriteCycle == FALSE)
										{
											/*
											 * USE CASE : File Size is Less than CHUNK SIZE = 2k
											 * */
											/* Initialise Flash Opr Flags . Erase required sector and write new FW */
//											FLASH_If_Init();
//												u32MemoryEraseStatus = FLASH_If_Erase(0,getNewFirmwareStorageLocation());
//											u32FlashMemoryWriteStatus = FLASH_If_Write(getNewFirmwareStorageLocation(),(uint8_t *)gau8FotaData);
//											u32FlashMemoryWriteStatus = WriteDatatoFlash((uint8_t *)gau8FotaData,0);
											if(u32MemoryEraseStatus != FLASHIF_OK)
											{
												/* Error Erasing Flash */
											}
										}//

										u32FlashMemoryWriteStatus = WriteDatatoFlash((uint8_t *)gau8FotaData,0);
//										u32FlashMemoryWriteStatus = WriteDatatoFlash(getNewFirmwareStorageLocation(),(uint8_t *)gau8FotaData);


										/* Memory Write Successful */
//										if(flashWriteBootSection(enmBOOTSEGMENT_Y_JUMP) == 0)
//										if(u32FlashMemoryWriteStatus)
										if(flashWriteBootSection(getNewFirmwareStorageLocation()) == 0)
//										if(flashWriteBootSection(enmBOOTSEGMENT_Y_JUMP) == 0)
										{
											/* Boot Info Updated Successfully*/
											u32FOTAFileBaseAddress = 0;
											gu32AttemptFota =  FALSE;
											if(u32MemoryWriteCycle == TRUE)
												u32MemoryWriteCycle = FALSE;
//											HAL_Delay(2000);
//											NVIC_SystemReset();
											gu32FotaRquestFlag = FALSE;
											unTelInfo.u32JBgaussCommandId[7] = 2; // complete FOTA
                                            Diagnostic(enmDiagnostic_FOTA_SUCCESS_OK);
											FOTACompleteFlag = TRUE;
//											while(1);
											gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
											gsmInstance.enmGSMCommand = enmGSMSTATE_ATCMPUB;
//											initFWSelection();
										}
										else
										{
											uint8_t cnt = 0;
											while(cnt<4)
											{
												HAL_GPIO_TogglePin(GPIOB,LED_COMM_Pin);
												HAL_Delay(1000);
												cnt++;
											}
											HAL_GPIO_WritePin(GPIOB,LED_COMM_Pin,GPIO_PIN_RESET);

											/*Boot Info Updation Failed */
//											restoreHTTPURLforData();
//											gu32FotaRquestFlag = FALSE;
//											unTelInfo.u32JBgaussCommandId[7] = gu32FotaRquestFlag;
//											u32MemoryWriteCycle = FALSE;
//											gsmInstance.u8IncrementGsmState = FALSE;
//											gu32AttemptFota = FALSE;
//											gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
//											gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
//											gsmInstance.enmGSMCommand = enmGSMSTATE_ATCMPUB;
											Diagnostic(enmDiagnostic_BOOT_REGION_SELECTION_ERROR);
											BootUpdateFailed();
										}

										/* Write Chunk to memory . Checksum check and Application Jump.*/
										u32MemoryWriteCycle = TRUE;

									}
									else
									{
										/* Http Upload */
										Diagnostic(enmDiagnostic_FLASH_ERRASE_ERROR);
										BootUpdateFailed();
									}
								}
								else
								{
									/*Continue HTTP Upload */
									Diagnostic(enmDiagnostic_FLASH_ERRASE_ERROR);
									BootUpdateFailed();
								}
								break;

							case enmGSMSTATE_RESTORE_URL_ATHTTPGETBREAK:
								gsmInstance.u8IncrementGsmState = TRUE;
								break;

							case enmGSMSTATE_RESTORE_URL:
								gsmInstance.u8IncrementGsmState = TRUE;
								break;
//
//							case enmGSMSTATE_HTTPSSL_DIS:
//								gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
//								gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
//								gsmInstance.u8IncrementGsmState = FALSE;
//								break;

						default:
//							memcpy(gsmInstance.agsmCommandResponse[gsmInstance.enmGSMCommand], (char *)&gsmInstance.as8GSM_Response_Buff, sizeof(gsmInstance.as8GSM_Response_Buff));
							break;
						}

					gsmInstance.u8GSM_Response_Character_Counter = 0;
					gsmInstance.enmGSMCommandState = enmGSM_CMDSEND;
					gsmInstance.enmGSMCommandResponseState = enmGSM_SENDCMD;
					gsmInstance.u8gsmRetryCount = GSM_MAX_RETRY;
					gsmInstance.u32GSMResponseTimer = 0;
					u8GSMCharRcv = 0;
					gsmInstance.u8GSM_Response_Character_Counter = 0;
					if(gsmInstance.enmGSMCommand != enmGSMSTATE_READFILE)
						memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR,(GSM_RESPONSE_ARRAY_SIZE * sizeof(uint8_t))); /* Clear Response Buffer */

					if(gsmInstance.u8IncrementGsmState == TRUE)
					{
						gsmInstance.enmGSMCommand++;
						gsmInstance.u32GSMTimer = ONE_SEC;
					}
				}
				else
				{
					/* Response not found : Try Again */
					gsmInstance.u8GSM_Response_Character_Counter = 0;
					//gsmInstance.enmGSMCommandState = enmGSM_CMDRESPONSEERROR;
					/* 1-4-2020 */
					//gsmInstance.enmGSMCommandState = enmGSM_CMDSEND;
					gsmInstance.enmGSMCommandResponseState = enmGSM_CMDSEND;//enmGSM_SENDCMD;
//					memcpy(gsmInstance.agsmCommandResponse[gsmInstance.enmGSMCommand], (char *)&gsmInstance.as8GSM_Response_Buff, sizeof(gsmInstance.as8GSM_Response_Buff));
					memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR,(GSM_RESPONSE_ARRAY_SIZE * sizeof(uint8_t))); /* Clear Response Buffer */
					u8GSMCharRcv = 0;
				}
			}// end of if((gu32GSMCharacterTimeout == 0) && (gsmInstance.u32GSMResponseTimer != 0) && (u8GSMCharRcv == 1))
			else if(gsmInstance.u32GSMResponseTimer == 0)
			{
				/* Time Out */
				gsmInstance.u8gsmRetryCount--;
				if(gsmInstance.u8gsmRetryCount == 0)
				{
					/* Max Retry Attempt Reached Yet No Response . Reset the modem */
					/* Clear Response Buffer */
					memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR, (GSM_RESPONSE_ARRAY_SIZE));
					if(gsmInstance.enmcurrentTask == enmGSMTASK_UPLOADDATA)
					{
						/* Check SMS even if upload data was not successful */
						gu8CheckSMS = TRUE;
					}
					initGSMSIM868();
				}
				else
				{
					/*Send Same Command Again */
					gsmInstance.enmGSMCommandState = enmGSM_CMDSEND;
					gsmInstance.enmGSMCommandResponseState = enmGSM_SENDCMD;
					/* Clear Response Buffer */
					memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR, (GSM_RESPONSE_ARRAY_SIZE));
					gsmInstance.u32GSMTimer = ONE_SEC;
					if(gsmInstance.enmGSMCommand == enmGSMSTATE_SENDDATA)
					{
//						gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
						gsmInstance.enmGSMCommand = enmGSMSTATE_ATCMPUB;
						gu8SendDataCounter--;
						if(gu8SendDataCounter == 0)
							initGSMSIM868();
					}
				}
				gsmInstance.u32GSMResponseTimer = 0;
				u8GSMCharRcv = 0;
			}

			break;
	}
}

/******************************************************************************
* Function : updateHttpDataLength()
*//**
* \b Description:
*
* This function is updates payload data length form HTTP upload.
* Configure data length and timeout for gsm data
  gau8GSM_ATHTTPDATA = datalength,timeout\r\n;
*
* PRE-CONDITION: None .
*
* POST-CONDITION: Payload length is updated in HTTP param
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	updateHttpDataLength();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* *
* </table><br><br>
* <hr>
*
*******************************************************************************/
void updateHttpDataLength()
{
	char * tempdata = "";

	//tempdata = gstrGMSPayloadLIFO.au8PayloadStack[gstrGMSPayloadLIFO.u32Stacktop];
	tempdata = gsmPayload.data[gsmPayload.tail];


	uint32_t payloadLength = strlen(tempdata);
	char buffer[payloadLength];
	memset(buffer, 0x00, (payloadLength * sizeof(char))); /* Clear Response Buffer */
//	if(strstr((char *)tempdata, (char *)",9,#") != NULL && payloadLength < 1024)
//	{
//		/* Buffer/History Data */
//		memset(gau8GSM_MQTTPayload, 0x00, ( sizeof(gau8GSM_MQTTPayload) * sizeof(char)));
//		strcpy(gau8GSM_MQTTPayload,gau8GSM_ATCMPUB);
//		strcat(gau8GSM_MQTTPayload,"\"");
//		strcat(gau8GSM_MQTTPayload,gau8MQTT_PubTopicBuffer);
//		strcat(gau8GSM_MQTTPayload,"\",");
//		strcat(gau8GSM_MQTTPayload,"1,0,");		// QOS & Retain
//		strcat(gau8GSM_MQTTPayload,"\"");
//		strcat(gau8GSM_MQTTPayload,(char *)tempdata);
//		strcat(gau8GSM_MQTTPayload,"\"");
//		strcat(gau8GSM_MQTTPayload,"\r\n");
//	}
//	else
//	{
		if(payloadLength == 0 || payloadLength >= 1024)
		{
			memset(gau8GSM_MQTTPayload, 0x00, ( sizeof(gau8GSM_MQTTPayload) * sizeof(char)));
			strcpy(gau8GSM_MQTTPayload,gau8GSM_ATCMPUB);
			strcat(gau8GSM_MQTTPayload,"\"");
	//		strcat(gau8GSM_MQTTPayload,gau8MQTT_PubTopicDiagnostic);
			strcat(gau8GSM_MQTTPayload,gau8MQTT_PubTopicGeo);
			strcat(gau8GSM_MQTTPayload,"\",");
			strcat(gau8GSM_MQTTPayload,"1,0,");		// QOS & Retain
			strcat(gau8GSM_MQTTPayload,"\"");

			strcat(gau8GSM_MQTTPayload,(char * )START_OF_FRAME);
			strcat(gau8GSM_MQTTPayload,(char * )",");
			/* Device UUID */
			strcat(gau8GSM_MQTTPayload,(char * )dinfo);
			strcat(gau8GSM_MQTTPayload,(char * )",");
			if(payloadLength == 0)
				strcat(gau8GSM_MQTTPayload,(char * )"Diagnostic, Data Corrupt,");
			else
			{
				strcat(gau8GSM_MQTTPayload,(char * )"Diagnostic, Data size more than 1024,");
				free(gsmPayload.data[gsmPayload.tail]);
				gsmPayload.data[gsmPayload.tail] = NULL;
				gsmPayload.tail++;
//				gbitFlagGsmPayloadFreed = 1;
			}
			strcat(gau8GSM_MQTTPayload,(char * )END_OF_FRAME);

			strcat(gau8GSM_MQTTPayload,"\"");
			strcat(gau8GSM_MQTTPayload,"\r\n");
		}
		else
		{
			/* MQTT Commands payload length updated */
			/* Geo Location data */
		//	if(payloadLength < 179 && payloadLength >174)	// payload length 125 for lat long & utc only. 178 for all GPS data
			if(strstr((char *)tempdata, (char *)"Geo"))
			{
				memset(gau8GSM_MQTTPayload, 0x00, ( sizeof(gau8GSM_MQTTPayload) * sizeof(char)));
				strcpy(gau8GSM_MQTTPayload,gau8GSM_ATCMPUB);
				strcat(gau8GSM_MQTTPayload,"\"");
				strcat(gau8GSM_MQTTPayload,gau8MQTT_PubTopicGeo);
				strcat(gau8GSM_MQTTPayload,"\",");
				strcat(gau8GSM_MQTTPayload,"1,0,");		// QOS & Retain
				strcat(gau8GSM_MQTTPayload,"\"");
				strcat(gau8GSM_MQTTPayload,(char *)tempdata);	//"Tor 2G MQTT TEST 0621"
				strcat(gau8GSM_MQTTPayload,"\"");
				strcat(gau8GSM_MQTTPayload,"\r\n");
			}
			/*Diagnostic Payload Data*/
			else if(strstr((char *)tempdata, (char *)"Diagnostic"))
			{
				memset(gau8GSM_MQTTPayload, 0x00, ( sizeof(gau8GSM_MQTTPayload) * sizeof(char)));
				strcpy(gau8GSM_MQTTPayload,gau8GSM_ATCMPUB);
				strcat(gau8GSM_MQTTPayload,"\"");
				strcat(gau8GSM_MQTTPayload,gau8MQTT_PubTopicDiagnostic);
				strcat(gau8GSM_MQTTPayload,"\",");
				strcat(gau8GSM_MQTTPayload,"1,0,");		// QOS & Retain
				strcat(gau8GSM_MQTTPayload,"\"");
				strcat(gau8GSM_MQTTPayload,(char *)tempdata);	//"Tor 2G MQTT TEST 0621"
				strcat(gau8GSM_MQTTPayload,"\"");
				strcat(gau8GSM_MQTTPayload,"\r\n");
			}
			/* System payload data */
			else //if(gbitFlagGeoPayload == 1)
			{
				memset(gau8GSM_MQTTPayload, 0x00, ( sizeof(gau8GSM_MQTTPayload) * sizeof(char)));
				strcpy(gau8GSM_MQTTPayload,gau8GSM_ATCMPUB);
				strcat(gau8GSM_MQTTPayload,"\"");
				strcat(gau8GSM_MQTTPayload,gau8MQTT_PubTopicPayload);
				strcat(gau8GSM_MQTTPayload,"\",");
				strcat(gau8GSM_MQTTPayload,"1,0,");		// QOS & Retain
				strcat(gau8GSM_MQTTPayload,"\"");
				strcat(gau8GSM_MQTTPayload,(char *)tempdata);	//"Tor 2G MQTT TEST 0621"
				strcat(gau8GSM_MQTTPayload,"\"");
				strcat(gau8GSM_MQTTPayload,"\r\n");
			}
		}

	/* Convert Integer to ASCII ( Decimal) */
	memset(gau8GSM_ATHTTPDATACOMMAND, 0x00, ( 30 * sizeof(char)));
	itoa(payloadLength,buffer,PAYLOAD_DATA_STRING_RADIX);
	strcat(buffer,gu8GSMDataTimeout);
	strcat((char *)gau8GSM_ATHTTPDATACOMMAND,(char *)gau8GSM_ATHTTPDATA);
	strcat((char *)gau8GSM_ATHTTPDATACOMMAND,buffer);
//	/* Convert Integer to ASCII ( Decimal) */
//	memset(gau8GSM_ATHTTPDATACOMMAND, 0x00, ( 30 * sizeof(char)));
//	itoa(payloadLength,buffer,PAYLOAD_DATA_STRING_RADIX);
//	strcat(buffer,gu8GSMDataTimeout);
//	strcat((char *)gau8GSM_ATHTTPDATACOMMAND,(char *)gau8GSM_ATHTTPDATA);
//	strcat((char *)gau8GSM_ATHTTPDATACOMMAND,buffer);
}

/****************************************************************************
 Function initHTTPURLforImmobilization
 Purpose: Update HTTP URL for Immobilization
 Input:	None.
 Return value: None.

 Note(s)(if-any) :
	ToDo : Add functionality to update Immobilization URL through SMS

 Change History:
 Author           	Date                Remarks
 KloudQ Team      31-03-2020			Initial Definitions
******************************************************************************/
void initHTTPURLforImmobilizer(void )
{
	strcpy((char *)gau8GSM_ATURL,(char *)gau8GSM_ATHTTPPARAURL);
	strcat((char *)gau8GSM_ATURL,(char *)"\"");
	strcat((char *)gau8GSM_ATURL,(char *)gau8ImmobilizerURL);//gau8FotaURL);
	strcat((char *)gau8GSM_ATURL,(char *)"?id=");
	strcat((char *)gau8GSM_ATURL,(char *)dinfo);
	strcat((char *)gau8GSM_ATURL,(char *)"\"");
	strcat((char *)gau8GSM_ATURL,"\r\n");
}


/****************************************************************************
 Function restoreHTTPURLforData
 Purpose: Restore HTTP Data URL
 Input:	None.
 Return value: None.

 Note(s)(if-any) :
	URL for Data and Fota Server will always be different .
	FW has to handle proper selection .

 Change History:
 Author           	Date                Remarks
 KloudQ Team      31-03-2020			Initial Definitions
******************************************************************************/
void restoreHTTPURLforData(void)
{
	strcpy((char *)gau8GSM_ATURL,(char *)gau8GSM_ATHTTPPARAURL);
	strcat((char *)gau8GSM_ATURL,(char *)"\"");
	strcat((char *)gau8GSM_ATURL,(char *)gau8GSM_url);
	strcat((char *)gau8GSM_ATURL,(char *)"\"");
	strcat((char *)gau8GSM_ATURL,"\r\n");

}


/******************************************************************************
* Function : sendSystemConfigurationSMS()
*//**
* \b Description:
*
* This function is updates SMS data in the SMS structure .
*
* PRE-CONDITION: None .
*
* POST-CONDITION: SMS is ready as per format for sending
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	sendSystemConfigurationSMS();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> HTTP Code of latest request added </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> Redundant code reduced (UUID)</td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void sendSystemConfigurationSMS(void)
{
//	/* Tor Signature */
//	strcpy(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)KLOUDQ_SIGNATURE);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Config: \r\n");
//	/*Tor Version */
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Version: \r\n");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)MODEL_NUMBER);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//	/* Tor Device Id */
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Id: ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)dinfo);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//	/* Tor Signal Strength in RSSI */
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"RSSI: ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,gsmInstance.agsmSignalStrength);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//	/* Tor Network IP , if connected */
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"IP: ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,gsmInstance.agsmNetworkIP);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//	/* Tor Network APN */
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"APN: ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,gau8GSM_apn);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//	/* Tor Server URL */
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"URL: ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,gau8GSM_url);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//
//	/* Tor Upload Frequency */
//	char ontime[10];
//	char offtime[10];
//	itoa(gsmInstance.u32ONPayloadUploadFreq,ontime,10);
//	itoa(gsmInstance.u32OFFPayloadUploadFreq,offtime,10);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Up Freq ON: ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,ontime);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Up Freq OFF: ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,offtime);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//
//	/* Last Known Location and time */
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Location: ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,gsmInstance.agpsLocationData);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//
//	/*Last HTTP Status Code */
//	char httpresp[5];
//	itoa(u8LastHttpResponseCode,httpresp,10);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"HTTP Code : ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,httpresp);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\n");
}

/******************************************************************************
* Function : updatePhoneNumber()
*//**
* \b Description:
*
* This function is used to updates phone number to send SMS .
*
* PRE-CONDITION: None .
*
* POST-CONDITION: Phone number is updated in SMS structure
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	updatePhoneNumber();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
**
* </table><br><br>
* <hr>
*
*******************************************************************************/
void updatePhoneNumber(void)
{
	memset(gau8GSM_smsto,0x00,sizeof(char) * 15);
	memcpy(gau8GSM_smsto, (char *)&gsmInstance.as8GSM_Response_Buff[25]
		,(strlen(strtok((char *)&gsmInstance.as8GSM_Response_Buff[26],","))));
	memset(gau8GSM_SMSRecepient, 0x00, ( 180 * sizeof(char)));
	strcat((char *)gau8GSM_SMSRecepient,(char *)gau8GSM_ATCMGS);
	strcat((char *)gau8GSM_SMSRecepient,(char *)"\"");
	strcat((char *)gau8GSM_SMSRecepient,(char *)gau8GSM_smsto);
	strcat((char *)gau8GSM_SMSRecepient,(char *)"\"");
	strcat(gau8GSM_SMSRecepient,"\r\n");
	sendSystemConfigurationSMS();
	gu8SendSMS = TRUE;
}

/******************************************************************************
* Function : syncrtcwithNetworkTime()
*//**
* \b Description:
*
* This function is used to synchronize internal RTC with server / GPS time .
* Updates RTC Structure values with network time
  Network Time format : "yy/MM/dd,hh:mm:ss � zz"
  zz - time zone
  (indicates the difference, expressed in quarters of an hour, between the
   local time and GMT; range -47...+48)

   E.g. 6th of May 2010, 00:01:52
   GMT+2 hours equals to "10/05/06,00:01:52+08".
*
* PRE-CONDITION: Initialize the internal RTC module .
*
* POST-CONDITION: Internal RTC is updated with server/gps time
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	syncrtcwithNetworkTime();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
**
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint32_t gu32Year = 0;
uint32_t gu32Month = 0;
uint32_t gu32Date = 0;
uint32_t gu32Hours = 0;
uint32_t gu32Minutes = 0;
uint32_t gu32Seconds = 0;
void syncrtcwithNetworkTime(void)
{
	if(gu32TimeSyncFlag == 1)
		return;

	gu32Year = (((gau8GSM_TimeStamp[0]-'0') * 10) + (gau8GSM_TimeStamp[1]-'0'));
	gu32Month = (((gau8GSM_TimeStamp[3]-'0') * 10) + (gau8GSM_TimeStamp[4]-'0'));
	gu32Date = (((gau8GSM_TimeStamp[6]-'0') * 10) + (gau8GSM_TimeStamp[7]-'0'));

	gu32Hours = (((gau8GSM_TimeStamp[9]-'0') * 10) + (gau8GSM_TimeStamp[10]-'0'));
	gu32Minutes = (((gau8GSM_TimeStamp[12]-'0') * 10) + (gau8GSM_TimeStamp[13]-'0'));
	gu32Seconds = (((gau8GSM_TimeStamp[15]-'0') * 10) + (gau8GSM_TimeStamp[16]-'0'));

	SDate1.Year = DecimalToBCD(gu32Year);
	SDate1.Month = DecimalToBCD(gu32Month);
	SDate1.Date = DecimalToBCD(gu32Date);
	STime1.Hours = DecimalToBCD(gu32Hours);
	STime1.Minutes = DecimalToBCD(gu32Minutes);
	STime1.Seconds = DecimalToBCD(gu32Seconds);

	/*Store Last Syncd date */
	//lastKnownSDate1 = SDate1 ;

	strTimeUpdate.u32RefTimeHH = gu32Hours;
	strTimeUpdate.u32RefTimeMin = gu32Minutes;
	strTimeUpdate.u32RefTimeSec = gu32Seconds;

	HAL_RTC_SetTime(&hrtc,&STime1,RTC_FORMAT_BCD);
	HAL_RTC_SetDate(&hrtc,&SDate1,RTC_FORMAT_BCD);
	/* Update/ Set RTC Structure */
	//backupCurrentRTC();
	gu32TimeSyncFlag = 1;
	gu32RTCTestFlag++;
}

/****************************************************************************
 Function updateHTTPReadLength
 Purpose: Read Next chunk of 2000 Bytes of FOTA File
 Input:	uint32_t fotaFileSizeBytes - File Size in Bytes .
 Return value: uint32_t status 0 - Null File
 	 	 	 	 	 	 	   1 - File Download Complete
 	 	 	 	 	 	 	   2 - File Download In Process

 Note(s)(if-any) :

	AT+HTTPREAD = <start_address> , <byte_size> \r\n
    Test Fota File size is 29630 Bytes .
	(fotaFileSizeBytes / 2000) + 1 = (29630/2000) + 1 = (14.8) + 1 = 15
	interations/read to get complete file



 Change History:
 Author           	Date                Remarks
 KloudQ Team      8-04-2020			Initial Definition
 Kloudq Team	  10-04-2020		Strcpy logic added
******************************************************************************/
#define FOTAFILECHUNKSIZEBYTES	(2000)
uint32_t updateHTTPReadLength(uint32_t fotaFileSizeBytes)
{

	if(fotaFileSizeBytes != 0)
	{
		/* Start of File */
		if(u32FOTAFileBaseAddress == 0)
		{
			u32FotaFileChunkCounter = (fotaFileSizeBytes / FOTAFILECHUNKSIZEBYTES);
			/* Calculates Remaining data bytes after multiples of 2000  */
			u32FotaFileRemainingBytes = (fotaFileSizeBytes % FOTAFILECHUNKSIZEBYTES);
			itoa(FOTAFILECHUNKSIZEBYTES,bufferChunkBytes,PAYLOAD_DATA_STRING_RADIX);
			itoa(u32FotaFileRemainingBytes,bufferRemBytes,PAYLOAD_DATA_STRING_RADIX);
		}

		strcpy(gau8GSM_ATHTTPREAD,(char *)"AT+HTTPREAD=");
		memset(buffer,0x00,sizeof(char) * sizeof(buffer));

		if(u32FOTAFileBaseAddress < u32FotaFileChunkCounter)
		{
			itoa((u32FOTAFileBaseAddress * FOTAFILECHUNKSIZEBYTES),buffer,PAYLOAD_DATA_STRING_RADIX);
			strcat(gau8GSM_ATHTTPREAD,(char *)buffer);
			strcat(gau8GSM_ATHTTPREAD,(char *)",");
			strcat(gau8GSM_ATHTTPREAD,(char *)bufferChunkBytes); /* Byte(s) Chunk to read*/
			strcat(gau8GSM_ATHTTPREAD,(char *)"\r\n");
			u32FOTAFileBaseAddress++;
			gsmInstance.u32FotaFileSizeBytes = FOTAFILECHUNKSIZEBYTES;
			return 2;
		}
		else
		{
			itoa(u32FotaFileChunkCounter * 2000,buffer,PAYLOAD_DATA_STRING_RADIX);
			strcat(gau8GSM_ATHTTPREAD,(char *)buffer);
			strcat(gau8GSM_ATHTTPREAD,(char *)",");
			strcat(gau8GSM_ATHTTPREAD,(char *)bufferRemBytes); /* Byte(s) Chunk to read*/
			strcat(gau8GSM_ATHTTPREAD,(char *)"\r\n");
			gsmInstance.u32FotaFileSizeBytes = u32FotaFileRemainingBytes;
			u32FOTAFileBaseAddress = 0;
			u32FotaFileChunkCounter = 0;
			u32FotaFileRemainingBytes = 0;
			u32FotaFileReadComplete = 1;
			return 1;
		}
	}
	else
		return 0;
}

/****************************************************************************
 Function verifyImmobilizerCommand
 Purpose: Operate Immobilizer
 Input:	None.
 Return value: None

 Note(s)(if-any) :

     1. Check if command is for current device . Match UUIDs
	 2. If yes the parse command .
	 3. Activate / Deactivate
	 4. Command Format $,054029295013478338576488114,1,@

 Change History:
 Author           	Date                Remarks
 KloudQ Team     21-06-2021			Initial Definition

******************************************************************************/
void verifyImmobilizerCommand(void)
{

	if(gu32ImmobilizerCommand == 1)
	{
		const char *SOF = strstr(gau8SUBRequest, "$,");
		if(SOF != NULL)
		{	/* SOF Found */
			const char *EOF = strstr(gau8SUBRequest, ",@");
			if(EOF != NULL)
			{
				/*EOF Found . Extract the data */
				 const size_t mlen = EOF - SOF ;
				 memcpy(gau8ImmobilizerCommandID, SOF+2, mlen-4);
				 memcpy(gau8ImmobilizerCommand, EOF-1, 1);
//				 if(strcmp(dinfo,gau8ImmobilizerCommandID) == 0)
//				 {
					 /* UUID matched . Operate Immobilizer*/
					 if((strcmp(gau8ImmobilizerCommand, "1") == 0))
					 {
						 /*Switch On the Immobilizer*/
						 gu32ImmobilizerActivatedFlag = 0;
						 gu32CurrImmobilizerState = 1;

					 }
					 else if((strcmp(gau8ImmobilizerCommand, "0") == 0))
					 {
						 gu32ImmobilizerActivatedFlag = 1;
						 unTelInfo.u32JBgaussCommandId[6] = gu32ImmobilizerActivatedFlag;
						 gu32CurrImmobilizerState = 0;

					 }

					 else
					 {
						 /*Undefined . CAN phy layer not available */
					 }

//				 }
			}
			else
			{
				/*SOF Not found . Ignore the command*/
				memset(gau8FotaData,0x00,sizeof(gau8FotaData));
				gu32ImmobilizerCommand = 0;
			}
		}
		else
		{
			/*SOF Not found . Ignore the command*/
			memset(gau8FotaData,0x00,sizeof(gau8FotaData));
			gu32ImmobilizerCommand = 0;
		}

	}
}//end of void verifyImmobilizerCommand(void)


/****************************************************************************
 Function initHTTPURLforFOTA
 Purpose: Update HTTP URL for Fota
 Input:	None.
 Return value: None.

 Note(s)(if-any) :
	ToDo : Add functioonality to update Fota URL through SMS

 Change History:
 Author           	Date                Remarks
 KloudQ Team      31-03-2020			Initial Definitions
******************************************************************************/
void initHTTPURLforFOTA(void )
{
	strcpy((char *)gau8GSM_ATURL,(char *)gau8GSM_ATHTTPPARAURL);
	strcat((char *)gau8GSM_ATURL,(char *)"\"");
	strcat((char *)gau8GSM_ATURL,(char *)gau8FotaURL);
	strcat((char *)gau8GSM_ATURL,(char *)"\"");
	strcat((char *)gau8GSM_ATURL,"\r\n");
}

//void Diagnostic()
//{
//#if (DATA_PROCESSING_METHOD == FIFO )
//			  enqueue(&gsmPayload,(char *)getDiagDataString());
//#elif(DATA_PROCESSING_METHOD == LIFO)
//			  pushDataToStack(&gstrGMSPayloadLIFO,(char *)getDiagDataString());
//#endif
//}
void Diagnostic(enmDiagnosticStatus DiagnosticStatus )
{
	enqueue(&gsmPayload,(char *)getDiagDataString(DiagnosticStatus));
}

void BootUpdateFailed()
{
	u32FOTAFileBaseAddress = 0;
	gu32FotaRquestFlag = FALSE;
	u32MemoryWriteCycle = FALSE;
	gsmInstance.u8IncrementGsmState = FALSE;
	gu32AttemptFota = FALSE;
	gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
//	gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATACOMMAND;
	gsmInstance.enmGSMCommand = enmGSMSTATE_ATCMPUB;
}
/*************** END OF FUNCTIONS ***************************************************************************/


