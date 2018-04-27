/*===============================================================================================*/
/**
 *   @file Plasma_state.c
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
#include "signals.h"
#include "command.h"
#include "task_cfg.h"
#include "Plasma_state.h"
#include "debugmsgcli.h"
#include "interface.h"
#include "tim.h"
#include "timergen.h"
#include "timers.h"

/*=================================================================================================
 LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
 LOCAL CONSTANTS
==================================================================================================*/
	
#define LED_ON(mask)	    HAL_GPIO_WritePin(mask##_GPIO_Port, mask##_Pin, GPIO_PIN_SET);
#define LED_OFF(mask)	    HAL_GPIO_WritePin(mask##_GPIO_Port, mask##_Pin, GPIO_PIN_RESET);


/*==================================================================================================
 LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
 LOCAL MACROS
==================================================================================================*/
#define PLASMA_SPAN_TIME_PWM_MSEC 10
#define PLASMA_STEP_TIME_PWM_MSEC 5 // SPAN*STEP은 정수이어야 함

#define TIM1_OUTPUT_CLK				(80000)	//TIM1 output clock = 80KHz
#define PLASMA_FREQ					((TIM1_COUNTER_CLK/TIM1_OUTPUT_CLK)-1)		// TIM1 Period

/*==================================================================================================
 LOCAL VARIABLES
==================================================================================================*/
static uint8_t PlasmaVol = 0;
static boolean isPlasmaOn = FALSE;

TimerHandle_t xplasmaOnTimer = NULL;
TimerHandle_t xplasmaOffTimer = NULL;

static uint32_t PlasmaOnTime=0,PlasmaOffTime=0;

/*==================================================================================================
 GLOBAL VARIABLES
==================================================================================================*/


/*==================================================================================================
 LOCAL FUNCTIONS
==================================================================================================*/

uint32_t getPlasmaVolLevel()
{
	uint32_t PlasmaVolLevel = 0, PlasmaADCStep = 0;
	PlasmaADCStep = 255/(PLASMA_STEP_TIME_PWM_MSEC-1);
	PlasmaVolLevel = (float)(((255-PlasmaVol)/PlasmaADCStep)+1.5);

	return PlasmaVolLevel;
}
uint32_t getPlasmaTime(boolean *isOnFlag)
{
	uint32_t ret_val, ontime;

	ontime = (PLASMA_SPAN_TIME_PWM_MSEC/PLASMA_STEP_TIME_PWM_MSEC)*getPlasmaVolLevel();
	if(*isOnFlag)
	{
		if(ontime < PLASMA_SPAN_TIME_PWM_MSEC)
		{
			*isOnFlag = FALSE;
			ret_val = ontime;
		}
		else{								// always ON
			*isOnFlag = TRUE;
			ret_val = ontime;
		}
	}
	else
	{
		if(ontime < PLASMA_SPAN_TIME_PWM_MSEC)
		{
			*isOnFlag = TRUE;
			ret_val = PLASMA_SPAN_TIME_PWM_MSEC - ontime;
		}
		else{								// always ON
			*isOnFlag = TRUE;
			ret_val = ontime;
		}
	}
    return ret_val;
}

boolean calcPlasmaOnOffTime()
{
    PlasmaOnTime = (PLASMA_SPAN_TIME_PWM_MSEC/PLASMA_STEP_TIME_PWM_MSEC)*getPlasmaVolLevel();
    PlasmaOffTime = (PLASMA_SPAN_TIME_PWM_MSEC - PlasmaOnTime);
    
    if(PlasmaOffTime == 0 ) 
        return FALSE;
    else
        return TRUE;
}


void Plasma_Start(void)
{
    HAL_GPIO_WritePin(EN__8V_GPIO_Port, EN__8V_Pin, GPIO_PIN_SET);
    
    MX_TIM1_Init(PLASMA_FREQ, PLASMA_FREQ/2, TRUE);

    /* TIM1 enable counter */
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1); 
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);    
}

