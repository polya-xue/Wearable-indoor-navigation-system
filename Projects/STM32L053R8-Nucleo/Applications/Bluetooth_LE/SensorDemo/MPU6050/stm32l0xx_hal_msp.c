///**
//  ******************************************************************************
//  * File Name          : stm32l0xx_hal_msp.c
//  * Date               : 05/04/2016 12:26:44
//  * Description        : This file provides code for the MSP Initialization 
//  *                      and de-Initialization codes.
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
//#include "usart1.h"
//#include "usart2.h"
///* USER CODE BEGIN 0 */

///* USER CODE END 0 */

///**
//  * Initializes the Global MSP.
//  */
//void HAL_MspInit(void)
//{
//  /* USER CODE BEGIN MspInit 0 */

//  /* USER CODE END MspInit 0 */

//  /* System interrupt init*/
///* SysTick_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

//  /* USER CODE BEGIN MspInit 1 */

//  /* USER CODE END MspInit 1 */
//}

////void HAL_UART_MspInit(UART_HandleTypeDef *huart)
////{  
////  GPIO_InitTypeDef  GPIO_InitStruct;
////  
////  /*##-1- Enable peripherals and GPIO Clocks #################################*/
//// 
////  /* Enable GPIO TX/RX clock */
////  USARTx_TX_GPIO_CLK_ENABLE();
////  USARTx_RX_GPIO_CLK_ENABLE();
////  
////  /* Enable USART2 clock */
////  USARTx_CLK_ENABLE(); 
////  
////  /*##-2- Configure peripheral GPIO ##########################################*/  
//// 
////  /* UART TX GPIO pin configuration  */
////  GPIO_InitStruct.Pin       = USARTx_TX_PIN;
////  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
////  GPIO_InitStruct.Pull      = GPIO_NOPULL;
////  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
////  GPIO_InitStruct.Alternate = USARTx_TX_AF;
////  
////  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);
////    
////  /* UART RX GPIO pin configuration  */
////  GPIO_InitStruct.Pin = USARTx_RX_PIN;
////  GPIO_InitStruct.Alternate = USARTx_RX_AF;
////    
////  HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);
////}

///**
//  * @brief UART MSP De-Initialization 
//  *        This function frees the hardware resources used in this example:
//  *          - Disable the Peripheral's clock
//  *          - Revert GPIO configuration to their default state
//  * @param huart: UART handle pointer
//  * @retval None
//  */
////void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
////{
////  /*##-1- Reset peripherals ##################################################*/
////  USARTx_FORCE_RESET();
////  USARTx_RELEASE_RESET();

////  /*##-2- Disable peripherals and GPIO Clocks #################################*/
////  /* Configure UART Tx as alternate function  */
////  HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
////  /* Configure UART Rx as alternate function  */
////  HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);
////}

///**
//  * @}
//  */

///**
//  * @}
//  */

///**
//  * @}
//  */

///* USER CODE BEGIN 1 */

///* USER CODE END 1 */

///**
//  * @}
//  */

///**
//  * @}
//  */

///************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/



/**
  ******************************************************************************
  * File Name          : stm32l0xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
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
#include "stm32l0xx_hal.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __SYSCFG_CLK_ENABLE();

  /* System interrupt init*/
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{

  if(htim_base->Instance==TIM6)
  {
  /* USER CODE BEGIN TIM6_MspInit 0 */

  /* USER CODE END TIM6_MspInit 0 */
    /* Peripheral clock enable */
    __TIM6_CLK_ENABLE();
  /* Peripheral interrupt init*/
    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
  /* USER CODE BEGIN TIM6_MspInit 1 */

  /* USER CODE END TIM6_MspInit 1 */
  }

}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{

  if(htim_base->Instance==TIM6)
  {
  /* USER CODE BEGIN TIM6_MspDeInit 0 */

  /* USER CODE END TIM6_MspDeInit 0 */
    /* Peripheral clock disable */
    __TIM6_CLK_DISABLE();

    /* Peripheral interrupt DeInit*/
    HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);

  }
  /* USER CODE BEGIN TIM6_MspDeInit 1 */

  /* USER CODE END TIM6_MspDeInit 1 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* Peripheral clock enable */
    __USART2_CLK_ENABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }

}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

  }
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
