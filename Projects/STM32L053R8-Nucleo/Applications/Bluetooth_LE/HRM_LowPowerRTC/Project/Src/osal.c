/**
******************************************************************************
* @file    osal.c 
* @author  AAS
* @version V1.0.0
* @date    04-July-2014
* @brief   This file have implementation of wrapper functions to use the  
*          Host Operating System's functionality.
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
******************************************************************************/
#include <host_config.h>
#include "hal_types.h"
#include "host_osal.h"
#include <string.h>
#include "stm32l0xx_hal.h"

/******************************************************************************
 * Macro Declarations
******************************************************************************/

/******************************************************************************
 * Local Function Prototypes
******************************************************************************/

/******************************************************************************
 * Local Variable Declarations
******************************************************************************/

/******************************************************************************
 * Global Variable Declarations
******************************************************************************/

/******************************************************************************
 * Function Definitions 
******************************************************************************/

void Osal_MemCpy(void* copyTo,const void* copyFrom, uint numBytes)
{
	memcpy (copyTo, copyFrom, numBytes);
}


int Osal_MemCmp ( void * src1, const void * src2, uint numBytes )
{
	return(memcmp( src1, src2, numBytes ));
}

void Osal_MemSet(void* pMemAddr,int value,uint count)
{
	memset (pMemAddr, value, count);
}

#if 0
ulint Osal_Get_Cur_Time(void)
{
    return Hal_Get_Timer_Value();
} 
#endif

void DisableInterrupts(void)
{
  __disable_irq();
}

void EnableInterrupts(void)
{
  __enable_irq();
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
