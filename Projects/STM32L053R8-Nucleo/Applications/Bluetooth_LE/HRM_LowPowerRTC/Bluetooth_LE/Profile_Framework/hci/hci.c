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
*   FILENAME        -  hci.c
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
*
*  File Description 
*  ---------------------
* This file will have Host HCI API implementations which are exposed to upper 
* layers. 
* 
******************************************************************************/


/******************************************************************************
 * To do:
 * Reset BLE controller and HCI in the case of invalid event packet received, 
   due to any Rx/Tx error at SPI interface level. 
 *****************************************************************************/

/******************************************************************************
 * Include Files
******************************************************************************/
#include <host_config.h>
#include "hal_types.h"
#include <list.h>
#include <ble_status.h>
#include <host_osal.h>
#include <debug.h>
#include "hci.h"
#include "hci_i.h"
#include <htl.h>
#include <profile_hci_interface.h>
#include <ble_profile.h>

/******************************************************************************
 * Macro Declarations
 *****************************************************************************/


/******************************************************************************
 * Local Function Prototypes
 *****************************************************************************/
void Htl_Hci_Event_Handler(tHtlEventType eventHtl, tBleStatus status, void *pBuff);
void HCI_Enqueue_Packet_To_HTL(void);
extern int HCI_Compose_Acl_Packet ( uint8 * HciTxDataPacket, tHciPacket * HCIAclPacket );
extern int HCI_Compose_Cmd_Packet ( uint8 * HciTxDataPacket, tHciPacket * HCICmdPacket );
/******************************************************************************
*  Local Variables
******************************************************************************/
/* pool of hci write packets */
static tHciPacket     hciWritePacketBuffer[HCI_WRITE_PACKET_NUM_MAX];

/* pool of hci read packets */
static tHciDataPacket     hciReadPacketBuffer[HCI_READ_PACKET_NUM_MAX];

uint8 gHciNumOfFreeCmdPktAtBLE = 0;

/******************************************************************************
 * Global Variable 
******************************************************************************/
HCI_READ_ACL_PACKET_NOTIFY_TYPE HCIAclPacketNotifyFunc = NULL;
HCI_READ_EVENT_NOTIFY_TYPE HCIEventPacketNotifyFunc = NULL;
int HCI_Process_Q_LP = 0;

/******************************************************************************
*  Variables imported from other files
******************************************************************************/

extern tListNode hciWritePktPool;
extern tListNode hciWritePktQueue;
extern tListNode hciReadPktPool;
extern tListNode hciReadPktRxQueue;

/******************************************************************************
 * Imported Variable
******************************************************************************/

extern void *hostdbgfile;


/*******************************************************************************
 * Function Definitions 
*******************************************************************************/
/**
 * HCI_Init 
 * 
 * Initializes the HCI RX and TX queues
 * Initializes the HTL layer
 * registers callback to the HTL
 */ 
tBleStatus HCI_Init (void)
{
	uint32 index = 0;
	  
    HCI_MESG_DBG(hostdbgfile,"HCI_Init\n");
    
	HCIAclPacketNotifyFunc = NULL;
    HCIEventPacketNotifyFunc = NULL;
	HCI_Process_Q_LP = 0;
	gHciNumOfFreeCmdPktAtBLE = 0;
	
	/* initialize the lists */

    /* Initialize the hci typed packet pool and packet data pool */
    Osal_MemSet ((void *)hciWritePacketBuffer, 0, sizeof(tHciPacket)*HCI_WRITE_PACKET_NUM_MAX);
    
    /* Initialize list heads of ready and free hci packet queues */
    list_init_head (&hciWritePktPool);
    list_init_head (&hciWritePktQueue);

    /* Initialize the queue of free hci packets */
    for (index = 0; index < HCI_WRITE_PACKET_NUM_MAX; index++)
    {
        hciWritePacketBuffer[index].hciPktType = HCI_PKT_TYPE_INVALID;
        list_insert_tail (&hciWritePktPool, (tListNode *)&hciWritePacketBuffer[index]);
    }
    HCI_MESG_DBG(hostdbgfile,"Num of packets in Write Pool %d \n",index);

    /* Initialize the hci data packet pool and data packet buffer pool */
    Osal_MemSet ((void *)hciReadPacketBuffer, 0, sizeof(tHciDataPacket)*HCI_READ_PACKET_NUM_MAX);
    
    /* Initialize list heads of ready and free hci data packet queues */
    list_init_head (&hciReadPktPool);
    list_init_head (&hciReadPktRxQueue);

    /* Initialize the queue of free hci data packets */
    for (index = 0; index < HCI_READ_PACKET_NUM_MAX; index++)
    {
        hciReadPacketBuffer[index].hciPktType = HCI_PKT_TYPE_INVALID;
        list_insert_tail (&hciReadPktPool, (tListNode *)&hciReadPacketBuffer[index]);
    }
    HCI_MESG_DBG(hostdbgfile,"Num of packets in Read Pool %d \n",index);

	/* Register the callback to HTL layer */
	HTL_RegisterCb(Htl_Hci_Event_Handler);
	gHciNumOfFreeCmdPktAtBLE = 1;
   
   /* enqueue a packet to HTL */
	HCI_Enqueue_Packet_To_HTL();
	return BLE_STATUS_SUCCESS;
}

/**
 * Htl_Hci_Event_Handler
 * 
 * This function is registered as a callback to the HTL.
 * It will be called when HTL has read one complete HCi packet
 * from SPI
 */ 
void Htl_Hci_Event_Handler(tHtlEventType eventHtl, tBleStatus status, void *pBuff)
{
	HCI_MESG_DBG(hostdbgfile,"Htl_Hci_Event_Handler\n");
	
	/* enqueue the event packet received into the event list.
	 * It will be processed in the main loop
	 */ 
	list_insert_tail(&hciReadPktRxQueue,(tListNode*)pBuff);
	
	HCI_Enqueue_Packet_To_HTL();
}

/*
 * Function: HCI_send_command 
 */
tBleStatus HCI_Send_Command(uint8 profileID,
							   uint16 opcode,
                               uint8 numOfCmdParam,
                               void * (* cmdParamPtrArray)[])
{
    tBleStatus retval = BLE_STATUS_FAILED;
    tHciPacket * cmdPacket = NULL;
    uint8 * dataBuff = NULL;
    void * (* cmdParam)[15];

#if (defined HCI_VENDOR_CMD_GATT_ADD_CHAR_DESC)
    uint8 charDescValueLen = 0;
#endif

#if ((defined HCI_HOST_CTL_CMD_HOST_NUM_COMP_PKTS) || \
     (defined HCI_VENDOR_CMD_GAP_SET_LIMITED_DISCOVERABLE) || \
     (defined HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE) || \
     (defined HCI_VENDOR_CMD_GATT_ADD_CHAR) || \
     (defined HCI_VENDOR_CMD_GATT_ADD_CHAR_DESC))
    uint8 indx = 0;
#endif

#if ((defined HCI_VENDOR_CMD_GAP_SET_LIMITED_DISCOVERABLE) ||  \
     (defined HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE))
    uint8 numofparam = 0;
    uint8 localNameLength = 0;
    uint8 serviceUuidLength = 0;
#endif

