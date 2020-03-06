/**
******************************************************************************
* @file    main.c 
* @author  CL
* @version V1.0.0
* @date    04-July-2014
* @brief   This sample code shows how to use the BlueNRG Bluetooth Low Energy 
*          shield to exchange data between two devices.
*          The communication is done using 2 Nucleo boards.
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
#include "stm32l0xx_nucleo_bluenrg.h"

#include "osal.h"
#include "sample_service.h"
#include "role_type.h"
#include "debug.h"
#include "bluenrg_interface.h"
#include "extra_led.h"

#include <stdio.h>

/** @addtogroup BlueNRG_Shield
 *  @{
 */

/** @defgroup BlueNRG_Shield_Sample_Application
 *  @brief Sample application for BlueNRG Shield on STM32 Nucleo boards.
 *  @{
 */

/* Private defines ------------------------------------------------------------*/
/* Private macros -------------------------------------------------------------*/
#define BDADDR_SIZE 6

/* Private variables ---------------------------------------------------------*/
/* Uncomment the line corresponding to the role you want to have */
BLE_RoleTypeDef BLE_Role = SERVER;
//BLE_RoleTypeDef BLE_Role = CLIENT;

extern volatile uint8_t set_connectable;
extern volatile int connected;
extern volatile uint8_t notification_enabled;

/* Private function prototypes -----------------------------------------------*/
void User_Process(void);
static void SystemClock_Config(void);

/**
 * @brief  Main function to show how to use the BlueNRG Bluetooth Low Energy 
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
  tHalUint8 CLIENT_BDADDR[] = {0xbb, 0x00, 0x00, 0xE1, 0x80, 0x02};
  tHalUint8 SERVER_BDADDR[] = {0xaa, 0x00, 0x00, 0xE1, 0x80, 0x02};
  //tHalUint8 SERVER_BDADDR[] = {0xfd, 0x00, 0x25, 0xec, 0x02, 0x04}; //[SO] BT address for HRM test
  tHalUint8 bdaddr[BDADDR_SIZE];
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
  int ret;
  
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
  
  Extra_LED_Init();
  
  /* Configure LED2 */
  BSP_LED_Init(LED2);
  
  /* Configure the User Button in GPIO Mode */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
  
  /* Configure the system clock */
  SystemClock_Config();
  
  /* Initialize the BlueNRG SPI driver */
  BNRG_SPI_Init();

  /* Initialize the BlueNRG HCI */
  HCI_Init();

  /* Reset BlueNRG hardware */
  BlueNRG_RST();
  
  if(BLE_Role == CLIENT) {
    Osal_MemCpy(bdaddr, CLIENT_BDADDR, sizeof(CLIENT_BDADDR));
  } else {
    Osal_MemCpy(bdaddr, SERVER_BDADDR, sizeof(SERVER_BDADDR));
  }
  
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
                                  CONFIG_DATA_PUBADDR_LEN,
                                  bdaddr);
  if(ret){
    PRINTF("Setting BD_ADDR failed.\n");
  }
  
  ret = aci_gatt_init();    
  if(ret){
    PRINTF("GATT_Init failed.\n");
  }
  
  if(BLE_Role == SERVER) {
    ret = aci_gap_init(GAP_PERIPHERAL_ROLE, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  } else {
    ret = aci_gap_init(GAP_CENTRAL_ROLE, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  }
  
  if(ret != BLE_STATUS_SUCCESS){
    PRINTF("GAP_Init failed.\n");
  }
    
  ret = aci_gap_set_auth_requirement(MITM_PROTECTION_REQUIRED,
                                     OOB_AUTH_DATA_ABSENT,
                                     NULL,
                                     7,
                                     16,
                                     USE_FIXED_PIN_FOR_PAIRING,
                                     123456,
                                     BONDING);
  if (ret == BLE_STATUS_SUCCESS) {
    PRINTF("BLE Stack Initialized.\n");
  }
  
  if(BLE_Role == SERVER) {
    PRINTF("SERVER: BLE Stack Initialized\n");
    ret = Add_Sample_Service();
    
    if(ret == BLE_STATUS_SUCCESS)
      PRINTF("Service added successfully.\n");
    else
      PRINTF("Error while adding service.\n");
    
  } else {
    PRINTF("CLIENT: BLE Stack Initialized\n");
  }
  
  /* Set output power level */
  ret = aci_hal_set_tx_power_level(1,4);

  while(1)
  {
    HCI_Process();
    User_Process();
  }
}

/**
 * @brief  Process user input (i.e. pressing the USER button on Nucleo board)
 *         and send a LED toggle command to the remote board.
 * @param  None
 * @retval None
 */
void User_Process(void)
{
  if(set_connectable){
    /* Establish connection with remote device */
    Make_Connection();
    set_connectable = FALSE;
  }
  
  if(BLE_Role == CLIENT) {
    if(connected && !notification_enabled){
      enableNotification();
    }
  }  

  /* Check if the user has pushed the button */
  if(BSP_PB_GetState(BUTTON_KEY) == RESET)
  {
    while (BSP_PB_GetState(BUTTON_KEY) == RESET);
    
    if(connected && notification_enabled){
      /* Send a toggle command to the remote device */
      tHalUint8 data[20] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','I','J'};
      sendData(data, sizeof(data));
      
      //BSP_LED_Toggle(LED2);  // [RG:] toggle the LED2 locally.
                               // If uncommented be sure BSP_LED_Init(LED2) is
                               // called in main().
                               // E.g. it can be enabled for debugging.
    }
  }
}

/**
 * @brief  SystemClock_Config System Clock Configuration
 * @param  None
 * @retval None
 */
  void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 0;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  __SYSCFG_CLK_ENABLE();  
}

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
