/**
******************************************************************************
* @file    hrm_profile_application.h 
* @author  AAS / CL
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
#ifndef __HRM_PROFILE_APPLICATION_H
#define __HRM_PROFILE_APPLICATION_H

/*******************************************************************************
 * Macros
*******************************************************************************/

/* application states */
#define APPL_UNINITIALIZED				(0x00)
#define APPL_INIT_DONE					(0x01)
#define APPL_ADVERTISING				(0x02)
#define APPL_CONNECTED					(0x03)
#define APPL_READ					(0x04)
#define APPL_WAIT					(0x05)
#define APPL_START_TIMER				(0x06)
#define APPL_UPDATE					(0x07)
#define APPL_SET_TX_PWR					(0x08)

#define APPL_TERMINATE_LINK                  (0x09)
#define APPL_SET_BODY_SENSOR_LOCATION        (0x0a)

//Macro for advertising the Profiles
#define APPL_START_DISCOVERY		  (52)


#endif // __HRM_PROFILE_APPLICATION_H

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