#if ((defined HCI_VENDOR_CMD_GATT_FIND_ATTR_BY_TYPE_VAL) ||  \
     (defined HCI_VENDOR_CMD_GATT_WRITE_ATTR) ||   \
     (defined HCI_VENDOR_CMD_GATT_PREPARE_WRITE) ||   \
     (defined HCI_VENDOR_CMD_GATT_WRITE_CHARAC_VAL) ||  \
     (defined HCI_VENDOR_CMD_GATT_WRITE_CHARAC_DESCRIPTOR) ||  \
     (defined HCI_VENDOR_CMD_GATT_WRITE_WITHOUT_RESPONSE) )
    uint8 attrValueLen = 0;
#endif

#if (defined HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL)
    uint8 charValueLen = 0;
#endif

#if (defined HCI_VENDOR_CMD_GAP_UPDATE_ADV_DATA)
    uint8 advtDataLen = 0;
#endif

#if (defined HCI_HOST_CTL_CMD_HOST_NUM_COMP_PKTS)
    uint8 numOfHandles = 0;
#endif

#if (defined HCI_HOST_CTL_CMD_HOST_NUM_COMP_PKTS)
    uint8 offset = 0;
#endif

#if (defined HCI_HOST_CTL_CMD_HOST_NUM_COMP_PKTS)
    uint16 tempParam16 = 0;
#endif

    HCI_MESG_DBG(hostdbgfile,"Composing HCI Write Packet \n");
  
    /* is hci data packet pool empty, return failure */
    if (list_is_empty (&hciWritePktPool) == NO)
    {
      int size;
      size = list_get_size(&hciWritePktPool);
        /* pic one pending data packet from the queue */ 
        list_remove_head (&hciWritePktPool, (tListNode **)&cmdPacket);
    }
    else
    {
        HCI_MESG_DBG(hostdbgfile,"There is no empty packet in the HCI write pool. \n");
		retval = BLE_STATUS_OUT_OF_MEMORY;
    }

    /* fill the data packet with parameters */
    if (cmdPacket != NULL)
    {
        HCI_MESG_DBG(hostdbgfile,"Forming A HCI Command Packet\n");
        cmdPacket->hciPktType = HCI_PKT_TYPE_COMMAND;
        cmdPacket->packet.command.opcode = opcode;
		cmdPacket->profileIdx = profileID;
        dataBuff = (uint8 *)cmdPacket->dataBuffer;
        cmdParam = cmdParamPtrArray;

        if (cmdParam != NULL)
        {
            switch (opcode)
            {
        #ifdef HCI_LINK_CTL_CMD_DISCONNECT
                case HCI_LINK_CTL_CMD_DISCONNECT:
                    HCI_PACK_2_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 2,(*cmdParam)[1]);
                    cmdPacket->packet.command.lengthOfParams = 3;
                    break;
        #endif
        #ifdef HCI_LINK_CTL_CMD_READ_REMOTE_VERSION
                case HCI_LINK_CTL_CMD_READ_REMOTE_VERSION:
                    HCI_PACK_2_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 2;
                    break;
        #endif
        #ifdef HCI_HOST_CTL_CMD_SET_EVENT_MASK
                case HCI_HOST_CTL_CMD_SET_EVENT_MASK:
                    HCI_MESG_DBG(hostdbgfile,"HCI_HOST_CTL_CMD_SET_EVENT_MASK\n");
                    Osal_MemCpy ((void *)dataBuff, (void *)(*cmdParam)[0], 8);
                    cmdPacket->packet.command.lengthOfParams = 8;
                    break;
        #endif
        #ifdef HCI_HOST_CTL_CMD_READ_TRANSMIT_POWER_LEVEL
                case HCI_HOST_CTL_CMD_READ_TRANSMIT_POWER_LEVEL:
                    HCI_PACK_2_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 2,(*cmdParam)[1]);
                    cmdPacket->packet.command.lengthOfParams = 3;
                    break;
        #endif
                    
#ifdef SET_TX_POWER_COMMAND /* TBR */
       #ifdef HCI_VENDOR_CMD_SET_TX_POWER_LEVEL        
             case HCI_VENDOR_CMD_SET_TX_POWER_LEVEL:
                    HCI_PACK_1_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 1,(*cmdParam)[1]);
                    cmdPacket->packet.command.lengthOfParams = 2;
                    break;   
       #endif 
#endif
                    
#ifdef SET_L2CAP_CONN_UPD_REQ_COMMAND /* TBR */
       #ifdef HCI_VENDOR_CMD_L2CAP_CONN_UPD_REQ 
            case HCI_VENDOR_CMD_L2CAP_CONN_UPD_REQ:     
                  HCI_PACK_2_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                  HCI_PACK_2_BYTE_PARAM( dataBuff + 2,(*cmdParam)[1]);
                  HCI_PACK_2_BYTE_PARAM( dataBuff + 4,(*cmdParam)[2]);
                  HCI_PACK_2_BYTE_PARAM( dataBuff + 6,(*cmdParam)[3]);
                  HCI_PACK_2_BYTE_PARAM( dataBuff + 8,(*cmdParam)[4]);
                  cmdPacket->packet.command.lengthOfParams = 0x0A;
              break;    
       #endif
