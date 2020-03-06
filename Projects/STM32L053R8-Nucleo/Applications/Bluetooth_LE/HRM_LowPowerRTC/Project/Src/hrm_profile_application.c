/**
******************************************************************************
* @file    hrm_profile_application.c 
* @author  AAS / CL
* @version V1.0.0
* @date    04-July-2014
* @brief   This file implements the main function executing the HRM profile.
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

/*******************************************************************************
 * Include Files
*******************************************************************************/
#include <host_config.h>
#if (PLATFORM_WINDOWS == 1)
#include <Windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <hal_types.h>
#include <ble_status.h>
#include <debug.h>
#include <host_osal.h>
#include <list.h>
#include <hci.h>
#include <htl.h>
#include <profile_hci_interface.h>
#include <ble_profile.h>
#include <ble_events.h>
#include <timer.h>
#include "stm32_bluenrg_ble.h"
#include "stm32xx_lpm.h"
#include "low_power_conf.h"
#include "hrm_profile_application.h"

#include "stm32l0xx_hal.h"
#include "stm32l0xx_nucleo.h"

//#include <platform_config.h>
#if (BLE_CURRENT_PROFILE_ROLES & HEART_RATE_SENSOR)
#include <heart_rate.h>
#endif

#include "bluenrg_interface.h"

/*******************************************************************************
 * Macros
*******************************************************************************/
/* IO capabilities */
#define SPI_HOST_MAIN_IO_CAPABILITY_DISPLAY_ONLY              (0x00)
#define SPI_HOST_MAIN_IO_CAPABILITY_DISPLAY_YES_NO            (0x01)
#define SPI_HOST_MAIN_IO_CAPABILITY_KEYBOARD_ONLY             (0x02)
#define SPI_HOST_MAIN_IO_CAPABILITY_NO_INPUT_NO_OUTPUT        (0x03)
#define SPI_HOST_MAIN_IO_CAPABILITY_KEYBOARD_DISPLAY          (0x04)

/* MITM modes */
#define MITM_PROTECTION_NOT_REQUIRED	(0x00)
#define MITM_PROTECTION_REQUIRED        (0x01)

/******************************************************************************
 * Local Variable Declarations
******************************************************************************/
extern volatile uint8_t send_hr_measurement;
uint8 connection_closed = 0; //[SO]fixme!!! add here disconnection bug details
uint8 deviceState = APPL_UNINITIALIZED;

extern RTC_HandleTypeDef RTCHandle;

#if (BLE_CURRENT_PROFILE_ROLES & HEART_RATE_SENSOR)
tHeartRateMeasure heartRateMeasurmentValue;

#ifdef SET_L2CAP_CONN_UPD_REQ_COMMAND /* TBR */
        
/*connection period, arg in msec*/
#define CONN_P(x) ((int)((x)/1.25f))

/* HRM L2CAP Connection Update request parameters used for test */
#define L2CAP_INTERVAL_MIN        CONN_P(1000) //100
#define L2CAP_INTERVAL_MAX        CONN_P(1000) //250
#define L2CAP_SLAVE_LATENCY       0x0000
#define L2CAP_TIMEOUT_MULTIPLIER  800          //0x258

                          
#endif

#endif

static tSecurityParams Security_Param;
uint32 count = 0;
uint8 battRead = 0;
uint8 battIdx;

/******************************************************************************
 * Global Variable Declarations
******************************************************************************/
void *hostdbgfile;
void *profiledbgfile;
void *ftdidbgfile;

extern int Profile_Process_Q_LP; //[SO]fixme!!! add here details on *_Process_Q idle state
extern int HCI_Process_Q_LP;  //[SO]fixme!!! add here details on *_Process_Q idle state

/******************************************************************************
 * Function Declarations
******************************************************************************/
static int Application_Process(void);

/******************************************************************************
 * Function Definitions 
******************************************************************************/
/**
 * @brief 
 * @param event
 * @param evtLen
 * @param evtData
 */
void InitButton(void)
{
#if 0
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* BlueNRG Eval Board (STEVAL-IDB002V1) push-button */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif /* 0 */
}

/**
 * @brief 
 * @param event
 * @param evtLen
 * @param evtData
 */
