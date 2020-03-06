/**
  ******************************************************************************
  * @file    htl.h 
  * @author  AAS
  * @version V1.0.0
  * @date    04-July-2014
  * @brief   This file have Host HCI Transport layer definitations.
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
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

void HTL_Init(void);

tBleStatus HTL_Write(uint8 *htlDataPacket,int len);

/* 
 * this function reads received data from spi interface 
 */
tBleStatus HTL_Read(void);

tBleStatus HTL_RegisterCb(tHtlAciCallback callBack);

void HTL_Enqueue(tHciDataPacket* pPacket);

#endif /* _HHTL_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

