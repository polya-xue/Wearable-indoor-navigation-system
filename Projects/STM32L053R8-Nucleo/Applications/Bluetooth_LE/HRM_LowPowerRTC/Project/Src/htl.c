/**
******************************************************************************
* @file    htl.c 
* @author  AAS / CL
* @version V1.0.0
* @date    04-July-2014
* @brief   This file have Host HCI Transport layer implementation which will  
*          be used by the Host HCI layer.
******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
 /******************************************************************************
 * Include Files
 *****************************************************************************/
#include "bluenrg_interface.h"

#include <host_config.h>
#include <hal_types.h>
#include <ble_status.h>
#include <list.h>
#include <host_osal.h>
#include <debug.h>
#include <hci.h>
#include "hci_i.h"
#include <htl.h>
#include <ble_profile.h>


/******************************************************************************
 * Macro Declarations
 *****************************************************************************/

/******************************************************************************
 * Local Variables 
******************************************************************************/

static tHtlContext gHtlContext;
/******************************************************************************
 * Global Variable Declarations
 *****************************************************************************/
#define HTL(var)  (gHtlContext.var)
/******************************************************************************
 * Imported Variables
 *****************************************************************************/
//extern void *hostdbgfile ;

/******************************************************************************
 * Function Definitions
******************************************************************************/
void HTL_Init(void)
{
    Osal_MemSet((void*)&gHtlContext, 0, sizeof(gHtlContext));
}

tBleStatus HTL_RegisterCb(tHtlAciCallback callBack)
{
    tBleStatus retStatus = BLE_STATUS_FAILED;

    if( callBack != NULL)
    {
        HTL(cbHtlAci) = callBack;
        retStatus = BLE_STATUS_SUCCESS;
    }

    return retStatus;
}

void HTL_Enqueue(tHciDataPacket* pPacket)
{
	HTL(pHciRxPkt) = pPacket;
}

/**
 * HTL_Write
 */ 
tBleStatus HTL_Write(uint8 *htlDataPacket,int len)
{
    SPI_Write(len, htlDataPacket);
    return BLE_STATUS_SUCCESS;
}