void BLE_Profiles_Evt_Notify_Cb(tNotificationEvent event,uint8 evtLen,uint8* evtData)
{  
  switch(event)
  {
  case EVT_BLUE_INITIALIZED:
    {
      APPL_MESG_DBG(profiledbgfile,"EVT_BLUE_INITIALIZED\n");
    }
    break;
  case EVT_ADVERTISING_TIMEOUT:
    {
      APPL_MESG_DBG(profiledbgfile,"EVT_ADVERTISING_TIMEOUT\n");
    }
    break;
  case EVT_CONNECTION_COMPLETE:
    {
      APPL_MESG_DBG(profiledbgfile,"EVT_CONNECTION_COMPLETE\n");
      deviceState = APPL_CONNECTED;//uncommented for the time test case csp_bv_02_c(cts).
    }
    break;
  case EVT_DISCONNECTION_COMPLETE:
    {
      APPL_MESG_DBG(profiledbgfile,"EVT_DISCONNECTION_COMPLETE %x\n",evtData[0]);
      deviceState = APPL_INIT_DONE;
    }
    break;
  case EVT_PASSKEY_REQUEST:
    {
      APPL_MESG_DBG(profiledbgfile,"EVT_PASSKEY_REQUEST\n");
      BLE_Profile_Send_Pass_Key(111111);
    }
    break;
  case EVT_PAIRING_COMPLETE:
    {
      APPL_MESG_DBG(profiledbgfile,"EVT_PAIRING_COMPLETE\n");
      deviceState = APPL_CONNECTED;
    }
    break;
#if (BLE_CURRENT_PROFILE_ROLES & HEART_RATE_SENSOR)
  case EVT_HRS_INITIALIZED:
    {
      APPL_MESG_DBG(profiledbgfile,"EVT_HRS_INITIALIZED\n");
      deviceState = APPL_SET_TX_PWR;
    }
    break;
  case EVT_HRS_CHAR_UPDATE_CMPLT:
    {
      APPL_MESG_DBG(profiledbgfile,"EVT_HRS_CHAR_UPDATE_CMPLT %x:%x:%x:%x:%x\n",evtData[0],evtData[1],evtData[2],evtData[3],evtData[4]);
    }
    break;
  case EVT_HRS_RESET_ENERGY_EXPENDED:
    {
      APPL_MESG_DBG(profiledbgfile,"EVT_HRS_RESET_ENERGY_EXPENDED\n");
    }
    break;
#endif		
  }/* end switch() */
}/* end BLE_Profiles_Evt_Notify_Cb() */

/**
 * @brief This function initializes the Heart Rate Profile by calling the
 *        function HRProfile_Init().
 * @param None
 * @param None
 */
void Init_Profile()
{
#if (BLE_CURRENT_PROFILE_ROLES & HEART_RATE_SENSOR)
  PRINTF ("Test Application: Initializing Heart Rate Profile \n" );
  PRINTF ("Initializing Heart Rate Profile \n" );
  
  if (HRProfile_Init((uint8)0xFF,
                     BLE_Profiles_Evt_Notify_Cb,
                     BODY_SENSOR_LOCATION_HAND) == BLE_STATUS_SUCCESS)
  {
    APPL_MESG_DBG(profiledbgfile,"Initialized Heart Rate Profile \n" );
  }
  else
  {
    APPL_MESG_DBG(profiledbgfile,"Failed Heart Rate Profile \n" );
  }
#endif
}/* end Init_Profile() */

/**
 * @brief This function enables advertising.
 * @param None
 * @param None
 */
void Advertize()
{
  uint8 useBoundedDeviceList = 0;
  
#if (BLE_CURRENT_PROFILE_ROLES & HEART_RATE_SENSOR)
  APPL_MESG_DBG(profiledbgfile,"set sensor location\n");
  HRProfile_Set_Body_Sensor_Location(BODY_SENSOR_LOCATION_HAND);
  APPL_MESG_DBG(profiledbgfile,"enable advertising\n");
  HR_Sensor_Make_Discoverable( useBoundedDeviceList );
#endif
}/* end Advertize() */

/**
 * @brief This function initializes the security parameters, initialize the
 *        BLE main profile, initializes the individual profiles and executes the
 *        main processes.
 * @param None
 * @param None
 */
void HRM_Profile_Application (void)
{
  int indx;
  uint32_t uwPRIMASK_Bit;

  while(1){
    deviceState = APPL_UNINITIALIZED;

    HTL_Init();
    
    /* Reset BlueNRG hardware */
    BlueNRG_RST();
    	
    PRINTF ("HRM Profile Application: Starting HRM Profile\n");
    
    /* initialize the security parameters */
    Osal_MemSet((void *)&Security_Param, 0, sizeof(Security_Param));
    Security_Param.ioCapability = SPI_HOST_MAIN_IO_CAPABILITY_DISPLAY_ONLY;
    Security_Param.mitm_mode = MITM_PROTECTION_REQUIRED;
    Security_Param.bonding_mode = 0x01;
    Security_Param.OOB_Data_Present = 0x00;
    Security_Param.Use_Fixed_Pin = 0x00;
    Security_Param.encryptionKeySizeMin = 8;
    Security_Param.encryptionKeySizeMax = 16;
    Security_Param.Fixed_Pin = 111111;
    
    Security_Param.initiateSecurity = NO_SECURITY_REQUIRED;
    for (indx=0;indx<16;indx++){
      Security_Param.OOB_Data[indx] = (uint8)indx;
    }
    
    /* initialize profile */
    BLE_Profile_Init(&Security_Param,BLE_Profiles_Evt_Notify_Cb);
    
    PRINTF ("HRM Profile Application: BLE main profile initialized. \n" );
    
    /* initialize the individual profiles */
    Init_Profile();
	
    do{
      HCI_Process_Q();
      Profile_Process_Q();
      Application_Process();
    } while (deviceState != APPL_ADVERTISING);//[RG:]fixme!!!
    
    do{
      
      //PROFILE_MESG_DBG ( profiledbgfile, "calling profile process Q%d\n",deviceState);
      HCI_Process_Q();
      Profile_Process_Q();
      
      if (
          (Application_Process() != 0)||
          (Profile_Process_Q_LP != 0)||
          (HCI_Process_Q_LP != 0)
         ){
        LPM_Mode_Request(eLPM_MAIN_LOOP_PROCESSES, eLPM_Mode_RUN);
      }else{
        LPM_Mode_Request(eLPM_MAIN_LOOP_PROCESSES, eLPM_Mode_LP_Stop);
      }
          
      LPM_Enter_Mode();
      
    } while (connection_closed == 0); //[SO]fixme!!! add here disconnection bug details
    
	// [SO] FIXME!!! Maybe we can move it to the top of the loop
    uwPRIMASK_Bit = __get_PRIMASK();	/**< backup PRIMASK bit */
    __disable_irq();	/**< Disable all interrupts by setting PRIMASK bit on Cortex*/
    connection_closed = 0;
    __set_PRIMASK(uwPRIMASK_Bit);	/**< Restore PRIMASK bit*/

  } /* end while(1) */
  
}/* end HRM_Profile_Application() */

/**
 * @brief  Application specific processing
 * @param  None
 * @retval int
 */
