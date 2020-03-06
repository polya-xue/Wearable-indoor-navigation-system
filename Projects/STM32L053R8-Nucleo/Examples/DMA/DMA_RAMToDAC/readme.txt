/**
  @page DMA_RAMToDAC DMA RAM To DAC Example
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    DMA/DMA_RAMToDAC/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    18-June-2014
  * @brief   Description of the DMA RAM To DAC example.
  ******************************************************************************
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
  @endverbatim

@par Example Description 

This example provides a description of how to use a DMA channel to transfer data 
buffer from RAM memory to DAC.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 2 MHz.

DMA1 channel1 is configured to transfer the contents of a Half word data 
buffer stored in RAM memory to the DAC peripheral.

The start of transfer is triggered by software. DMA1 channel1 memory-to-peripheral
transfer is enabled. Source and destination addresses incrementing is also enabled.
The transfer is started by setting the Channel enable bit for DMA1 channel1.

The DAC channel conversion is configured to be triggered by TIM6 TRGO triggers 
and without noise/triangle wave generation.
12bit right data alignment is selected.

PA4 pin should be connected to the scope to display the sine wave form.

STM32L053R8-Nucleo board's LED can be used to monitor the transfer status:
 - LED2 Toggles when there is an error in the transfer process or in initialisation 
                                 
@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then 
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents  

  - DMA/DMA_RAMToDAC/Src/system_stm32l0xx.c    STM32L0xx system clock configuration file
  - DMA/DMA_RAMToDAC/Src/stm32l0xx_it.c        Interrupt handlers
  - DMA/DMA_RAMToDAC/Src/main.c                Main program
  - DMA/DMA_RAMToDAC/Src/stm32l0xx_hal_msp.c   HAL MSP file
  - DMA/DMA_RAMToDAC/Inc/stm32l0xx_hal_conf.h  HAL Configuration file
  - DMA/DMA_RAMToDAC/Inc/stm32l0xx_it.h        Interrupt handlers header file
  - DMA/DMA_RAMToDAC/Inc/main.h                Main program header file
       
@par Hardware and Software environment

  - This example runs on STM32L051xx, STM32L052xx, STM32L053xx STM32L062xx and 
    STM32L063xx device lines RevZ.
    
  - This example has been tested with STM32L053R8-Nucleo RevC  board and can be
    easily tailored to any other supported device and development board.
  
  - STM32L053R8-Nucleo Set-up 	
     - Use LED2 connected to PA5.
     - Connect PA4 (DAC Channel1) pin to an oscilloscope (pin A2 in CN8 connector).    


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example 

   
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