#endif

        #ifdef HCI_HOST_CTL_CMD_SET_CONTROLLER_TO_HOST_FC
                case HCI_HOST_CTL_CMD_SET_CONTROLLER_TO_HOST_FC:
                    HCI_PACK_1_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 1;
                    break;
        #endif
        #ifdef HCI_HOST_CTL_CMD_HOST_BUFFER_SIZE
                case HCI_HOST_CTL_CMD_HOST_BUFFER_SIZE:
                    HCI_PACK_2_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 2,(*cmdParam)[1]);
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 3,(*cmdParam)[2]);
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 5,(*cmdParam)[3]);
                    cmdPacket->packet.command.lengthOfParams = 7;
                    break;
        #endif
        #ifdef HCI_HOST_CTL_CMD_HOST_NUM_COMP_PKTS
                case HCI_HOST_CTL_CMD_HOST_NUM_COMP_PKTS:
                    HCI_PACK_1_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    numOfHandles = (uint8)dataBuff[0];
                    cmdPacket->packet.command.lengthOfParams = 1;
                    offset = 1;
                    for (indx = 0; indx < numOfHandles; indx++)
                    {
                        tempParam16 = (uint16)(*((uint16 *)((*cmdParam)[1]) + indx));
                        HCI_PACK_2_BYTE_PARAM(dataBuff + offset, &tempParam16);
                        offset += 2;

                        tempParam16 = (uint16)(*((uint16 *)((*cmdParam)[2]) + indx));
                        HCI_PACK_2_BYTE_PARAM(dataBuff + offset, &tempParam16);
                        offset += 2;
                        
                        cmdPacket->packet.command.lengthOfParams += 4;
                    }
                    break;
        #endif
        #ifdef HCI_STATUS_PARAM_CMD_READ_RSSI
                case HCI_STATUS_PARAM_CMD_READ_RSSI:
                    HCI_PACK_2_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 2;
                    break;
        #endif
        #ifdef HCI_LE_CTL_SET_EVENT_MASK
                case HCI_LE_CTL_SET_EVENT_MASK:
                    HCI_MESG_DBG(hostdbgfile,"HCI_LE_CTL_SET_EVENT_MASK\n");
                    Osal_MemCpy ((void *)dataBuff, (void *)(*cmdParam)[0], 8);
                    cmdPacket->packet.command.lengthOfParams = 8;
                    break;
        #endif
        #ifdef HCI_LE_CTL_SET_RANDOM_ADDRESS
                case HCI_LE_CTL_SET_RANDOM_ADDRESS:
                    Osal_MemCpy ((void *)dataBuff, (void *)(*cmdParam)[0], 6);
                    cmdPacket->packet.command.lengthOfParams = 6;
                    break;
        #endif
        #ifdef HCI_LE_CTL_SET_ADV_PARAMETERS
                case HCI_LE_CTL_SET_ADV_PARAMETERS:
                    HCI_PACK_2_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 2,(*cmdParam)[1]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 4,(*cmdParam)[2]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 5,(*cmdParam)[3]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 6,(*cmdParam)[4]);
                    Osal_MemCpy ((void *)(dataBuff + 7), (void *)((*cmdParam)[5]), 6);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 13,(*cmdParam)[6]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 14,(*cmdParam)[7]);
                    cmdPacket->packet.command.lengthOfParams = 15;
                    break;
        #endif
        #ifdef HCI_LE_CTL_SET_ADV_DATA
                case HCI_LE_CTL_SET_ADV_DATA:
                    HCI_PACK_1_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    Osal_MemCpy ((void *)(dataBuff + 1), (void *)((*cmdParam)[1]), 31);
                    cmdPacket->packet.command.lengthOfParams = 32;
                    break;
        #endif
        #ifdef HCI_LE_CTL_SET_SCAN_RESPONSE
                case HCI_LE_CTL_SET_SCAN_RESPONSE:
                    HCI_PACK_1_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    Osal_MemCpy ((void *)(dataBuff + 1), (void *)((*cmdParam)[1]), 31);
                    cmdPacket->packet.command.lengthOfParams = 32;
                    break;
        #endif
        #ifdef HCI_LE_CTL_SET_ADVERTIZE_ENABLE
                case HCI_LE_CTL_SET_ADVERTIZE_ENABLE:
                   HCI_MESG_DBG(hostdbgfile,"HCI_LE_CTL_SET_ADVERTIZE_ENABLE\n");
                    HCI_PACK_1_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 1;
                    break;
        #endif
        #ifdef HCI_LE_CTL_ADD_DEVICE_TO_WHILE_LIST
                case HCI_LE_CTL_ADD_DEVICE_TO_WHILE_LIST:
        #endif
        #ifdef HCI_LE_CTL_REMOVE_DEVICE_FROM_WHITE_LIST
                case HCI_LE_CTL_REMOVE_DEVICE_FROM_WHITE_LIST:
        #endif
                    HCI_PACK_1_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    Osal_MemCpy ((void *)(dataBuff + 1), (void *)((*cmdParam)[1]), 6);
                    cmdPacket->packet.command.lengthOfParams = 7;
                    break;
        #ifdef HCI_LE_CTL_READ_CHANNEL_MAP
                case HCI_LE_CTL_READ_CHANNEL_MAP:
                    HCI_PACK_2_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 2;
                    break;
        #endif
        #ifdef HCI_LE_CTL_ENCRYPT
                case HCI_LE_CTL_ENCRYPT:
                    Osal_MemCpy ((void *)dataBuff, (void *)((*cmdParam)[0]), 16);
                    Osal_MemCpy ((void *)(dataBuff + 16), (void *)((*cmdParam)[1]), 16);
                    cmdPacket->packet.command.lengthOfParams = 32;
                    break;
        #endif
        #ifdef HCI_LE_CTL_LTK_REQ_REPLY
                case HCI_LE_CTL_LTK_REQ_REPLY:
                    HCI_PACK_2_BYTE_PARAM( dataBuff ,(*cmdParam)[0]);
                    Osal_MemCpy ((void *)(dataBuff + 2), (void *)((*cmdParam)[1]), 16);
                    cmdPacket->packet.command.lengthOfParams = 18;
                    break;
        #endif
        #ifdef HCI_LE_CTL_LTK_REQ_NEGATIVE_REPLY
                case HCI_LE_CTL_LTK_REQ_NEGATIVE_REPLY:
                    HCI_PACK_2_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 2;
                    break;
        #endif
        #ifdef HCI_LE_CTL_RECEIVER_TEST
                case HCI_LE_CTL_RECEIVER_TEST:
                    HCI_PACK_1_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 1;
                    break;
        #endif
        #ifdef HCI_LE_CTL_TRANSMITTER_TEST
                case HCI_LE_CTL_TRANSMITTER_TEST:
                    HCI_PACK_1_BYTE_PARAM( dataBuff, (*cmdParam)[0]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 1, (*cmdParam)[1]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 2, (*cmdParam)[2]);
                    cmdPacket->packet.command.lengthOfParams = 3;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_SET_LIMITED_DISCOVERABLE
                case HCI_VENDOR_CMD_GAP_SET_LIMITED_DISCOVERABLE:
                    indx = 0;
                    numofparam = 0;
                    HCI_MESG_DBG(hostdbgfile,"HCI_VENDOR_CMD_GAP_SET_LIMITED_DISCOVERABLE\n");

                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 1;
                    numofparam++;

                    HCI_PACK_2_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 2;
                    numofparam++;

                    HCI_PACK_2_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 2;
                    numofparam++;

                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 1;
                    numofparam++;

                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 1;
                    numofparam++;

                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 1;
                    numofparam++;

                    localNameLength = (uint8)dataBuff[indx - 1];
                    if (localNameLength != 0)
                    {
                        Osal_MemCpy ((void *)(dataBuff + indx), 
                                           (void *)((*cmdParam)[numofparam]), 
                                           localNameLength);
                        indx += localNameLength;
                        numofparam++;
                    }

                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 1;
                    numofparam++;

                    serviceUuidLength = (uint8)dataBuff[indx - 1];
                    if (serviceUuidLength != 0)
                    {
                        Osal_MemCpy ((void *)(dataBuff + indx), 
                                           (void *)((*cmdParam)[numofparam]), 
                                           serviceUuidLength);
                        indx += serviceUuidLength;
                        numofparam++;
                    }

                    /* slave connection intervals */
                    HCI_PACK_2_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 2;
                    numofparam++;

                    HCI_PACK_2_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 2;
                    numofparam++;

                    HCI_MESG_DBG( hostdbgfile, "len %d, num of param %d\n", indx, numofparam);
                    cmdPacket->packet.command.lengthOfParams = indx;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE
                case HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE:
                    indx = 0;
                    numofparam = 0;
                    HCI_MESG_DBG(hostdbgfile,"HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE\n");
                    
                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 1;
                    numofparam++;

                    HCI_PACK_2_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 2;
                    numofparam++;

                    HCI_PACK_2_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 2;
                    numofparam++;

                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 1;
                    numofparam++;

                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 1;
                    numofparam++;

                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 1;
                    numofparam++;

                    localNameLength = (uint8)dataBuff[indx - 1];
                    if (localNameLength != 0)
                    {
                        Osal_MemCpy ((void *)(dataBuff + indx), 
                                           (void *)((*cmdParam)[numofparam]), 
                                           localNameLength);
                        indx += localNameLength;
                        numofparam++;
                    }

                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 1;
                    numofparam++;

                    serviceUuidLength = (uint8)dataBuff[indx - 1];
                    if (serviceUuidLength != 0)
                    {
                        Osal_MemCpy ((void *)(dataBuff + indx), 
                                           (void *)((*cmdParam)[numofparam]), 
                                           serviceUuidLength);
                        indx += serviceUuidLength;
                        numofparam++;
                    }

                    /* slave connection intervals */
                    HCI_PACK_2_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 2;
                    numofparam++;

                    HCI_PACK_2_BYTE_PARAM( dataBuff + indx, (*cmdParam)[numofparam] );
                    indx += 2;
                    numofparam++;

                    HCI_MESG_DBG( hostdbgfile, "len %d, num of param %d\n", indx, numofparam);
                    cmdPacket->packet.command.lengthOfParams = indx;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_SET_DIRECT_CONNECTABLE
                case HCI_VENDOR_CMD_GAP_SET_DIRECT_CONNECTABLE:
                    HCI_PACK_1_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 1,(*cmdParam)[1]);
                    Osal_MemCpy ((void *)(dataBuff + 2), (void *)((*cmdParam)[2]), 6);
                    cmdPacket->packet.command.lengthOfParams = 8;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_SET_SET_IO_CAPABILITY
                case HCI_VENDOR_CMD_GAP_SET_SET_IO_CAPABILITY:
                    HCI_MESG_DBG(hostdbgfile,"HCI_VENDOR_CMD_GAP_SET_SET_IO_CAPABILITY\n");
                    HCI_PACK_1_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 1;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_SET_AUTHENTICATION_REQ
                case HCI_VENDOR_CMD_GAP_SET_AUTHENTICATION_REQ:
                    HCI_PACK_1_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 1,(*cmdParam)[1]);
                    Osal_MemCpy ((void *)(dataBuff + 2), (void *)((*cmdParam)[2]), 16);
                    HCI_PACK_1_BYTE_PARAM(dataBuff + 18,(*cmdParam)[3]);
                    HCI_PACK_1_BYTE_PARAM(dataBuff + 19,(*cmdParam)[4]);
                    HCI_PACK_1_BYTE_PARAM(dataBuff + 20,(*cmdParam)[5]);
					HCI_PACK_4_BYTE_PARAM(dataBuff + 21,(*cmdParam)[6]);
                    HCI_PACK_1_BYTE_PARAM(dataBuff + 25,(*cmdParam)[7]);
                    cmdPacket->packet.command.lengthOfParams = 26;
					break;
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_SET_AUTHORIZATION_REQ
                case HCI_VENDOR_CMD_GAP_SET_AUTHORIZATION_REQ:
                    HCI_PACK_1_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 1;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_PASS_KEY_RESP  /* TODO: Check parameters */
                case HCI_VENDOR_CMD_GAP_PASS_KEY_RESP: 
                    HCI_PACK_4_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 4;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_AUTHORIZATION_RESP
                case HCI_VENDOR_CMD_GAP_AUTHORIZATION_RESP:
                    HCI_PACK_1_BYTE_PARAM( dataBuff,(*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 1;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_INIT
                case HCI_VENDOR_CMD_GAP_INIT:
                    HCI_MESG_DBG(hostdbgfile,"HCI_VENDOR_CMD_GAP_INIT\n");
                    HCI_PACK_1_BYTE_PARAM( dataBuff, (*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 1;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_SET_NON_CONNECTABLE
                case HCI_VENDOR_CMD_GAP_SET_NON_CONNECTABLE:
                    HCI_PACK_1_BYTE_PARAM( dataBuff, (*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 1;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_SET_UNDIRECTED_CONNECTABLE
                case HCI_VENDOR_CMD_GAP_SET_UNDIRECTED_CONNECTABLE:
                    HCI_PACK_1_BYTE_PARAM( dataBuff, (*cmdParam)[0]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 1, (*cmdParam)[1]);
                    cmdPacket->packet.command.lengthOfParams = 2;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_SLAVE_SECURITY_REQUEST
                case HCI_VENDOR_CMD_GAP_SLAVE_SECURITY_REQUEST:
                    HCI_PACK_1_BYTE_PARAM( dataBuff, (*cmdParam)[0]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 1, (*cmdParam)[1]);
                    cmdPacket->packet.command.lengthOfParams = 2;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_UPDATE_ADV_DATA
                case HCI_VENDOR_CMD_GAP_UPDATE_ADV_DATA:
                    HCI_PACK_1_BYTE_PARAM( dataBuff, (*cmdParam)[0]);
                    advtDataLen = (uint8)dataBuff[0];
                    Osal_MemCpy ((void *)(dataBuff + 1), (void *)((*cmdParam)[1]), advtDataLen);
                    cmdPacket->packet.command.lengthOfParams = advtDataLen + 1;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_DELETE_AD_TYPE
                case HCI_VENDOR_CMD_GAP_DELETE_AD_TYPE:
                    HCI_PACK_1_BYTE_PARAM( dataBuff, (*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 1;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_SET_EVT_MASK
                case HCI_VENDOR_CMD_GAP_SET_EVT_MASK:
                    HCI_MESG_DBG(hostdbgfile,"HCI_VENDOR_CMD_GAP_SET_EVT_MASK\n");
                    HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 2;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_ADD_SERVICE
                case HCI_VENDOR_CMD_GATT_ADD_SERVICE:
                    HCI_MESG_DBG(hostdbgfile,"HCI_VENDOR_CMD_GATT_ADD_SERVICE\n");
                    HCI_PACK_1_BYTE_PARAM( dataBuff ,(*cmdParam)[0]);
                    if(dataBuff[0] == UUID_TYPE_16)
                    {
                        HCI_PACK_2_BYTE_PARAM( dataBuff + 1, (*cmdParam)[1]);
                        HCI_PACK_1_BYTE_PARAM( dataBuff + 3, (*cmdParam)[2]);
                        HCI_PACK_1_BYTE_PARAM( dataBuff + 4, (*cmdParam)[3]);
                        cmdPacket->packet.command.lengthOfParams = 5;
                        HCI_MESG_DBG(hostdbgfile,"UUID_TYPE_16\n");
                    }
                    else
                    {
                        Osal_MemCpy ((void *)(dataBuff + 1), (void *)((*cmdParam)[1]), 16);
                        HCI_PACK_1_BYTE_PARAM( dataBuff + 17, (*cmdParam)[2]);
                        HCI_PACK_1_BYTE_PARAM( dataBuff + 18, (*cmdParam)[3]);
                        cmdPacket->packet.command.lengthOfParams = 19;
                    }
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_INC_SERVICE
                case HCI_VENDOR_CMD_GATT_INC_SERVICE:
                    HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0]);
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 2, (*cmdParam)[1]);
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 4, (*cmdParam)[2]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 6, (*cmdParam)[3]);
                    if (UUID_TYPE_16 == (uint8)dataBuff[6])
                    {
                        HCI_PACK_2_BYTE_PARAM( dataBuff + 7, (*cmdParam)[4]);
                        cmdPacket->packet.command.lengthOfParams = 9;
                    }
                    else /* UUID_TYPE_128 */
                    {
                        Osal_MemCpy ((void *)(dataBuff + 7), (void *)((*cmdParam)[4]), 16);
                        cmdPacket->packet.command.lengthOfParams = 23;
                    }
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_ADD_CHAR
                case HCI_VENDOR_CMD_GATT_ADD_CHAR:
                    indx = 0;
                    HCI_MESG_DBG(hostdbgfile,"HCI_VENDOR_CMD_GATT_ADD_CHAR\n");
                    HCI_PACK_2_BYTE_PARAM( dataBuff + indx, (*cmdParam)[0] );
                    indx += 2;
                    HCI_MESG_DBG(hostdbgfile,"dataBuff %x,%x\n",dataBuff[0],dataBuff[1]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[1] );
                    indx += 1;
                    if(dataBuff[2] == UUID_TYPE_16)
                    {
                        HCI_PACK_2_BYTE_PARAM( dataBuff + indx, (*cmdParam)[2] );
                        indx += 2;
                    }
                    else /* UUID_TYPE_128 */
                    {
                        Osal_MemCpy ((void *)(dataBuff + indx), (void *)((*cmdParam)[2]), 16);
                        indx += 16;
                    }
                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[3] );
                    indx += 1;
                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[4] );
                    indx += 1;
                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[5] );
                    indx += 1;
                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[6] );
                    indx += 1;
                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[7] );
                    indx += 1;
                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[8] );
                    indx += 1;
                    cmdPacket->packet.command.lengthOfParams = indx;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_ADD_CHAR_DESC
                case HCI_VENDOR_CMD_GATT_ADD_CHAR_DESC:
                    indx = 0;
                    HCI_PACK_2_BYTE_PARAM( dataBuff + indx, (*cmdParam)[0] );
                    indx += 2;
                    HCI_PACK_2_BYTE_PARAM( dataBuff + indx, (*cmdParam)[1] );
                    indx += 2;
                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[2] );
                    indx += 1;
                    if(dataBuff[4] == UUID_TYPE_16)
                    {
                        HCI_PACK_2_BYTE_PARAM( dataBuff + indx, (*cmdParam)[3] );
                        indx += 2;
                    }
                    else /* UUID_TYPE_128 */
                    {
                        Osal_MemCpy ((void *)(dataBuff + indx), (void *)((*cmdParam)[3]), 16);
                        indx += 16;
                    }
                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[4] );
                    indx += 1;
                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[5] );
                    indx += 1;
                    charDescValueLen = (uint8)dataBuff[indx-1];
                    Osal_MemCpy ((void *)(dataBuff + indx), (void *)((*cmdParam)[6]), charDescValueLen);
                    indx += charDescValueLen;
                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[7] );
                    indx += 1;
                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[8] );
                    indx += 1;
                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[9] );
                    indx += 1;
                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[10] );
                    indx += 1;
                    HCI_PACK_1_BYTE_PARAM( dataBuff + indx, (*cmdParam)[11] );
                    indx += 1;
                    cmdPacket->packet.command.lengthOfParams = indx;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL
                case HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL:
                    HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 2, (*cmdParam)[1] );
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 4, (*cmdParam)[2] );
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 5, (*cmdParam)[3] );
                    charValueLen = (uint8)dataBuff[5];
                    Osal_MemCpy ((void *)(dataBuff + 6), (void *)((*cmdParam)[4]), charValueLen);
                    cmdPacket->packet.command.lengthOfParams = charValueLen + 6;

                    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Update_CharVal_Cmd \n" );
                    HCI_MESG_DBG ( hostdbgfile, "service handle: %02X%02X\n", dataBuff[1], dataBuff[0]);
                    HCI_MESG_DBG ( hostdbgfile, "char handle: %02X%02X \n", dataBuff[3], dataBuff[2]);
                    HCI_MESG_DBG ( hostdbgfile, "data offset: %02X \n", dataBuff[4]);
                    HCI_MESG_DBG ( hostdbgfile, "data len: %02X \n", dataBuff[5]);
                    HCI_MESG_DBG ( hostdbgfile, "data len: %02X \n", charValueLen );
                    HCI_MESG_DBG ( hostdbgfile, "total param len: %02X \n", cmdPacket->packet.command.lengthOfParams  );
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_DEL_CHAR
                case HCI_VENDOR_CMD_GATT_DEL_CHAR:
                    HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 2, (*cmdParam)[1] );
                    cmdPacket->packet.command.lengthOfParams = 4;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_DEL_SERVICE
                case HCI_VENDOR_CMD_GATT_DEL_SERVICE:
                    HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
                    cmdPacket->packet.command.lengthOfParams = 2;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_DEL_INC_SERVICE
                case HCI_VENDOR_CMD_GATT_DEL_INC_SERVICE:
                    HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 2, (*cmdParam)[1] );
                    cmdPacket->packet.command.lengthOfParams = 4;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_SET_EVT_MASK
                case HCI_VENDOR_CMD_GATT_SET_EVT_MASK:
                    HCI_MESG_DBG(hostdbgfile,"HCI_VENDOR_CMD_GATT_SET_EVT_MASK\n");
                    HCI_PACK_4_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
                    cmdPacket->packet.command.lengthOfParams = 4;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_FIND_INFO
                case HCI_VENDOR_CMD_GATT_FIND_INFO:
                    HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 2, (*cmdParam)[1] );
					HCI_PACK_2_BYTE_PARAM( dataBuff + 4,(*cmdParam)[2] );
                    cmdPacket->packet.command.lengthOfParams = 6;
                    break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_FIND_ATTR_BY_TYPE_VAL
                case HCI_VENDOR_CMD_GATT_FIND_ATTR_BY_TYPE_VAL:
                    HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 2, (*cmdParam)[1] );
					HCI_PACK_2_BYTE_PARAM( dataBuff + 4, (*cmdParam)[2] );
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 6, (*cmdParam)[3] );
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 8, (*cmdParam)[4] );
                    attrValueLen = (uint8)dataBuff[8];
                    if (attrValueLen != 0)
                    {
                        Osal_MemCpy ((void *)(dataBuff + 9), (void *)((*cmdParam)[5]), attrValueLen);
                    }
                    cmdPacket->packet.command.lengthOfParams = attrValueLen + 9;
                    break;
        #endif

        #ifdef HCI_VENDOR_CMD_GATT_READ_ATTR_BY_TYPE
                case HCI_VENDOR_CMD_GATT_READ_ATTR_BY_TYPE:
					HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 2, (*cmdParam)[1] );
					HCI_PACK_2_BYTE_PARAM( dataBuff + 4, (*cmdParam)[2] );
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 6, (*cmdParam)[3] );
					if(dataBuff[6] == UUID_TYPE_16)
					{
					HCI_PACK_2_BYTE_PARAM( dataBuff + 7, (*cmdParam)[4]);
					cmdPacket->packet.command.lengthOfParams = 9;
					}
					else
					{	
					Osal_MemCpy ((void *)(dataBuff + 7), (void *)((*cmdParam)[4]), 16);
                    cmdPacket->packet.command.lengthOfParams = 23;
                    }
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_READ_ATTR_BY_GROUP_TYPE
                case HCI_VENDOR_CMD_GATT_READ_ATTR_BY_GROUP_TYPE:
					HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 2, (*cmdParam)[1] );
					HCI_PACK_2_BYTE_PARAM( dataBuff + 4, (*cmdParam)[2] );
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 6, (*cmdParam)[3] );
					if(dataBuff[6] == UUID_TYPE_16)
					{
					HCI_PACK_2_BYTE_PARAM( dataBuff + 7, (*cmdParam)[4]);
					cmdPacket->packet.command.lengthOfParams = 9;
					}
					else
					{	
					Osal_MemCpy ((void *)(dataBuff + 7), (void *)((*cmdParam)[4]), 16);
                    cmdPacket->packet.command.lengthOfParams = 23;
                    }
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_PREPARE_WRITE
                case HCI_VENDOR_CMD_GATT_PREPARE_WRITE:
					HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 2, (*cmdParam)[1] );
					HCI_PACK_2_BYTE_PARAM( dataBuff + 4, (*cmdParam)[2] );
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 6, (*cmdParam)[3] );
					attrValueLen = (uint8)dataBuff[6];
                    if (attrValueLen != 0)
                    {
                        Osal_MemCpy ((void *)(dataBuff + 7), (void *)((*cmdParam)[4]), attrValueLen);
                    }
                    cmdPacket->packet.command.lengthOfParams = attrValueLen + 7;
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_EXECUTE_WRITES
                case HCI_VENDOR_CMD_GATT_EXECUTE_WRITES:
					HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 2, (*cmdParam)[1] );    
					cmdPacket->packet.command.lengthOfParams = 3;
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_CONFORM_INDICATION
                case HCI_VENDOR_CMD_GATT_CONFORM_INDICATION:
					HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
                    cmdPacket->packet.command.lengthOfParams = 2;
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_WRITE_RESP
                case HCI_VENDOR_CMD_GATT_WRITE_RESP:
                    HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
					HCI_PACK_2_BYTE_PARAM( dataBuff+2, (*cmdParam)[1] );
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 4, (*cmdParam)[2] );
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 5, (*cmdParam)[3] );
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 6, (*cmdParam)[4] );
                    attrValueLen = (uint8)dataBuff[6];
                    if (attrValueLen != 0)
                    {
                        Osal_MemCpy ((void *)(dataBuff + 7), (void *)((*cmdParam)[5]), attrValueLen);
                    }
                    cmdPacket->packet.command.lengthOfParams = attrValueLen + 7;
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_ALLOW_READ
                case HCI_VENDOR_CMD_GATT_ALLOW_READ:
				    HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
                    cmdPacket->packet.command.lengthOfParams = 2;
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_DISC_ALL_PRIMARY_SERVICES
                case HCI_VENDOR_CMD_GATT_DISC_ALL_PRIMARY_SERVICES:
				{
				    HCI_MESG_DBG(hostdbgfile,"HCI_VENDOR_CMD_GATT_DISC_PRIMARY_SERVICE_BY_UUID \n");
                    HCI_PACK_2_BYTE_PARAM( dataBuff ,(*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 2;
				}
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_DISC_PRIMARY_SERVICE_BY_UUID
                case HCI_VENDOR_CMD_GATT_DISC_PRIMARY_SERVICE_BY_UUID:
                    HCI_MESG_DBG(hostdbgfile,"HCI_VENDOR_CMD_GATT_DISC_PRIMARY_SERVICE_BY_UUID \n");
					HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 2 ,(*cmdParam)[1]);
                    if(dataBuff[2] == UUID_TYPE_16)
                    {
                        HCI_PACK_2_BYTE_PARAM( dataBuff + 3, (*cmdParam)[2]);
                        cmdPacket->packet.command.lengthOfParams = 3;
                    }
                    else
                    {
                        Osal_MemCpy ((void *)(dataBuff + 3), (void *)((*cmdParam)[1]), 16);
                        cmdPacket->packet.command.lengthOfParams = 19;
                    }
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_FIND_INCLUDED_SERVICES
                case HCI_VENDOR_CMD_GATT_FIND_INCLUDED_SERVICES:
					HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
					HCI_PACK_2_BYTE_PARAM( dataBuff+2, (*cmdParam)[1] );
					HCI_PACK_2_BYTE_PARAM( dataBuff+4, (*cmdParam)[2] );
					cmdPacket->packet.command.lengthOfParams = 6;
				break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_DISC_ALL_CHARAC_OF_A_SERVICE
                case HCI_VENDOR_CMD_GATT_DISC_ALL_CHARAC_OF_A_SERVICE:
				    HCI_PACK_2_BYTE_PARAM( dataBuff ,(*cmdParam)[0]);
                    HCI_PACK_2_BYTE_PARAM( dataBuff+2 ,(*cmdParam)[1]);
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 4, (*cmdParam)[2]);
                    cmdPacket->packet.command.lengthOfParams = 6;
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_DISC_CHARAC_BY_UUID
                case HCI_VENDOR_CMD_GATT_DISC_CHARAC_BY_UUID:
                    HCI_PACK_2_BYTE_PARAM( dataBuff, (*cmdParam)[0] );
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 2, (*cmdParam)[1] );
					HCI_PACK_2_BYTE_PARAM( dataBuff + 4, (*cmdParam)[2] );
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 6, (*cmdParam)[3] );
					if(dataBuff[6] == UUID_TYPE_16)
					{
					HCI_PACK_2_BYTE_PARAM( dataBuff + 7, (*cmdParam)[4]);
					cmdPacket->packet.command.lengthOfParams = 9;
					}
					else
					{	
					Osal_MemCpy ((void *)(dataBuff + 7), (void *)((*cmdParam)[4]), 16);
                    cmdPacket->packet.command.lengthOfParams = 23;
                    }
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_DISC_ALL_CHAR_DESCRIPTORS
                case HCI_VENDOR_CMD_GATT_DISC_ALL_CHAR_DESCRIPTORS:
                    HCI_PACK_2_BYTE_PARAM( dataBuff ,(*cmdParam)[0]);
					HCI_PACK_2_BYTE_PARAM( dataBuff + 2 ,(*cmdParam)[1]);
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 4, (*cmdParam)[2]);
                    cmdPacket->packet.command.lengthOfParams = 6;
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_READ_CHARACTERISTIC_VAL
                case HCI_VENDOR_CMD_GATT_READ_CHARACTERISTIC_VAL:
                    HCI_PACK_2_BYTE_PARAM( dataBuff ,(*cmdParam)[0]);
					HCI_PACK_2_BYTE_PARAM( dataBuff+2 ,(*cmdParam)[1]);
                    cmdPacket->packet.command.lengthOfParams = 4;
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_READ_CHARAC_USING_UUID
                case HCI_VENDOR_CMD_GATT_READ_CHARAC_USING_UUID:
                    #error "Some code is expected here !! "
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_READ_LONG_CHARAC_VAL
                case HCI_VENDOR_CMD_GATT_READ_LONG_CHARAC_VAL:
                    #error "Some code is expected here !! "
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_READ_MULTIPLE_CHARAC_VALUES
                case HCI_VENDOR_CMD_GATT_READ_MULTIPLE_CHARAC_VALUES:
                    #error "Some code is expected here !! "
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_WRITE_CHARAC_VAL
                case HCI_VENDOR_CMD_GATT_WRITE_CHARAC_VAL:
				    HCI_PACK_2_BYTE_PARAM( dataBuff ,(*cmdParam)[0]);
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 2,(*cmdParam)[1]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 4,(*cmdParam)[2]);
                    attrValueLen = (uint8)dataBuff[4];
                    if (attrValueLen != 0)
                    {
                        Osal_MemCpy ((void *)(dataBuff + 5), (void *)((*cmdParam)[3]), attrValueLen);
                    }
                    cmdPacket->packet.command.lengthOfParams = attrValueLen + 5;
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_WRITE_LONG_CHARAC_VAL
                case HCI_VENDOR_CMD_GATT_WRITE_LONG_CHARAC_VAL:
                    #error "Some code is expected here !! "
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_WRITE_CHARAC_RELIABLE
                case HCI_VENDOR_CMD_GATT_WRITE_CHARAC_RELIABLE:
                    #error "Some code is expected here !! "
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_READ_CHARAC_DESCRIPTOR
                case HCI_VENDOR_CMD_GATT_READ_CHARAC_DESCRIPTOR:
                    HCI_PACK_2_BYTE_PARAM( dataBuff ,(*cmdParam)[0]);
					HCI_PACK_2_BYTE_PARAM( dataBuff + 2 ,(*cmdParam)[1]);
                    cmdPacket->packet.command.lengthOfParams = 4;
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_READ_LONG_CHARAC_DESCRIPTOR
                case HCI_VENDOR_CMD_GATT_READ_LONG_CHARAC_DESCRIPTOR:
                    #error "Some code is expected here !! "
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_WRITE_CHARAC_DESCRIPTOR
                case HCI_VENDOR_CMD_GATT_WRITE_CHARAC_DESCRIPTOR:
				     HCI_PACK_2_BYTE_PARAM( dataBuff ,(*cmdParam)[0]);
                    HCI_PACK_2_BYTE_PARAM( dataBuff + 2 ,(*cmdParam)[1]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 4,(*cmdParam)[2]);
                    attrValueLen = (uint8)dataBuff[4];
                    if (attrValueLen != 0)
                    {
                        Osal_MemCpy ((void *)(dataBuff + 5), (void *)((*cmdParam)[3]), attrValueLen);
                    }
                    cmdPacket->packet.command.lengthOfParams = attrValueLen + 5;
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_WRITE_LONG_CHARAC_DESCRIPTOR
                case HCI_VENDOR_CMD_GATT_WRITE_LONG_CHARAC_DESCRIPTOR:
                    #error "Some code is expected here !! "
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_WRITE_WITHOUT_RESPONSE
                case HCI_VENDOR_CMD_GATT_WRITE_WITHOUT_RESPONSE:
		 #ifdef HCI_VENDOR_CMD_GATT_SIGNED_WRITE_WITHOUT_RESPONSE
				case HCI_VENDOR_CMD_GATT_SIGNED_WRITE_WITHOUT_RESPONSE:
		#endif
		            HCI_PACK_2_BYTE_PARAM( dataBuff ,(*cmdParam)[0]);
                    HCI_PACK_2_BYTE_PARAM( dataBuff+2 ,(*cmdParam)[1]);
                    HCI_PACK_1_BYTE_PARAM( dataBuff + 4, (*cmdParam)[2]);
                    attrValueLen = (uint8)dataBuff[4];
                    if (attrValueLen != 0)
                    {
                        Osal_MemCpy ((void *)(dataBuff + 5), (void *)((*cmdParam)[3]), attrValueLen);
                    }
                    cmdPacket->packet.command.lengthOfParams = attrValueLen + 5;
                break;
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_EXCHG_CONFIG
                case HCI_VENDOR_CMD_GATT_EXCHG_CONFIG:
        #endif
		            HCI_PACK_2_BYTE_PARAM( dataBuff ,(*cmdParam)[0]);
                    cmdPacket->packet.command.lengthOfParams = 2;
                break;
        #ifdef HCI_HOST_CTL_CMD_RESET
                case HCI_HOST_CTL_CMD_RESET:
        #endif
        #ifdef HCI_INFO_PARAM_CMD_READ_LOCAL_VERSION
                case HCI_INFO_PARAM_CMD_READ_LOCAL_VERSION:
        #endif
        #ifdef HCI_INFO_PARAM_CMD_READ_LOCAL_COMMANDS
                case HCI_INFO_PARAM_CMD_READ_LOCAL_COMMANDS:
        #endif
        #ifdef HCI_INFO_PARAM_CMD_READ_LOCAL_FEATURES
                case HCI_INFO_PARAM_CMD_READ_LOCAL_FEATURES:
        #endif
        #ifdef HCI_INFO_PARAM_CMD_READ_BD_ADDR
                case HCI_INFO_PARAM_CMD_READ_BD_ADDR:
        #endif
        #ifdef HCI_LE_CTL_READ_BUFFER_SIZE
                case HCI_LE_CTL_READ_BUFFER_SIZE:
        #endif
        #ifdef HCI_LE_CTL_READ_LOCAL_SUPPORTED_FEATURE
                case HCI_LE_CTL_READ_LOCAL_SUPPORTED_FEATURE:
        #endif
        #ifdef HCI_LE_CTL_READ_ADV_CHANNEL_TX_POWER
                case HCI_LE_CTL_READ_ADV_CHANNEL_TX_POWER:
        #endif
        #ifdef HCI_LE_CTL_READ_WHITE_LIST_SIZE
                case HCI_LE_CTL_READ_WHITE_LIST_SIZE:
        #endif
        #ifdef HCI_LE_CTL_CLEAR_WHILE_LIST
                case HCI_LE_CTL_CLEAR_WHILE_LIST:
        #endif
        #ifdef HCI_LE_CTL_RAND
                case HCI_LE_CTL_RAND:
        #endif
        #ifdef HCI_LE_CTL_READ_SUPPORTED_STATES
                case HCI_LE_CTL_READ_SUPPORTED_STATES:
        #endif
        #ifdef HCI_LE_CTL_TEST_END
                case HCI_LE_CTL_TEST_END:
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE
                case HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE:
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_GET_SEC_LEVEL
                case HCI_VENDOR_CMD_GAP_GET_SEC_LEVEL:
        #endif
        #ifdef HCI_VENDOR_CMD_GAP_CONFIGURE_WHITELIST
                case HCI_VENDOR_CMD_GAP_CONFIGURE_WHITELIST:
        #endif
        #ifdef HCI_VENDOR_CMD_GATT_SERVER_INIT
                case HCI_VENDOR_CMD_GATT_SERVER_INIT:
        #endif
                    cmdPacket->packet.command.lengthOfParams = 0;
                break;
            }
        }
        else
        {
            cmdPacket->packet.command.lengthOfParams = 0;
        }

        HCI_MESG_DBG(hostdbgfile,"HCI Command Param Length %d\n", 
                                   cmdPacket->packet.command.lengthOfParams);

		uint8 intfstatus = Profile_HCI_Interface_Get_Status(opcode);
		HCI_MESG_DBG ( hostdbgfile, "Interface status for %04X is %X \n", opcode, intfstatus );
		
		/* check if there is an ongoing GATT procedure and the command being
		 * enqueued is also a GATT procedure
		 */
		if((opcode >= HCI_VENDOR_CMD_GATT_EXCHG_CONFIG) &&
		   (opcode <= HCI_VENDOR_CMD_GATT_SIGNED_WRITE_WITHOUT_RESPONSE) &&
		   (Profile_HCI_Interface_Get_Current_GATT_Procedure_Opcode() != HCI_CMD_NO_OPERATION))
		{
			HCI_MESG_DBG(hostdbgfile,"gatt procedure ongoing\n");
			intfstatus = HCI_PROFILE_CMD_INTERFACE_BUSY;
		}
		
		/* if interface is busy, return busy status */
		if (intfstatus == HCI_PROFILE_CMD_INTERFACE_BUSY)
		{
			cmdPacket->packetState = HCI_PACKET_STATE_PENDING;
		}
		else
		{
			cmdPacket->packetState = HCI_PACKET_STATE_PROCESSING;
			Profile_HCI_Interface_Set_Status(opcode, HCI_PROFILE_CMD_INTERFACE_BUSY);
		}
        /* link the cmd packet to the ready hci packet queue */
        list_insert_tail (&hciWritePktQueue, (tListNode *)cmdPacket);
       
        HCI_MESG_DBG( hostdbgfile, "Inserted HCI Cmd into Ready List %d\n",list_get_size(&hciWritePktQueue));

        retval = BLE_STATUS_SUCCESS;
    }

    return (retval);
}

