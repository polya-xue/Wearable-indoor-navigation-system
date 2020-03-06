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
*   FILENAME        -  hhci_int.c
*
*   COMPILER        -  gnu gcc
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:     09/03/2012
*   $Revision$: first version
*   $Author$:   Nitin Kumar Tekriwal
*   Comments: 
*
*******************************************************************************
*
*  File Description 
*  ---------------------
* This file have implementation of host HCI layer.
*
******************************************************************************/


/******************************************************************************
* Include Files
******************************************************************************/
#include <host_config.h>
#include <hal_types.h>
#include <list.h>
#include <ble_status.h>
#include <hci.h>
#include <hci_i.h>
#include <profile_hci_interface.h>
#include <htl.h>
#include <host_osal.h>
#include <debug.h>
#include <ble_profile.h>
#include "stm32l0xx_hal.h"

/*******************************************************************************
* Locial Function Prototypes
*******************************************************************************/
static tBleStatus HCI_Read_Event_Processing ( uint8 * hciReceivedData );
static tBleStatus HCI_Read_ACL_Processing ( uint8 * hciReceivedData);
static tBleStatus HCI_Event_Vendor_Specific (uint8 * parameters, tHciResponsePacket * responsePkt);
static tBleStatus HCI_Event_Command_Complete (uint8 * parameters, tHciResponsePacket * responsePkt);
static void HCI_Gatt_Char_Val_Update_Cmplt_Handler(uint16 opcode,uint16* servHandle,uint16* charHandle);
/******************************************************************************
* Global Variable
******************************************************************************/
tListNode hciWritePktPool;
tListNode hciWritePktQueue;
tListNode hciReadPktPool;
tListNode hciReadPktRxQueue;

/*******************************************************************************
* Imported valiables
*******************************************************************************/
extern void *hostdbgfile;
extern HCI_READ_ACL_PACKET_NOTIFY_TYPE HCIAclPacketNotifyFunc;
extern HCI_READ_EVENT_NOTIFY_TYPE HCIEventPacketNotifyFunc;
extern uint8 gHciNumOfFreeCmdPktAtBLE;
extern uint8 connection_closed; //[SO]fixme!!! add here disconnection bug details
/*******************************************************************************
* Function definitions 
*******************************************************************************/
/**
* Process_RX_Data
* 
* @param[in] hciReadPacket : the data to be processed
* 
* parses the event or acl data received and notifies the
* upper layers
*/ 
void Process_RX_Data(tHciDataPacket* hciReadPacket)
{	
  HCI_MESG_DBG(hostdbgfile,"Process_RX_Data\n");
  
  switch (hciReadPacket->hciPktType)
  {
  case HCI_PKT_TYPE_EVENT:
    HCI_Read_Event_Processing ((uint8 *)hciReadPacket->dataBuff);
    break;
  case HCI_PKT_TYPE_ACL:
    HCI_Read_ACL_Processing ((uint8 *)hciReadPacket->dataBuff);
    break;
  case HCI_PKT_TYPE_COMMAND:
    /* As HCI command packet is not supported form controller to host in BLE spec,
    * we have received an invalid packet. */
  case HCI_PKT_TYPE_SCO:
    /* As SCO packets are not supported in BLE we received an invalid packet */
  default:
    /* We have received an invalid packet type. According to BT spec on receiving
    * an invalid hci packet type the system should be reset. */
    /* exit (1); */
    break;
  }
  list_insert_tail (&hciReadPktPool, (tListNode *)hciReadPacket);
  
  HCI_MESG_DBG( hostdbgfile, "HCI Read Path Processing Complete\n");
}

/**
* HCI_Compose_Acl_Packet
*/
int HCI_Compose_Acl_Packet ( uint8 * HciTxDataPacket, tHciPacket * HCIAclPacket )
{
  int copiedLenght = 0;
  uint16 temp = 0;
  /* HCI packet type header */
  *(HciTxDataPacket + copiedLenght) = HCI_PKT_TYPE_ACL;
  copiedLenght++;
  
  /* ACL packet header */
  temp = HCIAclPacket->packet.aclData.packetHeader;
  *(HciTxDataPacket + copiedLenght) = (uint8)(temp);
  copiedLenght++;
  *(HciTxDataPacket + copiedLenght) = (uint8)(temp >> 8);
  copiedLenght++;
  
  /* ACL payload length */
  temp = HCIAclPacket->packet.aclData.payloadLangth;
  *(HciTxDataPacket + copiedLenght) = (uint8)(temp);
  copiedLenght++;
  *(HciTxDataPacket + copiedLenght) = (uint8)(temp >> 8);
  copiedLenght++;
  
  /* ACL payload */
  temp = HCIAclPacket->packet.aclData.payloadLangth;
  Osal_MemCpy (HciTxDataPacket + copiedLenght, HCIAclPacket->dataBuffer, temp);
  copiedLenght += temp;
  
  return (copiedLenght);
}

