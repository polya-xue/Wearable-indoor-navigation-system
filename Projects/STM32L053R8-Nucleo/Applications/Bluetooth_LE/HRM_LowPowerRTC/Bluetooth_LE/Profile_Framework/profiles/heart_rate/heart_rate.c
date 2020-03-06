/******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2013 STMicroelectronics International NV
*
*   FILENAME        -  heart_rate.c
*
*   COMPILER        -  gnu gcc
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      09/03/2012
*   $Revision$:  first version
*   $Author$:    
*   Comments:    Heart Rate Profile Implementation File 
*
*   $Date$:      20/04/2012
*   $Revision$:  Second version
*   $Author$:    
*   Comments:    Redesigned the Heart Rate Profile according to new profile
*                framework.
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  This file will have implementation of Heart Rate Profile. 
* 
******************************************************************************/

/******************************************************************************
 * Include Files
******************************************************************************/
#include <host_config.h>
#include <hal_types.h>
#include <ble_status.h>
#include <hci.h>
#include <host_osal.h>
#include <debug.h>
#include <list.h>
#include <timer.h>
#include <ble_profile.h>
#include <profile_hci_interface.h>
#include <ble_events.h>
#include <uuid.h>
#include <heart_rate.h>

#if (BLE_CURRENT_PROFILE_ROLES & HEART_RATE_SENSOR)
/******************************************************************************
* Macro Declarations
******************************************************************************/
/* Advertising intervals in terms of 625 micro seconds */
#define HRP_FAST_CONN_ADV_INTERVAL_MIN	               (0x20) /* 20ms */
#define HRP_FAST_CONN_ADV_INTERVAL_MAX	               (0x30) /* 30ms */
#define HRP_LOW_PWR_ADV_INTERVAL_MIN	               (0x640) /* 1 second */
#define HRP_LOW_PWR_ADV_INTERVAL_MAX	               (0xFA0) /* 2.5 seconds */

/* heart rate profile main states */
#define HRPROFILE_STATE_UNINITIALIZED                  (0x00)
#define HRPROFILE_STATE_WAITING_BLE_INIT               (0x10)
#define HRPROFILE_STATE_INITIALIZING                   (0x20)
#define HRPROFILE_STATE_INITIALIZED                    (0x30)
#define HRPROFILE_STATE_ADVERTIZE                      (0x40)
#define HRPROFILE_STATE_CONNECTED                      (0x60)
#define HRPROFILE_STATE_EXITING                        (0xF0)
#define HRPROFILE_STATE_INVALID                        (0xFF)

/* substates for HRPROFILE_STATE_INITIALIZING main state */
#define HRPROFILE_STATE_ADDING_HEART_RATE_SERVICE	          (0x21)
#define HRPROFILE_STATE_ADDED_HEART_RATE_SERVICE              (0x22)
#define HRPROFILE_STATE_ADDING_HEART_RATE_CHAR	   	          (0x23)
#define HRPROFILE_STATE_ADDED_HEART_RATE_CHAR	   	          (0x24)
#define HRPROFILE_STATE_ADDING_SENSOR_LOC_CHAR	   	          (0x25)
#define HRPROFILE_STATE_ADDED_SENSOR_LOC_CHAR	   	          (0x26)
#define HRPROFILE_STATE_SETTING_DEFAULT_BSL_CHAR              (0x27)
#define HRPROFILE_STATE_SETTING_DONE_BSL_CHAR                 (0x28)
#define HRPROFILE_STATE_ADDING_CTRL_POINT_CHAR                (0x29)
#define HRPROFILE_STATE_ADDED_CTRL_POINT_CHAR                 (0x2A)
#define HRPROFILE_STATE_INITIALIZATION_ERROR                  (0x2F)

/* substates for HRPROFILE_STATE_INITIALIZED main state */
#define HRPROFILE_STATE_WAITING_FOR_ADVERTISE_STATE           (0x31)

/* substates for HRPROFILE_STATE_ADVERTIZE main state */
#define HRPROFILE_STATE_ADVERTISABLE_IDLE                            (0x41)
#define HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST_DO_START          (0x42)
#define HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST_STARTING          (0x43)
#define HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST                   (0x44)
#define HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP   (0x45)
#define HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST_STOPPING          (0x46)
#define HRPROFILE_STATE_FAST_DISCOVERABLE_DO_START                   (0x47)
#define HRPROFILE_STATE_FAST_DISCOVERABLE_STARTING                   (0x48)
#define HRPROFILE_STATE_FAST_DISCOVERABLE                            (0x49)
#define HRPROFILE_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP            (0x4A)
#define HRPROFILE_STATE_FAST_DISCOVERABLE_STOPPING                   (0x4B)
#define HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_DO_START              (0x4C)
#define HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_STARTING              (0x4D)
#define HRPROFILE_STATE_LOW_POWER_DISCOVERABLE                       (0x4E)
#define HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_TIMEOUT_DO_STOP       (0x4F)
#define HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_STOPPING              (0x50)
#define HRPROFILE_STATE_BLE_ADVERTISING                              (0x51)
#define HRPROFILE_STATE_PROCESSING_PAIRING                           (0x52)

/* substates for HRPROFILE_STATE_CONNECTED main state */
#define HRPROFILE_STATE_CONNECTED_IDLE                      (0x61)
#define HRPROFILE_STATE_DO_DISCONNECT                       (0x65)
#define HRPROFILE_STATE_DISCONNECTING                       (0x66)
#define HRPROFILE_STATE_DISCONNECTED                        (0x67)

/******************************************************************************
 * type definitions
******************************************************************************/
typedef struct _tHeartRateContext
{
	/**
	 * main state of the heart rate
	 * sensor state machine
	 */ 
    tProfileState mainState;
	
	/**
	 * sub state of the heart rate
	 * sensor state machine
	 */ 
    tProfileState subState;
    
	/**
	 * FOR INTERNAL USE ONLY
	 * the sensor location information is not sent by
	 * default in the heart rate measurement. If it needs to
	 * be sent, the application needs to enable it before 
	 * sending out any heart rate measurement updates.
	 * this flag keeps track of the communication status
	 * of the heart rate measurement
     */
	uint8 hrmCommunicationStarted;
	
	/**
	 * callback to notify the applciation with
	 * the profile specific events
	 */ 
    BLE_CALLBACK_FUNCTION_TYPE applicationNotifyFunc;

    /**
	 * connection handle
	 */ 
    uint16 connHandle;
    /**
	 * the characteristics supported in
	 * the heart rate service
	 */ 
    uint16 serviceSupport;
	
	/**
	 * heart rate service handle
	 */ 
	uint16 hrServiceHandle;
	
	/**
	 * heart rate measurement characteristic
	 * handle
	 */ 
	uint16 hrmCharHandle;
	
	/**
	 * heart rate measurement characteristic
	 * descriptor handle
	 */ 
    uint16 hrmCharDescHandle;
	
	/**
	 * body sensor location 
	 * characteristic handle
	 */ 
	uint16 bodySensorLocCharHandle;
	
	/**
	 * control point characteristic
	 * handle
	 */ 
	uint16 ctrlPointCharHandle;
	
	/**
	 * device information characteristic
	 * handle
	 */ 
    uint16 devInfoServiceHandle;
	
	/**
	 * manufacturer string characteristic
	 * handle of the device information service
	 */ 
	uint16 manufacturerStringCharHandle;
	
	/**
	 * sensor contact support flag
	 * when set, the sensor location
	 * information is included in the
	 * heart rate measurement characteristic
	 */ 
    uint8 sensorContactSupport;
    
	/**
	 * will contain the timer ID of the 
	 * timer started by the heart rate profile
	 */ 
    tTimerID timerID;
	
	/**
	 * profile interface context. This is
	 * registered with the main profile
	 */ 
    tProfileInterfaceContext HRStoBLEInf;
	
	/**
	 * body sensor location
	 */ 
	tBodySensorLocation hrsBodySensorLocation;
}tHeartRateContext;