tBleStatus HCI_Register_Rx_Event_Handler (HCI_READ_EVENT_NOTIFY_TYPE eventPktRecvHandler)
{
    /* register a callback function pointer to notify the profile layer about any received event */
	HCI_MESG_DBG(hostdbgfile,"HCI_Register_Rx_Event_Handler: %p, %p \n", HCIEventPacketNotifyFunc, eventPktRecvHandler);
   
    if(eventPktRecvHandler == NULL)
    {
        return BLE_STATUS_NULL_PARAM;
    }
    HCIEventPacketNotifyFunc = eventPktRecvHandler;
    return BLE_STATUS_SUCCESS;
}

/**
 * HCI_Process_Q
 * 
 * 1. writes the pending command packets to spi 
 * 2. reads data from HTL if available
 * 3. processes the events read from HTL
 */ 

void HCI_Process_Q(void)
{
  tHciDataPacket * hciReadPacket = NULL;
  tHciPacket * hciWritePacket = NULL;
  tListNode *pCurr = NULL;
  uint8 HCIWriteDataBuffer[HCI_PACKET_SIZE];
  uint HCIWriteDataLenght = 0;
  
  tBleStatus HTL_Read_ret;  //[SO]fixme!!! add here details on *_Process_Q idle state
  HCI_Process_Q_LP = 0;  //[SO]fixme!!! add here details on *_Process_Q idle state
  
  while (1)
  {
    /* send any pending command packets */
    while((gHciNumOfFreeCmdPktAtBLE > 0) &&
          (list_is_empty (&hciWritePktQueue) == FALSE))
    {
      HCI_Process_Q_LP = 1;
      
      HCI_MESG_DBG(hostdbgfile,"HCI_Process_Q : write cmd\n");
      pCurr = hciWritePktQueue.next;
      hciWritePacket = NULL;
      for(; pCurr != &hciWritePktQueue; pCurr = pCurr->next)
      {
        if(((tHciPacket*)pCurr)->packetState == HCI_PACKET_STATE_PROCESSING)
        {
          list_remove_node(pCurr);
          hciWritePacket = (tHciPacket*)pCurr;
          break;
        }
      }
      
      if(hciWritePacket != NULL)
      {
        switch (hciWritePacket->hciPktType)
        {
        case HCI_PKT_TYPE_ACL:
          HCIWriteDataLenght = HCI_Compose_Acl_Packet (HCIWriteDataBuffer, hciWritePacket);
          break;
        case HCI_PKT_TYPE_COMMAND:
          HCIWriteDataLenght = HCI_Compose_Cmd_Packet (HCIWriteDataBuffer, hciWritePacket);
          break;
        }
        if(HTL_Write (HCIWriteDataBuffer, HCIWriteDataLenght) != BLE_STATUS_SUCCESS)
        {
          /* we were unable to send the packet over spi this time.
          * so try later.
          * TODO : avoid processing of the packet multiple times. If
          * a spi packet cannot be sent the first time, the second time
          * retransmission should be tried without any additional processing
          */
          list_insert_head(&hciWritePktQueue, (tListNode *)(hciWritePacket));
        }
        gHciNumOfFreeCmdPktAtBLE --;
        HCI_MESG_DBG(hostdbgfile,"Written HCI command packet to HTL for sending \n");
        /* put the packet into a PENDING_EVT state. it will be freed when a command
        * status or command complete event will be received for the command
        * just sent
        */
        hciWritePacket->packetState = HCI_PACKET_STATE_PENDING_EVT;
        list_insert_tail (&hciWritePktQueue, (tListNode *)(hciWritePacket));
      }
      else
      {
        /* no packets to be processed */
        break;
      }
    }
    
    HTL_Read_ret = HTL_Read();
    if(HTL_Read_ret != BLE_STATUS_FAILED){
      HCI_Process_Q_LP = 1;
    }
    if(HTL_Read_ret == BLE_STATUS_SUCCESS)
    {
      HCI_MESG_DBG(hostdbgfile,"HCI_Process_Q : read event\n");
    }
    else
    {
      break;
    }
    
  }
  
  /* process any pending events read */
  DISABLE_INTERRUPTS();
  BOOL list_empty = list_is_empty(&hciReadPktRxQueue);        
  /* process any pending events read */
  while(list_empty == FALSE)
  {
    HCI_Process_Q_LP = 1;
    list_remove_head (&hciReadPktRxQueue, (tListNode **)&hciReadPacket);
    ENABLE_INTERRUPTS();
    Process_RX_Data(hciReadPacket);
    DISABLE_INTERRUPTS();
    list_empty = list_is_empty(&hciReadPktRxQueue);
  }
  ENABLE_INTERRUPTS();
	
}
#if 1
void HCI_Clear_Pending_Packets()
{
  tListNode *pCurr = NULL;
  tListNode *pNext = NULL;
  uint size = 0;
  tHciPacket * hciWritePacket = NULL;
  
  
  size = list_get_size(&hciWritePktQueue);
  pCurr = hciWritePktQueue.next;
  pNext = pCurr->next;
  HCI_MESG_DBG(hostdbgfile,"HCI_Clear_Pending_Packets %d\n",size);
  while(size--)
  {
    if((((tHciPacket*)pCurr)->packetState == HCI_PACKET_STATE_PENDING))
    {
      list_remove_node(pCurr);
      hciWritePacket = (tHciPacket*)pCurr;
      
      /* free the command interface */
      Profile_HCI_Interface_Set_Status(hciWritePacket->packet.command.opcode,HCI_PROFILE_CMD_INTERFACE_FREE);
      /* Reset the HCI Write packet and queue back to the hci write packet pool */
      hciWritePacket->hciPktType = HCI_PKT_TYPE_INVALID;
      hciWritePacket->packetState = HCI_PACKET_STATE_IDLE;
      hciWritePacket->profileIdx = 0;
      Osal_MemSet ((void *)(&hciWritePacket->packet), 0, sizeof(hciWritePacket->packet));
      Osal_MemSet ((void *)(hciWritePacket->dataBuffer), 0, HCI_PACKET_SIZE);
      list_insert_tail ( &hciWritePktPool, (tListNode *)(hciWritePacket));
      HCI_MESG_DBG(hostdbgfile,"released\n");
    }
  }
}
#endif
/**
 * HCI_Enqueue_Packet_To_HTL
 * 
 * enqueues a packet to HTL for read
 */ 
void HCI_Enqueue_Packet_To_HTL()
{
	tHciDataPacket * hciReadPacket = NULL;
	
	/* NOTE : interrupts have to be disabled before
	 * calling the function from the main loop as
	 * this function is called both from the main loop
	 * as well as the ISR
	 */ 
	if (list_is_empty (&hciReadPktPool) == FALSE)
	{
		HCI_MESG_DBG(hostdbgfile,"enqueueing a packet for read to HTL\n");
		list_remove_head (&hciReadPktPool, (tListNode **)&hciReadPacket);
		HTL_Enqueue(hciReadPacket);
	}
	else
	{
		HCI_MESG_DBG(hostdbgfile,"HCI Read Packet Pool is empty, wait for a free packet\n");
	}
}
