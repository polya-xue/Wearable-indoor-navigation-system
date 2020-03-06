/**
  ******************************************************************************
  * @file    stm32l0xx_nucleo_bluenrg.c
  * @author  CL
  * @version V1.0.0
  * @date    04-July-2014
  * @brief   This file contains inplementations for:
  *          - SPI communication on STM32L0XX-Nucleo Kit from STMicroelectronics
  *            for BLE BlueNRG shield (reference X-NUCLEO-IDB04A1)
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

//[SO] FIXME!!! To be removed and refactored to the shield BSP
//#include "hci.h"

/* Exported variables ---------------------------------------------------------*/
    
/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup STM32L0XX_NUCLEO
  * @{
  */   
    
/** @addtogroup STM32L0XX_NUCLEO_LOW_LEVEL 
  * @brief This file contains definitions for SPI communication on
  *        STM32L0XX-Nucleo Kit from STMicroelectronics for
  *        BLE BlueNRG shield (reference X-NUCLEO-IDB04A1)
  * @{
  */ 

/** @defgroup STM32L0XX_NUCLEO_LOW_LEVEL_Private_TypesDefinitions 
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32L0XX_NUCLEO_LOW_LEVEL_Private_Defines 
  * @{
  */ 

/**
  * @brief STM32L0XX NUCLEO BSP Driver version number V1.0.0
  */

/**
  * @brief LINK SD Card
  */

/**
  * @}
  */ 

/** @defgroup STM32L0XX_NUCLEO_LOW_LEVEL_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32L0XX_NUCLEO_LOW_LEVEL_Private_Variables
  * @{
  */
/**
  * @}
  */ 

/** @defgroup STM32L0XX_NUCLEO_LOW_LEVEL_Private_FunctionPrototypes
  * @{
  */
/**
  * @}
  */ 

/** @defgroup STM32L0XX_NUCLEO_LOW_LEVEL_Private_Functions
  * @{
  */ 

/******************************************************************************
                            BUS OPERATIONS
*******************************************************************************/
/**
 * @brief  This function is used for low level initialization of the SPI 
 *         communication with the BlueNRG Shield.
 * @param  SPI_HandleTypeDef* hspi Handle of the STM32Cube HAL SPI interface
 * @retval None
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(hspi->Instance==BNRG_SPI_INSTANCE)
  {
    /* Enable peripherals clock */
    
    /* Enable GPIO Ports Clock */  
    BNRG_SPI_RESET_CLK_ENABLE();
    BNRG_SPI_SCLK_CLK_ENABLE();
    BNRG_SPI_MISO_CLK_ENABLE();
    BNRG_SPI_MOSI_CLK_ENABLE();
    BNRG_SPI_CS_CLK_ENABLE();
    BNRG_SPI_IRQ_CLK_ENABLE();
    
    /* Enable SPI clock */
    BNRG_SPI_CLK_ENABLE();
    
    /* Reset */
    GPIO_InitStruct.Pin = BNRG_SPI_RESET_PIN;
    GPIO_InitStruct.Mode = BNRG_SPI_RESET_MODE;
    GPIO_InitStruct.Pull = BNRG_SPI_RESET_PULL;
    GPIO_InitStruct.Speed = BNRG_SPI_RESET_SPEED;
    GPIO_InitStruct.Alternate = BNRG_SPI_RESET_ALTERNATE;
    HAL_GPIO_Init(BNRG_SPI_RESET_PORT, &GPIO_InitStruct);	
    HAL_GPIO_WritePin(BNRG_SPI_RESET_PORT, BNRG_SPI_RESET_PIN, GPIO_PIN_RESET);	/*Added to avoid spurious interrupt from the BlueNRG */
    
    /* SCLK */
    GPIO_InitStruct.Pin = BNRG_SPI_SCLK_PIN;
    GPIO_InitStruct.Mode = BNRG_SPI_SCLK_MODE;
    GPIO_InitStruct.Pull = BNRG_SPI_SCLK_PULL;
    GPIO_InitStruct.Speed = BNRG_SPI_SCLK_SPEED;
    GPIO_InitStruct.Alternate = BNRG_SPI_SCLK_ALTERNATE;
    HAL_GPIO_Init(BNRG_SPI_SCLK_PORT, &GPIO_InitStruct); 
    
    /* MISO */
    GPIO_InitStruct.Pin = BNRG_SPI_MISO_PIN;
    GPIO_InitStruct.Mode = BNRG_SPI_MISO_MODE;
    GPIO_InitStruct.Pull = BNRG_SPI_MISO_PULL;
    GPIO_InitStruct.Speed = BNRG_SPI_MISO_SPEED;
    GPIO_InitStruct.Alternate = BNRG_SPI_MISO_ALTERNATE;
    HAL_GPIO_Init(BNRG_SPI_MISO_PORT, &GPIO_InitStruct);
    
    /* MOSI */
    GPIO_InitStruct.Pin = BNRG_SPI_MOSI_PIN;
    GPIO_InitStruct.Mode = BNRG_SPI_MOSI_MODE;
    GPIO_InitStruct.Pull = BNRG_SPI_MOSI_PULL;
    GPIO_InitStruct.Speed = BNRG_SPI_MOSI_SPEED;
    GPIO_InitStruct.Alternate = BNRG_SPI_MOSI_ALTERNATE;
    HAL_GPIO_Init(BNRG_SPI_MOSI_PORT, &GPIO_InitStruct);
    
    /* NSS/CSN/CS */
    GPIO_InitStruct.Pin = BNRG_SPI_CS_PIN;
    GPIO_InitStruct.Mode = BNRG_SPI_CS_MODE;
    GPIO_InitStruct.Pull = BNRG_SPI_CS_PULL;
    GPIO_InitStruct.Speed = BNRG_SPI_CS_SPEED;
    GPIO_InitStruct.Alternate = BNRG_SPI_CS_ALTERNATE;
    HAL_GPIO_Init(BNRG_SPI_CS_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(BNRG_SPI_CS_PORT, BNRG_SPI_CS_PIN, GPIO_PIN_SET);
    
    /* IRQ -- INPUT */
    GPIO_InitStruct.Pin = BNRG_SPI_IRQ_PIN;
    GPIO_InitStruct.Mode = BNRG_SPI_IRQ_MODE;
    GPIO_InitStruct.Pull = BNRG_SPI_IRQ_PULL;
    GPIO_InitStruct.Speed = BNRG_SPI_IRQ_SPEED;
    GPIO_InitStruct.Alternate = BNRG_SPI_IRQ_ALTERNATE;
    HAL_GPIO_Init(BNRG_SPI_IRQ_PORT, &GPIO_InitStruct);
    
    /* Configure the NVIC for SPI */  
    HAL_NVIC_SetPriority(BNRG_SPI_EXTI_IRQn, 4, 0);    
    HAL_NVIC_EnableIRQ(BNRG_SPI_EXTI_IRQn);
  }
}



/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */    

/**
  * @}
  */ 
    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
