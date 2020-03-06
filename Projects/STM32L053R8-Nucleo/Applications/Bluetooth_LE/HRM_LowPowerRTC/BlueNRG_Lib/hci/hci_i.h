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
*   FILENAME        -  hhci_int.h
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
* This file have Host HCI's internal declarations which are not exposed to 
* other layers. 
******************************************************************************/
#ifndef _HHCI_INT_H_
#define _HHCI_INT_H_

#include <list.h>

/******************************************************************************
 * Macro Declarations
******************************************************************************/

/* packet states */
#define HCI_PACKET_STATE_IDLE			 (0x00)
#define HCI_PACKET_STATE_PROCESSING		 (0x01)
#define HCI_PACKET_STATE_PENDING		 (0x02)
#define HCI_PACKET_STATE_PENDING_EVT	 (0x03)

typedef uint8 tHciPacketType;
/* Type of HCI packets */
#define HCI_PKT_TYPE_INVALID             (0x00)
#define HCI_PKT_TYPE_COMMAND             (0x01)
#define HCI_PKT_TYPE_EVENT               (0x04)
#define HCI_PKT_TYPE_ACL                 (0x02)
#define HCI_PKT_TYPE_SCO                 (0x03)
#define HCI_PKT_TYPE_DATA                (0x05)

#define HCI_WRITE_PACKET_NUM_MAX 		 (5)
#define HCI_READ_PACKET_NUM_MAX 		 (5)

/******************************************************************************
*  Macro functions
******************************************************************************/
#define HCI_PACK_1_BYTE_PARAM(dest,src) \
        *((uint8 *)dest + 0) = (uint8)((*((uint8 *)src)) >> 0);
        

#define HCI_PACK_2_BYTE_PARAM(dest,src) \
        *((uint8 *)dest + 0) = (uint8)((*((uint16 *)src)) >> 0);   \
        *((uint8 *)dest + 1) = (uint8)((*((uint16 *)src)) >> 8);


#define HCI_PACK_3_BYTE_PARAM(dest,src) \
        *((uint8 *)dest + 0) = (uint8)((*((uint32 *)src)) >> 0);  \
        *((uint8 *)dest + 2) = (uint8)((*((uint32 *)src)) >> 16);
        

#define HCI_PACK_4_BYTE_PARAM(dest,src) \
        *((uint8 *)dest + 0) = (uint8)((*((uint32 *)src)) >> 0);  \
        *((uint8 *)dest + 1) = (uint8)((*((uint32 *)src)) >> 8);  \
        *((uint8 *)dest + 2) = (uint8)((*((uint32 *)src)) >> 16); \
        *((uint8 *)dest + 3) = (uint8)((*((uint32 *)src)) >> 24);


/******************************************************************************
*  Type Declarations
******************************************************************************/
typedef struct _tHciCommandPacket
{
    tHciCommandOpcode opcode;
    tHciCommandLengthParam lengthOfParams;
}tHciCommandPacket;


typedef struct _tHciEventPacket
{
    tHciEventCode eventCode;
    tHciEventLengthParam lengthOfParams;
}tHciEventPacket;


typedef struct _tHciACLPacket
{
    tHciACLPktHeader packetHeader;
    tHciACLPayloadLength payloadLangth;
}tHciACLPacket;

/* structure used to send data */
typedef struct _tHciPacket
{
    tListNode currentNode;
    tHciPacketType hciPktType;
	uint8 packetState;
	uint8 profileIdx;
    union { 
        tHciCommandPacket command;
        tHciACLPacket aclData;
    }packet; 
    uint8 dataBuffer[HCI_PACKET_SIZE]; 
}tHciPacket;

/* structure used to read received data */
typedef struct _tHciDataPacket
{
    tListNode currentNode;
    tHciPacketType hciPktType;
	uint8 dataBuff[HCI_PACKET_SIZE];
}tHciDataPacket;

/******************************************************************************
*  Function Prototypes
******************************************************************************/
void Process_RX_Data(tHciDataPacket* hciReadPacket);
#endif /* _HHCI_INT_H_ */
