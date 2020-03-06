/**
  ******************************************************************************
  * @file    bluenrg_interface.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    04-July-2014
  * @brief   This file provides code for the BlueNRG Shield driver
  *          based on STM32Cube HAL for STM32 Nucleo boards.
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

#include "debug.h"
#include "ble_status.h"
#include "low_power_conf.h"
#include "hci.h"
#include "hci_i.h"
#include "htl.h"
#include "stm32_bluenrg_ble.h"
#include <host_osal.h>
#include "stm32xx_lpm.h"
#include "stm32l0xx_nucleo.h"

#define SPI_BUFFER_SIZE 255 // FIXME: it was 256 but as many length filed are uint8_t, 256 is a risk
#define NUM_OF_RETRANSMISSIONS 100

/* SPI handler declared in "stm32l0xx_nucleo_bluenrg.c" file */
extern SPI_HandleTypeDef SpiHandle;
extern tListNode hciReadPktPool;

volatile uint8_t send_hr_measurement = 0;

volatile unsigned int spi_ptr_in = 0, spi_buff_len = 0;
unsigned spi_ptr_out = 0;
uint8_t spi_buffer[SPI_BUFFER_SIZE];


/**
  * @brief  RTC Wake Up callback
  * @param  None
  * @retval None
  */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    uint32_t uwPRIMASK_Bit = __get_PRIMASK();	/**< backup PRIMASK bit */;
  
    /* Clear Wake Up Flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    __disable_irq();			/**< Disable all interrupts by setting PRIMASK bit on Cortex*/
    send_hr_measurement++;
    __set_PRIMASK(uwPRIMASK_Bit);	/**< Restore PRIMASK bit*/
    LPM_Mode_Request(eLPM_MAIN_LOOP_PROCESSES, eLPM_Mode_RUN);
}

/**
 * @brief EXTI line detection callback.
 * @param GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Clear Wake Up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  
  if(GPIO_Pin == BNRG_SPI_EXTI_PIN) {
    
    while (HAL_GPIO_ReadPin(BNRG_SPI_EXTI_PORT, BNRG_SPI_EXTI_PIN) == GPIO_PIN_SET) {
      BlueNRG_Profile_SPI_Read_All();
    }
    LPM_Mode_Request(eLPM_MAIN_LOOP_PROCESSES, eLPM_Mode_RUN); 
  }
  else {
    if(GPIO_Pin == KEY_BUTTON_EXTI_LINE) {
        uint32_t uwPRIMASK_Bit = __get_PRIMASK();	/**< backup PRIMASK bit */;
        __disable_irq();			/**< Disable all interrupts by setting PRIMASK bit on Cortex*/
        send_hr_measurement++;
        __set_PRIMASK(uwPRIMASK_Bit);	/**< Restore PRIMASK bit*/
        LPM_Mode_Request(eLPM_MAIN_LOOP_PROCESSES, eLPM_Mode_RUN);
    }
  }
}

/**
* @brief  This function reads from BlueNRG SPI buffer and store data into local
*         circular buffer.
* @param  None
* @retval int32_t
*/
int32_t BlueNRG_Profile_SPI_Read_All()
{  
  uint32_t i;
  uint16_t byte_count;
  int32_t result = 0;
  uint8_t temp_buffer[SPI_BUFFER_SIZE];
  
  byte_count = BlueNRG_SPI_Read_All(&SpiHandle, temp_buffer, SPI_BUFFER_SIZE);
  
  if (byte_count > 0) {
    
    for (i = 0; i < byte_count; i++) { 
      spi_input(temp_buffer[i]);
    }
    
    // Incoming data must be processed by profile framework. 		
    HTL_Read();
    
  } else {
    result = -1;
  }
  
  return result;
  
}

/**
* @brief  This function writes into the BlueNRG SPI buffer.
* @param  uint8_t* data buffer to be written
* @param  uint8_t  number of bytes to be written
* @retval int32_t  number of read bytes
*/
int32_t BlueNRG_Profile_SPI_Write(uint8_t* data, uint8_t Nb_bytes)
{  
  int32_t result;
  
  result = BlueNRG_SPI_Write(&SpiHandle, data, NULL, Nb_bytes, 0);
  return result;
}




/**
 * @brief  Reset the BlueNRG
 * @param  None
 * @retval None
 */
void BlueNRG_Profile_RST(void)
{   
  //[SO] FIXME!!! Create a new file to implement the interface between the BlueNRG SPI driver and the 
  // profile stack 
  send_hr_measurement = 0;
  spi_ptr_in = 0;
  spi_buff_len = 0;
  spi_ptr_out = 0;

  BlueNRG_RST();
}

/**
 * @brief  This function inserts a byte in the buffer.
 * @param  uint8_t byte
 * @retval int (1 = the byte is successfully inserted in the buffer, 0 = otherwise)
 */
int spi_input(uint8_t byte)
{
  if(spi_buff_len == SPI_BUFFER_SIZE)
    return 0;
  
  spi_buffer[spi_ptr_in] = byte;
  spi_ptr_in++;
  spi_buff_len++;
  if(spi_ptr_in == SPI_BUFFER_SIZE){
    spi_ptr_in = 0;
  }
  
  return 1;
}

/**
 * @brief  This function gets the length of the BlueNRG SPI buffer.
 * @param  None
 * @retval int (the length of the SPI buffer)
 */
int SPI_Get_Len(void)
{
    return spi_buff_len;
}

/**
 * @brief  This function reads from the BlueNRG SPI buffer.
 * @param  int the number of bytes to get
 * @param  char* the destination buffer
 * @retval None
 */
void SPI_Read(int count, char *data)
{
    int i;
    
    for(i = 0; spi_ptr_out != spi_ptr_in && i < count; i++){
        
        data[i] = spi_buffer[spi_ptr_out++];
        if(spi_ptr_out == SPI_BUFFER_SIZE)
            spi_ptr_out = 0;
        
    }
    spi_buff_len -= i;
}

/**
 * @brief  This function writes N times into the BlueNRG SPI buffer.
 * @param  int the number of bytes to write
 * @param  uint8_t* the source buffer
 * @retval None
 */
void SPI_Write(int count, uint8_t *data)
{
    int result;
    int counter = NUM_OF_RETRANSMISSIONS;
    
    while ((counter--)) {
        result = BlueNRG_Profile_SPI_Write(data, count);
        if (result == 0) {
            break;
        }
    }
}

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
