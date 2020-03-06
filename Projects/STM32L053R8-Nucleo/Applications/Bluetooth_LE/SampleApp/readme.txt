/**
  @page BlueNRG Shield for STM32 Nucleo Boards Sample Application
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    readme.txt  
  * @version V0.0.1
  * @date    19-May-2014
  * @brief   Description of the BlueNRG sample application.
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

This sample application shows how to use BlueNRG Bluetooth Low Energy 
stack.
To test this application you need two NucleoL0 boards with their
respective BlueNRG shields. One board needs to be configured as Server-Peripheral
role, while the other needs to be configured as Client-Central role.
Before flashing the boards, please make sure to specify the right role by
uncommenting the corresponding line at the beginning of Src\main.c file, either
BLE_RoleTypeDef BLE_Role = SERVER;
or
BLE_RoleTypeDef BLE_Role = CLIENT;

After connection has been established between the two boards, by pressing the
USER button on one board, the LD2 LED on the other one gets toggled and
viceversa.
The communication is done using a vendor specific profile.


@par Hardware and Software environment

  - This example runs on STM32L053R8 devices with BlueNRG shields
  - This example has been tested with STMicroelectronics STM32L0xx-Nucleo RevC
    boards and can be easily tailored to any other supported device
    and development board.
    

@par How to use it ? 

In order to make the program work, you must do the following:
 - WARNING: before opening the project with any toolchain be sure your folder
   installation path is not too in-depth since the toolchain may report errors
   after building.
 - Open IAR toolchain (this firmware has been successfully tested with
   Embedded Workbench V7.10.3 and V7.20.1).
   Alternatively you can use the Keil uVision toolchain (this firmware
   has been successfully tested with V5.10.0.2).
   Alternatively you can use the Atollic TrueSTUDIO toolchain (this firmware
   has been successfully tested with V5.0.0).
 - Rebuild all files and load your image into target memory.
 - Run the example.
 - Alternatvely, you can download the pre-built binaries in "Binaries" 
   folder included in the distributed package.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
