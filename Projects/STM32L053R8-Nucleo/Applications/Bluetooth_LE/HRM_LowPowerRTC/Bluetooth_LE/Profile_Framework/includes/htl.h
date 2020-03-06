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
*   FILENAME        -  hhtl.h
*
*   COMPILER        -  gnu gcc
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      09/03/2012
*   $Revision$:  first version
*   $Author$:    
*   Comments: 
*
*******************************************************************************
*  File Description 
*  ---------------------
* This file have Host HCI Transport layer definitations. 
******************************************************************************/
#ifndef _HHTL_H_
#define _HHTL_H_

#include <hal_types.h>
#include <ble_status.h>
#include <hci.h>
#include <hci_i.h>

/******************************************************************************
 * Macro Declerations
******************************************************************************/

/******************************************************************************
*  Type Definitions
******************************************************************************/

typedef uint32 tHciPacketReadState;
#define HTL_READ_HCI_PACKET_TYPE		(0x0)
#define HTL_READ_HCI_PACKET_HEADER		(0x1)
#define HTL_READ_HCI_PACKET_PARAMS		(0x2)

typedef uint32 tHciPacketWriteState;
#define HTL_WRITE_HCI_PACKET_TYPE		(0x0)
#define HTL_WRITE_HCI_PACKET_HEADER		(0x1)
#define HTL_WRITE_HCI_PACKET_PARAMS		(0x2)
#define HTL_WROTE_PACKET				(0x3)

typedef uint8  tHtlEventType;
#define HTL_PACKET_IND        			(0x01)
#define HTL_WRITE_COMPLETE    			(0x02)
#define HTL_TRANS_ERROR       			(0x03)

typedef void (* tHtlAciCallback )(tHtlEventType eventHtl, tBleStatus status, void *buff);

typedef struct _tHtlContext 
{
    tHciPacket *pHciTxPkt;

    tHciDataPacket *pHciRxPkt;

    tHtlAciCallback cbHtlAci;
    
	tHciPacketReadState HciPacketReadState;
	
	tHciPacketWriteState HciPacketWriteState;

    uint8 readLen;
    
    uint8 writeLen;
} tHtlContext;

/******************************************************************************
*  Function Prototypes
******************************************************************************/

tBleStatus HTL_Write(uint8 *htlDataPacket,int len);

/* 
 * this function reads received data from spi interface 
 */
tBleStatus HTL_Read(void);

tBleStatus HTL_RegisterCb(tHtlAciCallback callBack);

void HTL_Enqueue(tHciDataPacket* pPacket);

#endif /* _HHTL_H_ */