int HCI_Compose_Cmd_Packet ( uint8 * HciTxDataPacket, tHciPacket * HCICmdPacket )
{
  int copiedLenght = 0;
  uint8 paramlen = 0;
  
  HCI_MESG_DBG( hostdbgfile, "HCI_Compose_Cmd_Packet \n");
  
  /* HCI packet type header */
  *(HciTxDataPacket + copiedLenght) = HCI_PKT_TYPE_COMMAND;
  copiedLenght++;
  
  /* Command OpCode */
  *(HciTxDataPacket + copiedLenght) = (uint8)(HCICmdPacket->packet.command.opcode);
  copiedLenght++;
  *(HciTxDataPacket + copiedLenght) = (uint8)(HCICmdPacket->packet.command.opcode >> 8);
  copiedLenght++;
  
  /* Command Parameter langth */
  paramlen = (uint8)HCICmdPacket->packet.command.lengthOfParams;
  *(HciTxDataPacket + copiedLenght) = paramlen;
  copiedLenght++;
  
  /* Command Parameters */
  if (paramlen != 0)
  {
    Osal_MemCpy (HciTxDataPacket + copiedLenght, HCICmdPacket->dataBuffer, paramlen);
    copiedLenght += paramlen;
  }
  
  HCI_MESG_DBG(hostdbgfile,"HCI_Compose_Cmd_Packet length = %d\n",copiedLenght);
  return (copiedLenght);
}

void HCI_Enable_GATT_Cmd(uint16 opcode)
{
  tListNode *pCurr = NULL;
  
  HCI_MESG_DBG(hostdbgfile,"search %d\n",list_get_size(&hciWritePktQueue));
  pCurr = hciWritePktQueue.next;
  
  /* check if the command that was just released is a gatt 
  * procedure. If it is then free any of the pending gatt
  * procedure command packets
  */
  if((opcode >= HCI_VENDOR_CMD_GATT_EXCHG_CONFIG) &&
     (opcode <= HCI_VENDOR_CMD_GATT_SIGNED_WRITE_WITHOUT_RESPONSE))
  {
    for(; pCurr != &hciWritePktQueue; pCurr = pCurr->next)
    {
      if(((((tHciPacket*)pCurr)->packetState == HCI_PACKET_STATE_PENDING)) &&
         (((tHciPacket*)pCurr)->packet.command.opcode >= HCI_VENDOR_CMD_GATT_EXCHG_CONFIG) &&
           (((tHciPacket*)pCurr)->packet.command.opcode <= HCI_VENDOR_CMD_GATT_SIGNED_WRITE_WITHOUT_RESPONSE))
      {
        /* change the state to processing */
        ((tHciPacket*)pCurr)->packetState = HCI_PACKET_STATE_PROCESSING;
        Profile_HCI_Interface_Set_Status(((tHciPacket*)pCurr)->packet.command.opcode,HCI_PROFILE_CMD_INTERFACE_BUSY);
        Profile_HCI_Interface_Set_GATT_Procedure_Status(((tHciPacket*)pCurr)->packet.command.opcode);
        HCI_MESG_DBG(hostdbgfile,"another gatt procedure freed\n");
        break;
      }
    }
  }
}

/**
* HCI_Release_Command
*
* @param[in] opcode : opcode of the command to be released
* 
* returns the profile ID for which had enqueued this
* command to HCI
*/ 
uint8 HCI_Release_Command(uint16 opcode)
{
  tHciPacket * hciWritePacket = NULL;
  tListNode *pCurr = NULL;
  
  uint8 profileID = 0;
  
  HCI_MESG_DBG(hostdbgfile,"HCI_Release_Command\n");
  
  pCurr = hciWritePktQueue.next;
  
  for(; pCurr != &hciWritePktQueue; pCurr = pCurr->next)
  {
    if(((((tHciPacket*)pCurr)->packetState == HCI_PACKET_STATE_PENDING_EVT)) &&
       (((tHciPacket*)pCurr)->packet.command.opcode == opcode))
    {
      HCI_MESG_DBG(hostdbgfile,"found command %d\n",list_get_size(&hciWritePktQueue));
      
      list_remove_node(pCurr);
      hciWritePacket = (tHciPacket*)pCurr;
      profileID = hciWritePacket->profileIdx;
      
      /* Reset the HCI Write packet and queue back to the hci write packet pool */
      hciWritePacket->hciPktType = HCI_PKT_TYPE_INVALID;
      hciWritePacket->packetState = HCI_PACKET_STATE_IDLE;
      hciWritePacket->profileIdx = 0;
      Osal_MemSet ((void *)(&hciWritePacket->packet), 0, sizeof(hciWritePacket->packet));
      Osal_MemSet ((void *)(hciWritePacket->dataBuffer), 0, HCI_PACKET_SIZE);
      list_insert_tail ( &hciWritePktPool, (tListNode *)(hciWritePacket));
      HCI_MESG_DBG(hostdbgfile,"released\n");
      
      /* free the command interface */
      Profile_HCI_Interface_Set_Status(opcode,HCI_PROFILE_CMD_INTERFACE_FREE);
      break;
    }
  }
  
  /* if the opcode is for a gatt procedure, then
  * donot free any pending command here. It will be
  * freed when the gatt procedure completes
  */ 
  if((opcode >= HCI_VENDOR_CMD_GATT_EXCHG_CONFIG) &&
     (opcode <= HCI_VENDOR_CMD_GATT_SIGNED_WRITE_WITHOUT_RESPONSE))
  {
    return profileID;
  }
  
  pCurr = hciWritePktQueue.next;
  
  /* if there are any pending commands with the same opcode, 
  * change the packet state so that they can be processed
  */
  
  for(; pCurr != &hciWritePktQueue; pCurr = pCurr->next)
  {
    if(((((tHciPacket*)pCurr)->packetState == HCI_PACKET_STATE_PENDING)) &&
       (((tHciPacket*)pCurr)->packet.command.opcode == opcode))
    {
      /* change the state to processing */
      ((tHciPacket*)pCurr)->packetState = HCI_PACKET_STATE_PROCESSING;
      HCI_MESG_DBG(hostdbgfile,"another\n");
      Profile_HCI_Interface_Set_Status(opcode, HCI_PROFILE_CMD_INTERFACE_BUSY);
      break;
    }
  }
  
  return profileID;
}

