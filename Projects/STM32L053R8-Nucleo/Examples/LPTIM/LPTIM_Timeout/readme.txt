/**
  @page LPTIM_Timeout LPTIM Timeout example with LSE clock source.
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    LPTIM/LPTIM_Timeout/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    18-June-2014
  * @brief   Description of the LPTIM Timeout example.
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

This example describes how to implement a low power timeout to wake-up the 
system using the LPTIMER , through the STM32L0xx HAL API. 
  
The first trigger event will start the timer, any successive trigger event on PC3
(LPTIM_ETR) will reset the counter and the timer will restart. A low power
timeout function is realized. The timeout value corresponds to the compare
value (32768), if no trigger occurs within the expected time frame, the MCU is
woken-up by the compare match event and LED2 turns on.

In this example the LPTIM is clocked by the LSE (32768 Hz)

  Timeout = (Compare + 1) / LPTIM_Clock
          = (32767 + 1) / 32768
          = 1 s


@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then 
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents  

  - LPTIM/LPTIM_Timeout/Inc/stm32l0xx_hal_conf.h    HAL configuration file
  - LPTIM/LPTIM_Timeout/Inc/stm32l0xx_it.h          Interrupt handlers header file
  - LPTIM/LPTIM_Timeout/Inc/main.h                  Header for main.c module  
  - LPTIM/LPTIM_Timeout/Src/stm32l0xx_it.c          Interrupt handlers
  - LPTIM/LPTIM_Timeout/Src/main.c                  Main program
  - LPTIM/LPTIM_Timeout/Src/stm32l0xx_hal_msp.c     HAL MSP module
  - LPTIM/LPTIM_Timeout/Src/system_stm32l0xx.c      STM32L0xx system source file


@par Hardware and Software environment

  - LSE oscillator must be mounted on the board, else this example does not work.
  
  - This example runs on STM32L051xx, STM32L052xx, STM32L053xx STM32L062xx and 
    STM32L063xx device lines RevZ
    
  - This example has been tested with STM32L053R8-Nucleo RevC  board and can be
    easily tailored to any other supported device and development board.    
  
  - Connect an external signal (Trigger) on (PC.3)(pin 37 in CN7 connector). 
    System is woken up if the external trigger < 1Hz.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example 


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
