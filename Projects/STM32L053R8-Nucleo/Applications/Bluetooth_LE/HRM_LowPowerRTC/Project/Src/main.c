/**
******************************************************************************
* @file    main.c 
* @author  AAS / CL
* @version V1.0.0
* @date    04-July-2014
* @brief   This sample code shows how to use the BlueNRG Bluetooth Low Energy 
*          shield to exchange data between two devices.
*          The communication is done using 1 Nucleo boards and one Smartphone
*          with BLE chip and Android >= v4.3.
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

/* Includes ------------------------------------------------------------------*/
#include "bluenrg_interface.h"
#include "hal_types.h"

#include "host_osal.h"
#include "role_type.h"
#include "low_power_conf.h"
#include "debug.h"

#include <stdio.h>

/** @addtogroup BlueNRG_Shield
 *  @{
 */

/** @defgroup BlueNRG_Shield_Sample_Application
 *  @brief Sample application for BlueNRG Shield on STM32 Nucleo boards.
 *  @{
 */

/* Private define ------------------------------------------------------------*/
#define BDADDR_SIZE 6

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* RTC handler declaration */
RTC_HandleTypeDef RTCHandle;

extern volatile uint8_t set_connectable;
extern volatile int connected;
extern volatile uint8_t notification_enabled;

/* Private function prototypes -----------------------------------------------*/
void HRM_Profile_Application(void);

/**
 * @brief   Main function to show how to use the BlueNRG Bluetooth Low Energy 
 *          shield to exchange data between two Nucleo baords with their
 *          respective BlueNRG shields.
 *          One board will act as Server-Peripheral and the other as 
 *          Client-Central.
 *          After connection has been established, by pressing the USER button
 *          on one board, the LD2 LED on the other one gets toggled and
 *          viceversa.
 *          The communication is done using a vendor specific profile.
 * @param  None
 * @retval None
 */
int main(void)
{ 
 
  /* STM32Cube HAL library initialization:
   *  - Configure the Flash prefetch, Flash preread and Buffer caches
   *  - Systick timer is configured by default as source of time base, but user 
   *    can eventually implement his proper time base source (a general purpose 
   *    timer for example or other time source), keeping in mind that Time base 
   *    duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
   *    handled in milliseconds basis.
   *  - Low Level Initialization
   */
  HAL_Init();
  /* Configure the system clock in low power mode @ 32 Mhz */
  SystemClock_Config_LP();
  
  /* Configure the system Power in low power mode */
  SystemPower_Config_LP();
  
  /* Configure the RTC */
  RTC_Config();

  /* Configure LED2 */
  //BSP_LED_Init(LED2); //must be disabled when low power mode is enabled (+80uA when enabled!!!)
  
  /* Configure the User Button in GPIO Mode */
  //BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);
  
  /* Initialize the BlueNRG SPI driver */
  BNRG_SPI_Init();
    
  /* Reset BlueNRG hardware */
  //BlueNRG_RST(); FIXME: moved to HRM_Profile_Application()
  
  //Hal_Init_Timer();

  HRM_Profile_Application();
  
  return 0;
}


#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: PRINTF("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
