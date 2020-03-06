-----------
Readme.txt
-----------
This Readme.txt file describes some steps needed to overcome known bugs/issues.

---------
Contents
---------
In this folder there are four applications tested on the Nucleo-L0:
    - HRM_LowPowerRTC
	- SampleApp
	- SensorDemo
	- SensorDemo_GATTServer
To run these applications the BlueNRG expansion board on the Nucleo-L0 is needed.
Please read the respective readme.txt file within the application folders to understand each application and it's usage.

-----------
Known-Bugs
-----------

- BlueNRG is sometimes undiscoverable after premature disconnection

Applications in folder Projects\STM32L053R8-Nucleo\Applications\Bluetooth_LE:
    - HRM_LowPowerRTC
	- SensorDemo
	- SensorDemo_GATTServer
    
If you press the disconnect button on the smartphone app while a connection is being setup with the BlueNRG (acting as Peripheral), sometimes the device becomes undiscoverable (no advertising sent). This can be reproduced by pressing connect and then disconnect in a very short time and continuously repeating this procedure very quickly.
When this happens, you need to hard-reset the NucleoL0 board by unplugging it from the power and then restart the smartphone application.

This bug will be fixed with release 6.4 of the firmware for the BlueNRG expansion board (shield).

In the meantime, this problem can be avoided by simply waiting for the connection to be fully established before hitting the disconnect button on the phone.

Otherwise, if this issue is critical in your setup and you need to fix it now (without waiting for version 6.4 of the Firmware), then you can flash the firmware 'ifr_3v0_006_mode02-16MHz-RO.dat' on the BlueNRG expansion board by following the procedure described in the following "Firmware Update" section.
If you use the 'ifr_3v0_006_mode02-16MHz-RO.dat' firmware, however, the power consumption of the BlueNRG expansion board will increase. The fix that will be released with version 6.4, instead, will not affect consumption. Therefore we recommend that you wait for version 6.4.

----------------  
Firmware Update
---------------- 
To update the firmware for the BlueNRG expansion board, the following steps must be executed:

- Download from http://www.st.com/web/en/catalog/tools/PF259737# the STSW-IDB002V1 sw package and install it.
- Take a Nucleo L1 board (at the moment the flasher is compiled for L1), load the program BlueNRG DK 1.5.0\Firmware\BlueNRG_VCOM_1_3_Nucleo.hex image and then connect the Shield to the L1. 
- Open the <Installation Path>\BlueNRG DK 1.5.0\Application\BLUENRG_GUI.exe application and connect to the Nucleo L1 COM Port. 
- Click on Tools->BlueNRG IFR.. 
- Click on Load, open the file: BlueNRG DK 1.5.0\Firmware\BlueNRGcut3\ifr_3v0_006_mode02-16MHz-RO.dat
- Click on Write – you should see, by scrolling through the written messages, as it gets programmed
- Power cycle the board
