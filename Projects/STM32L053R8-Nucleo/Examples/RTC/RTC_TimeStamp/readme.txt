/**
  @page RTC_TimeStamp RTC time stamp Example
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    RTC/RTC_TimeStamp/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    18-June-2014
  * @brief   Description of the RTC time stamp example.
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
 
This example guides you through the different configuration steps by mean of HAL API 
to ensure Time Stamp configuration using the RTC peripheral.

- At the beginning of the main program the HAL_Init() function is called to reset 
  all the peripherals, initialize the Flash interface and the systick.
  Then the SystemClock_Config() function is used to configure the system
  clock (SYSCLK) to run at 2 MHz.

The RTC peripheral configuration is ensured by the HAL_RTC_Init() function.
This later is calling the HAL_RTC_MspInit()function which core is implementing
the configuration of the needed RTC resources according to the used hardware (CLK, 
PWR, RTC clock source and BackUp). You may update this function to change RTC configuration.

- LSE oscillator clock is used as RTC clock source by default.
  The user can use also LSI as RTC clock source. The user uncomment the adequate 
  define on the main.h file.

HAL_RTCEx_SetTamper()function is called to initialize the tamper feature.
HAL_RTCEx_SetTimeStamp_IT()function is then called to initialize the time stamp feature 
with interrupt mode. It configures The Time Stamp pin to be falling edge and enables
the time Stamp detection on tamper pin.
HAL_RTC_SetTime()and HAL_RTC_SetDate() functions are then called to initialize the 
time and the date.

The program behaves as follows:

1. After startup the program configure the RTC (Time date) and enable the feature 
   timeStamp. 

2. Applying a low level on the Time Stamp pin (PC13) by pressing to User button, 
   a time stamp event is detedced and the calendar is saved in the time stamp structures.
   Each time the user presses on tamper button, the current time and Time Stamp are updated and displayed 
   on the debugger in aShowTime and aShowTimeStamp variables .
   
- LED2 is toggling : This indicates that the system generates an error.

@note Using stdio.h may increase the code size, it is optional and intended only to
      ease the example by displaying the date variables (aShowTime and aShowTimeStamp) on the debugger.
      By default this option is enabled, user can disable it by commenting
       "#define DISPLAY_ON_DUBUGGER" in main.h file.
       
@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then 
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents  

  - RTC/RTC_TimeStamp/Inc/stm32l0xx_hal_conf.h    HAL configuration file
  - RTC/RTC_TimeStamp/Inc/stm32l0xx_it.h          Interrupt handlers header file
  - RTC/RTC_TimeStamp/Inc/main.h                  Header for main.c module  
  - RTC/RTC_TimeStamp/Src/stm32l0xx_it.c          Interrupt handlers handlers
  - RTC/RTC_TimeStamp/Src/main.c                  Main program
  - RTC/RTC_TimeStamp/Src/stm32l0xx_hal_msp.c     HAL MSP module
  - RTC/RTC_TimeStamp/Src/system_stm32l0xx.c      STM32L0xx system source file


@par Hardware and Software environment

  - This example runs on STM32L051xx, STM32L052xx, STM32L053xx STM32L062xx and 
    STM32L063xx device lines RevZ

  - This example has been tested with STM32L053R8-Nucleo RevC  board and can be
    easily tailored to any other supported device and development board.

  - STM32L053R8-Nucleo Set-up
    - Use KEY push button connected to PC.13 pin


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
 