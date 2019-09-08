#ifndef ADC_OPERATION_H_
#define ADC_OPERATION_H_
/*===============================================================================================*/
/**
 *   @file adc_operation.h
 *
 *   @version v1.0
 */
/*=================================================================================================


Revision History:

Modification Tracking
Author          Date            Number          Description of Changes
--------        --------        -------         ------------------------


===================================================================================================*/
/* Standard includes. */

/* Scheduler include files. */

/* Application include files. */
#include "ringbuf.h"

/*=================================================================================================
 CONSTANTS
=================================================================================================*/
#define ADC_BATT                3
#define ADC_CHARGE              4
#define ADC_BATT_TEMP           5


extern uint8_t     BatteryStatus;
extern uint8_t     ChargingStatus;
extern uint8_t     BatteryTemp;

/*=================================================================================================
 FUNCTION PROTOTYPES
=================================================================================================*/
boolean adcStartAdc(uint8_t chan);
boolean Read_ADC_value(uint8_t chan);

/*===============================================================================================*/
#endif  /* ADC_OPERATION_H_ */
