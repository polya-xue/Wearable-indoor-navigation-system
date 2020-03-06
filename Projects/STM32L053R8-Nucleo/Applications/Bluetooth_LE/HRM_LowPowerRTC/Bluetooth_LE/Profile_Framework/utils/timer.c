/******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2013 STMicroelectronics International NV
*
*   FILENAME        -  timer.c
*
*   COMPILER        -  gnu gcc
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      
*   $Revision$:  first version
*   $Author$:    
*   Comments: 
*
*******************************************************************************
*
*  File Description 
*  ---------------------
* This file have implementation of Timer task to be used for timeout 
* functionality. 
******************************************************************************/

/******************************************************************************
 * Include Files
******************************************************************************/
#include <hal_types.h>
#include <list.h>
#include <timer.h>

/******************************************************************************
 * Function Definitions 
******************************************************************************/
tBleStatus Timer_Init(void)
{
  // FIXME: implement me
  return (BLE_STATUS_SUCCESS);
}

tBleStatus Timer_Start(ulint expiryTime,
					   TIMER_TIMEOUT_NOTIFY_CALLBACK_TYPE timercb,
					   tTimerID *timerID)
{
  // FIXME: implement me
  return (BLE_STATUS_SUCCESS);
}					   

tBleStatus Timer_Stop(tTimerID timerID)
{
  // FIXME: implement me
  return (BLE_STATUS_SUCCESS);
}

void Timer_Process_Q(void)
{
  // FIXME: implement me
}

void Timer_ExpiryCallback(void)
{
  // FIXME: implement me
}