/**
* HCI_Gatt_Char_Val_Update_Cmplt_Handler
*
* @param[in] opcode : opcode of the command to be released
* 
* returns service and characteristic handles of the
* characteristic which was updated successfully
*/ 
void HCI_Gatt_Char_Val_Update_Cmplt_Handler(uint16 opcode,uint16* servHandle,uint16* charHandle)
{
  tHciPacket * hciWritePacket = NULL;
  tListNode *pCurr = NULL;
  
  HCI_MESG_DBG(hostdbgfile,"HCI_Gatt_Char_Val_Update_Cmplt_Handler\n");
  
  pCurr = hciWritePktQueue.next;
  
  for(; pCurr != &hciWritePktQueue; pCurr = pCurr->next)
  {
    if(((((tHciPacket*)pCurr)->packetState == HCI_PACKET_STATE_PENDING_EVT)) &&
       (((tHciPacket*)pCurr)->packet.command.opcode == opcode))
    {
      HCI_MESG_DBG(hostdbgfile,"found\n");
      hciWritePacket = (tHciPacket*)pCurr;
      *servHandle = LE_TO_NRG_16(hciWritePacket->dataBuffer);
      *charHandle = LE_TO_NRG_16(&hciWritePacket->dataBuffer[2]);
      HCI_MESG_DBG(hostdbgfile,"serv %x, char %x\n",*servHandle,*charHandle);
      
      break;
    }
  }
}

/*
* here the argument hciReceivedData don't has the packet type header bcz 
* there is not use of that header, hence has been removed. 
*/
tBleStatus HCI_Read_ACL_Processing ( uint8 * hciReceivedData)
{
  uint16 HCIReadAclHeader = 0;
  uint16 HCIReadAclPayloadLangth = 0;
  uint8 HCIReadAclDataBuff[HCI_PACKET_SIZE] = {0};
  uint16 lb = 0;
  uint16 hb = 0;
  
  if (hciReceivedData == NULL)
  {
    return (BLE_STATUS_NULL_PARAM);
  }
  
  lb = (uint16)(*(hciReceivedData + ACL_DATA_HANDLE_OFFSET));
  hb = (uint16)(*(hciReceivedData + ACL_DATA_HANDLE_OFFSET + 1));
  hb = (hb << 8);
  HCIReadAclHeader = (uint16)(hb | lb);
  
  lb = (uint16)(*(hciReceivedData + ACL_DATA_TOTAL_LENGTH_OFFSET));
  hb = (uint16)(*(hciReceivedData + ACL_DATA_TOTAL_LENGTH_OFFSET + 1));
  hb = (hb << 8);
  HCIReadAclPayloadLangth = (uint16)(hb | lb);
  
  Osal_MemCpy ( HCIReadAclDataBuff, hciReceivedData + HCI_ACL_PACKET_HDR_SIZE, HCIReadAclPayloadLangth );
  
  /* now call the callback function to pass the received acl data to profile */
  if (HCIAclPacketNotifyFunc != NULL)
  {
    HCIAclPacketNotifyFunc ( HCIReadAclHeader, HCIReadAclPayloadLangth, HCIReadAclDataBuff );
  }
  return (BLE_STATUS_SUCCESS);
}

