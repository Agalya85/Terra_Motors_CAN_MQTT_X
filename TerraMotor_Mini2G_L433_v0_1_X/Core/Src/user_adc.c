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
#include "externs.h"
#include "applicationdefines.h"
#include "user_adc.h"
#include "adc.h"
#include "timer.h"
//#include "user_adc.h"
/*Two Diodes are used for power path switch */
#define INPUT_DIODE_DROP	(float)(0.7)
#define MAX_INPUT_VTG			   (24)
#define MAX_BATT_VTG		(float)(4.2)
#define ADC_RESOLUTION			   (2979)
#define ADC_RESOLUTION_BATT		   (2730)

/* System timings will be delayed if supply is below this value
 * Tracks Input supply to battery supply transition  */
#define VIN_BATTERY_VTG		(float)(11.5)	/* System timings will be delayed if supply is switched to battery */

/*DMA Interrupt Stores data in this array */
uint32_t gau32AnalogPeripheralData[3] = {0};
uint32_t gau32BatteryPeripheralData[2] = {0};
/*User modified ADC Peripheral Data */
uint32_t gau32AnalogPeripheralProcessedData[5] = {0};
uint32_t gau32BatteryProcessedData[2] = {0};
volatile uint32_t g32AdcConversionStatus = FALSE;
volatile uint32_t g32AdcConversionStatusBatt = FALSE;
float gfInputSupplyVoltage = 0;
float gfBatteryVoltage = 0;

/* Tracks Power supply to battery switching */
uint32_t gu32InputSupplySwitchedAlert = FALSE;

enmAdcState enmAdcSMCurrentState = enmADC_IDLE;

extern volatile uint32_t gu32ADCPollTimer;
extern volatile uint32_t gu32ADCOperationTimer;

/****************************************************************************
 Function: operateadc
 Purpose: State machine implementation for ADC
 Input:	None.
 Return value: None


 Note(s)(if-any) :

gau32AnalogPeripheralProcessedData[ADC_CH1] = gau32AnalogPeripheralData[ADC_CH1];
gau32AnalogPeripheralProcessedData[ADC_CH2] = gau32AnalogPeripheralData[ADC_CH2];
gau32AnalogPeripheralProcessedData[ADC_CH3] = gau32AnalogPeripheralData[ADC_CH3];
gau32AnalogPeripheralProcessedData[ADC_CH4] = gau32AnalogPeripheralData[ADC_CH4];


 Change History:
 Author            	Date                Remarks
 KloudQ Team        11/11/20			Initial Logic
******************************************************************************/
void operateadc(void)
{
	if(gu32ADCPollTimer)
		return;

	switch(enmAdcSMCurrentState)
	{
		case enmADC_IDLE:
			/* Start ADC 1 and ADC 3 */
			if((HAL_ADC_Start_DMA(&hadc1, (uint32_t*)gau32AnalogPeripheralData, 2) != HAL_OK))
				enmAdcSMCurrentState = enmADC_ERROR;
			else
			{
				HAL_ADC_Start_IT(&hadc1);
				enmAdcSMCurrentState = enmADC_GETDATA;
				gu32ADCOperationTimer = ONE_MIN;
			}
			break;

		case enmADC_GETDATA:
			if((g32AdcConversionStatus == TRUE) && (gu32ADCOperationTimer != 0))
			{

				/* ADC conversion completed */
				gu32ADCOperationTimer = 0;
				gau32AnalogPeripheralProcessedData[ADC_CH1] = gau32AnalogPeripheralData[ADC_CH1];
				gau32AnalogPeripheralProcessedData[ADC_VIN] = gau32AnalogPeripheralData[ADC_VIN];
				/* Add other channels when configured */
				enmAdcSMCurrentState = enmADC_PROCESSDATA;
				HAL_ADC_Stop_DMA(&hadc1);
				g32AdcConversionStatus = FALSE;

			}
			else if((!g32AdcConversionStatus) | (!g32AdcConversionStatusBatt) | (gu32ADCOperationTimer == 0))
			{
				/*Error in ADC Conversion */
				enmAdcSMCurrentState = enmADC_IDLE;
			}
			break;

		case enmADC_ERROR:
			/* Process ADC Errors */
			gu32ADCPollTimer = TEN_SEC;
			enmAdcSMCurrentState = enmADC_IDLE;
			break;

		case enmADC_PROCESSDATA:
			/* Process Data as per connected slave / divider / sensor Output(s)*/
			calculateInputVoltage();
			gu32ADCPollTimer = TEN_SEC;
			enmAdcSMCurrentState = enmADC_IDLE;
			break;

		default:
			/* Undefined State . Restart the State Machine */
			gu32ADCPollTimer = TEN_SEC;
			enmAdcSMCurrentState = enmADC_IDLE;
			break;
	}
}
/****************************************************************************
 Function: calculateInputVoltage
 Purpose: Calculate Input Supply Voltage from adc data
 Input:	None
 Return value: None


 Note(s)(if-any) :


 Change History:
 Author            	Date                Remarks
 KloudQ Team        11/11/20			Initial Logic
******************************************************************************/
void calculateInputVoltage(void)
{
	/* Calculate Input Votage from adc data */
	gfInputSupplyVoltage = (float) ((MAX_INPUT_VTG * gau32AnalogPeripheralProcessedData[ADC_CH1])/ADC_RESOLUTION);
	//gfBatteryVoltage = (float) ((MAX_BATT_VTG * gau32BatteryProcessedData[ADC_VIN])/ADC_RESOLUTION_BATT);
//	gfBatteryVoltage += 1;
//	gfBatteryVoltage /= 0.6;

	gfBatteryVoltage = ((((3.3 * ( 2 * gau32AnalogPeripheralProcessedData[ADC_VIN])) / 4095) * 2 ) / 0.6 ) - 0.2;
	/* Add diode drop for actual voltage */
	gfInputSupplyVoltage += INPUT_DIODE_DROP;

	/* Check if system is powered from 8.4 V dc battery pack */
	if(gfInputSupplyVoltage <= VIN_BATTERY_VTG)
	{
		gu32InputSupplySwitchedAlert = TRUE;
	}
	else
	{
		gu32InputSupplySwitchedAlert = FALSE;
	}
}
/****************************************************************************
 Function: void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
 Purpose: callback function for ADC conversion complete ISR
 Input:	ADC structure.
 Return value: None


 Note(s)(if-any) :


 Change History:
 Author            	Date                Remarks
 KloudQ Team        21/01/19			Added info header
******************************************************************************/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	/*Sequence conversion complete . Read Processed Value */
	if(hadc->Instance == ADC1)
	{
		if(g32AdcConversionStatus == FALSE)
			g32AdcConversionStatus = TRUE;
	}
	else
	{
		/* Unknown Interrupt */
	}
}