/******************************************************************************
* Variable Declarations
******************************************************************************/
static tHeartRateContext heartRate;

/******************************************************************************
* Imported Variable
******************************************************************************/
extern void *profiledbgfile;

/******************************************************************************
 * Function Prototypes
******************************************************************************/
static void HRProfile_Rx_Event_Handler(uint8 event, 
									   tHciResponsePacket* response);

static tBleStatus HRProfile_ComComplete_Event_Handler(uint16 opcode,
													  tHciResponsePacket *response);

static tBleStatus HRProfile_CmdStatus_Event_Handler(uint16 opcode,
													uint8 status);

static void HRProfile_Advertise_Period_Timeout_Handler (void);

/*******************************************************************************
* SAP
*******************************************************************************/
tBleStatus HRProfile_Init(uint8 featureSupportFlag,
						  BLE_CALLBACK_FUNCTION_TYPE hrProfileCallbackFunc, 
						  tBodySensorLocation sensorLocation) 
{
    tBleStatus retval = BLE_STATUS_FAILED;
	
    HRPROFILE_MESG_DBG ( profiledbgfile, "HRProfile_Init\n" );

    if (Is_Profile_Present(PID_HEART_RATE_SENSOR) == BLE_STATUS_SUCCESS)
	{
		return (BLE_STATUS_PROFILE_ALREADY_INITIALIZED);
	}

    if (hrProfileCallbackFunc == NULL)
    {
        return (BLE_STATUS_NULL_PARAM);
    }
	
    /* Initialize Heart Rate Profile's Data Structures */
    Osal_MemSet(&heartRate,0,sizeof(tHeartRateContext));
    heartRate.applicationNotifyFunc = hrProfileCallbackFunc;
    heartRate.serviceSupport = 0x0000;
    heartRate.sensorContactSupport = 0x00;
 
    if (featureSupportFlag & BODY_SENSOR_LOCATION_SUPPORT_MASK)
    {
        heartRate.serviceSupport |= BODY_SENSOR_LOCATION_SUPPORT_MASK;
        if (sensorLocation <= BODY_SENSOR_LOCATION_FOOT)
        {
            heartRate.hrsBodySensorLocation = sensorLocation;
        }
    }
    if (featureSupportFlag & ENERGY_EXTENDED_INFO_SUPPORT_MASK)
    {
        heartRate.serviceSupport |= ENERGY_EXTENDED_INFO_SUPPORT_MASK;
    }

    /* Initialize Heart Rate Profile to BLE main Profile */
    Osal_MemSet ( &heartRate.HRStoBLEInf, 0, sizeof(heartRate.HRStoBLEInf) );
	heartRate.HRStoBLEInf.profileID = PID_HEART_RATE_SENSOR;
    heartRate.HRStoBLEInf.callback_func = HRProfile_Rx_Event_Handler;
    heartRate.HRStoBLEInf.voteForAdvertisableState = 0;
	/* manufacturer name characteristic
	 * has to be added to the device information service
	 */ 
	heartRate.HRStoBLEInf.DISCharRequired = (MANUFACTURER_NAME_STRING_CHAR_MASK);
    retval = BLE_Profile_Register_Profile ( &heartRate.HRStoBLEInf );
    if (retval != BLE_STATUS_SUCCESS)
    {
        /* Profile could not be registered with the BLE main profile */
        return (retval);
    }

    /* Initialize profile's state machine's state variables */
    heartRate.mainState = HRPROFILE_STATE_WAITING_BLE_INIT;
    heartRate.subState = HRPROFILE_STATE_WAITING_BLE_INIT;

    HRPROFILE_MESG_DBG ( profiledbgfile, "Heart Rate Profile is Initialized \n");
	
    return (BLE_STATUS_SUCCESS);
}

tBleStatus HR_Sensor_Make_Discoverable(uint8 useBoundedDeviceList)
{
    tBleStatus retval = BLE_STATUS_FAILED;
    tHciResponsePacket response;


    if ((heartRate.mainState == HRPROFILE_STATE_ADVERTIZE)  &&  
        (heartRate.subState == HRPROFILE_STATE_ADVERTISABLE_IDLE))
    {
        HRPROFILE_MESG_DBG ( profiledbgfile, "HRP Set Discoverable mode: %d \n", useBoundedDeviceList );
        if ((useBoundedDeviceList) && (gBLEProfileContext.bleSecurityParam.bonding_mode))
        {
            heartRate.subState = HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST_DO_START;
        }
        else
        {
            heartRate.subState = HRPROFILE_STATE_FAST_DISCOVERABLE_DO_START;
        }
        /* Call profile's State Machine engine for state change to get into affect */
        retval = HRProfile_ComComplete_Event_Handler ( HCI_CMD_NO_OPERATION, &response );
       
    }
    else
    {
        HRPROFILE_MESG_DBG ( profiledbgfile, "HRP Set Discoverable mode: Invalid Task State \n");
    }

    return (retval);
}

tBleStatus HRProfile_Set_Body_Sensor_Location(tBodySensorLocation bdsValue)
{
    tBleStatus retval = BLE_STATUS_INVALID_PARAMS;
    
    HRPROFILE_MESG_DBG ( profiledbgfile, "Body Sensor Location is to update !! \n");
    
    /* check whether the body sensor location value can be accepted for char update of not */
    /* body sensor contact char is support ?? */
    /* profile's main and sub state ?? */
    if((heartRate.serviceSupport & BODY_SENSOR_LOCATION_SUPPORT_MASK) &&
      ((heartRate.mainState == HRPROFILE_STATE_INITIALIZED) || (heartRate.mainState == HRPROFILE_STATE_ADVERTIZE)) &&
       (bdsValue <= BODY_SENSOR_LOCATION_FOOT))
	{
        heartRate.hrsBodySensorLocation = bdsValue;

        retval = HCI_GATT_Update_CharVal_Cmd (PID_HEART_RATE_SENSOR, heartRate.hrServiceHandle, 
                                                  heartRate.bodySensorLocCharHandle, 
                                                  (uint8)0,
                                                  (uint8)1, /* byte */
                                                  (uint8 *)&heartRate.hrsBodySensorLocation);
        if (retval == BLE_STATUS_SUCCESS)
        {
            HRPROFILE_MESG_DBG ( profiledbgfile, "Body sensor location is req of update \n");
        }
        else
        {
            HRPROFILE_MESG_DBG ( profiledbgfile, "FAILED to update body sensor location %02X \n", retval);
        }
    }
    else
    {
        HRPROFILE_MESG_DBG ( profiledbgfile, "Heart Rate Sensor is not in proper state to update Body Sensor Location \n");
        retval = BLE_STATUS_FAILED;
    }

    return (retval);

}

tBleStatus HRProfile_Set_Sensor_Contact_Support_Bit (void)
{
    if (heartRate.hrmCommunicationStarted == 0)
    {
        heartRate.sensorContactSupport = 0x01;
        return (BLE_STATUS_SUCCESS);
    }
    else
    {
        return (BLE_STATUS_FAILED);
    }
}

