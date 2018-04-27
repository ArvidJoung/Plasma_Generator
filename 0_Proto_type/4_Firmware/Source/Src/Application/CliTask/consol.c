/*===============================================================================================*/
/**
 *   @file consol.c
 *
 *   @version v1.0
 */
/*=================================================================================================

Revision History:

Modification Tracking
Author          Date            Number          Description of Changes
--------        --------        -------         ------------------------


*/

/*===============================================================================================
 INCLUDE FILES
=================================================================================================*/

#include <stdlib.h>
//#include <conio.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
//#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "task.h"
#include "queue.h"

/* Demo program include files. */
#include "target.h"
#include "comdef.h"
#include "command.h"
#include "consol.h"
#include "consolcli.h"
#include "task_cfg.h"
#include "FreeRTOS_CLI.h"
#if defined(USING_CUBEMX_USB_DRIVER)
#include "usb_device.h"
#include "usbd_cdc_if.h"
#endif
#include "usart.h"
#include "signals.h"
#include "ringbuf.h"
#include "task_cmds.h"
#include "debugmsgout.h"


/*=================================================================================================
 LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
void vConsolTask( void *pvParameters );

/*==================================================================================================
 LOCAL CONSTANTS
==================================================================================================*/
#define consolINPUT_QUEUE_LENGTH        100
#if defined (USE_PRINT_Q)
#define consolPRINT_QUEUE_LENGTH        200
#endif

#define cmdMAX_INPUT_SIZE		        50


/*==================================================================================================
 LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
 LOCAL VARIABLES
==================================================================================================*/
static QueueHandle_t xInputQueue;
#if defined (USE_PRINT_Q)
static QueueHandle_t xPrintQueue;
#endif

/* The handle of the queue set to which the queues are added. */
static QueueSetHandle_t xQueueSet;

//static const char * pcTaskStartMsg = "Consol task started.\r\n";

//static char consolBuffer[ consolPRINT_QUEUE_LENGTH ];

/* A buffer into which command outputs can be written is declared here, rather
than in the command console implementation, to allow multiple command consoles
to share the same buffer.  For example, an application may allow access to the
command interpreter by UART and by Ethernet.  Sharing a buffer is done purely
to save RAM.  Note, however, that the command console itself is not re-entrant,
so only one command interpreter interface can be used at any one time.  For that
reason, no attempt at providing mutual exclusion to the cOutputBuffer array is
attempted. */

static char cInputString[ cmdMAX_INPUT_SIZE ];

uint8_t     rxUartMessage ;


/*==================================================================================================
 LOCAL FUNCTIONS
==================================================================================================*/
void vProcessingInputChar(uint8_t cIn)
{
    char *pcOutputString;
    static uint8_t ucInputIndex = 0;
    /* Dimensions the buffer into which input characters are placed. */

    //taskENTER_CRITICAL();
    if( (cIn == '\n') || (cIn == '\r'))
    {
        portBASE_TYPE xReturned;
        pcOutputString = consolCLIGetOutBuf( );

        /* Pass the received command to the command interpreter.  The
        command interpreter is called repeatedly until it returns pdFALSE
        (indicating there is no more output) as it might generate more than
        one string. */
        if(ucInputIndex !=0)
        {
            do
            {
                /* Get the next output string from the command interpreter. */
                xReturned = FreeRTOS_CLIProcessCommand( cInputString, pcOutputString, COMMAND_INT_MAX_OUTPUT_SIZE );
                vDispMsg( pcOutputString );

            } while( xReturned != pdFALSE );
        }
        ucInputIndex = 0;
        memset(cInputString,0x00,cmdMAX_INPUT_SIZE);
    }
    else
    {
        if( ucInputIndex < cmdMAX_INPUT_SIZE )
        {
            cInputString[ ucInputIndex ] = cIn;
            ucInputIndex++;
        }
    }
	//taskEXIT_CRITICAL();
}


/*-----------------------------------------------------------*/
void consolTaskInit(void)
{
	consolDebugCLIregister();
    HAL_UART_Receive_IT(&huart1, &rxUartMessage, 1);
}