static int Application_Process(void)
{
  int indx;
  int hr;
  int ret_val = 1;
  int32_t uwPRIMASK_Bit = __get_PRIMASK();	/**< backup PRIMASK bit */

  switch(deviceState)
  {
  case APPL_SET_TX_PWR:
    {
#ifdef SET_TX_POWER_COMMAND
      HCI_LE_Set_TX_Power_Level_Cmd(PID_HEART_RATE_SENSOR,1, 4);
#endif
      deviceState = APPL_INIT_DONE;
    }
    break;  
  case APPL_INIT_DONE:
    {
      Advertize();
      deviceState = APPL_ADVERTISING;
    }
    break;
  case APPL_TERMINATE_LINK:
    {
      /* on disconnection complete, the state will be changed to INIT_DONE
      */ 
      BLE_Profile_Disconnect();
      deviceState = APPL_UNINITIALIZED;
    }
    break;
#ifdef SET_L2CAP_CONN_UPD_REQ_COMMAND /* TBR */
    case APPL_CONNECTED:
      {
        uint16 interval_min = L2CAP_INTERVAL_MIN;
        uint16 interval_max = L2CAP_INTERVAL_MAX;
        uint16 slave_latency = L2CAP_SLAVE_LATENCY;
        uint16 timeout_multiplier = L2CAP_TIMEOUT_MULTIPLIER;
        
        HRProfile_Sent_L2CAP_Conn_Req(interval_min,
                                      interval_max, 
                                      slave_latency,
                                      timeout_multiplier);
      
        deviceState = APPL_WAIT;
        
      }
      break;
#endif    
    default:
#if (BLE_CURRENT_PROFILE_ROLES & HEART_RATE_SENSOR)
      if (send_hr_measurement != 0)
        {
          hr = (rand()&0x1F)+60;

          __disable_irq();			/**< Disable all interrupts by setting PRIMASK bit on Cortex*/
          send_hr_measurement = 0;
          __set_PRIMASK(uwPRIMASK_Bit);	/**< Restore PRIMASK bit*/
          deviceState = APPL_WAIT;

          //HRProfile_Set_Sensor_Contact_Support_Bit(); //for test case TC_CN_BV_05
		  Osal_MemSet((void*)&heartRateMeasurmentValue, 0, sizeof(heartRateMeasurmentValue));
          heartRateMeasurmentValue.valueformat = HRM_VALUE_FORMAT_UINT8;//HRM_VALUE_FORMAT_UINT16;
          heartRateMeasurmentValue.sensorContact = HRM_SENSOR_CONTACT_PRESENT;
          heartRateMeasurmentValue.energyExpendedStatus = HRM_ENERGY_EXPENDED_PRESENT;
          heartRateMeasurmentValue.rrIntervalStatus = HRM_RR_INTERVAL_PRESENT;
          heartRateMeasurmentValue.heartRateValue = hr;//(uint8)0xE4;
          heartRateMeasurmentValue.enrgyExpended = 0x00;
          heartRateMeasurmentValue.numOfRRIntervalvalues = 0x05;

          for (indx=0; indx<9; indx++)
          {
            heartRateMeasurmentValue.rrIntervalValues[indx] = (uint16)(indx + 0);
          }
          HRProfile_Send_HRM_Value (heartRateMeasurmentValue);

        }else{
          ret_val = 0;
        }
#endif
    break;

  }
  return ret_val;
}

/**
 * @brief  Disable wakeup counter and set the wakeup time
 * @param  None
 * @retval None
 */
void LPM_EnterStopMode(void)
{
    /* Disable Wakeup Counter */
    HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);

    /*## Setting the Wake up time ############################################*/
    /*  RTC Wakeup Interrupt Generation:
        Wakeup Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE or LSI))
        Wakeup Time = Wakeup Time Base * WakeUpCounter 
                    = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE or LSI)) * WakeUpCounter
        ==> WakeUpCounter = Wakeup Time / Wakeup Time Base

        To configure the wake up timer to 4s the WakeUpCounter is set to 0x1FFF:
          RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16 
          Wakeup Time Base = 16 /(~39.000KHz) = ~0,410 ms
          Wakeup Time = ~4s = 0,410ms  * WakeUpCounter
          ==> WakeUpCounter = ~4s/0,410ms = 9750 = 0x2616 */
    //HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, 0x2616, RTC_WAKEUPCLOCK_RTCCLK_DIV16); /* Setting the Wake up time to 4s*/
    HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, 0x987, RTC_WAKEUPCLOCK_RTCCLK_DIV16); /* Setting the Wake up time to 1s*/
    //HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, 0x262, RTC_WAKEUPCLOCK_RTCCLK_DIV16); /* Setting the Wake up time to 0.25s*/    
}

/**
 * @brief  Call procedure for configuring the system clock after wake-up
 *         from STOP
 * @param  None
 * @retval None
 */
void LPM_ExitStopMode(void)
{
    /* Configures system clock after wake-up from STOP: enable HSE, PLL and select
    PLL as system clock source (HSE and PLL are disabled in STOP mode) */
    SystemClockConfig_LP_STOP();
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