tBleStatus HCI_Read_Event_Processing ( uint8 * hciReceivedData )
{
  tBleStatus retval = BLE_STATUS_FAILED;
  tHciResponsePacket HCIResponsePkt;
  uint16 HCIReadEventCode = 0;
  uint8 HCIReadLengthOfParams = 0;
  uint8 HCIReadParamBuff[HCI_PACKET_SIZE] = {0};
  uint16 opcode = 0x00;
  uint32 uwPRIMASK_Bit = __get_PRIMASK();	/**< backup PRIMASK bit */
  
  HCI_MESG_DBG( hostdbgfile,"HCI Read An Event Packet from HTL for Processing\n");
  
  if (hciReceivedData == NULL)
  {
    return (retval);
  }
  
  HCIReadEventCode = (uint8)(*(hciReceivedData + EVT_CODE_OFFSET));
  HCIReadLengthOfParams = (uint8)(*(hciReceivedData + EVT_PARAM_TOTAL_LENGTH_OFFSET));
  Osal_MemCpy (HCIReadParamBuff, hciReceivedData + HCI_EVT_PACKET_HDR_SIZE, HCIReadLengthOfParams);
  
  
  HCIResponsePkt.hciNotifPkt.eventCode = HCIReadEventCode;
  HCIResponsePkt.hciNotifPkt.eventParamLen = HCIReadLengthOfParams;
  HCI_MESG_DBG( hostdbgfile, "HCI Received Event %02X, Event Param Length %d\n", 
               HCIResponsePkt.hciNotifPkt.eventCode,  HCIResponsePkt.hciNotifPkt.eventParamLen);
  retval = BLE_STATUS_SUCCESS;
  
  /* Parse the event packet according to event code */
  switch (HCIReadEventCode)
  {
  case HCI_EVT_DISCONN_COMPLETE: 
    HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_Disconnection_Complete \n");
    HCIResponsePkt.evtPkt.disconnComplt.status = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff + 0);
    HCIResponsePkt.evtPkt.disconnComplt.connectionHandle = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 1);
    HCIResponsePkt.evtPkt.disconnComplt.reason = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff + 3);
    /* clear out all pending packets */
    HCI_Clear_Pending_Packets();
    __disable_irq();		/**< Disable all interrupts by setting PRIMASK bit on Cortex*/
    connection_closed = 1;
    __set_PRIMASK(uwPRIMASK_Bit);	/**< Restore PRIMASK bit*/
    break;
  case HCI_EVT_ENCRYPT_CHANGE:
    HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_Encryption_Change \n");
    HCIResponsePkt.evtPkt.encryptionChange.status = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff);
    HCIResponsePkt.evtPkt.encryptionChange.connectionHandle = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 1);
    HCIResponsePkt.evtPkt.encryptionChange.encryptionEnable = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff + 3);
    break;
  case HCI_EVT_READ_REMOTE_VERSION_COMPLETE:
    HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_Read_Remote_Version_Complete \n");
    HCIResponsePkt.evtPkt.readRemoteVersionComp.status = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff);
    HCIResponsePkt.evtPkt.readRemoteVersionComp.connectionHandle = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 1);
    HCIResponsePkt.evtPkt.readRemoteVersionComp.version = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff + 3);
    HCIResponsePkt.evtPkt.readRemoteVersionComp.manufacturerName = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 4);
    HCIResponsePkt.evtPkt.readRemoteVersionComp.subversion = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 6);
    break;
  case HCI_EVT_CMD_COMPLETE:
    retval = HCI_Event_Command_Complete (HCIReadParamBuff, &HCIResponsePkt);
    opcode= HCIResponsePkt.evtPkt.commandComplete.commandOpcode;
    break;
  case HCI_EVT_CMD_STATUS:
    HCI_MESG_DBG ( hostdbgfile, "Received HCI_Event_Command_Status \n");
    HCIResponsePkt.evtPkt.commandStatus.status = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff);
    HCIResponsePkt.evtPkt.commandStatus.numHCICommandPackets = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff + 1);
    opcode = HCIResponsePkt.evtPkt.commandStatus.commandOpcode = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 2);
    
    /* if the command status has not returned success and the 
    * command whihc has failed is a gatt procedure, then free the
    * gatt interface
    */ 
    if((HCIResponsePkt.evtPkt.commandStatus.status != BLE_STATUS_SUCCESS) &&
       (opcode == Profile_HCI_Interface_Get_Current_GATT_Procedure_Opcode()))
    {
      Profile_HCI_Interface_Reset_GATT_Procedure_Status();
    }
    gHciNumOfFreeCmdPktAtBLE = HCIResponsePkt.evtPkt.commandStatus.numHCICommandPackets;
    break;
  case HCI_EVT_HARDWARE_ERROR:
    HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_Hardware_Error \n");
    HCIResponsePkt.evtPkt.hardwareError.hardwareCode = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff);
    break;
  case HCI_EVT_NUM_COMP_PKTS:
    HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_Number_0f_Completed_Packets \n");
    HCIResponsePkt.evtPkt.numCompletedPkt.numberOfHandles = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff);
    HCIResponsePkt.evtPkt.numCompletedPkt.connectionHandleArrayPtr = (uint16 *)(HCIReadParamBuff + 1);
    HCIResponsePkt.evtPkt.numCompletedPkt.numOfCompletedPktArrPtr = 
      (uint16 *)( HCIReadParamBuff + 1 + (2*HCIResponsePkt.evtPkt.numCompletedPkt.numberOfHandles));
    break;
  case HCI_EVT_DATA_BUFFER_OVERFLOW:
    HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_Data_Buffer_Overflow \n");
    HCIResponsePkt.evtPkt.dataBuffOverflow.linkType = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff);
    break;
  case HCI_EVT_ENCRYPTION_KEY_REFRESH_COMPLETE:
    HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_Encryption_Key_Refresh_Complete \n");
    HCIResponsePkt.evtPkt.encryptionKeyRefresh.status = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff);
    HCIResponsePkt.evtPkt.encryptionKeyRefresh.connectionHandle = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 1);
    break;
  case HCI_EVT_LE_META_EVENT:
    HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_LE_Meta_Event \n");
    HCIResponsePkt.evtPkt.leEvent.subEventCode = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff);
    switch (HCIResponsePkt.evtPkt.leEvent.subEventCode)
    {
    case HCI_EVT_LE_CONN_COMPLETE:
      HCI_MESG_DBG( hostdbgfile, "Received HCI_LE_Connection_Complete_Event\n");
      HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leConnectionComplete.status = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff + 1);
      HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leConnectionComplete.connectionHandle = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 2);
      HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leConnectionComplete.role = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff + 4);
      HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leConnectionComplete.peerAddressType = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff + 5);
      Osal_MemCpy ((void *)HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leConnectionComplete.peerAddress, 
                   (void *)(HCIReadParamBuff + 6), 
                   6 );
      HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leConnectionComplete.connectionInterval = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 12);
      HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leConnectionComplete.connectionLatency = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 14);
      HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leConnectionComplete.supervisionTimeout = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 16);
      HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leConnectionComplete.masterClockAccuracy = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff + 18);
      break;
    case HCI_EVT_LE_CONN_UPDATE_COMPLETE:
      HCI_MESG_DBG( hostdbgfile, "Received HCI_LE_Connection_Update_Complete_Event \n");
      HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leConnectionUpdateComp.status = UNPACK_1_BYTE_PARAMETER(HCIReadParamBuff + 1);
      HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leConnectionUpdateComp.connectionHandle = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 2);
      HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leConnectionUpdateComp.connectionInterval = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 4);
      HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leConnectionUpdateComp.connectionLatency = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 6);
      HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leConnectionUpdateComp.supervisionTimeout = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 8);
      break;
    case HCI_EVT_LE_LTK_REQUEST:
      HCI_MESG_DBG( hostdbgfile, "Received HCI_LE_Long_Term_Key_Request_Event \n");
      HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leLTKRequest.connectionHandle = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 1);
      Osal_MemCpy ((void *)HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leLTKRequest.randomNumber, 
                   (void *)(HCIReadParamBuff + 3), 
                   8 );
      HCIResponsePkt.evtPkt.leEvent.leMetaEvtPkt.leLTKRequest.encryptionDiversifier = UNPACK_2_BYTE_PARAMETER(HCIReadParamBuff + 11);
      break;
    default:
      HCI_MESG_DBG( hostdbgfile, "Received an Invalid LE Meta event \n");
      break;
    }
    break; /* HCI_EVT_LE_META_EVENT */
  case HCI_EVT_VENDOR_SPECIFIC:
    retval = HCI_Event_Vendor_Specific (HCIReadParamBuff, &HCIResponsePkt);
    break;
  default:
    HCI_MESG_DBG( hostdbgfile, "Received an Invalid HCI event \n");
    break;
  }
  
  /* if we have received a command complete or command status event, then
  * release the pending command and notify the main profile about the profile
  * layer that had issue this comamnd
  */ 
  if((HCIResponsePkt.hciNotifPkt.eventCode == HCI_EVT_CMD_COMPLETE) || 
     (HCIResponsePkt.hciNotifPkt.eventCode == HCI_EVT_CMD_STATUS)) 
  {
    HCIEventPacketNotifyFunc(HCI_Release_Command(opcode),&HCIResponsePkt);
  }
  else
  {
    /* when the event is not command status or command
    * complete, opcode will have the value 0x00
    */ 
    HCIEventPacketNotifyFunc (opcode,&HCIResponsePkt);
  }
  
  return (retval);
}