tBleStatus HRProfile_Send_HRM_Value ( tHeartRateMeasure heartRateVal )
{
    uint8 hrmCharLength = 0;
    uint8 hrmCharValue[30] = {0};
    uint16 rrValTemp = 0;
    uint8 maxRRIntervalValues = 0;
    uint8 indx = 0;
    tBleStatus retval = BLE_STATUS_INVALID_PARAMS;

    HRPROFILE_MESG_DBG ( profiledbgfile, "Received a Heart Rate Measurement value to send \n");

    if ((heartRate.mainState == HRPROFILE_STATE_CONNECTED) && 
         (heartRate.subState == HRPROFILE_STATE_CONNECTED_IDLE))
    {
        {
            /* set the status that heart rate measurment value
             * communication has already started */
            heartRate.hrmCommunicationStarted = 1;

            /* reset the characteristic buffer and its length value. */
            Osal_MemSet ( (void*)hrmCharValue, 0, 30);
            hrmCharLength = 1;

            /* Heart Rate Measurment Value */
            if (heartRateVal.valueformat == HRM_VALUE_FORMAT_UINT8)
            {
                hrmCharValue[hrmCharLength] = (uint8)heartRateVal.heartRateValue;
                hrmCharLength++;
            }
            else if (heartRateVal.valueformat == HRM_VALUE_FORMAT_UINT16)
            {
                hrmCharValue[0] |= HRM_VALUE_FORMAT_UINT16;
                hrmCharValue[hrmCharLength] = (uint8)(heartRateVal.heartRateValue & 0xFF);
                hrmCharLength++;
                hrmCharValue[hrmCharLength] = (uint8)(heartRateVal.heartRateValue >> 8);
                hrmCharLength++;
            }
            else
            {
                HRPROFILE_MESG_DBG ( profiledbgfile, "Invalid HRM value format \n");
                return (retval);
            }

            /* Sensor Contact */
            if (heartRate.sensorContactSupport)
            {
                hrmCharValue[0] |= HRM_SENSOR_CONTACT_SUPPORTED;
                if (heartRateVal.sensorContact == HRM_SENSOR_CONTACT_NOT_PRESENT)
                {
                    hrmCharValue[0] |= HRM_SENSOR_CONTACT_NOT_PRESENT;
                }
                else if (heartRateVal.sensorContact == HRM_SENSOR_CONTACT_PRESENT)
                {
                    hrmCharValue[0] |= HRM_SENSOR_CONTACT_PRESENT;
                }
                else
                {
                    HRPROFILE_MESG_DBG ( profiledbgfile, "Invalid value for sensor contact \n");
                    return (retval);
                }
            }

            /* Energy Extended */
            if (heartRate.serviceSupport & ENERGY_EXTENDED_INFO_SUPPORT_MASK)
            {
                if (heartRateVal.energyExpendedStatus == HRM_ENERGY_EXPENDED_NOT_PRESENT)
                {
                    hrmCharValue[0] |= HRM_ENERGY_EXPENDED_NOT_PRESENT;
                }
                else if (heartRateVal.energyExpendedStatus == HRM_ENERGY_EXPENDED_PRESENT)
                {
					HRPROFILE_MESG_DBG ( profiledbgfile, "HRM energy expended is present \n");
                    hrmCharValue[0] |= HRM_ENERGY_EXPENDED_PRESENT;
                    hrmCharValue[hrmCharLength] = (uint8)(heartRateVal.enrgyExpended & 0xFF);
                    hrmCharLength++;
                    hrmCharValue[hrmCharLength] = (uint8)(heartRateVal.enrgyExpended >> 8);
                    hrmCharLength++;
                }
                else
                {
                    HRPROFILE_MESG_DBG ( profiledbgfile, "Invalid Value for energy expended parameter \n");
                    return (retval);
                }
            }

            /* RR Interval Values */
            if (heartRateVal.rrIntervalStatus == HRM_RR_INTERVAL_NOT_PRESENT)
            {
                hrmCharValue[0] |= HRM_RR_INTERVAL_NOT_PRESENT;
            }
            else if (heartRateVal.rrIntervalStatus == HRM_RR_INTERVAL_PRESENT)
            {
                hrmCharValue[0] |= HRM_RR_INTERVAL_PRESENT;
                maxRRIntervalValues = 9;
                if (heartRateVal.valueformat == HRM_VALUE_FORMAT_UINT16)
                {
                    maxRRIntervalValues--;
                }

                if (hrmCharValue[0] & HRM_ENERGY_EXPENDED_PRESENT)
                {
                    maxRRIntervalValues--;
                }

                if (maxRRIntervalValues > heartRateVal.numOfRRIntervalvalues)
                {
                    /* use only max applowed values and most latest values */
                    maxRRIntervalValues = heartRateVal.numOfRRIntervalvalues;
                }

                for ( indx = 0 ; indx < maxRRIntervalValues ; indx++ )
                {
                    rrValTemp = heartRateVal.rrIntervalValues[indx];
                    hrmCharValue[hrmCharLength] = (uint8)(rrValTemp & 0xFF);
                    hrmCharLength++;
                    hrmCharValue[hrmCharLength] = (uint8)(rrValTemp >> 8);
                    hrmCharLength++;
                }
            }
            else
            {
                HRPROFILE_MESG_DBG ( profiledbgfile, "Invalid value for RR Interval Present parameter \n");
                return (retval);
            }
        }

        retval = HCI_GATT_Update_CharVal_Cmd (PID_HEART_RATE_SENSOR, heartRate.hrServiceHandle, 
                                                  (heartRate.hrmCharHandle), 
                                                  (uint8)0,
                                                  hrmCharLength, 
                                                  (uint8 *)&hrmCharValue[0] 
                                                 );
        if (retval == BLE_STATUS_SUCCESS)
        {
            HRPROFILE_MESG_DBG ( profiledbgfile, "HRM value sent successfully \n");
       
            /* start a timer to count starvation of heart rate measurment value timeout. */
            /*Timer_Start_Timeout (20, HRProfile_Device_Disconnect);*/
        }
        else
        {
            HRPROFILE_MESG_DBG ( profiledbgfile, "FAILED to send HRM value %02X \n", retval);
        }
    }
    else
    {
        HRPROFILE_MESG_DBG ( profiledbgfile, "Heart Rate Profile is not in proper state to send HRM value \n");
        retval = BLE_STATUS_FAILED;
    }

    return (retval);
}

/******************************************************************************
* Local Functions
******************************************************************************/
/**
 * HRProfile_Rx_Event_Handler
 * 
 * @param[in] event : event received from the controller
 * @param[in] response : pointer to the event data
 * 
 * parses the events received from HCI according
 * to the profile's state machine
 */ 