/**
 * HTL_Read
 * 
 * When there is an interrupt mechanism present, this
 * function has to be called from the spi ISR instead of
 * the HCI_Process_Q
 */ 
   tBleStatus HTL_Read()
   {
     uint32 recvlen = 0;
     uint32 fifo_write_len = 0;
     tBleStatus retStatus = BLE_STATUS_PENDING;
     void *pBuff = NULL;
     uint8 idx = 0;
      uint32_t uwPRIMASK_Bit = __get_PRIMASK();	/**< backup PRIMASK bit */;
     
     //HCI_MESG_DBG(hostdbgfile,"HTL_Read\n");
     
      __disable_irq();			/**< Disable all interrupts by setting PRIMASK bit on Cortex*/
      
     /* check if there is data to be read 
     * we can read data only when ACI has enqueued a packet
     * to HTL to read and also there is data present in 
     * the fifo to read
     */
     if(HTL(pHciRxPkt) != NULL)
     {
       while(retStatus == BLE_STATUS_PENDING)
       {
         switch(HTL(HciPacketReadState))
         {
         case HTL_READ_HCI_PACKET_TYPE:
           {
             if(SPI_Get_Len() >= HCI_PACKET_TYPE_SIZE)
             {
               SPI_Read(HCI_PACKET_TYPE_SIZE,(char*)(&HTL(pHciRxPkt)->hciPktType));
               
               if((HTL(pHciRxPkt)->hciPktType != HCI_PKT_TYPE_COMMAND) &&
                  (HTL(pHciRxPkt)->hciPktType != HCI_PKT_TYPE_EVENT) &&
                    (HTL(pHciRxPkt)->hciPktType != HCI_PKT_TYPE_ACL))
               {
                 /* if the packet type is not a valid one, then we just go
                 * and read the next available byte and also
                 * Send the spi synchronization error to the host
                 * clear the spi buffers(assuming the data cannot be greater than 255 bytes)
                 */ 
                 //spi_read_ms(spi_fifolen_r(),HTL(pHciRxPkt)->pBuffer);
                 HCI_MESG_DBG (hostdbgfile,"HTL sync error\n");
                 break;
               }
               HTL(HciPacketReadState) = HTL_READ_HCI_PACKET_HEADER;
             }
             else
             {
               //HTL_MESG_DBG (hostdbgfile,"[htl]not enough bytes to read header\n");
               retStatus = BLE_STATUS_FAILED;
             }
           }
           break;
         case HTL_READ_HCI_PACKET_HEADER:
           {
             if((HTL(pHciRxPkt)->hciPktType == HCI_PKT_TYPE_COMMAND) &&
                (SPI_Get_Len() >= HCI_CMD_PACKET_HDR_SIZE))
             {
               SPI_Read(HCI_CMD_PACKET_HDR_SIZE,(char*)HTL(pHciRxPkt)->dataBuff); 
               HTL(readLen) = HTL(pHciRxPkt)->dataBuff[CMD_PARAM_TOTAL_LENGTH_OFFSET];
             }
             else if((HTL(pHciRxPkt)->hciPktType == HCI_PKT_TYPE_EVENT) &&
                     (SPI_Get_Len() >= HCI_EVT_PACKET_HDR_SIZE))
             {
               SPI_Read(HCI_EVT_PACKET_HDR_SIZE,(char*)HTL(pHciRxPkt)->dataBuff);	
               HTL(readLen) = (uint8)HTL(pHciRxPkt)->dataBuff[EVT_PARAM_TOTAL_LENGTH_OFFSET];
             }
             else if((HTL(pHciRxPkt)->hciPktType == HCI_PKT_TYPE_ACL) &&
                     (SPI_Get_Len() >= HCI_ACL_PACKET_HDR_SIZE))
             {
               SPI_Read(HCI_ACL_PACKET_HDR_SIZE,(char*)HTL(pHciRxPkt)->dataBuff);	
               HTL(readLen) = (uint8)LE_TO_NRG_16(&HTL(pHciRxPkt)->dataBuff[ACL_DATA_TOTAL_LENGTH_OFFSET]);
               
             }
             else
             {
               HCI_MESG_DBG (hostdbgfile,"[htl]not enough bytes to read header\n");
               retStatus = BLE_STATUS_FAILED;
               break;
             }
             HCI_MESG_DBG(hostdbgfile,"header %x %x %x\n",gHtlContext.pHciRxPkt->dataBuff[0],gHtlContext.pHciRxPkt->dataBuff[1],gHtlContext.pHciRxPkt->dataBuff[2]);
             if(!HTL(readLen))
             {
               /* if the command has no parameters
               * then we have already read the full command
               */
               goto EXIT;
             }
             HTL(HciPacketReadState) = HTL_READ_HCI_PACKET_PARAMS;
           }
           break;
         case HTL_READ_HCI_PACKET_PARAMS:
           {   
             if(HTL(pHciRxPkt)->hciPktType == HCI_PKT_TYPE_COMMAND)
             {
               recvlen = HTL(pHciRxPkt)->dataBuff[CMD_PARAM_TOTAL_LENGTH_OFFSET];
               idx = 3;
             }
             else if(HTL(pHciRxPkt)->hciPktType == HCI_PKT_TYPE_EVENT)
             {
               recvlen = HTL(pHciRxPkt)->dataBuff[EVT_PARAM_TOTAL_LENGTH_OFFSET];
               idx = 2;
             }
             /* we have an ACL packet */
             else
             {
               /* add 2 bytes so that we copy the data from the third byte in the buffer
               * the first 2 bytes will be filled by the link layer header
               */
               recvlen = (LE_TO_NRG_16(&HTL(pHciRxPkt)->dataBuff[ACL_DATA_TOTAL_LENGTH_OFFSET]));
               idx = 4;
             }
             
             /* if the total length to be read is greater than the
             * maximum receive buffer size of spi, then read the
             * maximum possible number of bytes
             */
             fifo_write_len = SPI_Get_Len();
             
             if(HTL(readLen) > fifo_write_len)
             {
               SPI_Read(fifo_write_len,(char*)&(HTL(pHciRxPkt)->dataBuff[idx+recvlen-HTL(readLen)]));
               //HCI_MESG_DATA(hostdbgfile,&(HTL(pHciRxPkt)->dataBuff[idx+recvlen-HTL(readLen)]),fifo_write_len);
               HTL(readLen) -= fifo_write_len;
               retStatus = BLE_STATUS_BUSY;
             }
             else
             {
                SPI_Read(HTL(readLen),(char*)&(HTL(pHciRxPkt)->dataBuff[idx+recvlen-HTL(readLen)]));
EXIT:           HTL(HciPacketReadState) = HTL_READ_HCI_PACKET_TYPE;
                pBuff = (void *) HTL(pHciRxPkt);
                HTL(pHciRxPkt) = NULL;
                HTL(cbHtlAci) (HTL_PACKET_IND, BLE_STATUS_SUCCESS,pBuff);
                retStatus = BLE_STATUS_SUCCESS;
                HTL(readLen) = 0;
             }
           }
           break;
         }
       }
     }
     
     __set_PRIMASK(uwPRIMASK_Bit);	/**< Restore PRIMASK bit*/
     
     /* when we have spi interrupts we have to check for TX_COMPLETE
     * interrupt as well?? even for master
     * no need to chcek for master as master initiates the write always
     */ 
     return retStatus;
   }
   
   /************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
