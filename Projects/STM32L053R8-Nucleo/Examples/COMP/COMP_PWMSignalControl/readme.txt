/**
  @page COMP_PWMSignalControl COMP PWM Signal Control example
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    COMP/COMP_PWMSignalControl/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    18-June-2014
  * @brief   Description of the COMP PWM Signal Control example.
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
  
  This example shows how to configure COMP1 peripheral to hold on safe state
 (low level) LPTIM channels as soon as COMP1 output is set at high level.

  - COMP1 is configured as following:
     - Inverting input is internally connected to VREFINT = 1.22V
     - Output is internally connected to LPTIM 
     
     
LPTIM generates a PWM at the lowest power consumption, using the output signal of 
Comparator.

  - While PA1 is higher than VREFINT (1.22V), PWM signal is displayed on PC1.
  - While PA1 is lower than VREFINT, PC1 is in low level.
  
The Autorelaod equal to 99 so the output frequency (OutputFrequency) will
be equal to the external counter clock (InputFrequency) divided by (99+1).

  OutputFrequency = InputFrequency / (Autoreload + 1)
                  = InputFrequency / 100

Pulse value equal to 49 and the duty cycle (DutyCycle) is computed as follow:

  DutyCycle = 1 - [(PulseValue + 1)/ (Autoreload + 1)]
  DutyCycle = 50%

  - External square signal should be connected to PA1 (Inverting input of COMP1),
    when the amplitude is higher than 1.22V, PWM signal is displayed on PC1, otherwise PC1 is in low level.

  - Connect an oscilloscope to LPTIM PC1 to display waveforms.
                         
@note To have a precise value of the pulse, HSI as a source of clock system should be 
calibrated or use an external clock.

If LED2 is toggling successively: This indicates that the system generates an error. 

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then 
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents  

  - COMP/COMP_PWMSignalControl/Inc/stm32l0xx_hal_conf.h    HAL configuration file
  - COMP/COMP_PWMSignalControl/Inc/stm32l0xx_it.h          DMA interrupt handlers header file
  - COMP/COMP_PWMSignalControl/Inc/main.h                  Header for main.c module  
  - COMP/COMP_PWMSignalControl/Src/stm32l0xx_it.c          DMA interrupt handlers
  - COMP/COMP_PWMSignalControl/Src/main.c                  Main program
  - COMP/COMP_PWMSignalControl/Src/stm32l0xx_hal_msp.c     HAL MSP file
  - COMP/COMP_PWMSignalControl/Src/system_stm32l0xx.c      STM32L0xx system source file
         
@par Hardware and Software environment

  - This example runs on STM32L051xx, STM32L052xx, STM32L053xx STM32L062xx and 
    STM32L063xx device lines RevZ
    
  - This example has been tested with STM32L053R8-Nucleo RevC  board and can be
    easily tailored to any other supported device and development board.

  - STM32L053R8-Nucleo Set-up
    - Use LED2 connected to PA5 pin.
    - Connect an external signal to PA.01 pin (pin A1 in CN8 connector).
    - Connect the scope to PC1 (pin 7 in CN7 connector) to display the PWM signal.
       
@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
  
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */

