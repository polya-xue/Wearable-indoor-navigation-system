/**
  ******************************************************************************
  * @file    sensor_service.h 
  * @author  CL
  * @version V1.0.0
  * @date    04-July-2014
  * @brief   
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
#ifndef _SENSOR_SERVICE_H_
#define _SENSOR_SERVICE_H_

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "cube_hal.h"
#include "hal_types.h"
#include "gatt_server.h"
#include "gap.h"
#include "string.h"
#include "bluenrg_hci.h"
#include "hci_internal.h"
#include "gp_timer.h"
#include "bluenrg_hci_internal.h"
#include "hci.h"
#include "hal.h"
#include "sm.h"
#include "debug.h"

#include "role_type.h"

#include <stdlib.h>

typedef enum {
  MEMS_SUCCESS = 0x01,
  MEMS_ERROR   = 0x00	
} status_t;

typedef short int i16_t;
typedef struct {
  i16_t AXIS_X;
  i16_t AXIS_Y;
  i16_t AXIS_Z;
} AxesRaw_t;

/** 
* @brief  Handle of TX Characteristic on the Server. The handle should be
*         discovered, but it is fixed only for this demo.
*/
tBleStatus Add_Acc_Service(void);
tBleStatus Acc_Update(AxesRaw_t *data);
tBleStatus Add_Environmental_Sensor_Service(void);
void       setConnectable(void);
void       enableNotification(void);
void       GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle);
void       GAP_DisconnectionComplete_CB(void);
void       HCI_Event_CB(void *pckt);

tBleStatus Add_Time_Service(void);
tBleStatus Seconds_Update(void);
tBleStatus Minutes_Notify(void);
void       Update_Time_Characteristics(void);

tBleStatus Add_LED_Service(void);
void       Attribute_Modified_CB(tHalUint16 handle, tHalUint8 data_length,
                                 tHalUint8 *att_data);

#ifdef __cplusplus
}
#endif

#endif /* _SENSOR_SERVICE_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

