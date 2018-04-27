#ifndef DEBUGMSGOUT_H
#define DEBUGMSGOUT_H
/*===============================================================================================*/
/**
 *   @file debugmsgout.h
 *
 *   @version v1.0
 */
/*=================================================================================================

 (c) Copyright , All Rights Reserved

Revision History:

Author          Date            Number          Description of Changes
--------        --------        -------         ------------------------
jsyoon          2015/01/05
===================================================================================================*/


/* Standard includes. */
/* FreeRTOS kernel includes. */
/* Application includes. */
#include "comdef.h"
#include "target.h"

/* CONSTANTS */

#define USE_STDIO

#define COMMAND_INT_MAX_OUTPUT_SIZE     500//1024//500
#define MAX_TXDATA_RINGBUF              1024
#define MAX_PRINT_STRBUF                128


/* FUNCTION PROTOTYPES */
char *consolCLIGetOutBuf( void );
void SendMsgToUART(void);
void TxCompleteFromUART(void);
void vDispMsg(char * ppcMessageToSend );
void vnDispMsg(  char * ppcMessageToSend , uint16_t len );
void InitDebugDataOutRB(void);


/*===============================================================================================*/
#endif 
