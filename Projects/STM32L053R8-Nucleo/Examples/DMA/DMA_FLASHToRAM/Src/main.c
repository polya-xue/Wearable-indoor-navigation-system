/**
  ******************************************************************************
  * @file    DMA/DMA_FLASHToRAM/Src/main.c  
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    18-June-2014
  * @brief   This example provides a description of how to use a DMA channel 
  *          to transfer a word data buffer from FLASH memory to embedded 
  *          SRAM memory through the STM32L0xx HAL API.
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

/** @addtogroup DMA_FLASHToRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* DMA Handle declaration */
DMA_HandleTypeDef     DmaHandle;

static const uint32_t aSRC_Const_Buffer[BUFFER_SIZE]= {
                                    0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
                                    0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
                                    0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
                                    0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
                                    0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
                                    0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
                                    0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
                                    0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};

static uint32_t aDST_Buffer[BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
static void DMA_Config(void);
static void SystemClock_Config(void);
static void Error_Handler(void);
static void TransferComplete(DMA_HandleTypeDef *DmaHandle);
static void TransferError(DMA_HandleTypeDef *DmaHandle);

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
  /* Configure the system clock to 2 Mhz */
  SystemClock_Config();

  /* Configure and enable the DMA Stream for Memory to Memory transfer */
  DMA_Config(); 
  
  /* Infinite loop */
  while (1)
  {
  }
}
    
/**
  * @brief  Configure the DMA controller according to the Channel parameters
  *         defined in main.h file
  * @note  This function is used to :
  *        -1- Enable DMA clock
  *        -2- Select the DMA functional Parameters
  *        -3- Select the DMA instance to be used for the transfer
  *        -4- Select Callbacks functions called after Transfer complete and 
               Transfer error interrupt detection
  *        -5- Initialize the DMA Channel
  *        -6- Configure NVIC for DMA transfer complete/error interrupts
  *        -7- Start the DMA transfer using the interrupt mode
  * @param  None
  * @retval None
  */
static void DMA_Config(void)
{
  /*## -1- Enable DMA1 clock #################################################*/
  __DMA1_CLK_ENABLE();
    
  /*## -2- Select the DMA functional Parameters ###############################*/
  DmaHandle.Init.Direction = DMA_MEMORY_TO_MEMORY;          /* M2M transfer mode                */           
  DmaHandle.Init.PeriphInc = DMA_PINC_ENABLE;               /* Peripheral increment mode Enable */                 
  DmaHandle.Init.MemInc = DMA_MINC_ENABLE;                  /* Memory increment mode Enable     */                   
  DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; /* Peripheral data alignment : Word */    
  DmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;    /* memory data alignment : Word     */     
  DmaHandle.Init.Mode = DMA_NORMAL;                         /* Normal DMA mode                  */  
  DmaHandle.Init.Priority = DMA_PRIORITY_HIGH;              /* priority level : high            */  

  /*## -3- Select the DMA instance to be used for the transfer : DMA1_Channel1 #*/
  DmaHandle.Instance = DMA_CHANNEL;

  /*## -4- Select Callbacks functions called after Transfer complete and Transfer error */
  DmaHandle.XferCpltCallback  = TransferComplete;
  DmaHandle.XferErrorCallback = TransferError;
    
  /*## -5- Initialize the DMA stream ##########################################*/
  if(HAL_DMA_Init(&DmaHandle) != HAL_OK)
    {
    /* Initialization Error */
    Error_Handler();  
    } 
    
  /*## -6- Configure NVIC for DMA transfer complete/error interrupts ##########*/
  HAL_NVIC_SetPriority(DMA_CHANNEL_IRQ, 0, 0);
  
  /* Enable the DMA STREAM global Interrupt */
  HAL_NVIC_EnableIRQ(DMA_CHANNEL_IRQ);

  /*## -7- Start the DMA transfer using the interrupt mode ####################*/
  /* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
  /* Enable All the DMA interrupts */
  if(HAL_DMA_Start_IT(&DmaHandle, (uint32_t)&aSRC_Const_Buffer, (uint32_t)&aDST_Buffer, BUFFER_SIZE) != HAL_OK)
  {
    /* Transfer Error */
    Error_Handler();        
  }
} 

/**
  * @brief  DMA Transfer complete callback
  * @note   This function is executed when the transfer complete interrupt 
  *         is generated
  * @retval None
  */
static void TransferComplete(DMA_HandleTypeDef *DmaHandle)
{
  /* Turn LED2 on: Transfer correct */
  BSP_LED_On(LED2);
}
 
/**
  * @brief  DMA Transfer error callback
  * @note   This function is executed when the transfer error interrupt 
  *         is generated during DMA transfer
  * @retval None
  */
static void TransferError(DMA_HandleTypeDef *DmaHandle)
{
  /* Turn LED2 off: Transfer Error */
  BSP_LED_Off(LED2);
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = MSI
  *            SYSCLK(Hz)                     = 2000000
  *            HCLK(Hz)                       = 2000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            Flash Latency(WS)              = 0
  *            Main regulator output voltage  = Scale3 mode
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  
  /* Enable MSI Oscillator */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.MSICalibrationValue=0x00;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  
  /* Select MSI as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
    /* Turn LED2 off */
    BSP_LED_Off(LED2);
    while(1)
    {
    }
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
