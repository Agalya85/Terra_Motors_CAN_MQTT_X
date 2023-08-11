/*
 * user_adc.h
 *
 *  Created on: Sep 28, 2021
 *      Author: admin
 */

#ifndef INC_USER_ADC_H_
#define INC_USER_ADC_H_


#define ADC_VIN	(0)		/* Hardware Connection :  VBAT */
#define ADC_CH1 (1)		/* Hardware Connection :  Vin_Sys*/

//float gfInputSupplyVoltage = 0;
//float gfBatteryVoltage = 0;

typedef enum
{
	enmADC_IDLE,
	enmADC_GETDATA,
	enmADC_PROCESSDATA,
	enmADC_ERROR
}enmAdcState;

void operateadc(void);
void calculateInputVoltage(void);

#endif /* INC_USER_ADC_H_ */
