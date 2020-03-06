/**
  ******************************************************************************
  * @file    I2C/I2C_TwoBoards_ComDMA/Src/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    18-June-2014
  * @brief   This sample code shows how to use STM32L0xx I2C HAL API to transmit 
  *          and receive a data buffer with a communication process based on
  *          DMA transfer. 
  *          The communication is done using 2 Boards.
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
#include "main.h"

/** @addtogroup STM32L0xx_HAL_Examples
  * @{
  */

/** @addtogroup I2C_TwoBoards_ComDMA
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Uncomment this line to use the board as master, if not it is used as slave */
#define MASTER_BOARD
#define I2C_ADDRESS        0x30F

/* Timing samples with SYSCLK 32MHz set in SystemClock_Config() */ 
#define I2C_TIMING_100KHZ       0x10A13E56 /* Analog Filter ON, Rise Time 400ns, Fall Time 100ns */ 
#define I2C_TIMING_400KHZ       0x00B1112E /* Analog Filter ON, Rise Time 250ns, Fall Time 100ns */     
/* Private variables ---------------------------------------------------------*/
/* I2C handler declaration */
I2C_HandleTypeDef I2CxHandle;

/* Buffer used for transmission */
uint8_t aTxBuffer[] = " ****I2C_TwoBoards communication based on DMA****  ****I2C_TwoBoards communication based on DMA****  ****I2C_TwoBoards communication based on DMA**** ";

