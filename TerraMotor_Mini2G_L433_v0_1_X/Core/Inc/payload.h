/*
 * payload.h
 *
 *  Created on: Apr 19, 2021
 *      Author: admin
 */

#ifndef INC_PAYLOAD_H_
#define INC_PAYLOAD_H_
#include "user_error.h"

#define START_OF_FRAME			"*"									/* Payload SOF Character */
#define END_OF_FRAME			"#"									/* Payload EOF Character */
#define FIRMWARE_VER			"v1.0"								/* Firmware Version  */
#define BOOT_REGION				"X"
/* Payload Funtion Prototypes */
void updateSystemParameters(void);
char * getMachineDataString(void);
void updateCanPayload(uint32_t data ,char * systemPayload );
void updateTelInfo(uint32_t data ,char * systemPayload);
void updateTelVersion(uint32_t data ,char * systemPayload);
void SperateGPSString(void);
char * getDiagDataString(enmDiagnosticStatus DiagnosticStatus);
void DiagnosticString(char * systemPayload,enmDiagnosticStatus DiagnosticStatus);
void CANIDPrase(char * systemPayload);
void PaylodTime(char * systemPayload);
void updateInputVoltage(float Volt);
#endif /* INC_PAYLOAD_H_ */