void Plasma_Pause(void)
{
    MX_TIM1_Init(PLASMA_FREQ, PLASMA_FREQ/2, FALSE);
    //MX_TIM1_Preload_Set(FALSE);
}

void Plasma_Stop(void)
{
    /* TIM1 disable counter */
    HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1); 
    HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_4); 
    HAL_GPIO_WritePin(EN__8V_GPIO_Port, EN__8V_Pin, GPIO_PIN_RESET);
}

/*==================================================================================================
 GLOBAL FUNCTIONS
==================================================================================================*/

void PlasmaState_init(TimerCallbackFunction_t pxCallbackFunction)
{

	/* Create a one-shot timer for use later on in this test. */
	xplasmaOnTimer = xTimerCreate(	"",				/* Text name to facilitate debugging.  The kernel does not use this itself. */
								250,					/* The period for the timer(1 sec). */
								pdFALSE,				/* Don't auto-reload - hence a one shot timer. */
								( void * )PLASMA_ON_TIMER_F,	/* The timer identifier. */
								pxCallbackFunction );	/* The callback to be called when the timer expires. */

	xplasmaOffTimer = xTimerCreate(	"",				/* Text name to facilitate debugging.  The kernel does not use this itself. */
								250,					/* The period for the timer(1 sec). */
								pdFALSE,				/* Don't auto-reload - hence a one shot timer. */
								( void * )PLASMA_OFF_TIMER_F,	/* The timer identifier. */
								pxCallbackFunction );	/* The callback to be called when the timer expires. */

    // Start Plasma module trigger
    //sendtoOutEventCmd(PLASMA_EVENT_START_F);
}
    
boolean CheckIsPlasmaCmd(command_type *command)
{
    boolean         isprocessed = FALSE;
    uint8_t         *data = (uint8_t *)command->msg;
    
    if(ISPLASMASTATE_EVENT(command->cmd))
    {
        isprocessed = TRUE;
        
        switch(command->cmd)
        {
			case PLASMA_ON_TIMER_F:
				if(isPlasmaOn)
				{
					if(PlasmaOffTime > 0)
					{
                        Plasma_Pause();
                        timer_set_timer(xplasmaOffTimer,PlasmaOffTime);
					}
					else
					{
                        calcPlasmaOnOffTime();
                        timer_set_timer(xplasmaOnTimer,PlasmaOnTime);
					}
				}
				break;

			case PLASMA_OFF_TIMER_F:
				if(isPlasmaOn)
				{
                    calcPlasmaOnOffTime();
                    timer_set_timer(xplasmaOnTimer,PlasmaOnTime);
                    Plasma_Start();
				}
				break;
                
            case PLASMA_LED_ON_F:
                LED_ON(OP_LED);
                DBGHI(OUTEVENT,"Plasma LED On.\r\n");
                break;
                
            case PLASMA_LED_OFF_F:
                LED_OFF(OP_LED);
                DBGHI(OUTEVENT,"Plasma LED Off.\r\n");
                break;
				
        	case PLASMA_VOL_CTRL_F:
                PlasmaVol = (uint8_t)data[FDPOS(outevent_cmd_msg_field_type,Cmd)];              
				break;
				
			case PLASMA_START_CMD_F:
                isPlasmaOn = TRUE;
                calcPlasmaOnOffTime();
                timer_set_timer(xplasmaOnTimer,PlasmaOnTime);
                Plasma_Start();
                DBGHI(OUTEVENT,"Plasma Start.\r\n");
				break;
						
			case PLASMA_STOP_CMD_F:
                Plasma_Stop();
                isPlasmaOn = FALSE;
                DBGHI(OUTEVENT,"Plasma Stopped.\r\n");
				break;
       
			default:
				break;
        }
    }

    return isprocessed;
}    

/*===============================================================================================*/
