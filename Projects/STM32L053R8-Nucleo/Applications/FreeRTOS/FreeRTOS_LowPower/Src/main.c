/**
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_LowPower/Src/main.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    18-June-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define blckqSTACK_SIZE   configMINIMAL_STACK_SIZE
#define QUEUE_SIZE        (uint32_t) 1

/* Private variables ---------------------------------------------------------*/
osMessageQId osQueue;

/* The number of items the queue can hold.  This is 1 as the Rx task will
remove items as they are added so the Tx task should always find the queue
empty */
#define QUEUE_LENGTH             (1)

/* The rate at which the Tx task sends to the queue */
#define TX_DELAY                 (500)

/* The value that is sent from the Tx task to the Rx task on the queue */
#define QUEUED_VALUE             (100)

/* The length of time the LED will remain on for.  It is on just long enough
to be able to see with the human eye so as not to distort the power readings too
much */
#define LED_TOGGLE_DELAY         (20)

/* Private function prototypes -----------------------------------------------*/
static void QueueReceiveThread (const void *argument);
static void QueueSendThread (const void *argument);
static void GPIO_ANConfig(void);
static void SystemClock_Config(void);

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
  
  /* Configure the System clock to 2 MHz (Up to 32MHz possible) */
  SystemClock_Config();
  
  /* Configure GPIO to Analog */
  GPIO_ANConfig();
  
  /* Initialize LEDs */
  BSP_LED_Init(LED2);
  
  /* Create the queue used by the two threads */
  osMessageQDef(osqueue, QUEUE_LENGTH, uint16_t);
  osQueue = osMessageCreate (osMessageQ(osqueue), NULL);
  
  /* Note that the Tx has a lower priority than the Rx when the threads are
     spawned */
  osThreadDef(RxThread, QueueReceiveThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  osThreadCreate(osThread(RxThread), NULL);
  
  osThreadDef(TxThread, QueueSendThread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE);
  osThreadCreate(osThread(TxThread), NULL);
  
  /* Start scheduler */
  osKernelStart (NULL, NULL);

  /* We should never get here as control is now taken by the scheduler */
  for(;;);
}

/**
  * @brief  Message Queue Producer Thread.
  * @param  argument: Not used
  * @retval None
  */
static void QueueSendThread (const void *argument)
{
  for(;;)
  {		
    /* Place this thread into the blocked state until it is time to run again.
       The kernel will place the MCU into the Retention low power sleep state
       when the idle thread next runs */
    osDelay(TX_DELAY);

    /* Send to the queue - causing the queue receive thread to flash its LED.
       It should not be necessary to block on the queue send because the Rx
       thread will already have removed the last queued item */    
    osMessagePut (osQueue, (uint32_t)QUEUED_VALUE, 0);
  }
}

/**
  * @brief  Message Queue Consumer Thread.
  * @param  argument: Not used
  * @retval None
  */
static void QueueReceiveThread (const void *argument)
{
  osEvent event;
  
  for(;;)
  {
    /* Wait until something arrives in the queue */
    event = osMessageGet(osQueue, osWaitForever);
    
    /*  To get here something must have arrived, but is it the expected
	value?  If it is, turn the LED on for a short while */
    if(event.status == osEventMessage)
    {
      if(event.value.v == QUEUED_VALUE)
      {  
        BSP_LED_On(LED2);
        osDelay(LED_TOGGLE_DELAY);
        BSP_LED_Off(LED2);
      }			
    }		
  }
}

/**
  * @brief  MCU Pre Sleep process.
  * @param  ulExpectedIdleTime: Not used
  * @retval None
  */
void PreSleepProcessing(unsigned long *ulExpectedIdleTime)
{
  /* Called by the kernel before it places the MCU into a sleep mode because
  configPRE_SLEEP_PROCESSING() is #defined to PreSleepProcessing().
  
  NOTE:  Additional actions can be taken here to get the power consumption
  even lower.  For example, peripherals can be turned	off here, and then back
  on again in the post sleep processing function.  For maximum power saving
  ensure all unused pins are in their lowest power state */

  /* Avoid compiler warnings about the unused parameter */
  (void ) ulExpectedIdleTime;
  
  /* Disable the peripheral clock during Low Power (Sleep) mode.*/
  __GPIOA_CLK_SLEEP_DISABLE();
}

/**
  * @brief  MCU Post Sleep process.
  * @param  ulExpectedIdleTime: Not used
  * @retval None
  */
void PostSleepProcessing(unsigned long *ulExpectedIdleTime)
{
	/* Called by the kernel when the MCU exits a sleep mode because
	configPOST_SLEEP_PROCESSING is #defined to PostSleepProcessing() */

	/* Avoid compiler warnings about the unused parameter */
	(void) ulExpectedIdleTime;
}

/**
  * @brief  GPIO analog Configuration
  * @param  None
  * @retval None
  */
static void GPIO_ANConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIOs clock */
  //__GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();
  __GPIOC_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();
  __GPIOH_CLK_ENABLE();

  /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
  GPIO_InitStructure.Pin = GPIO_PIN_All;
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  //HAL_GPIO_Init(GPIOA, &GPIO_InitStructure); 
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);

  /* Disable GPIOs clock */
  //__GPIOA_CLK_DISABLE();
  __GPIOB_CLK_DISABLE();
  __GPIOC_CLK_DISABLE();
  __GPIOD_CLK_DISABLE();
  __GPIOH_CLK_DISABLE();
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
  RCC_OscInitStruct.MSICalibrationValue = 0x00;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  
  /* Select MSI as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0); 
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
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
  {}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