tBleStatus HCI_Event_Command_Complete (uint8 * parameters, tHciResponsePacket * responsePkt)
{
  HCI_MESG_DBG ( hostdbgfile, "Received HCI_Event_Command_Complete\n" );
  responsePkt->evtPkt.commandComplete.numHCICommandPackets = UNPACK_1_BYTE_PARAMETER(parameters);
  responsePkt->evtPkt.commandComplete.commandOpcode = UNPACK_2_BYTE_PARAMETER(parameters + 1);
  /* all commands which return a command complete do return a status */
  responsePkt->evtPkt.commandComplete.status = UNPACK_1_BYTE_PARAMETER(parameters + 3);
  
  HCI_MESG_DBG ( hostdbgfile, "Command Complete Opcode %04X\n", 
                responsePkt->evtPkt.commandComplete.commandOpcode);
  
  switch (responsePkt->evtPkt.commandComplete.commandOpcode)
  {
#ifdef HCI_HOST_CTL_CMD_READ_TRANSMIT_POWER_LEVEL
  case HCI_HOST_CTL_CMD_READ_TRANSMIT_POWER_LEVEL:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readTxPowerLevelResp.connectionHandle = UNPACK_2_BYTE_PARAMETER(parameters + 4);
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readTxPowerLevelResp.txPowerLevel = UNPACK_1_BYTE_PARAMETER(parameters + 6);
    HCI_MESG_DBG ( hostdbgfile, "Command Complete Opcode %d\n", 
                  responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readTxPowerLevelResp.txPowerLevel);           
    break;
#endif 
#ifdef HCI_INFO_PARAM_CMD_READ_LOCAL_VERSION
  case HCI_INFO_PARAM_CMD_READ_LOCAL_VERSION:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readLocalVersionResp.HCIVersion = UNPACK_1_BYTE_PARAMETER(parameters + 4);
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readLocalVersionResp.HCIRevision = UNPACK_2_BYTE_PARAMETER(parameters + 5);
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readLocalVersionResp.LMPVersion = UNPACK_1_BYTE_PARAMETER(parameters + 7);
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readLocalVersionResp.manufacturerName = UNPACK_2_BYTE_PARAMETER(parameters + 8);
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readLocalVersionResp.LMPSubversion = UNPACK_2_BYTE_PARAMETER(parameters + 10);
    break;
#endif
#ifdef HCI_INFO_PARAM_CMD_READ_LOCAL_COMMANDS
  case HCI_INFO_PARAM_CMD_READ_LOCAL_COMMANDS:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readLocalCommandsResp.supportedCommandBuff = (uint8 *)(parameters + 4);
    break;
#endif
#ifdef HCI_INFO_PARAM_CMD_READ_LOCAL_FEATURES
  case HCI_INFO_PARAM_CMD_READ_LOCAL_FEATURES:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readLocalFeaturesResp.LMPFeaturesBuff = (uint8 *)(parameters + 4);
    break;
#endif
#ifdef HCI_INFO_PARAM_CMD_READ_BD_ADDR
  case HCI_INFO_PARAM_CMD_READ_BD_ADDR:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readBRAddressResp.BRAddressBuff = (uint8 *)(parameters + 4);
    break;
#endif
#ifdef HCI_STATUS_PARAM_CMD_READ_RSSI
  case HCI_STATUS_PARAM_CMD_READ_RSSI:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readRSSIResp.handle = UNPACK_2_BYTE_PARAMETER(parameters + 4);
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readRSSIResp.rssi = UNPACK_1_BYTE_PARAMETER(parameters + 6);
    break;
#endif
#ifdef HCI_LE_CTL_READ_BUFFER_SIZE
  case HCI_LE_CTL_READ_BUFFER_SIZE:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readBufferSizeResp.numHCICommandPackets = UNPACK_2_BYTE_PARAMETER(parameters + 4);
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readBufferSizeResp.totalNumOfLEDataPackets = UNPACK_1_BYTE_PARAMETER(parameters + 6);
    break;
#endif
#ifdef HCI_LE_CTL_READ_LOCAL_SUPPORTED_FEATURE
  case HCI_LE_CTL_READ_LOCAL_SUPPORTED_FEATURE:
    Osal_MemCpy ((void*)responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readLocalSupportedFeatureResp.localFeatures, 
                 (void*)(parameters + 4),
                 (uint)8 );
    break;
#endif
    
#ifdef HCI_LE_CTL_READ_ADV_CHANNEL_TX_POWER
  case HCI_LE_CTL_READ_ADV_CHANNEL_TX_POWER:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readAdvtChnlTxPwrResp.txPwrLevel = UNPACK_1_BYTE_PARAMETER(parameters + 4);
    break;
#endif
    
#ifdef HCI_LE_CTL_READ_WHITE_LIST_SIZE
  case HCI_LE_CTL_READ_WHITE_LIST_SIZE:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readWhiteListSizeResp.whiteListSize = UNPACK_1_BYTE_PARAMETER(parameters + 4);
    break;
#endif
    
#ifdef HCI_LE_CTL_READ_CHANNEL_MAP
  case HCI_LE_CTL_READ_CHANNEL_MAP:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readChannelMapResp.connectionHandle = UNPACK_2_BYTE_PARAMETER(parameters + 4);
    Osal_MemCpy ((void*)responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readChannelMapResp.leChannelMap, 
                 (void*)(parameters + 6),
                 (uint)5 );
    break;
#endif
#ifdef HCI_LE_CTL_ENCRYPT
  case HCI_LE_CTL_ENCRYPT:
    Osal_MemCpy ((void*)responsePkt->evtPkt.commandComplete.cmdCmpltPacket.encryptResp.encryptedData, 
                 (void*)(parameters + 4),
                 (uint)16 );
    break;
#endif
#ifdef HCI_LE_CTL_RAND
  case HCI_LE_CTL_RAND:
    Osal_MemCpy ((void*)responsePkt->evtPkt.commandComplete.cmdCmpltPacket.randomNumResp.randomNum, 
                 (void*)(parameters + 4),
                 (uint)8 );
    break;
#endif
#ifdef HCI_LE_CTL_LTK_REQ_REPLY
  case HCI_LE_CTL_LTK_REQ_REPLY:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.ltkReqReplyResp.connectionHandle = UNPACK_2_BYTE_PARAMETER(parameters + 4);
    break;
#endif
#ifdef HCI_LE_CTL_LTK_REQ_NEGATIVE_REPLY
  case HCI_LE_CTL_LTK_REQ_NEGATIVE_REPLY:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.ltkReqNegativeReplyResp.connectionHandle = UNPACK_2_BYTE_PARAMETER(parameters + 4);
    break;
#endif
#ifdef HCI_LE_CTL_READ_SUPPORTED_STATES
  case HCI_LE_CTL_READ_SUPPORTED_STATES:
    Osal_MemCpy ((void*)responsePkt->evtPkt.commandComplete.cmdCmpltPacket.readSupportedStatesResp.leStates, 
                 (void*)(parameters + 4),
                 (uint)8 );
    break;
#endif
    
#ifdef HCI_LE_CTL_TEST_END
  case HCI_LE_CTL_TEST_END:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.testEndResp.numOfPackets = UNPACK_2_BYTE_PARAMETER(parameters + 4);
    break;
#endif
    
#ifdef HCI_VENDOR_CMD_GAP_INIT
  case HCI_VENDOR_CMD_GAP_INIT:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.gapInitResp.serviceHandle = UNPACK_2_BYTE_PARAMETER(parameters + 4);
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.gapInitResp.devNameCharHandle = UNPACK_2_BYTE_PARAMETER(parameters + 6);
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.gapInitResp.appearanceCharHandle = UNPACK_2_BYTE_PARAMETER(parameters + 8);
    break;
#endif
    
#ifdef HCI_VENDOR_CMD_GAP_GET_SEC_LEVEL
  case HCI_VENDOR_CMD_GAP_GET_SEC_LEVEL:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.gapGetSecurityLevelResp.mimtProtectionRequired = UNPACK_1_BYTE_PARAMETER(parameters + 4);
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.gapGetSecurityLevelResp.bondingRequired = UNPACK_1_BYTE_PARAMETER(parameters + 5);
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.gapGetSecurityLevelResp.oobDataPresent = UNPACK_1_BYTE_PARAMETER(parameters + 6);
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.gapGetSecurityLevelResp.passkeyRequired = UNPACK_1_BYTE_PARAMETER(parameters + 7);
    break;
#endif
#ifdef HCI_VENDOR_CMD_GATT_ADD_SERVICE
  case HCI_VENDOR_CMD_GATT_ADD_SERVICE:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.gattAddServiceResp.serviceHandle = UNPACK_2_BYTE_PARAMETER(parameters + 4);
    break;
#endif
#ifdef HCI_VENDOR_CMD_GATT_INC_SERVICE
  case HCI_VENDOR_CMD_GATT_INC_SERVICE:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.gattIncludeServiceResp.includedHandle = UNPACK_2_BYTE_PARAMETER(parameters + 4);
    break;
#endif
#ifdef HCI_VENDOR_CMD_GATT_ADD_CHAR
  case HCI_VENDOR_CMD_GATT_ADD_CHAR:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.gattAddCharResp.charHandle = UNPACK_2_BYTE_PARAMETER(parameters + 4);
    break;
#endif
#ifdef HCI_VENDOR_CMD_GATT_ADD_CHAR_DESC
  case HCI_VENDOR_CMD_GATT_ADD_CHAR_DESC:
    responsePkt->evtPkt.commandComplete.cmdCmpltPacket.gattAddCharDescResp.charDescHandle = UNPACK_2_BYTE_PARAMETER(parameters + 4);
    break;
#endif
#ifdef HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL
  case HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL:
    HCI_Gatt_Char_Val_Update_Cmplt_Handler(responsePkt->evtPkt.commandComplete.commandOpcode,
                                           &responsePkt->evtPkt.commandComplete.cmdCmpltPacket.gattUpdateCharValRespPkt.servHandle,
                                           &responsePkt->evtPkt.commandComplete.cmdCmpltPacket.gattUpdateCharValRespPkt.charHandle);
    break;
#endif
  }
  
  gHciNumOfFreeCmdPktAtBLE = responsePkt->evtPkt.commandComplete.numHCICommandPackets;
  
  return (BLE_STATUS_SUCCESS);
}