/*-----------------------------------------------------------*/
void vConsolTask( void *pvParameters )
{
	command_type	        cmd;
	QueueSetMemberHandle_t  xQueue;
	QueueSetMemberHandle_t  xActivatedMember;
	uint8_t			        *data = NULL;

	xQueue = ( QueueSetMemberHandle_t * ) pvParameters;
    configASSERT( xQueue );

	consolTaskInit();

	for(;;)
	{
		xActivatedMember =  xQueueSelectFromSet( xQueue, queueSHORT_DELAY );

		/* Which set member was selected?  Receives/takes can use a block time
        of zero as they are guaranteed to pass because xQueueSelectFromSet() would
        not have returned the handle unless something was available. */
        if( xActivatedMember == xCliTaskQueue )
        {
            xQueueReceive( xActivatedMember, &cmd, 0 );
            data = (uint8_t *)cmd.msg;

    		switch(cmd.cmd)
    		{
                case CONSOL_RESET_CMD_F:
                    break;
                    
                case CONSOL_TASK_STOP_F:
                    break;

                case CONSOL_RX_COMPLETE_EVT:
                    HAL_UART_Receive_IT(&huart1, &rxUartMessage, 1);

#if(0)
                    portENTER_CRITICAL();
                    xQueueSend(xInputQueue, &rxMessage, 1);
                    portEXIT_CRITICAL();
#else                    
                    vProcessingInputChar(rxUartMessage);
                    vnDispMsg((char *)(&rxUartMessage),1);
#endif
                    break;

                case CONSOL_TX_COMPLETE_EVT:
                    TxCompleteFromUART();
                    break;

                case CONSOL_SEND_MSG_F:
                    SendMsgToUART();
                    break;
                    
    			default:
    				break;
    		}

            if( (cmd.msg != NULL) && (cmd.isconst == FALSE))
    		{
    			cmd_mfree(cmd.msg);
    		}
        }
        
#if defined (USE_PRINT_Q)
        else if( xActivatedMember == xPrintQueue )
        {
            xQueueReceive( xActivatedMember, &rxMessage, 0 );
            // Send Routine
        }
#endif
#if(0)
        else if( xActivatedMember == xInputQueue )
        {
            xQueueReceive( xActivatedMember, &rxMessage, 0 );
            vProcessingInputChar(rxMessage);
        }
#endif        
        else
        {
            // No cmd rxed, 2000 msec wait timeout
        }
	}
}


/*==================================================================================================
 GLOBAL FUNCTIONS
==================================================================================================*/
void vStartCONSOLTasks( void )
{

	/*First Create the queue set such that it will be able to hold a message for
	every space in every queue in the set. */
#if defined (USE_PRINT_Q)
    xQueueSet = xQueueCreateSet( taskCLI_QUEUE_LENGTH + consolINPUT_QUEUE_LENGTH + consolPRINT_QUEUE_LENGTH);
#else
    xQueueSet = xQueueCreateSet( taskCLI_QUEUE_LENGTH + consolINPUT_QUEUE_LENGTH );
#endif

	/* Create the queue that we are going to use for the
	prvSendFrontAndBackTest demo. */
	xCliTaskQueue = xQueueCreate( taskCLI_QUEUE_LENGTH, sizeof( command_type ) );
    configASSERT( xCliTaskQueue );

#if defined (USE_PRINT_Q)
	xPrintQueue = xQueueCreate( consolPRINT_QUEUE_LENGTH, sizeof( uint8_t ) );
    configASSERT( xPrintQueue );
#endif
	/* Create the queue on which errors will be reported. */
	xInputQueue = xQueueCreate( consolINPUT_QUEUE_LENGTH, sizeof( uint8_t ) );
    configASSERT( xInputQueue );

	xQueueAddToSet( xCliTaskQueue, xQueueSet );
#if defined (USE_PRINT_Q)
	xQueueAddToSet( xPrintQueue, xQueueSet );
#endif    
	xQueueAddToSet( xInputQueue, xQueueSet );

    InitDebugDataOutRB();    

	/* Spawn the task. */
	//xTaskCreate( vColsolTask, "CLIx", taskCLI_TASK_STACK_SIZE, ( void * ) xCliTaskQueue, taskCLI_TASK_PRIORITY, ( TaskHandle_t * ) NULL );
	xTaskCreate( vConsolTask, "CLIx", taskCLI_TASK_STACK_SIZE, ( void * )xQueueSet, taskCLI_TASK_PRIORITY, ( TaskHandle_t * ) NULL );

}

