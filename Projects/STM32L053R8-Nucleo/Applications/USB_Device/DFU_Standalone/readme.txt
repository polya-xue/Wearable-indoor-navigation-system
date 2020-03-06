/**
  @page DFU_Standalone USB Device Firmware Upgrade (DFU) example
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    USB_Device/DFU_Standalone/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    18-June-2014
  * @brief   Description of the USB DFU example.
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

@par Example Description 

This example is a part of the USB Device Library package using STM32Cube firmware. 
It is a typical example on how to use USB device application based on the
Device Firmware Upgrade class (DFU) on the STM32L0xx devices.

This example uses the DFU capability to upgrade the Application area in the embedded 
flash memory of the STM32L0xx devices. The Internal flash memory is organized as follows:
 - DFU area located in [0x08000000 : USBD_DFU_APP_DEFAULT_ADD-1]: Only read access
 - Application area located in [USBD_DFU_APP_DEFAULT_ADD : Device's end address]: Read, Write, and Erase
   access.

In this application, two operating modes are available:
 1. DFU operating mode: 
    This mode is entered after a MCU reset using the two following methods:
     - The DFU mode is forced by the user by pressing the key button in the STM32L053-Nucleo Board .
     - No valid code found in the application area: a code is considered valid if the MSB of the initial
       Main Stack Pointer (MSP) value located in the first address of the application area is equal to 
       0x2000.
      
 2. Run-time application mode: 
    This is the normal run-time behavior. A binary file with basic example which toggles LED2 on the 
    STM32L053-Nucleo Board is provided in Binary directory.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick. The user is 
provided with the SystemClock_Config()function to configure the system clock (SYSCLK).
In this application the system clock is set depending on the USB clock source 
  - 32 MHz from the PLL(HSE), when PLL is used as USB clock source
  - 16 MHz from the HSI, when HSI48 is used as USB clock source

The 48 MHz clock for the USB can be derived from one of the two following sources:
  � PLL clock(clocked by the HSE): If the USB uses the PLL as clock source, the PLL VCO clock must be programmed
    to output 96 MHz frequency (USBCLK = PLLVCO/2).
  � RC48 Clock (HSI48): The HSI48 clock is generated from an internal 48 MHz RC oscillator, coupled with
    the clock recovery system (CRS). 
    The CRS allows automatic trimming of the internal HSI48, synchronized with USB SOF signal at 1KHz rate, 
    to guarantee its optimal accuracy over the whole device operational range.    
User can select USB clock from HSI48 or PLL through macro defined in main.h
(USE_USB_CLKSOURCE_CRSHSI48 and USE_USB_CLKSOURCE_PLL).


@note To reduce the example footprint, the toolchain dynamic allocation is replaced by a static allocation
      by returning the address of a pre-defined static buffer with the DFU class structure size.     

Basically, firmware are stored in Hex, S19 or Binary files, but these formats do not contain the 
necessary information to perform the upgrade operation, they contain only the actual data of the program
to be downloaded. While, the DFU operations require more information, like the product identifier, 
vendor identifier, Firmware version and the Alternate setting number (Target ID) of the target to be 
used, this information makes the upgrade targeted and more secure. To add this information, DFU file 
format is used. For more details refer to the "DfuSe File Format Specification" document (UM0391).

To generate a DFU image, download "DFUse Demonstration" tool and use DFU File Manager to convert a 
binary image into a DFU image. This tool is for download from www.st.com
To download a *.dfu image, use "DfuSe Demo" available within "DFUse Demonstration" install directory. 

Please refer to UM0412, DFuSe USB device firmware upgrade STMicroelectronics extension for more details
on the driver installation and PC host user interface.
    
It is possible to fine tune needed USB Device features by modifying defines values in USBD configuration
file �usbd_conf.h� available under the project includes directory, in a way to fit the application
requirements, such as �USBD_DFU_APP_DEFAULT_ADD, specifying the address from where user's application will be downloaded.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on the the SysTick ISR. Thus if HAL_Delay() is called from a peripheral ISR process, the SysTick interrupt 
      must have higher priority (numerically lower) than the peripheral interrupt, Otherwise the caller ISR process 
      will be blocked. To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function or statically
      define it in the HAL configuration file through the "TICK_INT_PRIORITY" define.
      
@note The application needs to ensure that the SysTick time base is set to 1 millisecond
      to have correct HAL configuration.


For more details about the STM32Cube USB Device library, please refer to UM1734 
"STM32Cube USB Device library".

@par Directory contents

  - USB_Device/DFU_Standalone/Src/main.c                  Main program
  - USB_Device/DFU_Standalone/Src/system_stm32l0xx.c      STM32L0xx system clock configuration file
  - USB_Device/DFU_Standalone/Src/stm32l0xx_it.c          Interrupt handlers
  - USB_Device/DFU_Standalone/Src/usbd_conf.c             General low level driver configuration
  - USB_Device/DFU_Standalone/Src/usbd_desc.c             USB device descriptor
  - USB_Device/DFU_Standalone/Src/usbd_dfu_flash.c        Internal flash memory management
  - USB_Device/DFU_Standalone/Inc/main.h                  Main program header file
  - USB_Device/DFU_Standalone/Inc/stm32l0xx_it.h          Interrupt handlers header file
  - USB_Device/DFU_Standalone/Inc/stm32l0xx_hal_conf.h    HAL configuration file
  - USB_Device/DFU_Standalone/Inc/usbd_conf.h             USB device driver Configuration file
  - USB_Device/DFU_Standalone/Inc/usbd_desc.h             USB device descriptor header file
  - USB_Device/DFU_Standalone/Inc/usbd_dfu_flash.h        Internal flash memory management header file


@par Hardware and Software environment

  - This example runs on STM32L052xx, STM32L053xx STM32L062xx and 
    STM32L063xx device lines RevZ.

  - This example has been tested with STM32L053R8-Nucleo RevC board with a USB shield daughter
   board, and can be easily tailored to any other supported device and development board.

  - STM32L053-Nucleo RevC Set-up
      Since there is no USB 2.0 Full speed connector (Type B) on the nucleo board, user has to make 
      his own USB shield daughter board with the a USB connector and plug it on top of the  CN11 and CN12 
      connectors of the STM32L053-Nucleo. The USB connector has to be connected to the USB device associated GPIOs
      as follows:
       - DP (D+ of the USB connector) <======> PA12 (Nucleo board)
       - DM (D- of the USB connector) <======> PA11 (Nucleo board)

      To improve EMC performance (noise immunity and signal integrity), it is recommended to connect a 100nF
      ceramic capacitor to the USB VDD pin.


@par How to use it ?

In order to make the program work; you need first to install the "DfuSe Demonstrator" software 
and DFU driver, then proceed as follows:
 - Open project file
 - Rebuild all the project and load the image into the target memory
 - Run the example 
 - Open "DfuSe Demo" and download the "STM32L0xx_Nucleo_SysTick_0x08007000.dfu" provided in the Binary
   directory.
 - To run the downloaded application, execute the command "leave the DFU mode" or Reset the 
   board. 

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