void HRProfile_Rx_Event_Handler(uint8 event, 
								tHciResponsePacket* response )
{
    uint16 attrHandle = 0;
    uint8 attValLen = 0;
    uint8 attValue = 0;
    
    HRPROFILE_MESG_DBG(profiledbgfile, "Heart Rate Sensor States: %02X %02X \n", 
                         heartRate.mainState, heartRate.subState );

    switch(event)
    {
        case HCI_EVT_DISCONN_COMPLETE:
        {
            HRPROFILE_MESG_DBG(profiledbgfile, "Received event HCI_EVT_DISCONN_COMPLETE \n");
            if ( ( (heartRate.mainState == HRPROFILE_STATE_ADVERTIZE) && 
                   (heartRate.subState == HRPROFILE_STATE_PROCESSING_PAIRING) ) || 
                 ( heartRate.mainState == HRPROFILE_STATE_CONNECTED ) )
            {
                    if (response->evtPkt.disconnComplt.status == BLE_STATUS_SUCCESS)
                    {
                        /* ENTER CRITICAL SECTION */
                        heartRate.mainState = HRPROFILE_STATE_ADVERTIZE;
                        heartRate.subState = HRPROFILE_STATE_ADVERTISABLE_IDLE;

                        heartRate.sensorContactSupport = 0;
                        heartRate.hrmCommunicationStarted = 0;
                    }
            }
        }
        break;
        case HCI_EVT_CMD_COMPLETE:
        {
            HRProfile_ComComplete_Event_Handler ( response->evtPkt.commandComplete.commandOpcode,
                                                  response );
        }
        break;
        case HCI_EVT_CMD_STATUS:
        {
            HRPROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_CMD_STATUS \n");
            HRProfile_CmdStatus_Event_Handler ( response->evtPkt.commandStatus.commandOpcode, 
                                                response->evtPkt.commandStatus.status );
        }
        break;
        case HCI_EVT_LE_META_EVENT:
        {
            switch(response->evtPkt.leEvent.subEventCode)
            {
                case HCI_EVT_LE_CONN_COMPLETE:
                {
					/* check event status */
					if (response->evtPkt.leEvent.leMetaEvtPkt.leConnectionComplete.status == BLE_STATUS_SUCCESS)
					{
						heartRate.connHandle = response->evtPkt.leEvent.leMetaEvtPkt.leConnectionComplete.connectionHandle;
						/* if the main profile is in the connected state that means
						 * we need not wait for pairing
						 */ 
						if(BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTED)
						{
							heartRate.mainState = HRPROFILE_STATE_CONNECTED;
                            heartRate.subState = HRPROFILE_STATE_CONNECTED_IDLE;
						}
						else
						{
							heartRate.subState = HRPROFILE_STATE_PROCESSING_PAIRING;
						}
					}
					else
					{
						/* connection was not successful, now we need to wait for
						 * the application to put the device in discoverable mode
						 */
						heartRate.subState = HRPROFILE_STATE_ADVERTISABLE_IDLE;
					}
					/* Here there is not need to do anything extra if this profile is not
					 * controlling the advertising process. Above if-else statement does
					 * the needful. */
					/* Stop if any timer is running */
                    
                }
                break; /* HCI_EVT_LE_CONN_COMPLETE */
            } /* switch(response->leEvent.subEventCode) */
        }
        break; /* HCI_EVT_LE_META_EVENT */
        case HCI_EVT_VENDOR_SPECIFIC:
        {
            switch (response->evtPkt.vendorSpecificEvt.vsEventCode)
            {
                case HCI_EVT_BLUE_GAP_PAIRING_CMPLT:
                {
                    HRPROFILE_MESG_DBG( profiledbgfile, "HCI_EVT_BLUE_GAP_PAIRING_CMPLT \n");
                    if ((heartRate.mainState == HRPROFILE_STATE_ADVERTIZE) && 
                        (heartRate.subState == HRPROFILE_STATE_PROCESSING_PAIRING))
                    {
                        if (response->evtPkt.vendorSpecificEvt.vsEvtPkt.status == BLE_STATUS_SUCCESS)
                        {
							HRPROFILE_MESG_DBG( profiledbgfile, "HCI_EVT_BLUE_GAP_PAIRING_CMPLT success\n");
                            heartRate.mainState = HRPROFILE_STATE_CONNECTED;
                            heartRate.subState = HRPROFILE_STATE_CONNECTED_IDLE;
                        }
                    }
                }
                break; /* HCI_EVT_BLUE_GAP_PAIRING_CMPLT */
                case HCI_EVT_BLUE_GATT_WRITE_PERMIT_REQ:
                {
                    HRPROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_GATT_WRITE_PERMIT_REQ \n");
                    attrHandle = response->evtPkt.vendorSpecificEvt.vsEvtPkt.gattWritePermitReq.attHandle;
                    attValLen = response->evtPkt.vendorSpecificEvt.vsEvtPkt.gattWritePermitReq.dataLength;
                    attValue = *(uint8 *)(response->evtPkt.vendorSpecificEvt.vsEvtPkt.gattWritePermitReq.attData);

                    if ( (attrHandle == (heartRate.ctrlPointCharHandle + 1)) && 
                         (heartRate.mainState == HRPROFILE_STATE_CONNECTED) && 
                         (heartRate.subState == HRPROFILE_STATE_CONNECTED_IDLE))
                    {
                        HRPROFILE_MESG_DBG ( profiledbgfile, "Received write req to HRM Control Point Char. \n");
                        if (attValue == HR_CNTL_POINT_RESET_ENERGY_EXPENDED)
                        {
                            /* received a correct value for HRM control point char */
                            HRPROFILE_MESG_DBG ( profiledbgfile, "Received Reset Energy Expended Value Command \n");
                            HCI_GATT_Write_Response_Cmd ( PID_HEART_RATE_SENSOR,
							                              heartRate.connHandle,
														  attrHandle, 
                                                          (uint8)GATT_WRITE_RESP_STATUS_NO_ERROR, 
                                                          (uint8)HRM_CNTL_POINT_VALUE_IS_SUPPORTED, 
                                                          attValLen, 
                                                          (uint8 *)&attValue );
                            /* Notify to application to Reaseat The Energy Expended Value */
                            heartRate.applicationNotifyFunc(EVT_HRS_RESET_ENERGY_EXPENDED,0,NULL);
                        }
                        else
                        {
                            /* received value of HRM control point char is incorrect */
                            HCI_GATT_Write_Response_Cmd ( PID_HEART_RATE_SENSOR,
							                              heartRate.connHandle,
							                               attrHandle, 
                                                          (uint8)GATT_WRITE_RESP_STATUS_VALUE_ERROR, 
                                                          (uint8)HRM_CNTL_POINT_VALUE_NOT_SUPPORTED, 
                                                          attValLen, 
                                                          (uint8 *)&attValue );
                        }
                    }
                }
                break;
            }
        }
        break; /* HCI_EVT_VENDOR_SPECIFIC */
    }
}

/**
 * HRProfile_ComComplete_Event_Handler
 * 
 * @param[in] opcode : opcode of the command for which the command complete
 *            event has been received
 * @param[in] response : pointer to the event data
 * 
 * parses the command complete event according to the profile's state machine
 */ 
