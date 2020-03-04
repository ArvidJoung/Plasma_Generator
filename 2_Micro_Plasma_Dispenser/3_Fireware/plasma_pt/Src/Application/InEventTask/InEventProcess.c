/*===============================================================================================*/
/**
 *   @file InEventProcess.c
 *
 *   @version v1.0
 */
/*=================================================================================================

Revision History:

Modification Tracking
Author          Date            Number          Description of Changes
--------        --------        -------         ------------------------

Portability:
Indicate if this module is portable to other compilers or
platforms. If not, indicate specific reasons why is it not portable.
*/

/*===============================================================================================
 INCLUDE FILES
=================================================================================================*/

/* Standard includes. */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Application includes. */
#include "target.h"
#include "comdef.h"
#include "signals.h"
#include "command.h"
#include "task_cfg.h"
#include "interface.h"
#include "InEventProcess.h"
#include "ADC/adc_operation.h"
#include "debugmsgcli.h"
#include "task_cmds.h"
#include "stm32f0xx_hal.h"

/*=================================================================================================
 LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
 LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
 LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
 LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
 LOCAL VARIABLES
==================================================================================================*/
uint8_t IsCharging = 0xFF;
boolean IsExtPwrInState = FALSE;

// for op key
boolean isOpSWKeyChecking = FALSE;
static uint8_t OpKeyDownCount = 0;
static uint8_t OpKeyUpCount = 0;


// for down key
boolean isDownKeyChecking = FALSE;
static uint8_t DownKeyCount = 0;

// for up key
boolean isUpKeyChecking = FALSE;
static uint8_t UpKeyCount = 0;


boolean isPlasmaOperate = FALSE;

/*==================================================================================================
 GLOBAL VARIABLES
==================================================================================================*/


/*==================================================================================================
 LOCAL FUNCTIONS
==================================================================================================*/
void ProcessBattADCval(uint16_t command, uint8_t adc_val );

/*==================================================================================================
 GLOBAL FUNCTIONS
==================================================================================================*/
uint8_t InEventCheckDebounce(void)
{
    uint8_t isTimerRequired = 0;

    if(isOpSWKeyChecking) 
    {
        if(HAL_GPIO_ReadPin(KEY_OP_GPIO_Port,KEY_OP_Pin) == GPIO_PIN_RESET)
        {
            OpKeyDownCount++;
            OpKeyUpCount = 0;
            if(OpKeyDownCount >= 3)
            {
                OpKeyDownCount = 0;
                if(isPlasmaOperate == FALSE)
                {
                    isPlasmaOperate = TRUE;
                    isOpSWKeyChecking = FALSE; 	// Arvid 2019.12.15
                    sendMcCommand(MC_OPERATE_ON_F);
                    DBGHI(INEVENT,"Operate Switch on state !!!\r\n");
                }
                //Arvid 2019.12.15
                else
                {
                    isPlasmaOperate = FALSE;
                    isOpSWKeyChecking = FALSE;
                    sendMcCommand(MC_OPERATE_OFF_F);
                    DBGHI(INEVENT,"Operate Switch off state !!!\r\n");
                }
            }
            isTimerRequired++;                
        }
        /*
        else
        {
            OpKeyUpCount++;
            OpKeyDownCount = 0;
            if(OpKeyUpCount >= 3)
            {
                OpKeyUpCount = 0;
                if(isPlasmaOperate == TRUE)
                {
                    isPlasmaOperate = FALSE;
                    isOpSWKeyChecking = FALSE;
                    sendMcCommand(MC_OPERATE_OFF_F);
                    DBGHI(INEVENT,"Operate Switch off state !!!\r\n");
                }
            }
            else
            {
                isTimerRequired++;    
            }

        }*/
    }

    if(isDownKeyChecking) 
    {
        if(HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port,KEY_DOWN_Pin) == GPIO_PIN_RESET)
        {
            DownKeyCount++;
            
            if(DownKeyCount >= 3)
            {
                isDownKeyChecking = FALSE;
                sendMcCommand(MC_DOWN_KEY_PRESSED_F);
                DBGHI(INEVENT,"Down key Pressed !!!\r\n");
            }
            else
            {
                isTimerRequired++;                
            }
        }
        else
        {
            DownKeyCount = 0;
            isDownKeyChecking = FALSE;
        }
    }

    if(isUpKeyChecking) 
    {
        if(HAL_GPIO_ReadPin(KEY_UP_GPIO_Port,KEY_UP_Pin) == GPIO_PIN_RESET)
        {
            UpKeyCount++;
            
            if(UpKeyCount >= 3)
            {
                isUpKeyChecking = FALSE;
                sendMcCommand(MC_UP_KEY_PRESSED_F);
                DBGHI(INEVENT,"Up key Pressed !!!\r\n");
            }
            else
            {
                isTimerRequired++;                
            }
        }
        else
        {
            UpKeyCount = 0;
            isUpKeyChecking = FALSE;
        }
    }

    return isTimerRequired;
}

void InEventCheckKeyOPStart(void)
{
    OpKeyDownCount = 1;
    OpKeyUpCount = 0;
    isOpSWKeyChecking = TRUE;
}

void InEventCheckKeyDownStart(void)
{
    DownKeyCount = 1;
    isDownKeyChecking = TRUE;
}

void InEventCheckKeyUpStart(void)
{
    UpKeyCount = 1;
    isUpKeyChecking = TRUE;
}

void InEventCheckUSBDetectPin(void)
{
    if(HAL_GPIO_ReadPin(USB_DET_GPIO_Port,USB_DET_Pin) == GPIO_PIN_SET)
    {
        IsExtPwrInState = TRUE;
    }
}

void InEventBattResultReport(uint8_t adc_chan)
{
    if(adc_chan == ADC_BATT)
    {
        ProcessBattADCval(INEVENT_BATT_VAL_EVT, BatteryStatus);
    }
}

/*===========================================================================
FUNCTION           
DESCRIPTION     
DEPENDENCIES
RETURN VALUE
===========================================================================*/
void ProcessBattADCval(uint16_t command, uint8_t adc_val )
{
    switch(command)
    {

        case INEVENT_BATT_VAL_EVT:
        	//if(IsExtPwrInState == FALSE)
			//{
				if(adc_val < BATT_CUTOFF_VOLT_VAL)
				{
					DBGERR(INEVENT,"Battery Cutoff Event occurred!!!\r\n");
					//sendMcCommand(MC_POWEROFF_CMD_F);
				}
				else if(adc_val > BATT_FULL_VOLT_VAL)
				{
					DBGERR(INEVENT,"Battery Cutoff Event occurred!!!\r\n");
					//sendMcCommand(MC_POWEROFF_CMD_F);
				}
			//}

            break;

        default:
            break;
    }
}
/*===============================================================================================*/
