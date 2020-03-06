/**
******************************************************************************
* @file    main.c 
* @author  CL
* @version V1.0.0
* @date    2016.5.10
* @brief   This application contains an example which shows how implementing
*          a proprietary Bluetooth Low Energy profile: the sensor profile.
*          The communication is done using a Nucleo board and a Smartphone
*          with BTLE.
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

#include "stm32_bluenrg_ble.h"
#include "osal.h"
#include "sensor_service.h"
#include "role_type.h"
#include "debug.h"
#include "MPU6050.h"
#include "i2c.h"
#include "gpio.h"
#include "usart1.h"
#include "usart2.h"
#include "stdio.h"
#include "Ultra.h"
/** @addtogroup BlueNRG_Shield
 *  @{
 */

/** @defgroup BlueNRG_Shield_Sensor_Demo
 *  @brief Sample application for BlueNRG Shield on STM32 Nucleo boards.
 *  @{
 */

/* Private define ------------------------------------------------------------*/
   
/* Private macro -------------------------------------------------------------*/
#define BDADDR_SIZE 6

/* Private variables ---------------------------------------------------------*/

/* SPI handler declaration */
//SPI_HandleTypeDef SpiHandle;

extern volatile uint8_t set_connectable;
extern volatile int connected;
extern AxesRaw_t axes_data;
/* Private function prototypes -----------------------------------------------*/
void User_Process(AxesRaw_t* p_axes);
void SystemClock_Config(void);

/*****************My definition********************/

extern uint16_t AccelX_Vlue,AccelY_Vlue,AccelZ_Vlue;
extern uint16_t GyroX_Vlue,GyroY_Vlue,GyroZ_Vlue;
extern uint16_t Ultra_Dis1,Ultra_Dis2,Ultra_Dis3;
extern uint8_t Ultra_Flag;
uint8_t State;//用来记录盲人执行查询动作 

/**************************************************/

/**
 * @brief  Main function to show how to use the BlueNRG Bluetooth Low Energy 
 *         shield to send data from a Nucleo baord to a smartphone with
 *         the support BLE and the "BlueNRG" app freely available on both
 *         GooglePlay and iTunes.
 *         The board will act as Server-Peripheral.
 *         After connection has been established:
 *          - by pressing the USER button on the board, the cube showed by
 *            the app on the smartphone will rotate in one direction;
 *          - by moving the USER joystick on the board, the cube showed by
 *            the app on the smartphone will rotate accordingly (FEATURE NOT
 *            AVAILABLE YET).
 *          
 *         The communication is done using a vendor specific profile.
 * @param  None
 * @retval None
 */
int main(void)
{
/*define variables*/
	uint8_t t;
	uint8_t len;	
	const char *name = "BlueNRG";
  tHalUint8 SERVER_BDADDR[] = {0x12, 0x34, 0x00, 0xE1, 0x80, 0x02};
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
  
  /* The Nucleo board must be configured as SERVER */
  Osal_MemCpy(bdaddr, SERVER_BDADDR, sizeof(SERVER_BDADDR));
  
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
  
  ret = aci_gap_init(GAP_PERIPHERAL_ROLE, &service_handle, 
                     &dev_name_char_handle, &appearance_char_handle);
  if(ret != BLE_STATUS_SUCCESS){
    PRINTF("GAP_Init failed.\n");
  }

  ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0,
                                   strlen(name), (tHalUint8 *)name);

  if(ret){
    PRINTF("aci_gatt_update_char_value failed.\n");            
    while(1);
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
  
  PRINTF("SERVER: BLE Stack Initialized\n");
  
  ret = Add_Acc_Service();
  
  if(ret == BLE_STATUS_SUCCESS)
    PRINTF("Acc service added successfully.\n");
  else
    PRINTF("Error while adding Acc service.\n");
  
  ret = Add_Environmental_Sensor_Service();
  
  if(ret == BLE_STATUS_SUCCESS)
    PRINTF("Environmental Sensor service added successfully.\n");
  else
    PRINTF("Error while adding Environmental Sensor service.\n");

  /* Set output power level */
  ret = aci_hal_set_tx_power_level(1,4);
  
/* Initialize all configured peripherals */
  MX_GPIO_Init();	
	
	Ultra_GPIO_Init();
	MX_TIM6_Init();
	Set_Echo1_Rising_Interrupt();
	Set_Echo2_Rising_Interrupt();
	Set_Echo3_Rising_Interrupt();
	MX_USART2_UART_Init();
	
  MX_I2C1_Init();
  InitMPU6050(); 
/* Infinite loop */  
  while(1)
  {
	Trig1_Enable();
	HAL_Delay(20);//必要的延时	
	Trig2_Enable();
	HAL_Delay(20);
	Trig3_Enable();
	HAL_Delay(20);
	if(Ultra_Dis1<800)
			Ultra_Flag=1;
	else if(Ultra_Dis2<800)
		  Ultra_Flag=2;
	else if(Ultra_Dis3<800)
		  Ultra_Flag=3;
	else
		  Ultra_Flag=0;	
	AccelData_Process();//加速度处理
	GYROData_Process();//角速度处理
	Gesture_Judge();//手势判断
	HCI_Process();//蓝牙处理函数
	User_Process(&axes_data);//蓝牙处理函数  
  }
}
/**
 * @brief  Process user input (i.e. pressing the USER button on Nucleo board)
 *         and send a LED toggle command to the remote board.
 * @param  AxesRaw_t* p_axes
 * @retval None
 */
void User_Process(AxesRaw_t* p_axes)
{
  if(set_connectable){
    setConnectable();
    set_connectable = FALSE;
  }  
  
  /* Check if the user has pushed the button */
  if(BSP_PB_GetState(BUTTON_KEY) == RESET)
  {
    while (BSP_PB_GetState(BUTTON_KEY) == RESET);
    
    BSP_LED_Toggle(LED2); // FIXME: remove this
    
    if(connected){
      /* Update acceleration data */
      p_axes->AXIS_X += 200;
      //PRINTF("ACC: X=%6d Y=%6d Z=%6d\r\n", p_axes->AXIS_X, p_axes->AXIS_Y, p_axes->AXIS_Z);
      Acc_Update(p_axes);
    }
  }
}

/**
 * @brief  SystemClock_Config System Clock Configuration
 * @param  None
 * @retval None
 * @function BluNRG System Clock
 */
//void SystemClock_Config(void)
//{
//  RCC_ClkInitTypeDef RCC_ClkInitStruct;
//  RCC_OscInitTypeDef RCC_OscInitStruct;
//  RCC_PeriphCLKInitTypeDef PeriphClkInit;

//  __PWR_CLK_ENABLE();

//  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

//  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
//  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
//  RCC_OscInitStruct.HSICalibrationValue = 0;
//  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
//  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
//  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
//  HAL_RCC_OscConfig(&RCC_OscInitStruct);

//  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
//  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
//  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
//  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

//	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
//  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
//  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

//  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

//  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

//  /* SysTick_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
//  __SYSCFG_CLK_ENABLE();  
//}

/*Ultra Clock*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_6;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_3;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