tBleStatus HCI_Event_Vendor_Specific (uint8 * parameters, tHciResponsePacket * responsePkt)
{
  tBleStatus retval = BLE_STATUS_SUCCESS;
  
  responsePkt->evtPkt.vendorSpecificEvt.vsEventCode = UNPACK_2_BYTE_PARAMETER(parameters);
  HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_Vendor_Specific %x\n",responsePkt->evtPkt.vendorSpecificEvt.vsEventCode);
  
  switch (responsePkt->evtPkt.vendorSpecificEvt.vsEventCode)
  {
  case HCI_EVT_BLUE_INITIALIZED:
    {
      responsePkt->evtPkt.vendorSpecificEvt.vsEvtPkt.BlueNRGMode = UNPACK_1_BYTE_PARAMETER(parameters + 2);
      HCI_MESG_DBG( hostdbgfile, "BlueNRG has been initialized, mode %x\n",responsePkt->evtPkt.vendorSpecificEvt.vsEvtPkt.BlueNRGMode);
    }
    break;
  case HCI_EVT_BLUE_GAP_SET_LIMITED_DISCOVERABLE:
    HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_Blue_Gap_Set_Limited_Discoverable \n");
    break;
  case HCI_EVT_BLUE_GAP_PAIRING_CMPLT:
    HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_Blue_Gap_Paring_Complete \n");
    responsePkt->evtPkt.vendorSpecificEvt.vsEvtPkt.status = UNPACK_1_BYTE_PARAMETER(parameters + 4);
    break;
  case HCI_EVT_BLUE_GAP_PASS_KEY_REQUEST:
    HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_Blue_Gap_Pass_Key_Request \n");
    break;
  case HCI_EVT_BLUE_GAP_AUTHORIZATION_REQUEST:
    HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_Blue_Gap_Authorization_Request \n");
    break;
  case HCI_EVT_BLUE_GAP_SLAVE_SECURITY_INITIATED:
    HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_Blue_Gap_Slave_Security_Initiated \n");
    break;
  case HCI_EVT_BLUE_L2CAP_CONN_UPD_RESP:
    HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_Blue_L2CAP_Connection_Update_Response \n"); /* TODO: check parameters */
    responsePkt->evtPkt.vendorSpecificEvt.vsEvtPkt.l2capConnUpdateResp.dataLength = UNPACK_1_BYTE_PARAMETER(parameters + 2);
    responsePkt->evtPkt.vendorSpecificEvt.vsEvtPkt.l2capConnUpdateResp.dataBuff = parameters + 3;
    break;
  case HCI_EVT_BLUE_L2CAP_PROCEDURE_TIMEOUT:
    retval = BLE_STATUS_FAILED;
    break;
  case HCI_EVT_BLUE_DEBUG:
    retval = BLE_STATUS_FAILED;
    break;
  case HCI_EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
    HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_Blue_Gatt_Attribure_Modified \n");
    responsePkt->evtPkt.vendorSpecificEvt.vsEvtPkt.gattAttrModified.attributeHandle = UNPACK_2_BYTE_PARAMETER(parameters + 4);
    responsePkt->evtPkt.vendorSpecificEvt.vsEvtPkt.gattAttrModified.dataLength = UNPACK_1_BYTE_PARAMETER(parameters + 6);
    responsePkt->evtPkt.vendorSpecificEvt.vsEvtPkt.gattAttrModified.attData = parameters + 7;
    break;
  case HCI_EVT_BLUE_GATT_PROCEDURE_TIMEOUT:
    HCI_MESG_DBG( hostdbgfile, "Received HCI_Event_Blue_Gatt_Procedure_Timeout \n");
    //responsePkt->evtPkt.vendorSpecificEvt.vsEvtPkt.gattRole = UNPACK_1_BYTE_PARAMETER(parameters + 2);
    break;
    
    /* all the following verndor specific events returns with same packet type */
  case HCI_EVT_BLUE_EXCHANGE_MTU_RESP:
  case HCI_EVT_BLUE_ATT_FIND_INFORMATION_RESP:
  case HCI_EVT_BLUE_ATT_FIND_BY_TYPE_VAL_RESP:
  case HCI_EVT_BLUE_ATT_READ_BY_TYPE_RESP:
  case HCI_EVT_BLUE_ATT_READ_RESP:
  case HCI_EVT_BLUE_ATT_READ_BLOB_RESP:
  case HCI_EVT_BLUE_ATT_READ_MULTIPLE_RESP:
  case HCI_EVT_BLUE_ATT_READ_BY_GROUP_RESP:
  case HCI_EVT_BLUE_ATT_WRITE_RESP:
  case HCI_EVT_BLUE_ATT_PREPARE_WRITE_RESP:
  case HCI_EVT_BLUE_ATT_EXEC_WRITE_RESP:
  case HCI_EVT_BLUE_GATT_INDICATION:
  case HCI_EVT_BLUE_GATT_NOTIFICATION:
  case HCI_EVT_BLUE_GATT_PROCEDURE_COMPLETE:
  case HCI_EVT_BLUE_GATT_ERROR_RESP:
  case HCI_EVT_BLUE_GATT_DISC_READ_CHAR_BY_UUID_RESP:
    responsePkt->evtPkt.vendorSpecificEvt.vsEvtPkt.gattClientProcedureResp.dataLength = UNPACK_1_BYTE_PARAMETER(parameters + 4);
    responsePkt->evtPkt.vendorSpecificEvt.vsEvtPkt.gattClientProcedureResp.attData = parameters + 5;
    break;
  case HCI_EVT_BLUE_GATT_WRITE_PERMIT_REQ:
    HCI_MESG_DBG( hostdbgfile, "Received HCI_EVT_BLUE_GATT_WRITE_PERMIT_REQ \n");
    responsePkt->evtPkt.vendorSpecificEvt.vsEvtPkt.gattWritePermitReq.attHandle = UNPACK_2_BYTE_PARAMETER(parameters + 4);
    responsePkt->evtPkt.vendorSpecificEvt.vsEvtPkt.gattWritePermitReq.dataLength = UNPACK_1_BYTE_PARAMETER(parameters + 6);
    responsePkt->evtPkt.vendorSpecificEvt.vsEvtPkt.gattWritePermitReq.attData = parameters + 7;
    break;
  case HCI_EVT_BLUE_GATT_READ_PERMIT_REQ:
    
    responsePkt->evtPkt.vendorSpecificEvt.vsEvtPkt.gattReadPermitReq.attHandle = UNPACK_2_BYTE_PARAMETER(parameters + 4);
    HCI_MESG_DBG( hostdbgfile, "Received HCI_EVT_BLUE_GATT_READ_PERMIT_REQ %x \n",responsePkt->evtPkt.vendorSpecificEvt.vsEvtPkt.gattReadPermitReq.attHandle);
    break;
  default:
    HCI_MESG_DBG( hostdbgfile, "Received an Invalid Vendor Specific event !!!!  \n");
    break;
  }
  return (retval);
}




