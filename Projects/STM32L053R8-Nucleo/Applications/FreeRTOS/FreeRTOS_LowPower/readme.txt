/**
  @page FreeRTOS_LowPower FreeRTOS Low Power example
 
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    FreeRTOS/FreeRTOS_LowPower/readme.txt
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    18-June-2014
  * @brief   Description of the FreeRTOS Low Power example.
  ******************************************************************************
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
  @endverbatim

@par Description

This directory contains a set of source files that implement an example
that uses message queues with CMSIS RTOS API

This example creates two threads.

   + A Rx thread that blocks on a queue to wait for data, blipping a LED each 
     time data is received (turning it on and then off again) before returning 
     to block on the queue once more.

   + A Tx thread that repeatedly enters the Blocked state for 500ms.  
     On exiting the blocked state the Tx thread sends a value through the queue 
     to the Rx thread (causing the Rx thread to exit the blocked state and blip 
     the LED).

Blocking for a finite period allows the kernel to stop the tick interrupt
and place the STM32 into sleep mode - the lowest power mode possible
that allows the CPU registers and RAM to retain their state.

In order to reduce the device power consumption, the unused GPIO's are configured
as analog.

Running this example will allow observing two behaviors.
The first one lasts about 500ms, during which the device is in low power state.
The wake up is signaled by switching the LED2 ON.
The second behavior is systematically consecutive to the first one, and it places
the device in Low power state for 20ms.
The wake up is then signaled by the switch off of the LED2.

The RTOS tick is suppressed while the MCU is in its low power state.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. 
      This implies that if HAL_Delay() is called from a peripheral ISR process, 
      then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications 
on STM32Cube with RTOS".


@par Directory contents

    - FreeRTOS/FreeRTOS_LowPower/Src/main.c                Main program
    - FreeRTOS/FreeRTOS_LowPower/Src/stm32l0xx_it.c        Interrupt handlers
    - FreeRTOS/FreeRTOS_LowPower/Src/system_stm32l0xx.c    STM32L0xx system clock configuration file
    - FreeRTOS/FreeRTOS_LowPower/Inc/main.h                Main program header file
    - FreeRTOS/FreeRTOS_LowPower/Inc/stm32l0xx_hal_conf.h  HAL Library Configuration file
    - FreeRTOS/FreeRTOS_LowPower/Inc/stm32l0xx_it.h        Interrupt handlers header file
    - FreeRTOS/FreeRTOS_LowPower/Inc/FreeRTOSConfig.h      FreeRTOS Configuration file


@par Hardware and Software environment

  - This example runs on STM32L051xx, STM32L052xx, STM32L053xx STM32L062xx and 
    STM32L063xx device lines RevZ.
    
  - This example has been tested with STM32L053R8-Nucleo board and can be
    easily tailored to any other supported device and development board. 


@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
 
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
