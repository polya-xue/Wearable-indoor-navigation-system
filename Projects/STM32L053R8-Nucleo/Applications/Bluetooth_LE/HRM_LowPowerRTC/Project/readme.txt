/**
  @page BlueNRG Shield for STM32 Nucleo Boards HRM Profile Application
   
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    readme.txt
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    04-July-2014
  * @brief   Description of the HRM Profile Application
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

This sample application shows how to use BlueNRG Bluetooth Low Energy 
stack.
To test this application you need a NucleoL0 board with its BlueNRG shield and
a Smartphone with Bluetooth Low Energy (BLE) chip and Android OS >= v4.3.

On the Android smartphone the STM32_BLE_Toolbox App must be installed and running.
The STM32_BLE_Toolbox App can be found in this package at:
$PATH_TO_STM32Cube_FW_L0_V1.1.0$\Utilities\Android_Software\HRM_Central

This sample application configures the board as Server-Peripheral, while the 
smartphone plays the Client-Central role.

After the connection has been established between the board and the smartphone, 
the STM32_BLE_Toolbox App will show the Heart Rate values in bpm (beats per minute)
coming from the NucleoL0 board.

The communication is done using a vendor specific profile.

@par Hardware and Software environment

  - This example runs on STM32L053R8 devices with BlueNRG shields
  - This example has been tested with STMicroelectronics STM32L0xx-Nucleo RevC boards
    and can be easily tailored to any other supported device and development board.
    
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