/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{    
  /* STM32L0xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();
  
  /* Configure LED2 */
  BSP_LED_Init(LED2);

  /* Configure the system clock to 32 Mhz */
  SystemClock_Config();

  /*##-1- Configure the I2C peripheral #######################################*/
  I2CxHandle.Instance              = I2Cx;
  I2CxHandle.Init.AddressingMode   = I2C_ADDRESSINGMODE_10BIT;
  I2CxHandle.Init.Timing           = I2C_TIMING_400KHZ;
  I2CxHandle.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLED;
  I2CxHandle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  I2CxHandle.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLED;
  I2CxHandle.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLED;
  I2CxHandle.Init.OwnAddress1      = I2C_ADDRESS;
  I2CxHandle.Init.OwnAddress2      = 0xFE;
  if(HAL_I2C_Init(&I2CxHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();    
  }
  
#ifdef MASTER_BOARD
  
  /* Configure User Button */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
  
  /* Wait for User Button press before starting the Communication */
  while (BSP_PB_GetState(BUTTON_KEY) != 1)
  {
  }
  
  /* Wait for User Button release before starting the Communication */
  while (BSP_PB_GetState(BUTTON_KEY) != 0)
  {
  }
  
  /* The board sends the message and expects to receive it back */
  
  /*##-2- Start the transmission process #####################################*/  
  /* While the I2C in reception process, user can transmit data through 
     "aTxBuffer" buffer */
  while(HAL_I2C_Master_Transmit_DMA(&I2CxHandle, (uint16_t)I2C_ADDRESS, (uint8_t*)aTxBuffer, TXBUFFERSIZE)!= HAL_OK)
  {
    /* Error_Handler() function is called when Timout error occurs.
       When Acknowledge failure ocucurs (Slave don't acknowledge it's address)
       Master restarts communication */
    if (HAL_I2C_GetError(&I2CxHandle) != HAL_I2C_ERROR_AF)
    {
      Error_Handler();
    }
  }
  
  /*##-3- Wait for the end of the transfer ###################################*/  
  /*  Before starting a new communication transfer, you need to check the current   
      state of the peripheral; if it’s busy you need to wait for the end of current
      transfer before starting a new one.
      For simplicity reasons, this example is just waiting till the end of the 
      transfer, but application may perform other tasks while transfer operation
      is ongoing. */  
  while (HAL_I2C_GetState(&I2CxHandle) != HAL_I2C_STATE_READY)
  {
  } 
  
  /* Wait for User Button press before starting the Communication */
  while (BSP_PB_GetState(BUTTON_KEY) != 1)
  {
  }

  /* Wait for User Button release before starting the Communication */
  while (BSP_PB_GetState(BUTTON_KEY) != 0)
  {
  }
  
  /*##-4- Put I2C peripheral in reception process ###########################*/  
  while(HAL_I2C_Master_Receive_DMA(&I2CxHandle, (uint16_t)I2C_ADDRESS, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
  {
    /* Error_Handler() function is called when Timout error occurs.
       When Acknowledge failure ocucurs (Slave don't acknowledge it's address)
       Master restarts communication */
    if (HAL_I2C_GetError(&I2CxHandle) != HAL_I2C_ERROR_AF)
    {
      Error_Handler();
    }   
  }

#else
  
  /* The board receives the message and sends it back */

  /*##-2- Put I2C peripheral in reception process ###########################*/  
  if(HAL_I2C_Slave_Receive_DMA(&I2CxHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
  {
    /* Transfer error in reception process */
    Error_Handler();        
  }
  
  /*##-3- Wait for the end of the transfer ###################################*/  
  /*  Before starting a new communication transfer, you need to check the current   
      state of the peripheral; if it’s busy you need to wait for the end of current
      transfer before starting a new one.
      For simplicity reasons, this example is just waiting till the end of the 
      transfer, but application may perform other tasks while transfer operation
      is ongoing. */  
  while (HAL_I2C_GetState(&I2CxHandle) != HAL_I2C_STATE_READY)
  {
  } 
  
  /*##-4- Start the transmission process #####################################*/  
  /* While the I2C in reception process, user can transmit data through 
     "aTxBuffer" buffer */
  if(HAL_I2C_Slave_Transmit_DMA(&I2CxHandle, (uint8_t*)aTxBuffer, TXBUFFERSIZE)!= HAL_OK)
  {
    /* Transfer error in transmission process */
    Error_Handler();    
  }
  
#endif /* MASTER_BOARD */
  
  /*##-5- Wait for the end of the transfer ###################################*/  
  /*  Before starting a new communication transfer, you need to check the current   
      state of the peripheral; if it’s busy you need to wait for the end of current
      transfer before starting a new one.
      For simplicity reasons, this example is just waiting till the end of the 
      transfer, but application may perform other tasks while transfer operation
      is ongoing. */  
  while (HAL_I2C_GetState(&I2CxHandle) != HAL_I2C_STATE_READY)
  {
  } 
  
  /*##-6- Compare the sent and received buffers ##############################*/
  if(Buffercmp((uint8_t*)aTxBuffer,(uint8_t*)aRxBuffer,RXBUFFERSIZE))
  {
    /* Processing Error */
    Error_Handler();     
  }
 
  /* Infinite loop */  
  while (1)
  {
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Infinite loop */
  while(1)
  {
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 32000000
  *            HCLK(Hz)                       = 32000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_MUL                        = 4
  *            PLL_DIV                        = 2
  *            Flash Latency(WS)              = 1
  *            Main regulator output voltage  = Scale1 mode
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSI Oscillator and activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  RCC_OscInitStruct.HSICalibrationValue = 0x10;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);  
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
}

/**
  * @brief  Tx Transfer completed callback.
  * @param  I2CxHandle: I2C handle
  * @note   This example shows a simple way to report end of DMA Tx transfer, and 
  *         you can add your own implementation. 
  * @retval None
  */
#ifdef MASTER_BOARD
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2CxHandle)
{
}
#else
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *I2CxHandle)
{
  /* Turn LED2 on: Transfer in transmission process is correct */
  BSP_LED_On(LED2);
}
#endif /* MASTER_BOARD */

/**
  * @brief  Rx Transfer completed callback.
  * @param  I2CxHandle: I2C handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and 
  *         you can add your own implementation.
  * @retval None
  */
#ifdef MASTER_BOARD
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2CxHandle)
{
  /* Turn LED2 on: Transfer in reception process is correct */
  BSP_LED_On(LED2);
}
#else
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *I2CxHandle)
{
}
#endif /* MASTER_BOARD */

/**
  * @brief  I2C error callbacks.
  * @param  I2CxHandle: I2C handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
 void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2CxHandle)
{
  /* Transfer error in reception/transmission process */
  while(1)
  {
  }
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */
static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
