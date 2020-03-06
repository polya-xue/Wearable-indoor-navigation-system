/******************************************************************************/
/*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2013 STMicroelectronics International NV
*
*   FILENAME        -  ble_status.h
*
*   COMPILER        -  gnu gcc v4.4.5
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$: 30/07/2011
*   $Revision$: first version
*   $Author$: 
*   Comments: Bluetooth Status and Error Codes       
*
*******************************************************************************
*
*  File Description 
*  ---------------------
* The status codes here are to notify the profiles internally about
* the status of the function calls. The error codes are not the HCI compliant codes
* since they need not be sent over HCI
*******************************************************************************/
#ifndef _BLE_STATUS_H
#define _BLE_STATUS_H

#include "hal_types.h"


/**
 * Bluetooth Status/Error Codes
 */ 
 
typedef uint8 tBleStatus; 

#define BLE_STATUS_SUCCESS            			(0x00)
#define BLE_STATUS_FAILED             			(0x01)
#define BLE_STATUS_INVALID_PARAMS     			(0x02)
#define BLE_STATUS_NULL_PARAM					(0x03)
#define BLE_STATUS_BUSY               			(0x04)
#define BLE_STATUS_PENDING            			(0x05)
#define BLE_STATUS_OUT_OF_MEMORY      			(0x06)
#define BLE_STATUS_ACTION_DISALLOWED			(0x07)
#define BLE_STATUS_PROFILE_CANNOT_COEXIST		(0x08)
#define BLE_STATUS_UNKNOWN_COMMAND				(0x09)
#define BLE_STATUS_HCI_CMD_INTF_BUSY			(0x0A)
#define BLE_STATUS_ALREADY_ADVERTISING			(0x0B)
#define BLE_STATUS_PROFILE_ALREADY_INITIALIZED	(0x0C)

#endif /* _BLE_STATUS_H */
