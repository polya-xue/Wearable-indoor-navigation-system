/**
  ******************************************************************************
  * @file    stm32l0xx_it.c 
  * @author  CL
  * @version V1.0.0
  * @date    04-July-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
#include "stm32l0xx_it.h" 
#include "stm32l0xx_hal.h"
#include "stm32l0xx.h"
#include "stm32l0xx_nucleo_bluenrg.h"
#include "debug.h"

extern UART_HandleTypeDef huart2;
/** @addtogroup STM32L0xx_HAL_Examples
  * @{
  */

/** @addtogroup SPI_FullDuplex_ComIT
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint8_t button_event = 0;

/* SPI handler declared in "main.c" file */
extern SPI_HandleTypeDef SpiHandle;

//extern UART_HandleTypeDef huart2;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0+ Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  NMI_Handler This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  HardFault_Handler This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  SVC_Handler This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  DebugMon_Handler This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  PendSV_Handler This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  SysTick_Handler This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}


/******************************************************************************/
/*                 STM32L0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l0xx.s).                                               */
/******************************************************************************/

/**
  * @brief  BNRG_SPI_EXTI_IRQHandler This function handles External line
  *         interrupt request for BlueNRG.
  * @param  None
  * @retval None
  */
void BNRG_SPI_EXTI_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(BNRG_SPI_EXTI_PIN);
}
//My Code
//void EXTI0_1_IRQHandler(void)
//{
//  /* USER CODE BEGIN EXTI0_1_IRQn 0 */

//  /* USER CODE END EXTI0_1_IRQn 0 */
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
//  /* USER CODE BEGIN EXTI0_1_IRQn 1 */

//  /* USER CODE END EXTI0_1_IRQn 1 */
//}
//My Code
extern TIM_HandleTypeDef htim6;
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */

  /* USER CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}
/**
  * @brief  EXTI4_15_IRQHandler This function handles External lines 4 to 15 interrupt request.
  * @param  None
  * @retval None
  */
// FIXME: replace with a macro to make it independent of STM32 family
void EXTI4_15_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(KEY_BUTTON_EXTI_LINE);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);	
  button_event = 1;
}

/******************************************************************************/
/*                 STM32L0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l0xx.s).                                               */
/******************************************************************************/

/**
  * @brief  SPIx_IRQHandler This function handles SPI interrupt request.  
  * @param  None
  * @retval None  
  */
// FIXME: check if we can remove this as it probably is useless
void SPIx_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&SpiHandle);
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/**
  * @}
  */ 

/**
  * @}
  */





/////************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

///**
//  ******************************************************************************
//  * @file    stm32l0xx_it.c
//  * @brief   Interrupt Service Routines.
//  ******************************************************************************
//  *
//  * COPYRIGHT(c) 2016 STMicroelectronics
//  *
//  * Redistribution and use in source and binary forms, with or without modification,
//  * are permitted provided that the following conditions are met:
//  *   1. Redistributions of source code must retain the above copyright notice,
//  *      this list of conditions and the following disclaimer.
//  *   2. Redistributions in binary form must reproduce the above copyright notice,
//  *      this list of conditions and the following disclaimer in the documentation
//  *      and/or other materials provided with the distribution.
//  *   3. Neither the name of STMicroelectronics nor the names of its contributors
//  *      may be used to endorse or promote products derived from this software
//  *      without specific prior written permission.
//  *
//  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  *
//  ******************************************************************************
//  */
///* Includes ------------------------------------------------------------------*/
//#include "stm32l0xx_hal.h"
//#include "stm32l0xx.h"
//#include "stm32l0xx_it.h"

///* USER CODE BEGIN 0 */

///* USER CODE END 0 */

///* External variables --------------------------------------------------------*/
//extern TIM_HandleTypeDef htim6;

///******************************************************************************/
///*            Cortex-M0+ Processor Interruption and Exception Handlers         */ 
///******************************************************************************/

///**
//* @brief This function handles System tick timer.
//*/
//void SysTick_Handler(void)
//{
//  /* USER CODE BEGIN SysTick_IRQn 0 */

//  /* USER CODE END SysTick_IRQn 0 */
//  HAL_IncTick();
//  HAL_SYSTICK_IRQHandler();
//  /* USER CODE BEGIN SysTick_IRQn 1 */

//  /* USER CODE END SysTick_IRQn 1 */
//}

///******************************************************************************/
///* STM32L0xx Peripheral Interrupt Handlers                                    */
///* Add here the Interrupt Handlers for the used peripherals.                  */
///* For the available peripheral interrupt handler names,                      */
///* please refer to the startup file (startup_stm32l0xx.s).                    */
///******************************************************************************/

///**
//* @brief This function handles EXTI line 0 and line 1 interrupts.
//*/
////void EXTI0_1_IRQHandler(void)
////{
////  /* USER CODE BEGIN EXTI0_1_IRQn 0 */

////  /* USER CODE END EXTI0_1_IRQn 0 */
////  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
////  /* USER CODE BEGIN EXTI0_1_IRQn 1 */

////  /* USER CODE END EXTI0_1_IRQn 1 */
////}


///**
//* @brief This function handles EXTI line 4 and line 15 interrupts.
//*/
//void EXTI4_15_IRQHandler(void)
//{
//  /* USER CODE BEGIN EXTI0_1_IRQn 0 */

//  /* USER CODE END EXTI0_1_IRQn 0 */
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
//  /* USER CODE BEGIN EXTI0_1_IRQn 1 */

//  /* USER CODE END EXTI0_1_IRQn 1 */
//}

///**
//* @brief This function handles TIM6 global interrupt and DAC1/DAC2 underrun error interrupts.
//*/
//void TIM6_DAC_IRQHandler(void)
//{
//  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */

//  /* USER CODE END TIM6_DAC_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim6);
//  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

//  /* USER CODE END TIM6_DAC_IRQn 1 */
//}

///* USER CODE BEGIN 1 */

///* USER CODE END 1 */
///************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