tBleStatus HRProfile_ComComplete_Event_Handler(uint16 opcode,
											   tHciResponsePacket *response )
{
    uint8 cmdCmpltStatus = BLE_STATUS_SUCCESS;
    tBleStatus hciCmdResult = BLE_STATUS_FAILED;
    uint16 UUID = 0;
    uint8 tempBuf[20] = {0};
    uint8 serviceMaxAttributeRecord = 0;

    if(response != NULL)
	{
		cmdCmpltStatus = response->evtPkt.commandComplete.status;
	}
	
    /* heartRate.mainState, heartRate.subState */
    switch (heartRate.mainState)
    {
        case HRPROFILE_STATE_UNINITIALIZED:
        break;
        case HRPROFILE_STATE_WAITING_BLE_INIT:
        {
                /* chech whether the main profile has done with ble init or not */
            if (BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_INIT_PROFILE)
            {
                /* Add Heart Rate Service */
                HRPROFILE_MESG_DBG( profiledbgfile, "Adding Heart Rate Service \n");

                /* Max_Attribute_Records = 2*no_of_char + 1 */
                /* serviceMaxAttributeRecord = 1 for heart rate service +
                 *                             2 for hear rate measurment characteristic +
                 *                             1 for client char configuration descriptor +
                 *                             2 for body sensor location characteristic + 
                 *                             2 for control point characteristic
                 */
                serviceMaxAttributeRecord = 4;

                if (heartRate.serviceSupport & BODY_SENSOR_LOCATION_SUPPORT_MASK)
                {   /* for optional body sensor location characteristic */
                    serviceMaxAttributeRecord += 2;
                }
                if (heartRate.serviceSupport & ENERGY_EXTENDED_INFO_SUPPORT_MASK)
                {
                    /* for optional heart rate control point characteristic */
                    serviceMaxAttributeRecord += 2;
                }

                /* store the heart rate service UUID in LE format */ 
                UUID = HEART_RATE_SERVICE_UUID;

                hciCmdResult = HCI_GATT_Add_Service_Cmd ( PID_HEART_RATE_SENSOR,UUID_TYPE_16, 
                                                          (uint8 *)&UUID, 
                                                          PRIMARY_SERVICE, 
                                                          serviceMaxAttributeRecord
                                                        );
                if (hciCmdResult == BLE_STATUS_SUCCESS)
                {
                    /* change profiles main and sub states */
                    heartRate.mainState = HRPROFILE_STATE_INITIALIZING;
                    heartRate.subState = HRPROFILE_STATE_ADDING_HEART_RATE_SERVICE;
                }
            }
        }
        break; /* HRPROFILE_STATE_WAITING_BLE_INIT */
        case HRPROFILE_STATE_INITIALIZING:
        {
            switch (heartRate.subState)
            {
                case HRPROFILE_STATE_ADDING_HEART_RATE_SERVICE:
                {
                    if (opcode == HCI_VENDOR_CMD_GATT_ADD_SERVICE)
                    {
                        /* check for command status also, its important here */
                        if (cmdCmpltStatus == BLE_STATUS_SUCCESS)
                        {
                            HRPROFILE_MESG_DBG (profiledbgfile, "Heart Rate Service is added Successfully \n");

                            heartRate.hrServiceHandle = 
                            response->evtPkt.commandComplete.cmdCmpltPacket.gattAddServiceResp.serviceHandle;

                            HID_DEVICE_MESG_ERR ( profiledbgfile, "Heart Rate Service Handle: %04X !!\n", 
                                                  heartRate.hrServiceHandle );

                            /* change profile's sub states */
                            heartRate.subState = HRPROFILE_STATE_ADDED_HEART_RATE_SERVICE;
                        }
                        else
                        {
                            HRPROFILE_MESG_ERR (profiledbgfile,"FAILED to add Heart Rate Service: Error: %02X !!\n", cmdCmpltStatus);
                            heartRate.subState = HRPROFILE_STATE_INITIALIZATION_ERROR;
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                case HRPROFILE_STATE_ADDED_HEART_RATE_SERVICE:
                {
                    /* Add Heart Rate Measurment Characteristic */
                    HRPROFILE_MESG_DBG (profiledbgfile,"Adding Heart Rate Measurement Characteristic. \n");

                    UUID = HEART_RATE_MEASURMENT_UUID;
                    hciCmdResult = 
                    HCI_GATT_Add_Characterisitic_Cmd ( PID_HEART_RATE_SENSOR,heartRate.hrServiceHandle, 
                                                       UUID_TYPE_16, 
                                                       (uint8 *)&UUID , 
                                                       (uint8)30, /* size of the characteristic flags(1b)+measure(2b)+EE(2b)+RR(7b)*/
                                                       NOTIFY, 
                                                       ATTR_PERMISSION_NONE, 
                                                       GATT_DONT_NOTIFY_EVENTS, 
                                                       10, 
                                                       CHAR_VALUE_LEN_VARIABLE
                                                     );
                    if (hciCmdResult == BLE_STATUS_SUCCESS)
                    {
                        /* change profile's sub states */
                        heartRate.subState = HRPROFILE_STATE_ADDING_HEART_RATE_CHAR;
                    }
                }
                break;
                case HRPROFILE_STATE_ADDING_HEART_RATE_CHAR:
                {
                    if(opcode == HCI_VENDOR_CMD_GATT_ADD_CHAR)
                    {
                        /* check for command status also, its important here */
                        if (cmdCmpltStatus == BLE_STATUS_SUCCESS)
                        {
                            HRPROFILE_MESG_DBG (profiledbgfile, "Heart Rate Measurement Characteristic Added Successfully \n");

                            heartRate.hrmCharHandle = 
                            response->evtPkt.commandComplete.cmdCmpltPacket.gattAddCharResp.charHandle;

                            HID_DEVICE_MESG_ERR ( profiledbgfile, "Heart Rate Measurement Char Handle: %04X !!\n", 
                                                  heartRate.hrmCharHandle );

                            /* change profile's sub states */
                            heartRate.subState = HRPROFILE_STATE_ADDED_HEART_RATE_CHAR;
                        }
                        else
                        {
                            HRPROFILE_MESG_ERR (profiledbgfile,"FAILED to add Heart Rate Measurement Characteristic: Error: %02X !!\n", cmdCmpltStatus);
                            heartRate.subState = HRPROFILE_STATE_INITIALIZATION_ERROR;
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                case HRPROFILE_STATE_ADDED_HEART_RATE_CHAR:
                {
                    if (heartRate.serviceSupport & BODY_SENSOR_LOCATION_SUPPORT_MASK)
                    {
                        /* Add Sensor Location Characteristic */
                        HRPROFILE_MESG_DBG (profiledbgfile,"Adding Sensor Location Characteristic. \n");

                        UUID = SENSOR_LOCATION_UUID;
                        hciCmdResult = 
                        HCI_GATT_Add_Characterisitic_Cmd ( PID_HEART_RATE_SENSOR,heartRate.hrServiceHandle, 
                                                           UUID_TYPE_16, 
                                                           (uint8 *)&UUID, 
                                                           (uint8)1, /* bytes */
                                                           READ, 
                                                           ATTR_PERMISSION_NONE, 
                                                           GATT_DONT_NOTIFY_EVENTS, 
                                                           10,
                                                           CHAR_VALUE_LEN_CONSTANT
                                                         );
                        if (hciCmdResult == BLE_STATUS_SUCCESS)
                        {
                            /* change profile's sub states */
                            heartRate.subState = HRPROFILE_STATE_ADDING_SENSOR_LOC_CHAR;
                        }
                    }
                    else
                    {
                        /* change profile's sub states */
                        heartRate.subState = HRPROFILE_STATE_SETTING_DONE_BSL_CHAR;
                        HRProfile_ComComplete_Event_Handler ( HCI_CMD_NO_OPERATION, response );
                    }
                }
                break;
                case HRPROFILE_STATE_ADDING_SENSOR_LOC_CHAR:
                {
                    if(opcode == HCI_VENDOR_CMD_GATT_ADD_CHAR)
                    {
                        /* check for command status also, its important here */
                        if (cmdCmpltStatus == BLE_STATUS_SUCCESS)
                        {
                            HRPROFILE_MESG_DBG (profiledbgfile, "Sensor Location Characteristic Added Successfully \n");

                            heartRate.bodySensorLocCharHandle = 
                            response->evtPkt.commandComplete.cmdCmpltPacket.gattAddCharResp.charHandle;

                            HID_DEVICE_MESG_ERR ( profiledbgfile, "Body Sensor Location Char Handle: %04X !!\n", 
                                                  heartRate.bodySensorLocCharHandle );

                            /* change profile's sub states */
                            heartRate.subState = HRPROFILE_STATE_ADDED_SENSOR_LOC_CHAR;
                        }
                        else
                        {
                            HRPROFILE_MESG_ERR (profiledbgfile,"FAILED to add Sensor Location Characteristic: Error: %02X !!\n", cmdCmpltStatus);
                            heartRate.subState = HRPROFILE_STATE_INITIALIZATION_ERROR;
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                case HRPROFILE_STATE_ADDED_SENSOR_LOC_CHAR:
                {
                    HRPROFILE_MESG_DBG (profiledbgfile,"Setting default Body Sensor Location. \n");
					HRPROFILE_MESG_DBG (profiledbgfile,"Body sensor value is %d \n",heartRate.hrsBodySensorLocation); 

                    hciCmdResult = HCI_GATT_Update_CharVal_Cmd ( PID_HEART_RATE_SENSOR,heartRate.hrServiceHandle, 
                                                                 heartRate.bodySensorLocCharHandle, 
                                                                 (uint8)0,
                                                                 (uint8)1, /* byte */
                                                                 (uint8 *)&heartRate.hrsBodySensorLocation  );
                    if (hciCmdResult == BLE_STATUS_SUCCESS)
                    {
                        heartRate.subState = HRPROFILE_STATE_SETTING_DEFAULT_BSL_CHAR;
                    }
                }
                break;
                case HRPROFILE_STATE_SETTING_DEFAULT_BSL_CHAR:
                {
                    if(opcode == HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL)
                    {
                        if (cmdCmpltStatus == BLE_STATUS_SUCCESS)
                        {
                            HRPROFILE_MESG_DBG (profiledbgfile, "Body Sensor Location has been set Successfully \n");

                            heartRate.subState = HRPROFILE_STATE_SETTING_DONE_BSL_CHAR;
                        }
                        else
                        {
                            HRPROFILE_MESG_ERR (profiledbgfile,"FAILED to set default Body Sensor Location: Error: %02X !!\n", cmdCmpltStatus);
                            heartRate.subState = HRPROFILE_STATE_INITIALIZATION_ERROR;
                            break;
                        }
                    }
                }
                case HRPROFILE_STATE_SETTING_DONE_BSL_CHAR:
                {
                    if (heartRate.serviceSupport & ENERGY_EXTENDED_INFO_SUPPORT_MASK)
                    {
                        HRPROFILE_MESG_DBG (profiledbgfile,"Adding Control Point Characteristic. \n");
                        
                        UUID = CONTROL_POINT_UUID;
                        hciCmdResult = 
                        HCI_GATT_Add_Characterisitic_Cmd ( PID_HEART_RATE_SENSOR,heartRate.hrServiceHandle, 
                                                           UUID_TYPE_16, 
                                                           (uint8 *)&UUID, 
                                                           (uint8)1, /* bytes */
                                                           WRITE, 
                                                           ATTR_PERMISSION_NONE,
                                                           GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP, 
                                                           10,
                                                           CHAR_VALUE_LEN_CONSTANT
                                                           );
                        if (hciCmdResult == BLE_STATUS_SUCCESS)
                        {
                            /* change profile's sub states */
                            heartRate.subState = HRPROFILE_STATE_ADDING_CTRL_POINT_CHAR;
                        }
                    }
                    else
                    {
                        /* change profile's sub states */
                        heartRate.subState = HRPROFILE_STATE_ADDED_CTRL_POINT_CHAR;
                        HRProfile_ComComplete_Event_Handler ( HCI_CMD_NO_OPERATION, response );
                    }
                }
                break;
                case HRPROFILE_STATE_ADDING_CTRL_POINT_CHAR:
                {
                    if(opcode == HCI_VENDOR_CMD_GATT_ADD_CHAR)
                    {
                        /* check for command status also, its important here */
                        if (cmdCmpltStatus == BLE_STATUS_SUCCESS)
                        {
                            HRPROFILE_MESG_DBG (profiledbgfile, "Control Point Characteristic Added Successfully \n");

                            heartRate.ctrlPointCharHandle = 
                            response->evtPkt.commandComplete.cmdCmpltPacket.gattAddCharResp.charHandle;

                            HID_DEVICE_MESG_ERR ( profiledbgfile, "Control Point Char Handle: %04X !!\n", 
                                                  heartRate.ctrlPointCharHandle );

                            /* change profile's sub states */
                            heartRate.subState = HRPROFILE_STATE_ADDED_CTRL_POINT_CHAR;
                        }
                        else
                        {
                            HRPROFILE_MESG_ERR (profiledbgfile,"FAILED to add Control Point Characteristic: Error: %02X !!\n", cmdCmpltStatus);
                            heartRate.subState = HRPROFILE_STATE_INITIALIZATION_ERROR;
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                case HRPROFILE_STATE_ADDED_CTRL_POINT_CHAR:
                {
                    HRPROFILE_MESG_DBG (profiledbgfile,"Added Control point char\n");

                    /* change current profiles Main and Sub both states */
                    /* change profiles main and sub states */
                    heartRate.mainState = HRPROFILE_STATE_INITIALIZED;
                    heartRate.subState = HRPROFILE_STATE_INVALID;

                    /* vote to the main profile to go to advertise state */
                    (void) BLE_Profile_Vote_For_Advertisable_State (&heartRate.HRStoBLEInf);

                    BLE_Profile_Add_Advertisment_Service_UUID (HEART_RATE_SERVICE_UUID);
                }
                break;
            } /* switch (heartRate.subState) */

            if (heartRate.subState == HRPROFILE_STATE_INITIALIZATION_ERROR)
            {
				cmdCmpltStatus = BLE_STATUS_FAILED;
                /* Notify to application that Heart Rate profile initialization has failed */
                heartRate.applicationNotifyFunc(EVT_HRS_INITIALIZED,1,&cmdCmpltStatus);
                /* Change profile state to represent uninitialized profile */
                heartRate.mainState = HRPROFILE_STATE_UNINITIALIZED;
                heartRate.subState = HRPROFILE_STATE_INVALID;
                /* unregister the profile from BLE registered profile list */
                BLE_Profile_Unregister_Profile ( &heartRate.HRStoBLEInf );
                /* Need to remove all added services and characteristics ??? */
            }
        }
        break; /* HRPROFILE_STATE_INITIALIZING */
        case HRPROFILE_STATE_INITIALIZED:
        {
            /* Heart Rate profile has completed its initialization process and voted the 
             * main profile for Advertisable state. Now its waiting for the main profile
             * to set to Advertisable state. */

            /* check main profile's state, if its in advertisable state, set the same for 
             * current profile as well */
            if ((BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE) && 
                (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_CONNECTABLE_IDLE))
            {
                heartRate.mainState = HRPROFILE_STATE_ADVERTIZE;
                heartRate.subState = HRPROFILE_STATE_ADVERTISABLE_IDLE;
				cmdCmpltStatus = BLE_STATUS_SUCCESS;
				heartRate.applicationNotifyFunc(EVT_HRS_INITIALIZED,1,&cmdCmpltStatus);
            }
        }
        break; /* HRPROFILE_STATE_INITIALIZED */
        case HRPROFILE_STATE_ADVERTIZE:
        {
			if((opcode == HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL) &&
			        (response->evtPkt.commandComplete.cmdCmpltPacket.gattUpdateCharValRespPkt.servHandle == heartRate.hrServiceHandle) &&
			        (response->evtPkt.commandComplete.cmdCmpltPacket.gattUpdateCharValRespPkt.charHandle == heartRate.bodySensorLocCharHandle))
			{
				HRPROFILE_MESG_DBG ( profiledbgfile, "body sensor location updated\n");
				STORE_LE_16(tempBuf,heartRate.hrServiceHandle);
				STORE_LE_16(&tempBuf[2],heartRate.bodySensorLocCharHandle);
				tempBuf[4] = cmdCmpltStatus;
				heartRate.applicationNotifyFunc(EVT_HRS_CHAR_UPDATE_CMPLT,5,tempBuf);
			}
			else
			{
				switch (heartRate.subState)
				{
				case HRPROFILE_STATE_ADVERTISABLE_IDLE:
				{
					if ( (opcode == HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE) ||
					        (opcode == HCI_VENDOR_CMD_GAP_SET_UNDIRECTED_CONNECTABLE) ||
					        (opcode == HCI_VENDOR_CMD_GAP_SET_LIMITED_DISCOVERABLE) )
					{
						/* This profile is in Advertisable_Idle state, it means that the profile is
						 * waiting for set to discoverable mode request from application and hence
						 * has not issued any command to BLE. Now a Command Complete Event for
						 * Gap_Set_Discoverable HCI Command is received. It means that some other
						 * profile is controlling the Advertising process
						 */
						/* Just change the profile's state and wait to BLE for connection complete event */
						heartRate.subState = HRPROFILE_STATE_BLE_ADVERTISING;
					}
				}
				break;
				case HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST_DO_START:
				{
					HRPROFILE_MESG_DBG ( profiledbgfile, "STARTING Advertising for White List devices \n");
					tempBuf[0] = 0x08;
					Osal_MemCpy ((void *)(tempBuf+1), (void *)"HeartRateSensor", (uint)15 );
					hciCmdResult =
					    HCI_GAP_Set_Discoverable_Cmd( PID_HEART_RATE_SENSOR,ADV_IND,
					                                  (uint16)HRP_FAST_CONN_ADV_INTERVAL_MIN,
					                                  (uint16)HRP_FAST_CONN_ADV_INTERVAL_MAX,
					                                  PUBLIC_ADDR,
					                                  WHITE_LIST_FOR_ALL,
					                                  16,
					                                  tempBuf,
					                                  gBLEProfileContext.advtServUUIDlen,
					                                  gBLEProfileContext.advtServUUID,
					                                  0x0,
					                                  0x0);
					if ( hciCmdResult == BLE_STATUS_SUCCESS)
					{
						/* change profile's sub states */
						heartRate.subState = HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST_STARTING;
						/* start a timer of 10 secconds to try to connnect to white list device */
						Timer_Start (10, HRProfile_Advertise_Period_Timeout_Handler,&heartRate.timerID);
					}
				}
				break;
				case HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST_STARTING:
				{
					if (opcode == HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE)
					{
						/* change profile's sub states */
						heartRate.subState = HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST;
						/* request for main profile's state change */
						BLE_Profile_Change_Advertise_Sub_State ( &heartRate.HRStoBLEInf,
						        BLE_PROFILE_STATE_ADVERTISING);
					}
				}
				break;
				case HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST:
					break;
				case HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP:
				{
					HRPROFILE_MESG_DBG ( profiledbgfile, "STOPPING Advertising to White List devices \n");
					hciCmdResult = HCI_GAP_Set_Non_Discoverable_Cmd (PID_HEART_RATE_SENSOR);
					if ( hciCmdResult == BLE_STATUS_SUCCESS)
					{
						/* change profile's sub states */
						heartRate.subState = HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST_STOPPING;
					}
				}
				break;
				case HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST_STOPPING:
				{
					if (opcode == HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE)
					{
						/* change profile's sub states */
						heartRate.subState = HRPROFILE_STATE_FAST_DISCOVERABLE_DO_START;
						/* request for main profile's state change */
						BLE_Profile_Change_Advertise_Sub_State ( &heartRate.HRStoBLEInf,
						        BLE_PROFILE_STATE_CONNECTABLE_IDLE);
					}
					else
					{
						break;
					}
				}
				case HRPROFILE_STATE_FAST_DISCOVERABLE_DO_START:
				{
					HRPROFILE_MESG_DBG ( profiledbgfile, "STARTING FAST General Discoverable Mode \n");
					tempBuf[0] = 0x08;
					Osal_MemCpy ((void *)(tempBuf+1), (void *)"HeartRateSensor", (uint)15 );
					hciCmdResult =
					    HCI_GAP_Set_Discoverable_Cmd( PID_HEART_RATE_SENSOR,ADV_IND,
					                                  (uint16)HRP_FAST_CONN_ADV_INTERVAL_MIN,
					                                  (uint16)HRP_FAST_CONN_ADV_INTERVAL_MAX,
					                                  PUBLIC_ADDR,/* on what basis should the address type be chosen ?*/
					                                  NO_WHITE_LIST_USE,
					                                  16,
					                                  tempBuf,
					                                  gBLEProfileContext.advtServUUIDlen,
					                                  gBLEProfileContext.advtServUUID,
					                                  0x0,
					                                  0x0);
					if ( hciCmdResult == BLE_STATUS_SUCCESS)
					{
						/* change profile's sub states */
						heartRate.subState = HRPROFILE_STATE_FAST_DISCOVERABLE_STARTING;
						/* start a timer of 30 seconds for first phase of discoverable mode (fast discoverable mode) */
						Timer_Start (30, HRProfile_Advertise_Period_Timeout_Handler,&heartRate.timerID);
					}
				}
				break;
				case HRPROFILE_STATE_FAST_DISCOVERABLE_STARTING:
				{
					if (opcode == HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE)
					{
						HRPROFILE_MESG_DBG ( profiledbgfile, "Device is now in FAST General Discoverable Mode \n");
						/* change profile's sub states */
						heartRate.subState = HRPROFILE_STATE_FAST_DISCOVERABLE;
						/* request for main profile's state change */
						BLE_Profile_Change_Advertise_Sub_State ( &heartRate.HRStoBLEInf,
						        BLE_PROFILE_STATE_ADVERTISING);
						HRPROFILE_MESG_DBG ( profiledbgfile, "main profile state has been set.  \n");
					}
				}
				break;
				case HRPROFILE_STATE_FAST_DISCOVERABLE:
					break;
				case HRPROFILE_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP:
				{
					HRPROFILE_MESG_DBG ( profiledbgfile, "STOPPING FAST General Discoverable Mode \n");
					hciCmdResult = HCI_GAP_Set_Non_Discoverable_Cmd (PID_HEART_RATE_SENSOR);
					if ( hciCmdResult == BLE_STATUS_SUCCESS)
					{
						/* change profile's sub states */
						heartRate.subState = HRPROFILE_STATE_FAST_DISCOVERABLE_STOPPING;
					}
				}
				break;
				case HRPROFILE_STATE_FAST_DISCOVERABLE_STOPPING:
				{
					if (opcode == HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE)
					{
						/* change profile's sub states */
						heartRate.subState = HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_DO_START;
						/* request for main profile's state change */
						BLE_Profile_Change_Advertise_Sub_State ( &heartRate.HRStoBLEInf,
						        BLE_PROFILE_STATE_CONNECTABLE_IDLE);
					}
					else
					{
						break;
					}
				}
				case HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_DO_START:
				{
					HRPROFILE_MESG_DBG ( profiledbgfile, "STARTING Low Power General Discoverable Mode \n");
					tempBuf[0] = 0x08;
					Osal_MemCpy ((void *)(tempBuf+1), (void *)"HeartRateSensor", (uint)15 );
					hciCmdResult =
					    HCI_GAP_Set_Discoverable_Cmd( PID_HEART_RATE_SENSOR,ADV_IND,
					                                  (uint16)HRP_LOW_PWR_ADV_INTERVAL_MIN,
					                                  (uint16)HRP_LOW_PWR_ADV_INTERVAL_MAX,
					                                  PUBLIC_ADDR,
					                                  NO_WHITE_LIST_USE,
					                                  16,
					                                  tempBuf,
					                                  gBLEProfileContext.advtServUUIDlen,
					                                  gBLEProfileContext.advtServUUID,
					                                  0x0,
					                                  0x0
					                                );
					if ( hciCmdResult == BLE_STATUS_SUCCESS)
					{
						/* change profile's sub states */
						heartRate.subState = HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_STARTING;
						/* start a timer of 60 seconds for second phase of discoverable mode (low power discoverable mode) */
						Timer_Start (60, HRProfile_Advertise_Period_Timeout_Handler,&heartRate.timerID);
					}
				}
				break;
				case HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_STARTING:
				{
					if (opcode == HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE)
					{
						/* change profile's sub states */
						heartRate.subState = HRPROFILE_STATE_LOW_POWER_DISCOVERABLE;
						/* request for main profile's state change */
						BLE_Profile_Change_Advertise_Sub_State ( &heartRate.HRStoBLEInf,
						        BLE_PROFILE_STATE_ADVERTISING);
					}
				}
				break;
				case HRPROFILE_STATE_LOW_POWER_DISCOVERABLE:
					break;
				case HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_TIMEOUT_DO_STOP:
				{
					HRPROFILE_MESG_DBG ( profiledbgfile, "STOPPING Low Power General Discoverable Mode \n");
					hciCmdResult = HCI_GAP_Set_Non_Discoverable_Cmd (PID_HEART_RATE_SENSOR);
					if ( hciCmdResult == BLE_STATUS_SUCCESS)
					{
						/* change profile's sub states */
						heartRate.subState = HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_STOPPING;
					}
				}
				break;
				case HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_STOPPING:
				{
					if (opcode == HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE)
					{
						/* change profile's sub states */
						heartRate.subState = HRPROFILE_STATE_ADVERTISABLE_IDLE;
						/* request for main profile's state change */
						BLE_Profile_Change_Advertise_Sub_State ( &heartRate.HRStoBLEInf,
						        BLE_PROFILE_STATE_CONNECTABLE_IDLE);
						/* notify the application that the advertising has stopped */
						heartRate.applicationNotifyFunc(EVT_ADVERTISING_TIMEOUT,0x00,NULL);
					}
				}
				break;
				case HRPROFILE_STATE_BLE_ADVERTISING:
				{
					if (opcode == HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE)
					{
						/* Advertising process controlled by some other profile has been timedout
						 * and hence stopped */
						/* change profile's sub states */
						heartRate.subState = HRPROFILE_STATE_ADVERTISABLE_IDLE;
					}
				}
				break;
				case HRPROFILE_STATE_PROCESSING_PAIRING:
				{
					/* check if any other profile has initiated the disconnect procedure
					 * at BLE main profile. */
					if (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_CMD_DISCONNECT)
					{
						heartRate.mainState = HRPROFILE_STATE_CONNECTED;
						heartRate.subState = HRPROFILE_STATE_DO_DISCONNECT;
					}
				}
				break;
				}  /* switch (heartRate.subState) */
			}
        }
        break; /* HRPROFILE_STATE_ADVERTIZE */
        case HRPROFILE_STATE_CONNECTED:
        {
            switch (heartRate.subState)
            {
				case HRPROFILE_STATE_CONNECTED_IDLE:
				{
					if(opcode == HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL)
					{
						/* Heart rate measurment char updated */
						HRPROFILE_MESG_DBG ( profiledbgfile, "a gatt charac updated\n");
						if((response->evtPkt.commandComplete.cmdCmpltPacket.gattUpdateCharValRespPkt.servHandle == heartRate.hrServiceHandle) &&
						   (response->evtPkt.commandComplete.cmdCmpltPacket.gattUpdateCharValRespPkt.charHandle == heartRate.hrmCharHandle))
						{
							HRPROFILE_MESG_DBG ( profiledbgfile, "Heart Rate measurment value updated\n");
							STORE_LE_16(tempBuf,heartRate.hrServiceHandle);
							STORE_LE_16(&tempBuf[2],heartRate.hrmCharHandle);
							tempBuf[4] = cmdCmpltStatus;
							heartRate.applicationNotifyFunc(EVT_HRS_CHAR_UPDATE_CMPLT,5,tempBuf);
						}
						else if((response->evtPkt.commandComplete.cmdCmpltPacket.gattUpdateCharValRespPkt.servHandle == heartRate.hrServiceHandle) &&
						   (response->evtPkt.commandComplete.cmdCmpltPacket.gattUpdateCharValRespPkt.charHandle == heartRate.bodySensorLocCharHandle))
						{
							HRPROFILE_MESG_DBG ( profiledbgfile, "body sensor location updated\n");
							STORE_LE_16(tempBuf,heartRate.hrServiceHandle);
							STORE_LE_16(&tempBuf[2],heartRate.bodySensorLocCharHandle);
							tempBuf[4] = cmdCmpltStatus;
							heartRate.applicationNotifyFunc(EVT_HRS_CHAR_UPDATE_CMPLT,5,tempBuf);
						}
					}
				}
				break;
                case HRPROFILE_STATE_DO_DISCONNECT:
                break;
                case HRPROFILE_STATE_DISCONNECTING:
                break;
                case HRPROFILE_STATE_DISCONNECTED:
                break;
            }
        }
        break; /* HRPROFILE_STATE_CONNECTED */
    }  /* switch (heartRate.mainState) */
    return (hciCmdResult);
}  /* HRProfile_ComComplete_Event_Handler () */


/**
 * HRProfile_CmdStatus_Event_Handler
 * 
 * @param[in] opcode : opcode of the command for which the command
 *            status event has been received
 * @param[in] status : status of the command as returned by the
 *            controller
 * 
 * parses the command status according to the profile's state machine
 */ 
tBleStatus HRProfile_CmdStatus_Event_Handler(uint16 opcode,
											 uint8 status )
{
    switch (opcode)
    {
    #ifdef HCI_LINK_CTL_CMD_DISCONNECT
        case HCI_LINK_CTL_CMD_DISCONNECT:
        {
            HRPROFILE_MESG_DBG ( profiledbgfile, "Command Status received for disconnect command %02X \n", status );
            if ( (heartRate.mainState == HRPROFILE_STATE_CONNECTED) && 
                 (heartRate.subState == HRPROFILE_STATE_DO_DISCONNECT))
            {
                if (status == BLE_STATUS_SUCCESS)
                {
                    HRPROFILE_MESG_DBG ( profiledbgfile, "Disconnection Processing !! \n");
                    heartRate.subState = HRPROFILE_STATE_DISCONNECTING;
                }
                else
                {
                    HRPROFILE_MESG_DBG ( profiledbgfile, "Disconnection Error !! \n");
                }
            }
        }
        break;
    #endif
    }
    return (BLE_STATUS_SUCCESS);
}


/**
 * HRProfile_Advertise_Period_Timeout_Handler
 * 
 * this function is called when the timer started
 * by the heart rate profile for advertising times out
 */ 
void HRProfile_Advertise_Period_Timeout_Handler (void)
{
    tProfileState nextState = heartRate.subState;
    HRPROFILE_MESG_DBG ( profiledbgfile, "Advertising Period Timedout \n");
    if (heartRate.mainState == HRPROFILE_STATE_ADVERTIZE)
    {
        switch (heartRate.subState)
        {
            case HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST:
                nextState = HRPROFILE_STATE_ADVERTISING_FOR_WHITE_LIST_TIMEOUT_DO_STOP;
                HRPROFILE_MESG_DBG ( profiledbgfile, "White list Advertising Timedout \n");
            break;
            case HRPROFILE_STATE_FAST_DISCOVERABLE:
                nextState = HRPROFILE_STATE_FAST_DISCOVERABLE_TIMEOUT_DO_STOP;
                HRPROFILE_MESG_DBG ( profiledbgfile, "Fast Advertising Timedout \n");
            break;
            case HRPROFILE_STATE_LOW_POWER_DISCOVERABLE:
                nextState = HRPROFILE_STATE_LOW_POWER_DISCOVERABLE_TIMEOUT_DO_STOP;
                HRPROFILE_MESG_DBG ( profiledbgfile, "Low Power Advertising Timedout \n");
            break;
        }

        if (nextState != heartRate.subState)
        {
            heartRate.subState = nextState;

            /* Call profile's State Machine engine for state change be effective. */
		    HRProfile_ComComplete_Event_Handler(HCI_CMD_NO_OPERATION, NULL);
        }
    }
}


#ifdef SET_L2CAP_CONN_UPD_REQ_COMMAND /* TBR */

#ifdef HCI_VENDOR_CMD_L2CAP_CONN_UPD_REQ 
tBleStatus HRProfile_Sent_L2CAP_Conn_Req(uint16 interval_min,
					 uint16 interval_max, 
                                         uint16 slave_latency,
					 uint16 timeout_multiplier)

{
   tBleStatus retval = BLE_STATUS_INVALID_PARAMS;

   HRPROFILE_MESG_DBG ( profiledbgfile, "Received a Heart Rate Measurement value to send, heartRate.mainState = %02X,heartRate.subState = %02X\n", heartRate.mainState,heartRate.subState);

   if ((heartRate.mainState == HRPROFILE_STATE_CONNECTED) && 
        (heartRate.subState == HRPROFILE_STATE_CONNECTED_IDLE))
   
 
   //if (heartRate.mainState == HRPROFILE_STATE_CONNECTED) 
   {
        
       HRPROFILE_MESG_DBG ( profiledbgfile, "Received a Heart Rate Measurement value to send \n");
       retval = HCI_L2CAP_Connection_Parameter_Update_Request(PID_HEART_RATE_SENSOR,
                                                              heartRate.connHandle, 
                                                              interval_min,
                                                              interval_max,
                                                              slave_latency, 
                                                              timeout_multiplier);
       
       if (retval == BLE_STATUS_SUCCESS)
        {
            HRPROFILE_MESG_DBG ( profiledbgfile, "HRM L2CAP Connection Update Request sent successfully \n");
        }
        else
        {
            HRPROFILE_MESG_DBG ( profiledbgfile, "FAILED to send HRM L2CAP Connection Update Request %02X \n", retval);
        }
    }
    else
    {
        HRPROFILE_MESG_DBG ( profiledbgfile, "Heart Rate Profile is not in proper state to send HRM L2CAP Connection Update Request \n");
        retval = BLE_STATUS_FAILED;
    }
    return (retval);
}
#endif /* HCI_VENDOR_CMD_L2CAP_CONN_UPD_REQ */
#endif /* SET_L2CAP_CONN_UPD_REQ_COMMAND */

#endif 
