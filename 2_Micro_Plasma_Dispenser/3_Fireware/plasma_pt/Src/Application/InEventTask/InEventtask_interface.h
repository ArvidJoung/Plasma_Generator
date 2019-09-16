#ifndef INEVENTTASK_INTERFACE_H
#define INEVENTTASK_INTERFACE_H
/*===============================================================================================
 *
 *   @file InEventtask_interface.h
 *
 *   @version v1.0
 *
 =================================================================================================


Revision History:

Modification Tracking
Author          Date            Number          Description of Changes
--------        --------        -------         ------------------------

Portability:
Indicate if this module is portable to other compilers or
platforms. If not, indicate specific reasons why is it not portable.
*/

/*================================================================================================
 INCLUDE FILES
=================================================================================================*/

/*=================================================================================================
 CONSTANTS
=================================================================================================*/


/*=================================================================================================
 ENUMS
=================================================================================================*/


/*=================================================================================================
 STRUCTURES AND OTHER TYPEDEFS
=================================================================================================*/

typedef struct {
    uint8_t             Cmd[1];
    uint8_t             Dummy[1];
    uint8_t             EndofData[1];
} inevent_cmd_msg_field_type;


/*===============================================================================================*/
#endif  /* INEVENTTASK_INTERFACE_H*/