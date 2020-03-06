/******************************************************************************/
/*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2013 STMicroelectronics International NV
*
*   FILENAME        -  profile.c
*
*   COMPILER        -  gnu gcc
*
*******************************************************************************
*
*   CHANGE HISTORY
* ---------------------------
*
*   $Date$:      20/04/2012
*   $Revision$:  first version
*   $Author$:    
*   Comments:    BLE main profile implementation source file. 
*
*******************************************************************************
*
*  File Description 
*  ---------------------
*  This file will have BLE Main Profile implementation. 
******************************************************************************/

/*******************************************************************************
*******************************************************************************/

/*******************************************************************************
 * Include Files
*******************************************************************************/
#include <host_config.h>
#include <hal_types.h>
#include <ble_status.h>
#include <hci.h>
#include <host_osal.h>
#include <debug.h>
#include <list.h>
#include <ble_events.h>
#include <ble_profile.h>
#include <profile_hci_interface.h>
#include <timer.h>
#include <uuid.h> 
/*******************************************************************************
 * types
*******************************************************************************/
/* gap roles */
#define GAP_PERIPHERAL_ROLE                (0x01)
#define GAP_BROADCASTER_ROLE               (0x02)
#define GAP_CENTRAL_ROLE                   (0x03)
#define GAP_OBSERVER_ROLE                  (0x04)

/**
 * device information service context
 * contains the handles of all the 
 * characteristics added to the device
 * information service as specified by bitmask of
 * deviceInfoCharToBeAdded field. The value of these
 * characteristics cannot be set by the
 * application and will have to be set
 * at the profile level itself. These values are
 * hard coded at the profile level. It is assumed that
 * the software, firmware revisions correspond to
 * the revision of the profile code
 */ 
typedef struct _tDevInfoServiceContext
{
	/**
	 * This flag keeps track of the characteristics
	 * that have to be added to the device information service
	 */ 
	uint16 deviceInfoCharToBeAdded;
	
	/**
	 * handle of the device information service
	 */ 
    uint16 devInfoServHandle; 

	/**
	 * handle of the system ID characteristic
	 */ 
    uint16 systemIDCharHandle; 
	
	/**
	 * handle of the model number characteristic
	 */ 
    uint16 modelNumCharHandle; 
	
	/**
	 * handle of the serial number characteristic
	 */ 
    uint16 serialNumCharHandle; 
	
	/**
	 * handle of the firmware revision characteristic
	 */ 
    uint16 firmwareRevisionCharHandle; 
	
	/**
	 * handle of the hardware revision characteristic
	 */ 
    uint16 hardwareRevisionCharHandle; 
	
	/**
	 * handle of the software revision characteristic
	 */
    uint16 softwareRevisionCharHandle; 
	
	/**
	 * handle of the manufacturer name characteristic
	 */
    uint16 manufacturerNameCharHandle; 
	
	/**
	 * handle of the IEEE Certification characteristic
	 */
    uint16 ieeeCretificationCharHandle; 
	
	/**
	 * handle of the PNP ID characteristic
	 */
    uint16 pnpIDCharHandle; 
}tDevInfoServiceContext;
 
typedef struct
{
	/**
	 * list of all the currently
	 * registered profiles
	 */ 
	tListNode bleRegstedProfileList;
	
   /**
    * callback to be called to notify the application
	*/ 
   BLE_CALLBACK_FUNCTION_TYPE bleProfileApplNotifyFunc;
   
   /**
    * number of profiles currently registered
	* with the main profile
	*/ 
   int8 bleProfileNumOfRegsProfiles;
   
   /**
    * this is a counter to maintain the status of initialization of all profiles
	* Each time a profile notifies that it has completed its initialization, 
	* this counter will be incremented. The device will be allowed to start 
	* advertising only when all the profiles have completd theur initialization
    */	
   int8 bleProfileCanAdvertise;
   
   /**
    * main state of the profile state
	* machine
	*/ 
	tProfileState bleProfileMainState;
	
	 /**
    * sub state of the profile state
	* machine
	*/ 
	tProfileState bleProfileSubState;
	
	/**
	 * contains the information related to
	 * device information service
	 */
	tDevInfoServiceContext devInfoService;
	
}tBLEProfileCtxt;


/*******************************************************************************
 * Variable Declarations
*******************************************************************************/
tBLEProfileGlobalContext gBLEProfileContext;
static tBLEProfileCtxt gMainProfileCtxt;
static uint8 bleProfileIintFlag = 0;
BOOL profileProcess = FALSE;
int Profile_Process_Q_LP = 0; //[SO]fixme!!! add here details on *_Process_Q idle state
/*******************************************************************************
 * Imported Variable
*******************************************************************************/
extern void *profiledbgfile;

/*******************************************************************************
* Function Prototypes 
*******************************************************************************/
static void BLE_Profile_Rx_Event_Handler(uint8 event, 
										 tHciResponsePacket* response);

static tBleStatus BLE_Profile_Rx_ComComplete_Event_Parser(uint16 opcode, 
														  tHciResponsePacket *response);

static tBleStatus BLE_Profile_CmdStatus_Event_Parser(uint16 opcode,uint8 status);

static tBleStatus BLE_Profile_GATT_Procedure_Complete_EvtHandler(tHciCommandOpcode procedureCode);

static tBleStatus BLE_Profile_Is_Registered_Profile (tProfileInterfaceContext* profileContext);

static tBleStatus BLE_Profile_Notify_To_All_Profiles (tHciResponsePacket* response);

static void BLE_Profile_Notify_Profile(uint8 profileID,tHciResponsePacket* response);

static void BLE_Profile_Notify_State_Change(void);
static void Set_DIS_Next_State(void);

extern void HCI_Enable_GATT_Cmd(uint16 opcode);
/*******************************************************************************
* SAP
*******************************************************************************/

tBleStatus BLE_Profile_Init(tSecurityParams * securityParameters, 
							BLE_CALLBACK_FUNCTION_TYPE bleCallbackFunc)
{
    PROFILE_MESG_DBG (profiledbgfile, "Initializing BLE Profile \n");
	
	Profile_Process_Q_LP = 0;
	
	profileProcess = FALSE;
	
	tBleStatus status = 0;
    
	bleProfileIintFlag = 0;
    
    if ((securityParameters == NULL) || (bleCallbackFunc == NULL))
    {
        return (BLE_STATUS_NULL_PARAM);
    }

    Osal_MemSet((void *)&gBLEProfileContext,0,sizeof(tBLEProfileGlobalContext));
	Osal_MemSet((void *)&gMainProfileCtxt,0,sizeof(gMainProfileCtxt));
	
    Osal_MemCpy((void *)&(gBLEProfileContext.bleSecurityParam), 
                       (void *)securityParameters, 
                       sizeof(tSecurityParams));
	
	/* initialize HCI */
	HCI_Init();
	
	 /* save the application notification function ptr */
    gMainProfileCtxt.bleProfileApplNotifyFunc = bleCallbackFunc;

	list_init_head (&gMainProfileCtxt.bleRegstedProfileList);
    
    gBLEProfileContext.advtServUUIDlen = 1;
    gBLEProfileContext.advtServUUID[0] = AD_TYPE_16_BIT_SERV_UUID;
	
	/* register the event handler for HCI */
	HCI_Register_Rx_Event_Handler (BLE_Profile_Rx_Event_Handler);
    
    /* Initialize the profile HCI interface */
    Profile_HCI_Interface_Init ();
	
	/* initialize the timer */
	Timer_Init();
	
	/* initialize ble profile states */
	gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_INIT_BLE;
    gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_DETECT_BLE;
	
	 /* set the task initialization flag */
    bleProfileIintFlag = 0x01;
	/* TODO: CHECK */
	/* to know the BlueNRG is ready, issue a reset and then wait for the
	 * EVT_BLUE_HCI_INITIALIZED event
	 */
//	 status = HCI_Reset_Cmd(PID_BLE_PROFILE);
//	 if(status == BLE_STATUS_SUCCESS)
//	 {
//	    PROFILE_MESG_DBG(profiledbgfile,"HCI_Reset_Cmd\n");
//	 }
	 
    return (status);
}

int Profile_Process_Q()
{
  uint8 doNotifyStateChange = 0;
  tBleStatus cmdReqResult = BLE_STATUS_FAILED;
  uint8 evtmask[8]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
  uint16 gapEvtMask = 0xFFFF;
  uint32 gattEvtMask = 0xFFFFFFFF;
  uint16 uuid;
	
    Profile_Process_Q_LP = 0; //[SO]fixme!!! add here details on *_Process_Q idle state
    
    if (bleProfileIintFlag == 0x00)
    {
      /* the profile is not yet initialized
      * so donot do any processing
      */ 
      return 0x01;
    }

   /* process the timer Q */
   Timer_Process_Q();
   
   if(profileProcess)
   {
    PROFILE_MESG_DBG(profiledbgfile,"Profile_Process_Q\n");
   
        PROFILE_MESG_DBG(profiledbgfile, "BLE profile MainState %02X, SubState %02X \n", gMainProfileCtxt.bleProfileMainState, gMainProfileCtxt.bleProfileSubState);

        doNotifyStateChange = 0;
        cmdReqResult = BLE_STATUS_FAILED;
        
        Profile_Process_Q_LP = 1; //[SO]fixme!!! add here details on *_Process_Q idle state

        switch (gMainProfileCtxt.bleProfileMainState)
        {
            case BLE_PROFILE_STATE_INIT_BLE:
            {
                /* Steps for controller stack Initialization 
                * 1. GATT Init
                * 2. GAP Init
                * 3. set IO capabilities
                * 4. set authentication requirement
                * 5. set event mask
                * 6. set le event mask
                * 7. set GAP event mask
                * 8. set GATT event mask 
                * 9. configure whitelist 
                */
                switch (gMainProfileCtxt.bleProfileSubState)
                {
                    case BLE_PROFILE_STATE_INITIALIZING_GATT:
                    {
                        PROFILE_MESG_DBG ( profiledbgfile, "Initializing GATT Server \n" );
                        /* initialize GATT */
                        HCI_GATT_Init_Cmd(PID_BLE_PROFILE);
                    }
                    break;
                    case BLE_PROFILE_STATE_INITIALIZING_GAP:
                    {
                        PROFILE_MESG_DBG ( profiledbgfile, "Initializing GAP \n" );
                        HCI_GAP_Init_Cmd (PID_BLE_PROFILE,GAP_PERIPHERAL_ROLE);
                    }
                    break;
                    case BLE_PROFILE_STATE_SETTING_IO_CAPABILITY:
                    {
                        PROFILE_MESG_DBG ( profiledbgfile, "Setting I/O Capability. \n" );
                        HCI_GAP_Set_IOCapability_Cmd (PID_BLE_PROFILE, gBLEProfileContext.bleSecurityParam.ioCapability );
                    }
                    break;
                    case BLE_PROFILE_STATE_SETTING_AUTH_REQ:
                    {
                        PROFILE_MESG_DBG ( profiledbgfile, "Setting Authentication Request Param. \n");
                        HCI_GAP_Set_Authentication_Req_Cmd( PID_BLE_PROFILE,
															gBLEProfileContext.bleSecurityParam.mitm_mode, 
                                                            gBLEProfileContext.bleSecurityParam.OOB_Data_Present, 
                                                            gBLEProfileContext.bleSecurityParam.OOB_Data,
                                                            gBLEProfileContext.bleSecurityParam.encryptionKeySizeMin,
                                                            gBLEProfileContext.bleSecurityParam.encryptionKeySizeMax,
                                                            gBLEProfileContext.bleSecurityParam.Use_Fixed_Pin,
                                                            gBLEProfileContext.bleSecurityParam.Fixed_Pin,
                                                            gBLEProfileContext.bleSecurityParam.bonding_mode );
                    }
                    break;
                    case BLE_PROFILE_STATE_SETTING_EVENT_MASK:
                    {
                        PROFILE_MESG_DBG ( profiledbgfile, "Setting Event Mask. \n");
                        HCI_Set_Event_Mask_Cmd(PID_BLE_PROFILE,evtmask);
                    }
                    break;
                    case BLE_PROFILE_STATE_SETTING_LE_EVENT_MASK:
                    {
                        PROFILE_MESG_DBG ( profiledbgfile, "Setting LE Event Mask. \n");
                        HCI_LE_Set_Event_Mask_Cmd(PID_BLE_PROFILE,evtmask);
                    }
                    break;
                    case BLE_PROFILE_STATE_SETTING_GAP_EVENT_MASK:
                    {
                        PROFILE_MESG_DBG ( profiledbgfile, "Setting GAP Event Mask. \n");
                        HCI_GAP_Set_Event_Mask_Cmd(PID_BLE_PROFILE,gapEvtMask);
                    }
                    break;
                    case BLE_PROFILE_STATE_SETTING_GATT_EVENT_MASK:
                    {
                        PROFILE_MESG_DBG ( profiledbgfile, "Setting GATT Event Mask. \n");
                        HCI_GATT_Set_Event_Mask_Cmd(PID_BLE_PROFILE,gattEvtMask);
                    }
                    break;
                    case BLE_PROFILE_STATE_CONFIGURING_WHITELIST:
                    {
                        PROFILE_MESG_DBG ( profiledbgfile, "Configuring White List. \n");
                        if (gBLEProfileContext.bleSecurityParam.bonding_mode)
                        {
                            HCI_GAP_Configure_WhiteList_Cmd(PID_BLE_PROFILE);
                        }
                        else
                        {
                            /* as white list configuration is not required, go to next state */
                            gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_INIT_PROFILE;
                            gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_INVALID;
                            doNotifyStateChange = 1;
                        }
                    }
                    break;
					case BLE_PROFILE_STATE_ADD_DEVICE_INFORMATION_SERVICE:
					{
						PROFILE_MESG_DBG(profiledbgfile,"adding device information service\n");
						
						 uuid = DEVICE_INFORMATION_SERVICE_UUID;

                         HCI_GATT_Add_Service_Cmd(PID_BLE_PROFILE,
												  UUID_TYPE_16, 
                                                  (uint8 *)&uuid, 
												  PRIMARY_SERVICE, 
												  MAX_DIS_LEN);
					}
					break;
					case BLE_PROFILE_STATE_DIS_ADD_SYSTEM_ID_CHAR:
					{
						uuid = SYSTEM_ID_UUID;
						HCI_GATT_Add_Characterisitic_Cmd(PID_BLE_PROFILE,
														 gMainProfileCtxt.devInfoService.devInfoServHandle, 
														 UUID_TYPE_16, 
														 (uint8 *)&uuid , 
														 SYSTEM_ID_LEN_MAX,
														 READ, 
														 ATTR_PERMISSION_NONE, 
														 GATT_DONT_NOTIFY_EVENTS, 
														 10,
														 CHAR_VALUE_LEN_CONSTANT); 
					}
					break;
					case BLE_PROFILE_STATE_DIS_UPD_MODEL_NUM_STR_CHAR:
					{
						uuid = MODEL_NUMBER_UUID;
						HCI_GATT_Add_Characterisitic_Cmd(PID_BLE_PROFILE,
														 gMainProfileCtxt.devInfoService.devInfoServHandle, 
														 UUID_TYPE_16, 
														 (uint8 *)&uuid , 
														 MODEL_NUMBER_STRING_LEN_MAX,
														 READ, 
														 ATTR_PERMISSION_NONE, 
														 GATT_DONT_NOTIFY_EVENTS, 
														 10,
														 CHAR_VALUE_LEN_VARIABLE); 
					}
					break;
					case BLE_PROFILE_STATE_DIS_UPD_SERIAL_NUM_STR_CHAR:
					{
						uuid = SERIAL_NUMBER_UUID;
						HCI_GATT_Add_Characterisitic_Cmd(PID_BLE_PROFILE,
														 gMainProfileCtxt.devInfoService.devInfoServHandle, 
														 UUID_TYPE_16, 
														 (uint8 *)&uuid , 
														 SERIAL_NUMBER_STRING_LEN_MAX,
														 READ, 
														 ATTR_PERMISSION_NONE, 
														 GATT_DONT_NOTIFY_EVENTS, 
														 10,
														 CHAR_VALUE_LEN_VARIABLE); 
					}
					break;
					case BLE_PROFILE_STATE_DIS_UPD_FIRMWARE_REV_STR_CHAR:
					{
						uuid = FIRMWARE_REVISION_UUID;
						HCI_GATT_Add_Characterisitic_Cmd(PID_BLE_PROFILE,
														 gMainProfileCtxt.devInfoService.devInfoServHandle, 
														 UUID_TYPE_16, 
														 (uint8 *)&uuid , 
														 FIRMWARE_REVISION_STRING_LEN_MAX,
														 READ, 
														 ATTR_PERMISSION_NONE, 
														 GATT_DONT_NOTIFY_EVENTS, 
														 10,
														 CHAR_VALUE_LEN_VARIABLE); 
					}
					break;
					case BLE_PROFILE_STATE_DIS_UPD_HARDWARE_REV_STR_CHAR:
					{
						uuid = HARDWARE_REVISION_UUID;
						HCI_GATT_Add_Characterisitic_Cmd(PID_BLE_PROFILE,
														 gMainProfileCtxt.devInfoService.devInfoServHandle, 
														 UUID_TYPE_16, 
														 (uint8 *)&uuid , 
														 HARDWARE_REVISION_STRING_LEN_MAX,
														 READ, 
														 ATTR_PERMISSION_NONE, 
														 GATT_DONT_NOTIFY_EVENTS, 
														 10,
														 CHAR_VALUE_LEN_VARIABLE); 
					}
					break;
					case BLE_PROFILE_STATE_DIS_UPD_SOFTWARE_REV_STR_CHAR:
					{
						uuid = SOFTWARE_REVISION_UUID;
						HCI_GATT_Add_Characterisitic_Cmd(PID_BLE_PROFILE,
														 gMainProfileCtxt.devInfoService.devInfoServHandle, 
														 UUID_TYPE_16, 
														 (uint8 *)&uuid , 
														 SOFTWARE_REVISION_STRING_LEN_MAX,
														 READ, 
														 ATTR_PERMISSION_NONE, 
														 GATT_DONT_NOTIFY_EVENTS, 
														 10,
														 CHAR_VALUE_LEN_VARIABLE); 
					}
					break;
					case BLE_PROFILE_STATE_DIS_UPD_MANUFACTURER_NAME_STR_CHAR:
					{
						uuid = MANUFACTURER_NAME_UUID;
						HCI_GATT_Add_Characterisitic_Cmd(PID_BLE_PROFILE,
														 gMainProfileCtxt.devInfoService.devInfoServHandle, 
														 UUID_TYPE_16, 
														 (uint8 *)&uuid , 
														 MANUFACTURER_NAME_STRING_LEN_MAX,
														 READ, 
														 ATTR_PERMISSION_NONE, 
														 GATT_DONT_NOTIFY_EVENTS, 
														 10,
														 CHAR_VALUE_LEN_VARIABLE); 
					}
					break;
					case BLE_PROFILE_STATE_DIS_UPD_IEEE_CERTIFICATION_CHAR:
					{
						uuid = IEEE_CERTIFICATION_UUID;
						HCI_GATT_Add_Characterisitic_Cmd(PID_BLE_PROFILE,
														 gMainProfileCtxt.devInfoService.devInfoServHandle, 
														 UUID_TYPE_16, 
														 (uint8 *)&uuid , 
														 IEEE_CERTIFICATION_LEN_MAX,
														 READ, 
														 ATTR_PERMISSION_NONE, 
														 GATT_DONT_NOTIFY_EVENTS, 
														 10,
														 CHAR_VALUE_LEN_VARIABLE); 
					}
					break;
					case BLE_PROFILE_STATE_DIS_UPD_PNP_ID_CHAR:
					{
						uuid = PNP_ID_UUID;
						HCI_GATT_Add_Characterisitic_Cmd(PID_BLE_PROFILE,
														 gMainProfileCtxt.devInfoService.devInfoServHandle, 
														 UUID_TYPE_16, 
														 (uint8 *)&uuid , 
														 PNP_ID_LEN_MAX,
														 READ, 
														 ATTR_PERMISSION_NONE, 
														 GATT_DONT_NOTIFY_EVENTS, 
														 10,
														 CHAR_VALUE_LEN_CONSTANT); 
					}
					break;
					case BLE_PROFILE_STATE_MAIN_PROFILE_INIT_DONE:
					{
						/* notify sub profiles that main profile initialization is complete
						 * so that the profile specific initialization can be done
						 */ 
                            gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_INIT_PROFILE;
                            gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_INVALID;
                            doNotifyStateChange = 1;
					}
					break;
                    case BLE_PROFILE_STATE_INITIALIZATION_ERROR:
                    {
						/* the profile initialization was completed with an error
						 * notify the application
						 */ 
                    }
                    break;
                }
            }
            break; /* BLE_PROFILE_STATE_INIT_BLE: */
            case BLE_PROFILE_STATE_INIT_PROFILE:
            {
                /* the profile task is waiting here for votes from all sub profiles to 
                 * go to advertisable state. */
                /* check the advertising vote counter and change state to Advertisable 
                 * if all the registered profiles have been voted */
                /* ENTER CRITICAL SECTION */
                if (gMainProfileCtxt.bleProfileCanAdvertise == gMainProfileCtxt.bleProfileNumOfRegsProfiles)
                {
                    /* Set the BLE Profile task into connectable state */
                    gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTABLE;
                    gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_CONNECTABLE_IDLE;
                    
                    /* Now how to notify the sub profiles about main profile state change */
                    doNotifyStateChange = 1;

                }
            }
            break;
            case BLE_PROFILE_STATE_CONNECTING:
            {
                switch (gMainProfileCtxt.bleProfileSubState)
                {
                    case BLE_PROFILE_STATE_PAIRING_STARTED:
                    {
						PROFILE_MESG_DBG ( profiledbgfile, "pair\n");
						PROFILE_MESG_DBG ( profiledbgfile, "send pairing request\n");
						cmdReqResult =
						    HCI_GAP_Slave_Security_Req_Cmd ( PID_BLE_PROFILE,
						                                     gBLEProfileContext.bleSecurityParam.bonding_mode,
						                                     gBLEProfileContext.bleSecurityParam.mitm_mode );
						if (cmdReqResult == BLE_STATUS_SUCCESS)
						{
							gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_SLAVE_SECURITY_REQUESTING;
						}
						else
						{
							/* what to do now so that processing wont stop here ?? */
						}						
                    }
                    break;
                    case BLE_PROFILE_STATE_SLAVE_SECURITY_REQUESTING:
                    break;
                    case BLE_PROFILE_STATE_SLAVE_SECURITY_REQUESTED:
                    break;
                    case BLE_PROFILE_STATE_SLAVE_SECURITY_INITIATED:
                    break;
                    case BLE_PROFILE_STATE_PASS_KEY_REQUESTED:
                    break;
                    case BLE_PROFILE_STATE_PASS_KEY_RESPONDED:
                    break;
                    case BLE_PROFILE_STATE_PAIRING_COMPLETE_WAITING:
                    break;
                }
            }
            break; /* BLE_PROFILE_STATE_CONNECTING */
            case BLE_PROFILE_STATE_CONNECTED:
            {
              switch (gMainProfileCtxt.bleProfileSubState)
              {
              case BLE_PROFILE_STATE_CONNECTED_IDLE:
                break;
              case BLE_PROFILE_STATE_EXCHANGE_CONFIG:
                {
                  PROFILE_MESG_DBG ( profiledbgfile, "Starting Configuration Exchange Process \n");
                  if ((HCI_GATT_Exchange_Configuration_Cmd(PID_BLE_PROFILE,gBLEProfileContext.connectionHandle)) == BLE_STATUS_SUCCESS)
                  {
                    /* change profile's sub states */
                    gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_EXCHANGING_CONFIG;
                  }
                }
                break;
              case BLE_PROFILE_STATE_EXCHANGING_CONFIG:
                break;
              case BLE_PROFILE_STATE_CMD_DISCONNECT:
                break;
              case BLE_PROFILE_STATE_DISCONNECTING:
                break;
              }
            }
            break; /* BLE_PROFILE_STATE_CONNECTED */
            default:  //[SO]fixme!!! add here details on *_Process_Q idle state
              Profile_Process_Q_LP = 0;
            break;
        }
        
        /* BLE Profile's Notification to registered profiles */
        if (doNotifyStateChange)
        {
          /* when the profiles have been notified, the profile processQ
          * needs to run once again so that it can set its state machine.
          * So donot set the profileProcess flag to false
          */ 
          PROFILE_MESG_DBG ( profiledbgfile, "Notifying to all registered profiles about main profile state change \n");
          BLE_Profile_Notify_State_Change();
        }
        else
        {
          profileProcess = FALSE;
        }
   }
   
   return profileProcess;
}

tProfileState BLE_Profile_Get_Main_State ()
{
    return (gMainProfileCtxt.bleProfileMainState);
}

tProfileState BLE_Profile_Get_Sub_State ()
{
    return (gMainProfileCtxt.bleProfileSubState);
}

tBleStatus Is_Profile_Present(uint8 profileID)
{
  tListNode * node = NULL;
  tProfileInterfaceContext *temp;
	
   PROFILE_MESG_DBG(profiledbgfile, "Is_Profile_Present\n");
   
   list_get_next_node (&gMainProfileCtxt.bleRegstedProfileList, &node);
	while (node != &gMainProfileCtxt.bleRegstedProfileList)
	{
		temp = (tProfileInterfaceContext*)node;
		if (temp->profileID == profileID)
		{
			 PROFILE_MESG_DBG(profiledbgfile, "found profile %x\n",profileID);
			/* profile is a registered one */
			return BLE_STATUS_SUCCESS;
		}
		/* get the next node "registered profile context" in the list */
		list_get_next_node (node, &node);
	}
	PROFILE_MESG_DBG(profiledbgfile, "did not find profile %x\n",profileID);
	return BLE_STATUS_FAILED;
}


tBleStatus BLE_Profile_Register_Profile ( tProfileInterfaceContext * profileContext)
{
    if (profileContext == NULL)
    {
        return (BLE_STATUS_NULL_PARAM);
    }

    if (profileContext->callback_func == NULL)
    {
        return (BLE_STATUS_NULL_PARAM);
    }

	/* Check whether the profile is already registered */
    if ((BLE_Profile_Is_Registered_Profile (profileContext)) == BLE_STATUS_SUCCESS)
    {
        /* profile is already registered */
        return (BLE_STATUS_FAILED);
    }

    /* Check whether at this point the main profile can accept a new proifle */
    if ((gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_INIT_BLE) && 
		(gMainProfileCtxt.bleProfileSubState != BLE_PROFILE_STATE_INITIALIZATION_ERROR))
    {
        /* BLE main profile can accept a new profile */
    }
    else
    {
        /* BLE profile is not in the state to accept a new profile at this time */
        return (BLE_STATUS_FAILED);
    }

	PROFILE_MESG_DBG(profiledbgfile,"[b]DISCharRequired %x\n",gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded);
	gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded |= profileContext->DISCharRequired;
	PROFILE_MESG_DBG(profiledbgfile,"[a]DISCharRequired %x\n",gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded);
	
    gMainProfileCtxt.bleProfileNumOfRegsProfiles++;

    /* add the new profile interface context to registered profile list */
    list_insert_tail ( (tListNode *)&gMainProfileCtxt.bleRegstedProfileList, (tListNode *)profileContext );

    return (BLE_STATUS_SUCCESS);
}


tBleStatus BLE_Profile_Unregister_Profile ( tProfileInterfaceContext * profileContext)
{
    if (profileContext == NULL)
    {
        return (BLE_STATUS_NULL_PARAM);
    }

    /* first verify that the requesting profile is actually registered with BLE main profile */
    if ((BLE_Profile_Is_Registered_Profile (profileContext)) != BLE_STATUS_SUCCESS)
    {
        /* profile not registered */
        return (BLE_STATUS_FAILED);
    }

    /* here the profile context cant be removed directly from the registered list, because at 
     * the moment the profile call this function, the same list is being used for notification 
     * processing. hence removing current profile context will cause problem. */
    list_remove_node ((tListNode *)profileContext);
    gMainProfileCtxt.bleProfileNumOfRegsProfiles--;

    return (BLE_STATUS_SUCCESS);
}

tBleStatus BLE_Profile_Vote_For_Advertisable_State(tProfileInterfaceContext* profileContext)
{
    /* verify that the requesting profile is registered with BLE main profile */
    if ((BLE_Profile_Is_Registered_Profile (profileContext)) != BLE_STATUS_SUCCESS)
    {
        /* profile not registered */
        return (BLE_STATUS_FAILED);
    }

    /* check that requesting profile has not voted before for Advertisable state */
    if (profileContext->voteForAdvertisableState != 0)
    {
        /* the profile has alreadt voted for advertisable state */
        return (BLE_STATUS_FAILED);
    }

	/* The host can command the controller to go into advertising state only when all
	* the profiles have completed their initialization in the scenario where there are
	* multiple profiles running. So each profile will notify the main profile that
	* it has completed its initialization. The main profile will allow the advertising
	* command only when all registered profiles notify that they have completed their
	* profile specific initialization
	*/
    gMainProfileCtxt.bleProfileCanAdvertise++;

    profileContext->voteForAdvertisableState = 1;

	profileProcess = TRUE;
	
    return (BLE_STATUS_SUCCESS);    
}

void BLE_Profile_Change_Advertise_Sub_State(tProfileInterfaceContext*  profileContext, tProfileState nextState)
{
	/* if the main state of the ble profile is connectable and
	 * the profile requesting for a state change is a registered
	 * profile, then change the state
	 */ 
    if((gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTABLE) &&
	   (BLE_Profile_Is_Registered_Profile(profileContext) == BLE_STATUS_SUCCESS))
    {
       gMainProfileCtxt.bleProfileSubState = nextState;
    }
}

/**
 * BLE_Profile_Set_State
 * 
 * @param[in] state : state to be set
 * 
 * sets the substate of the BLE profile
 */ 
void BLE_Profile_Set_State(tProfileState state)
{
	gMainProfileCtxt.bleProfileSubState = state;
}

tBleStatus BLE_Profile_Disconnect()
{
	PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Disconnect\n");
	
	if((gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTED) ||
	   (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTING))
	{
		PROFILE_MESG_DBG(profiledbgfile,"Sending disconnect command\n");
		
		if (HCI_Disconnect_Cmd(PID_BLE_PROFILE,
							   gBLEProfileContext.connectionHandle,
							   DISCONNECT_REASON_RMT_USR_TERM_CONN) == BLE_STATUS_SUCCESS)
		{
			/* change profile's sub states */
			gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_CMD_DISCONNECT;
			return BLE_STATUS_SUCCESS;
		}
	}
	return BLE_STATUS_FAILED;
}

tBleStatus BLE_Profile_Send_Pass_Key(uint32 passkey)
{
    tBleStatus retval = BLE_STATUS_FAILED;
	
	PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Send_Pass_Key\n");
    if(((gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTING) ||
         (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTED)) && 
         (gMainProfileCtxt.bleProfileSubState == BLE_PROFILE_STATE_PASS_KEY_REQUESTED))
    {
        retval = HCI_GAP_PassKey_Response_Cmd (PID_BLE_PROFILE,gBLEProfileContext.connectionHandle,passkey); /* TODO: CHECK */
        if (retval == BLE_STATUS_SUCCESS)
        {
            gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_PASS_KEY_RESPONDED;
        }
    }

    return (retval);
}

tBleStatus BLE_Profile_Start_Configuration_Exchange(void)
{
    PROFILE_MESG_DBG ( profiledbgfile, "BLE_Profile_Start_Configuration_Exchange: %02X, %02X \n", gMainProfileCtxt.bleProfileMainState, gMainProfileCtxt.bleProfileSubState);

	if ( (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTED) && 
	     (gMainProfileCtxt.bleProfileSubState == BLE_PROFILE_STATE_CONNECTED_IDLE) )
	{
		gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_EXCHANGE_CONFIG;
	}

	return (BLE_STATUS_SUCCESS);
}

void BLE_Profile_Add_Advertisment_Service_UUID(uint16 servUUID)
{
    uint8 indx = gBLEProfileContext.advtServUUIDlen;
	
    gBLEProfileContext.advtServUUID[indx] = (uint8)(servUUID & 0xFF);
    indx++;
    gBLEProfileContext.advtServUUID[indx] = (uint8)(servUUID >> 8) & 0xFF;
    indx++;
    gBLEProfileContext.advtServUUIDlen = indx;
	PROFILE_MESG_DBG(profiledbgfile,"[add UUID]%x ,%d\n",servUUID,gBLEProfileContext.advtServUUIDlen);
}

tBleStatus BLE_Profile_Remove_Advertisment_Service_UUID(uint16 servUUID)
{
    tBleStatus retval = BLE_STATUS_FAILED;
    uint8 indx = 1;
    uint16 uuid;
    while (indx < gBLEProfileContext.advtServUUIDlen)
    {
        uuid = (uint16)(gBLEProfileContext.advtServUUID[indx] >> 8);
        indx++;
        uuid |= (uint16)(gBLEProfileContext.advtServUUID[indx]);
        indx++;
        if (uuid == servUUID)
        {
            /* this is the UUID we need to remove from list */
            indx -= 2;
            for (; indx < gBLEProfileContext.advtServUUIDlen ; indx++ )
            {
                gBLEProfileContext.advtServUUID[indx] = gBLEProfileContext.advtServUUID[indx + 1];
            }
            gBLEProfileContext.advtServUUIDlen -= 2;
            retval = BLE_STATUS_SUCCESS;
            break;
        }
    }
    
    return (retval);
}

/*******************************************************************************
* Local Functions
*******************************************************************************/
/**
 * BLE_Profile_Rx_Event_Handler
 * 
 * @param[in] profileID : ID of the profile for which this event
 *            is intended. If it is a generic event (like connection complete)
 *            then the profileID is set to 0x00
 * @param[in] response : pointer to the event data
 * 
 * parses the events received from HCI according
 * to the profile's state machine and notifies the
 * other profiles based on the profile ID
 */ 
void BLE_Profile_Rx_Event_Handler(uint8 profileID, 
								  tHciResponsePacket *response)
{
	tHciCommandOpcode gattProcedureOpcode = 0x00;
	uint8 notifyApp = 0x00;
	uint8 appNotifEvt;
	uint8 appNotifStatus = BLE_STATUS_SUCCESS;
	
    PROFILE_MESG_DBG ( profiledbgfile, "BLE_PROFILE_RX_EVENT_HANDLER: ENTER >>>> \n");
    PROFILE_MESG_DBG ( profiledbgfile, "BLE profile MainState %02X, SubState %02X \n", 
                       gMainProfileCtxt.bleProfileMainState, gMainProfileCtxt.bleProfileSubState);
    PROFILE_MESG_DBG ( profiledbgfile, "BLE_Profile_Rx_Event_Handler: %02X \n", response->hciNotifPkt.eventCode );

    profileProcess = TRUE;
	
    switch (response->hciNotifPkt.eventCode)
    {
        case HCI_EVT_DISCONN_COMPLETE: 
        {
            PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_DISCONN_COMPLETE \n");
            
            if ( (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTING) || 
                 (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTED) )
            {
                /* change state */
                if (response->evtPkt.disconnComplt.connectionHandle == gBLEProfileContext.connectionHandle)
                {
                    if (response->evtPkt.disconnComplt.status == BLE_STATUS_SUCCESS)
                    {
                        gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTABLE;
                        gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_CONNECTABLE_IDLE;
                    }
					notifyApp = 0x01;
					appNotifEvt = EVT_DISCONNECTION_COMPLETE;
					appNotifStatus = response->evtPkt.disconnComplt.status;
                }
            }
        }
        break;
        case HCI_EVT_ENCRYPT_CHANGE:
        {
            PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_ENCRYPT_CHANGE \n");
        }
        break;
        case HCI_EVT_READ_REMOTE_VERSION_COMPLETE:
        {
            PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_READ_REMOTE_VERSION_COMPLETE \n");
        }
        break;
        case HCI_EVT_CMD_COMPLETE:
        {
            PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_CMD_COMPLETE \n" );
            BLE_Profile_Rx_ComComplete_Event_Parser ( response->evtPkt.commandComplete.commandOpcode, 
                                                      response );
        }
        break;
        case HCI_EVT_CMD_STATUS:
        {
            PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_CMD_STATUS \n");
            
            BLE_Profile_CmdStatus_Event_Parser ( response->evtPkt.commandStatus.commandOpcode, 
                                                 response->evtPkt.commandStatus.status );
        }
        break;
        case HCI_EVT_HARDWARE_ERROR:
        {
            PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_HARDWARE_ERROR \n");
        }
        break;
        case HCI_EVT_NUM_COMP_PKTS:
        {
            PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_NUM_COMP_PKTS \n");
        }
        break;
        case HCI_EVT_DATA_BUFFER_OVERFLOW:
        {
            PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_DATA_BUFFER_OVERFLOW \n");
        }
        break;
        case HCI_EVT_ENCRYPTION_KEY_REFRESH_COMPLETE:
        {
            PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_ENCRYPTION_KEY_REFRESH_COMPLETE \n");
        }
        break;
        case HCI_EVT_LE_META_EVENT:
        {
            PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_LE_META_EVENT \n");
            
            switch (response->evtPkt.leEvent.subEventCode)
            {
                case HCI_EVT_LE_CONN_COMPLETE:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_LE_CONN_COMPLETE %x\n",
					                  response->evtPkt.leEvent.leMetaEvtPkt.leConnectionComplete.status);
                    /* check task states */
                    if(gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTABLE) 
                    {
						PROFILE_MESG_DBG ( profiledbgfile, "state\n");
                        /* check comand status */
                        if (response->evtPkt.leEvent.leMetaEvtPkt.leConnectionComplete.status == BLE_STATUS_SUCCESS)
                        {
							PROFILE_MESG_DBG ( profiledbgfile, "BLE_STATUS_SUCCESS\n");
                            /* save connection handle */
                            gBLEProfileContext.connectionHandle = 
                            response->evtPkt.leEvent.leMetaEvtPkt.leConnectionComplete.connectionHandle;
							
                            if(gBLEProfileContext.bleSecurityParam.initiateSecurity == SLAVE_INITIATE_SECURITY)
							{
								gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTING;
								gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_PAIRING_STARTED;
							}
							else if(gBLEProfileContext.bleSecurityParam.initiateSecurity == WAIT_FOR_PAIRING_CMPLT)
							{
								gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTING;
								gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_WAIT_FOR_PAIRING;
							}
							else
							{
								gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTED;
								gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_CONNECTED_IDLE;
							}
                        }
                        else
                        {
                            /* connection was not successful, then we need to wait for
                             * the application to put the BLE in discoverable mode again
                             */
                            gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTABLE;
                            gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_CONNECTABLE_IDLE;
                        }
						notifyApp = 0x01;
						appNotifEvt = EVT_CONNECTION_COMPLETE;
						appNotifStatus = response->evtPkt.leEvent.leMetaEvtPkt.leConnectionComplete.status;
						
                        /* Stop if any timer is running */
                    }
                }
                break; /* HCI_EVT_LE_CONN_COMPLETE */
                case HCI_EVT_LE_CONN_UPDATE_COMPLETE:
                break;
                case HCI_EVT_LE_LTK_REQUEST:
                break;
            }
        }
        break; /* HCI_EVT_LE_META_EVENT */
        case HCI_EVT_VENDOR_SPECIFIC:
        {
            PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_VENDOR_SPECIFIC \n");
            switch (response->evtPkt.vendorSpecificEvt.vsEventCode)
            {
				case HCI_EVT_BLUE_INITIALIZED:
				{
				PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_INITIALIZED\n");
					/* if the BlueNRG is initialized in the
					 * normal mode, we need to proceed with
					 * further processing
					 */ 
					if(response->evtPkt.vendorSpecificEvt.vsEvtPkt.BlueNRGMode == 0x01)
					{
						PROFILE_MESG_DBG ( profiledbgfile, "BlueNRG initialized\n");
										
						if(gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_INIT_BLE)
						{
						gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_INITIALIZING_GATT;
						}
					}
					else
					{
						PROFILE_MESG_DBG ( profiledbgfile, "BlueNRG initialization error\n");
					}

					notifyApp = 0x01;
					appNotifStatus = response->evtPkt.vendorSpecificEvt.vsEvtPkt.BlueNRGMode - 1;
					appNotifEvt = EVT_BLUE_INITIALIZED;
				}
				break;
                case HCI_EVT_BLUE_GAP_SET_LIMITED_DISCOVERABLE:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_BLUE_GAP_SET_LIMITED_DISCOVERABLE \n");
                }
                break;
                case HCI_EVT_BLUE_GAP_PAIRING_CMPLT:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_BLUE_GAP_PAIRING_CMPLT \n");

                    if ( (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTING) ||
						 (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTED) )
                    {
                        if (response->evtPkt.vendorSpecificEvt.vsEvtPkt.status == BLE_STATUS_SUCCESS)
                        {
                            /* Pairing successful. Now the device is paired with another BT device. */
                            gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTED;
                            gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_CONNECTED_IDLE;
                        }
						else
						{
							if(gBLEProfileContext.bleSecurityParam.initiateSecurity == SLAVE_INITIATE_SECURITY)
							{
								gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTING;
								gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_PAIRING_STARTED;
							}
							else if(gBLEProfileContext.bleSecurityParam.initiateSecurity == WAIT_FOR_PAIRING_CMPLT)
							{
								gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTING;
								gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_WAIT_FOR_PAIRING;
							}
							else
							{
								gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTED;
								gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_CONNECTED_IDLE;
							}
						}
                       
						notifyApp = 0x01;
						appNotifStatus = response->evtPkt.vendorSpecificEvt.vsEvtPkt.status;
						appNotifEvt = EVT_PAIRING_COMPLETE;
                    }
                }
                break;
                case HCI_EVT_BLUE_GAP_PASS_KEY_REQUEST:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "Received event HCI_EVT_BLUE_GAP_PASS_KEY_REQUEST \n");
                    /* check state */
                     if ( (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTING) ||
						 (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTED) )
                    {
                            PROFILE_MESG_DBG ( profiledbgfile, "Requesting Authentication key from application \n");
                            gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_PASS_KEY_REQUESTED;
                            /* pass event to application for user provided pass key 
							 * if using fixed pin then we would not have
							 * got this event at all. So no need to check for
							 * fixed pin option here
							 */
                            gMainProfileCtxt.bleProfileApplNotifyFunc(EVT_PASSKEY_REQUEST,0,NULL);
                    }
                }
                break;
                case HCI_EVT_BLUE_GAP_AUTHORIZATION_REQUEST:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_GAP_AUTHORIZATION_REQUEST \n");
					/* send the authorization response.
					 * currently we donot have a profile 
					 * which requires authorization
					 */ 
					 HCI_GAP_Set_Authorization_Resp_Cmd(PID_BLE_PROFILE,0x01);
                }
                break;
                case HCI_EVT_BLUE_GAP_SLAVE_SECURITY_INITIATED:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_GAP_SLAVE_SECURITY_INITIATED \n");

                    if ( (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTING) && 
                         (gMainProfileCtxt.bleProfileSubState == BLE_PROFILE_STATE_SLAVE_SECURITY_REQUESTED) )
                    {
                        if (gBLEProfileContext.bleSecurityParam.mitm_mode)
                        {
                            /* man in the middle is required to exchange pass key for authentication process.
                             * hence wait in the same state for pass key request from master side */
                            PROFILE_MESG_DBG ( profiledbgfile, "Man in the middle is required for authentication \n");
                            gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_SLAVE_SECURITY_INITIATED;
                        }
                        else
                        {
                            /* if mitm is not required, we are only left for pairing complete event */
                            PROFILE_MESG_DBG ( profiledbgfile, "Man in the middle is not required for authentication \n");
                            gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_PAIRING_COMPLETE_WAITING;
                        }
                    }
                }
                break;
                case HCI_EVT_BLUE_L2CAP_CONN_UPD_RESP:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_L2CAP_CONN_UPD_RESP \n");
                }
                break;
                case HCI_EVT_BLUE_L2CAP_PROCEDURE_TIMEOUT:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_L2CAP_PROCEDURE_TIMEOUT \n");
                }
                break;
                case HCI_EVT_BLUE_DEBUG:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_DEBUG \n");
                }
                break;
                case HCI_EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_GATT_ATTRIBUTE_MODIFIED \n");
                }
                break;
                case HCI_EVT_BLUE_GATT_PROCEDURE_TIMEOUT:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_GATT_PROCEDURE_TIMEOUT \n");
					/* when a gatt procedure timeout occurs no more gatt pdus can be exchanged
					 * since profiles are hugely dependant on GATT we can as well start a 
					 * disconnection here
					 */ 
					 BLE_Profile_Disconnect();
                }
                break;
                case HCI_EVT_BLUE_EXCHANGE_MTU_RESP:
                {
					PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_EXCHANGE_MTU_RESP \n");
                }
                break;
                case HCI_EVT_BLUE_ATT_FIND_INFORMATION_RESP:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_ATT_FIND_INFORMATION_RESP \n");
                }
                break;
                case HCI_EVT_BLUE_ATT_FIND_BY_TYPE_VAL_RESP:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_ATT_FIND_BY_TYPE_VAL_RESP \n");
                }
                break;
                case HCI_EVT_BLUE_ATT_READ_BY_TYPE_RESP:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_ATT_READ_BY_TYPE_RESP \n");
                }
                break;
                case HCI_EVT_BLUE_ATT_READ_RESP:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_ATT_READ_RESP \n");
                }
                break;
                case HCI_EVT_BLUE_ATT_READ_BLOB_RESP:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_ATT_READ_BLOB_RESP \n");
                }
                break;
                case HCI_EVT_BLUE_ATT_READ_MULTIPLE_RESP:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_ATT_READ_MULTIPLE_RESP \n");
                }
                break;
                case HCI_EVT_BLUE_ATT_READ_BY_GROUP_RESP:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_ATT_READ_BY_GROUP_RESP \n");
                }
                break;
                case HCI_EVT_BLUE_ATT_WRITE_RESP:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_ATT_WRITE_RESP \n");
                }
                break;
                case HCI_EVT_BLUE_ATT_PREPARE_WRITE_RESP:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_ATT_PREPARE_WRITE_RESP \n");
                }
                break;
                case HCI_EVT_BLUE_ATT_EXEC_WRITE_RESP:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_ATT_EXEC_WRITE_RESP \n");
                }
                break;
                case HCI_EVT_BLUE_GATT_INDICATION:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_GATT_INDICATION \n");
                }
                break;
                case HCI_EVT_BLUE_GATT_NOTIFICATION:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "Received HCI_EVT_BLUE_GATT_NOTIFICATION \n");
                }
                break;
				case HCI_EVT_BLUE_GATT_PROCEDURE_COMPLETE:
				{
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_GATT_PROCEDURE_COMPLETE \n");
					gattProcedureOpcode = Profile_HCI_Interface_Get_Current_GATT_Procedure_Opcode ();					
					response->evtPkt.vendorSpecificEvt.vsEvtPkt.gattProcedureComplete.procedureOpcode = gattProcedureOpcode;
					BLE_Profile_GATT_Procedure_Complete_EvtHandler (gattProcedureOpcode);
				}
				break;
                case HCI_EVT_BLUE_GATT_ERROR_RESP:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_GATT_ERROR_RESP \n");
                }
                break;
                case HCI_EVT_BLUE_GATT_DISC_READ_CHAR_BY_UUID_RESP:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_GATT_DISC_READ_CHAR_BY_UUID_RESP \n");
                }
                break;
                case HCI_EVT_BLUE_GATT_WRITE_PERMIT_REQ:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_GATT_WRITE_PERMIT_REQ \n");
                }
                break;
                case HCI_EVT_BLUE_GATT_READ_PERMIT_REQ:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_GATT_READ_PERMIT_REQ \n");
                }
                break;
                case HCI_EVT_BLUE_GATT_READ_MULTI_PERMIT_REQ:
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "HCI_EVT_BLUE_GATT_READ_MULTI_PERMIT_REQ \n");
                }
                break;
            }
        }
        break; /* HCI_EVT_VENDOR_SPECIFIC */
        default:
        {
            PROFILE_MESG_DBG ( profiledbgfile, "Received an Invalid Event !! \n");
        }
        break;
    }

    if(profileID == PID_BLE_PROFILE)
	{
		/* this event was intended for the main profile only
		 * so donot notify any other profiles
		 */
		 PROFILE_MESG_DBG ( profiledbgfile, "profileID == PID_BLE_PROFILE\n");
		return; 
	}
	else if(profileID != 0x00)
    {
       /* notify only the profile for which this event is intended */
	   PROFILE_MESG_DBG(profiledbgfile,"notifying profile %x with event %02X \n",profileID,response->hciNotifPkt.eventCode);
	   BLE_Profile_Notify_Profile(profileID,response);
    }
	else
	{
		/* this is a generic event so pass it to all profiles */
		PROFILE_MESG_DBG(profiledbgfile, "notifying all profiles\n");
		BLE_Profile_Notify_To_All_Profiles (response);
	}
 
    if(notifyApp)
	{
		PROFILE_MESG_DBG(profiledbgfile, "notifying application\n");
		gMainProfileCtxt.bleProfileApplNotifyFunc(appNotifEvt,1,&appNotifStatus);
	}
	
	if(gattProcedureOpcode)
	{
		PROFILE_MESG_DBG(profiledbgfile, "gattProcedureOpcode\n");
		Profile_HCI_Interface_Reset_GATT_Procedure_Status ();
		HCI_Enable_GATT_Cmd(gattProcedureOpcode);
	}
    PROFILE_MESG_DBG ( profiledbgfile, "BLE_PROFILE_RX_EVENT_HANDLER: EXIT >>>> \n");

    return;
}

/**
 * BLE_Profile_Rx_ComComplete_Event_Parser
 * 
 * @param[in] opcode : opcode of the command for which the command complete
 *            event has been received
 * @param[in] response : pointer to the event data
 * 
 * parses the command complete event according to the profile's state machine
 */ 
tBleStatus BLE_Profile_Rx_ComComplete_Event_Parser(uint16 opcode, 
												   tHciResponsePacket *response)
{
    uint8 cmdCmpltStatus = BLE_STATUS_SUCCESS;
    PROFILE_MESG_DBG ( profiledbgfile, "BLE_Profile_Rx_ComComplete_Event_Parser: opcode %04X \n", opcode);

    /* COMMAND COMPLETE EVENT STATE MACHINE */
    /* Here the parsing is done with respect to the task's main and sub states, 
     * and not with respect to the command for which this command complete is 
     * received because differnet states may have used same HCI command (i.e. 
     * task state to hci command mapping is in the form of many to one). Hence 
     * parsing wrt task state is easier. 
     */
	 if(response != NULL)
	 {
		cmdCmpltStatus = response->evtPkt.commandComplete.status;
	 }
	 
    PROFILE_MESG_DBG (profiledbgfile,"Command %04X Complete with Status %02X !!\n", opcode, cmdCmpltStatus);
	PROFILE_MESG_DBG (profiledbgfile,"gmainProfileCtxt.bleprofilemainstate is %d : gMainProfileCtxt.bleProfileSubState is %d\n", 
					  gMainProfileCtxt.bleProfileMainState,
					  gMainProfileCtxt.bleProfileSubState);
    switch (gMainProfileCtxt.bleProfileMainState)
    {
        case BLE_PROFILE_STATE_INIT_BLE:
        {
            if(cmdCmpltStatus != BLE_STATUS_SUCCESS)
            {
                PROFILE_MESG_DBG (profiledbgfile,"Command Complete Status: Error: %02X !!\n", cmdCmpltStatus);
                gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_INITIALIZATION_ERROR;
                return (BLE_STATUS_FAILED);
            }

            switch (gMainProfileCtxt.bleProfileSubState)
            {
                case BLE_PROFILE_STATE_INITIALIZING_GATT:
                {
                    if (opcode == HCI_VENDOR_CMD_GATT_SERVER_INIT)
                    {
                        PROFILE_MESG_DBG (profiledbgfile, "GATT Initialized Successfully \n");
                        gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_INITIALIZING_GAP;
                    }
                }
                break;
                case BLE_PROFILE_STATE_INITIALIZING_GAP:
                {
                    if (opcode == HCI_VENDOR_CMD_GAP_INIT)
                    {
                        PROFILE_MESG_DBG (profiledbgfile, "GAP Initialized Successfully \n");
                        gBLEProfileContext.gapServiceHandle = 
                        response->evtPkt.commandComplete.cmdCmpltPacket.gapInitResp.serviceHandle;
                        
                        gBLEProfileContext.devNameCharHandle = 
						response->evtPkt.commandComplete.cmdCmpltPacket.gapInitResp.devNameCharHandle;
                        
                        gBLEProfileContext.appearanceCharHandle = 
						response->evtPkt.commandComplete.cmdCmpltPacket.gapInitResp.appearanceCharHandle;
                        
                        gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_SETTING_IO_CAPABILITY;
                    }
                }
                break;
                case BLE_PROFILE_STATE_SETTING_IO_CAPABILITY:
                {
                    if (opcode == HCI_VENDOR_CMD_GAP_SET_SET_IO_CAPABILITY)
                    {
                        PROFILE_MESG_DBG (profiledbgfile, "I/O capability Set Successfully \n");
                        gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_SETTING_AUTH_REQ;
                    }
                }
                break;
                case BLE_PROFILE_STATE_SETTING_AUTH_REQ:
                {
                    if (opcode == HCI_VENDOR_CMD_GAP_SET_AUTHENTICATION_REQ)
                    {
                        PROFILE_MESG_DBG (profiledbgfile, "Authentication Req set Successfully \n");
                        gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_SETTING_EVENT_MASK;
                    }
                }
                break;
                case BLE_PROFILE_STATE_SETTING_EVENT_MASK:
                {
                    if (opcode == HCI_HOST_CTL_CMD_SET_EVENT_MASK)
                    {
                        PROFILE_MESG_DBG (profiledbgfile, "Event Mask Set Successfully \n");
                        gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_SETTING_LE_EVENT_MASK;
                    }
                }
                break;
                case BLE_PROFILE_STATE_SETTING_LE_EVENT_MASK:
                {
                    if (opcode == HCI_LE_CTL_SET_EVENT_MASK)
                    {
                        PROFILE_MESG_DBG (profiledbgfile, "LE Event Mask Set Successfully \n");
                        gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_SETTING_GAP_EVENT_MASK;
                    }
                }
                break;
                case BLE_PROFILE_STATE_SETTING_GAP_EVENT_MASK:
                {
                    if (opcode == HCI_VENDOR_CMD_GAP_SET_EVT_MASK)
                    {
                        PROFILE_MESG_DBG (profiledbgfile, "GAP Event Mask Set Successfully \n");
                        gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_SETTING_GATT_EVENT_MASK;
                    }
                }
                break;
                case BLE_PROFILE_STATE_SETTING_GATT_EVENT_MASK:
                {
                    if (opcode == HCI_VENDOR_CMD_GATT_SET_EVT_MASK)
                    {
                        PROFILE_MESG_DBG (profiledbgfile, "GATT Event Mask Set Successfully \n");
						if(gBLEProfileContext.bleSecurityParam.bonding_mode)
						{
							gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_CONFIGURING_WHITELIST;
						}
						else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded)
						{
							/* we have to add the device information service */
							gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_ADD_DEVICE_INFORMATION_SERVICE;
						}
						else
						{
							gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_MAIN_PROFILE_INIT_DONE;
						}
                    }
                }
                break;
                case BLE_PROFILE_STATE_CONFIGURING_WHITELIST:
                {
                    if (opcode == HCI_VENDOR_CMD_GAP_CONFIGURE_WHITELIST)
                    {
                        PROFILE_MESG_DBG (profiledbgfile, "White List Configured Successfully \n");
						
                        if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded)
						{
							/* we have to add the device information service */
							gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_ADD_DEVICE_INFORMATION_SERVICE;
						}
						else
						{
							gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_MAIN_PROFILE_INIT_DONE;
						}
                    }
                }
                break;
				case BLE_PROFILE_STATE_ADD_DEVICE_INFORMATION_SERVICE:
				{
					if(cmdCmpltStatus == BLE_STATUS_SUCCESS)
					{
						PROFILE_MESG_DBG (profiledbgfile, "Device Information Service is added Successfully \n");
						gMainProfileCtxt.devInfoService.devInfoServHandle =
						    response->evtPkt.commandComplete.cmdCmpltPacket.gattAddServiceResp.serviceHandle;
						PROFILE_MESG_DBG ( profiledbgfile, "Device Information Service Handle: %04X !!\n",
						                   gMainProfileCtxt.devInfoService.devInfoServHandle);
						/* change profile's sub states */
						Set_DIS_Next_State();
					}
					else
					{
						/*  there was an error while adding the device information
						 * service 
						 */
						gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_INITIALIZATION_ERROR;
					}
				}
				break;
				case BLE_PROFILE_STATE_DIS_ADD_SYSTEM_ID_CHAR:
				{
					if(cmdCmpltStatus == BLE_STATUS_SUCCESS)
					{
						PROFILE_MESG_DBG (profiledbgfile, "[DIS] system ID added Successfully \n");
						gMainProfileCtxt.devInfoService.systemIDCharHandle =
						    response->evtPkt.commandComplete.cmdCmpltPacket.gattAddCharResp.charHandle;
						PROFILE_MESG_DBG ( profiledbgfile, "[DIS] system ID Handle: %04X !!\n",
						                   gMainProfileCtxt.devInfoService.systemIDCharHandle);
										   
						gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded &= ~SYSTEM_ID_CHAR_MASK;
						
						/* change profile's sub states */
						Set_DIS_Next_State();
					}
					else
					{
						/* there was an error while adding the device information
						 * service 
						 */
						gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_INITIALIZATION_ERROR;
					}
				}
				break;
				case BLE_PROFILE_STATE_DIS_ADD_MODEL_NUM_STR_CHAR:
				{
					if(cmdCmpltStatus == BLE_STATUS_SUCCESS)
					{
						PROFILE_MESG_DBG (profiledbgfile, "[DIS] model number added Successfully \n");
						gMainProfileCtxt.devInfoService.modelNumCharHandle =
						    response->evtPkt.commandComplete.cmdCmpltPacket.gattAddCharResp.charHandle;
						PROFILE_MESG_DBG ( profiledbgfile, "[DIS] model number Handle: %04X !!\n",
						                   gMainProfileCtxt.devInfoService.modelNumCharHandle);
										   
						gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded &= ~MODEL_NUMBER_STRING_CHAR_MASK;
						
						/* change profile's sub states */
						Set_DIS_Next_State();
					}
					else
					{
						/* there was an error while adding the device information
						 * service 
						 */
						gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_INITIALIZATION_ERROR;
					}
				}
				break;
				case BLE_PROFILE_STATE_DIS_ADD_SERIAL_NUM_STR_CHAR:
				{
					if(cmdCmpltStatus == BLE_STATUS_SUCCESS)
					{
						PROFILE_MESG_DBG (profiledbgfile, "[DIS] serial number added Successfully \n");
						gMainProfileCtxt.devInfoService.serialNumCharHandle =
						    response->evtPkt.commandComplete.cmdCmpltPacket.gattAddCharResp.charHandle;
						PROFILE_MESG_DBG(profiledbgfile, "[DIS] serial number Handle: %04X !!\n",
						                   gMainProfileCtxt.devInfoService.serialNumCharHandle);
										   
						gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded &= ~SERIAL_NUMBER_STRING_CHAR_MASK;
						
						/* change profile's sub states */
						Set_DIS_Next_State();
					}
					else
					{
						/* there was an error while adding the device information
						 * service 
						 */
						gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_INITIALIZATION_ERROR;
					}
				}
				break;
				case BLE_PROFILE_STATE_DIS_ADD_FIRMWARE_REV_STR_CHAR:
				{
					if(cmdCmpltStatus == BLE_STATUS_SUCCESS)
					{
						PROFILE_MESG_DBG (profiledbgfile, "[DIS] firmware revision added Successfully \n");
						gMainProfileCtxt.devInfoService.firmwareRevisionCharHandle =
						    response->evtPkt.commandComplete.cmdCmpltPacket.gattAddCharResp.charHandle;
						PROFILE_MESG_DBG(profiledbgfile, "[DIS] firmware revision Handle: %04X !!\n",
						                   gMainProfileCtxt.devInfoService.firmwareRevisionCharHandle);
										   
						gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded &= ~FIRMWARE_REVISION_STRING_CHAR_MASK;
						
						/* change profile's sub states */
						Set_DIS_Next_State();
					}
					else
					{
						/* there was an error while adding the device information
						 * service 
						 */
						gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_INITIALIZATION_ERROR;
					}
				}
				break;
				case BLE_PROFILE_STATE_DIS_ADD_HARDWARE_REV_STR_CHAR:
				{
					if(cmdCmpltStatus == BLE_STATUS_SUCCESS)
					{
						PROFILE_MESG_DBG (profiledbgfile, "[DIS] hardware revision added Successfully \n");
						gMainProfileCtxt.devInfoService.hardwareRevisionCharHandle =
						    response->evtPkt.commandComplete.cmdCmpltPacket.gattAddCharResp.charHandle;
						PROFILE_MESG_DBG(profiledbgfile, "[DIS] hardware revision Handle: %04X !!\n",
						                   gMainProfileCtxt.devInfoService.hardwareRevisionCharHandle);
										   
						gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded &= ~HARDWARE_REVISION_STRING_CHAR_MASK;
						
						/* change profile's sub states */
						Set_DIS_Next_State();
					}
					else
					{
						/* there was an error while adding the device information
						 * service 
						 */
						gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_INITIALIZATION_ERROR;
					}
				}
				break;
				case BLE_PROFILE_STATE_DIS_ADD_SOFTWARE_REV_STR_CHAR:
				{
					if(cmdCmpltStatus == BLE_STATUS_SUCCESS)
					{
						PROFILE_MESG_DBG (profiledbgfile, "[DIS] software revision added Successfully \n");
						gMainProfileCtxt.devInfoService.softwareRevisionCharHandle =
						    response->evtPkt.commandComplete.cmdCmpltPacket.gattAddCharResp.charHandle;
						PROFILE_MESG_DBG(profiledbgfile, "[DIS] software revision Handle: %04X !!\n",
						                   gMainProfileCtxt.devInfoService.softwareRevisionCharHandle);
										   
						gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded &= ~SOFTWARE_REVISION_STRING_CHAR_MASK;
						
						/* change profile's sub states */
						Set_DIS_Next_State();
					}
					else
					{
						/* there was an error while adding the device information
						 * service 
						 */
						gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_INITIALIZATION_ERROR;
					}
				}
				break;
				case BLE_PROFILE_STATE_DIS_ADD_MANUFACTURER_NAME_STR_CHAR:
				{
					if(cmdCmpltStatus == BLE_STATUS_SUCCESS)
					{
						PROFILE_MESG_DBG (profiledbgfile, "[DIS] manufacturer name added Successfully \n");
						gMainProfileCtxt.devInfoService.manufacturerNameCharHandle =
						    response->evtPkt.commandComplete.cmdCmpltPacket.gattAddCharResp.charHandle;
						PROFILE_MESG_DBG(profiledbgfile, "[DIS] manufacturer name Handle: %04X !!\n",
						                   gMainProfileCtxt.devInfoService.manufacturerNameCharHandle);
										   
						gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded &= ~MANUFACTURER_NAME_STRING_CHAR_MASK;
						
						/* change profile's sub states */
						Set_DIS_Next_State();
					}
					else
					{
						/* there was an error while adding the device information
						 * service 
						 */
						gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_INITIALIZATION_ERROR;
					}
				}
				break;
				case BLE_PROFILE_STATE_DIS_ADD_IEEE_CERTIFICATION_CHAR:
				{
					if(cmdCmpltStatus == BLE_STATUS_SUCCESS)
					{
						PROFILE_MESG_DBG (profiledbgfile, "[DIS] IEEE certification added Successfully \n");
						gMainProfileCtxt.devInfoService.ieeeCretificationCharHandle =
						    response->evtPkt.commandComplete.cmdCmpltPacket.gattAddCharResp.charHandle;
						PROFILE_MESG_DBG(profiledbgfile, "[DIS] IEE certification Handle: %04X !!\n",
						                   gMainProfileCtxt.devInfoService.ieeeCretificationCharHandle);
										   
						gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded &= ~IEEE_CERTIFICATION_CHAR_MASK;
						
						/* change profile's sub states */
						Set_DIS_Next_State();
					}
					else
					{
						/* there was an error while adding the device information
						 * service 
						 */
						gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_INITIALIZATION_ERROR;
					}
				}
				break;
				case BLE_PROFILE_STATE_DIS_ADD_PNP_ID_CHAR:
				{
					if(cmdCmpltStatus == BLE_STATUS_SUCCESS)
					{
						PROFILE_MESG_DBG (profiledbgfile, "[DIS] PNP ID added Successfully \n");
						gMainProfileCtxt.devInfoService.pnpIDCharHandle =
						    response->evtPkt.commandComplete.cmdCmpltPacket.gattAddCharResp.charHandle;
						PROFILE_MESG_DBG(profiledbgfile, "[DIS] PNP ID Handle: %04X !!\n",
						                   gMainProfileCtxt.devInfoService.pnpIDCharHandle);
										   
						gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded &= ~PNP_ID_CHAR_MASK;
						
						/* change profile's sub states */
						Set_DIS_Next_State();
					}
					else
					{
						/* there was an error while adding the device information
						 * service 
						 */
						gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_INITIALIZATION_ERROR;
					}
				}
				break;
				case BLE_PROFILE_STATE_INITIALIZATION_ERROR:
				break;
            }
        }
        break; /* BLE_PROFILE_STATE_INIT_BLE */
		case BLE_PROFILE_STATE_INIT_PROFILE:
		{
		}
		break;/* BLE_PROFILE_STATE_INIT_PROFILE */
    }

    return (BLE_STATUS_SUCCESS);
}

void Set_DIS_Next_State(void)
{
	if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & SYSTEM_ID_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_DIS_ADD_SYSTEM_ID_CHAR;
	}
	else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & MODEL_NUMBER_STRING_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_DIS_ADD_MODEL_NUM_STR_CHAR;
	}
	else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & SERIAL_NUMBER_STRING_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_DIS_ADD_SERIAL_NUM_STR_CHAR;
	}
	else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & FIRMWARE_REVISION_STRING_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_DIS_ADD_FIRMWARE_REV_STR_CHAR;
	}
	else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & HARDWARE_REVISION_STRING_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_DIS_ADD_HARDWARE_REV_STR_CHAR;
	}
	else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & SOFTWARE_REVISION_STRING_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_DIS_ADD_SOFTWARE_REV_STR_CHAR;
	}
	else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & MANUFACTURER_NAME_STRING_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_DIS_ADD_MANUFACTURER_NAME_STR_CHAR;
	}
	else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & IEEE_CERTIFICATION_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_DIS_ADD_IEEE_CERTIFICATION_CHAR;
	}
	else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & PNP_ID_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_DIS_ADD_PNP_ID_CHAR;
	}
	else
	{
		gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_MAIN_PROFILE_INIT_DONE;
	}
}

void Send_DIS_Update_Cmplt_Evt(uint8 status)
{
	if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & SYSTEM_ID_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileApplNotifyFunc(EVT_DIS_SYSTEM_ID_UPDATE_CMPLT,1,&status);
	}
	else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & MODEL_NUMBER_STRING_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileApplNotifyFunc(EVT_DIS_MODEL_NUM_UPDATE_CMPLT,1,&status);
	}
	else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & SERIAL_NUMBER_STRING_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileApplNotifyFunc(EVT_DIS_SERIAL_NUM_UPDATE_CMPLT,1,&status);
	}
	else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & FIRMWARE_REVISION_STRING_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileApplNotifyFunc(EVT_DIS_FIRMWARE_STRING_UPDATE_CMPLT,1,&status);
	}
	else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & HARDWARE_REVISION_STRING_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileApplNotifyFunc(EVT_DIS_HARDWARE_STRING_UPDATE_CMPLT,1,&status);
	}
	else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & SOFTWARE_REVISION_STRING_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileApplNotifyFunc(EVT_DIS_SOFTWARE_STRING_UPDATE_CMPLT,1,&status);
	}
	else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & MANUFACTURER_NAME_STRING_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileApplNotifyFunc(EVT_DIS_MANUFACTURER_NAME_STRING_UPDATE_CMPLT,1,&status);
	}
	else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & IEEE_CERTIFICATION_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileApplNotifyFunc(EVT_DIS_IEEE_CERTIFICATION_UPDATE_CMPLT,1,&status);
	}
	else if(gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded & PNP_ID_CHAR_MASK)
	{
		gMainProfileCtxt.bleProfileApplNotifyFunc(EVT_DIS_PNP_ID_UPDATE_CMPLT,1,&status);
	}
}

tBleStatus BLE_Profile_Update_DIS_SystemID(uint8 len,uint8 *SystemId)
{
	tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
	
	PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_SystemID %d\n",len);
	if(len <= SYSTEM_ID_LEN_MAX)
	{	
	    ret = HCI_GATT_Update_CharVal_Cmd(PID_HID_DEVICE,
								gMainProfileCtxt.devInfoService.devInfoServHandle, 
								gMainProfileCtxt.devInfoService.systemIDCharHandle, 
								(uint8)0,
							    len, 
								 SystemId);
		if(ret == BLE_STATUS_SUCCESS)
		{
			gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= SYSTEM_ID_CHAR_MASK;
		}
	}
	return ret;
}

tBleStatus BLE_Profile_Update_DIS_ModelNum(uint8 len,uint8 *modelNum)
{
	tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
	
	PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_ModelNum %d\n",len);
	if(len <= MODEL_NUMBER_STRING_LEN_MAX)
	{	
	    ret = HCI_GATT_Update_CharVal_Cmd(PID_HID_DEVICE,
								gMainProfileCtxt.devInfoService.devInfoServHandle, 
								gMainProfileCtxt.devInfoService.modelNumCharHandle, 
								(uint8)0,
							    len, 
								modelNum);
		if(ret == BLE_STATUS_SUCCESS)
		{
			gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= MODEL_NUMBER_STRING_CHAR_MASK;
		}
	}
	return ret;
}

tBleStatus BLE_Profile_Update_DIS_SerialNum(uint8 len,uint8 *serialNum)
{
	tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
	
	PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_SerialNum %d\n",len);
	if(len <= SERIAL_NUMBER_STRING_LEN_MAX)
	{	
	    ret = HCI_GATT_Update_CharVal_Cmd(PID_HID_DEVICE,
								gMainProfileCtxt.devInfoService.devInfoServHandle, 
								gMainProfileCtxt.devInfoService.serialNumCharHandle, 
								(uint8)0,
							    len, 
								serialNum);
		if(ret == BLE_STATUS_SUCCESS)
		{
			gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= SERIAL_NUMBER_STRING_CHAR_MASK;
		}
	}
	return ret;
}

tBleStatus BLE_Profile_Update_DIS_FirmwareRev(uint8 len,uint8 *firmwareRev)
{
	tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
	
	PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_FirmwareRev %d\n",len);
	if(len <= FIRMWARE_REVISION_STRING_LEN_MAX)
	{	
	    ret = HCI_GATT_Update_CharVal_Cmd(PID_HID_DEVICE,
								gMainProfileCtxt.devInfoService.devInfoServHandle, 
								gMainProfileCtxt.devInfoService.firmwareRevisionCharHandle, 
								(uint8)0,
							    len, 
								firmwareRev);
		if(ret == BLE_STATUS_SUCCESS)
		{
			gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= FIRMWARE_REVISION_STRING_CHAR_MASK;
		}
	}
	return ret;
}

tBleStatus BLE_Profile_Update_DIS_HardwareRev(uint8 len,uint8 *hardwareRev)
{
	tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
	
	PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_HardwareRev %d\n",len);
	if(len <= HARDWARE_REVISION_STRING_LEN_MAX)
	{	
	    ret = HCI_GATT_Update_CharVal_Cmd(PID_HID_DEVICE,
								gMainProfileCtxt.devInfoService.devInfoServHandle, 
								gMainProfileCtxt.devInfoService.hardwareRevisionCharHandle, 
								(uint8)0,
							    len, 
								hardwareRev);
		if(ret == BLE_STATUS_SUCCESS)
		{
			gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= HARDWARE_REVISION_STRING_CHAR_MASK;
		}
	}
	return ret;
}

tBleStatus BLE_Profile_Update_DIS_SoftwareRev(uint8 len,uint8 *softwareRev)
{
	tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
	
	PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_SoftwareRev %d\n",len);
	if(len <= SOFTWARE_REVISION_STRING_LEN_MAX)
	{	
	    ret = HCI_GATT_Update_CharVal_Cmd(PID_HID_DEVICE,
								gMainProfileCtxt.devInfoService.devInfoServHandle, 
								gMainProfileCtxt.devInfoService.softwareRevisionCharHandle, 
								(uint8)0,
							    len, 
								softwareRev);
		if(ret == BLE_STATUS_SUCCESS)
		{
			gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= SOFTWARE_REVISION_STRING_CHAR_MASK;
		}
	}
	return ret;
}

tBleStatus BLE_Profile_Update_DIS_manufacturerName(uint8 len,uint8 *name)
{
	tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
	
	PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_manufacturerName %d\n",len);
	if(len <= MANUFACTURER_NAME_STRING_LEN_MAX)
	{	
	    ret = HCI_GATT_Update_CharVal_Cmd(PID_HID_DEVICE,
								gMainProfileCtxt.devInfoService.devInfoServHandle, 
								gMainProfileCtxt.devInfoService.manufacturerNameCharHandle, 
								(uint8)0,
							    len, 
								name);
		if(ret == BLE_STATUS_SUCCESS)
		{
			gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= MANUFACTURER_NAME_STRING_CHAR_MASK;
		}
	}
	return ret;
}

tBleStatus BLE_Profile_Update_DIS_IEEECertification(uint8 len,uint8 *ieeeCert)
{
	tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
	
	PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_manufacturerName %d\n",len);
	if(len <= IEEE_CERTIFICATION_LEN_MAX)
	{	
	    ret = HCI_GATT_Update_CharVal_Cmd(PID_HID_DEVICE,
								gMainProfileCtxt.devInfoService.devInfoServHandle, 
								gMainProfileCtxt.devInfoService.ieeeCretificationCharHandle, 
								(uint8)0,
							    len, 
								ieeeCert);
		if(ret == BLE_STATUS_SUCCESS)
		{
			gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= IEEE_CERTIFICATION_CHAR_MASK;
		}
	}
	return ret;
}

tBleStatus BLE_Profile_Update_DIS_pnpId(uint8 len,uint8 *pnpId)
{
	tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
	
	PROFILE_MESG_DBG(profiledbgfile,"BLE_Profile_Update_DIS_pnpId %d\n",len);
	if(len <= PNP_ID_LEN_MAX)
	{	
	    ret = HCI_GATT_Update_CharVal_Cmd(PID_HID_DEVICE,
								gMainProfileCtxt.devInfoService.devInfoServHandle, 
								gMainProfileCtxt.devInfoService.pnpIDCharHandle, 
								(uint8)0,
							    len, 
								pnpId);
		if(ret == BLE_STATUS_SUCCESS)
		{
			gMainProfileCtxt.devInfoService.deviceInfoCharToBeAdded  |= PNP_ID_CHAR_MASK;
		}
	}
	return ret;
}

/**
 * BLE_Profile_CmdStatus_Event_Parser
 * 
 * @param[in] opcode : opcode of the command for which the command
 *            status event has been received
 * @param[in] status : status of the command as returned by the
 *            controller
 * 
 * parses the command status according to the profile's state machine
 */ 
tBleStatus BLE_Profile_CmdStatus_Event_Parser(uint16 opcode,uint8 status)
{
	PROFILE_MESG_DBG(profiledbgfile,"Command Status for %x command, status %x\n",opcode,status);
    switch (opcode)
    {
        case HCI_LINK_CTL_CMD_DISCONNECT:
        {
            PROFILE_MESG_DBG ( profiledbgfile, "Command Status received for disconnect command %02X \n", status );
            if ( (gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTED) && 
                 (gMainProfileCtxt.bleProfileSubState == BLE_PROFILE_STATE_CMD_DISCONNECT))
            {
                if (status == BLE_STATUS_SUCCESS)
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "Disconnection Processing !! \n");
                    gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_DISCONNECTING;
                }
                else
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "Disconnection Error !! \n");
                }
            }
        }
        break;

        case HCI_VENDOR_CMD_GAP_PASS_KEY_RESP:
        {
            PROFILE_MESG_DBG ( profiledbgfile, "Command Status for HCI_VENDOR_CMD_GAP_PASS_KEY_RESP %02X \n", status );
            if ((gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTING) && 
                (gMainProfileCtxt.bleProfileSubState == BLE_PROFILE_STATE_PASS_KEY_RESPONDED))
            {
                if (status == BLE_STATUS_SUCCESS)
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "Pass Key Response Accepted \n");

                    /* as pass key has been send to master as authentication parameter, the only
                     * thing remained to the pairing complete event. hence wait for pairing complete. 
                     */
                    gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_PAIRING_COMPLETE_WAITING;
                }
                else
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "Pass Key Response Declined \n");
                    gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_PAIRING_STARTED;
                }
            }
        }
        break;

        case HCI_VENDOR_CMD_GAP_SLAVE_SECURITY_REQUEST:
        {
            PROFILE_MESG_DBG ( profiledbgfile, "Command Status for HCI_VENDOR_CMD_GAP_SLAVE_SECURITY_REQUEST %02X \n", status );
            if ((gMainProfileCtxt.bleProfileMainState == BLE_PROFILE_STATE_CONNECTING) && 
                (gMainProfileCtxt.bleProfileSubState == BLE_PROFILE_STATE_SLAVE_SECURITY_REQUESTING))
            {
                if (status == BLE_STATUS_SUCCESS)
                {
                    PROFILE_MESG_DBG ( profiledbgfile, "Slave Security Request has been send to master \n");
                    gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_SLAVE_SECURITY_REQUESTED;
                }
				else
				{
					/* security could not be requested */
					gMainProfileCtxt.bleProfileMainState = BLE_PROFILE_STATE_CONNECTED;
				}
            }
        }
        break;
    }

    return (BLE_STATUS_SUCCESS);
}

/**
 * BLE_Profile_GATT_Procedure_Complete_EvtHandler
 * 
 * @param[in] procedureCode : opcode of the gatt procedure that
 *           has been completed
 */ 
tBleStatus BLE_Profile_GATT_Procedure_Complete_EvtHandler(tHciCommandOpcode procedureCode)
{
	PROFILE_MESG_DBG ( profiledbgfile, "BLE_Profile_GATT_Procedure_Complete_EvtHandler: opcode %04X \n", procedureCode);
	switch (gMainProfileCtxt.bleProfileMainState)
	{
		case BLE_PROFILE_STATE_CONNECTED:
		{
			switch (gMainProfileCtxt.bleProfileSubState)
			{
				case BLE_PROFILE_STATE_EXCHANGING_CONFIG:
				{
					if (procedureCode == HCI_VENDOR_CMD_GATT_EXCHG_CONFIG)
					{
						gMainProfileCtxt.bleProfileSubState = BLE_PROFILE_STATE_CONNECTED_IDLE;
					}
				}
				break;
			}
		}
		break;
	}
	return (BLE_STATUS_SUCCESS);
}

/**
 * BLE_Profile_Is_Registered_Profile
 * 
 * @param[in] profileContext : pointer to the profile interface context
 *            which has to be checked for its registration with the main profile
 * 
 * searches through the list of registered profiles to find the requested
 * profile
 * 
 * @return returns BLE_STATUS_SUCCESS if the desired profile is found else
 * returns BLE_STATUS_FAILED
 */ 
tBleStatus BLE_Profile_Is_Registered_Profile(tProfileInterfaceContext *profileContext)
{
    tListNode * node = NULL;

	list_get_next_node (&gMainProfileCtxt.bleRegstedProfileList, &node);
    while (node != &gMainProfileCtxt.bleRegstedProfileList)
    {
        if (node == (tListNode *)profileContext)
        {
            /* profile is a registered one */
            return BLE_STATUS_SUCCESS;
        }

        /* get the next node "registered profile context" in the list */
        list_get_next_node (node, &node);
	}
	
	return BLE_STATUS_FAILED;
}

/**
 * BLE_Profile_Notify_To_All_Profiles
 * 
 * @param[in] response : pointer to the event data
 * 
 * notifies all the profiles registered with the main profile
 * about the event
 */ 
tBleStatus BLE_Profile_Notify_To_All_Profiles(tHciResponsePacket* response)
{
    tListNode * node = NULL;
    tProfileInterfaceContext * profileContext = NULL;
    
    if (response == NULL)
    {
        return (BLE_STATUS_NULL_PARAM);
    }


    /* traverse through the registered profile list */
	list_get_next_node (&gMainProfileCtxt.bleRegstedProfileList, &node);
	while (node != &gMainProfileCtxt.bleRegstedProfileList)
	{
		profileContext = (tProfileInterfaceContext *)node;
        if ( (profileContext->callback_func) != NULL )
        {
            profileContext->callback_func ( response->hciNotifPkt.eventCode, 
                                            response );
        }
        /* get the next node "registered profile context" in the list */
        list_get_next_node (node, &node);
	}

	return (BLE_STATUS_SUCCESS);
}

/**
 * BLE_Profile_Notify_Profile
 * 
 * @param[in] profileID : ID of the profile
 * @param[in] response : pointer to the event data
 * 
 * notifies the profile specified by profileID about the event if it
 * is registered with the profile
 */ 
void BLE_Profile_Notify_Profile(uint8 profileID,tHciResponsePacket *response)
{
    tListNode * node = NULL;
    tProfileInterfaceContext * profileContext = NULL;
    
    if (response == NULL)
    {
        return;
    }

    /* traverse through the registered profile list */
	list_get_next_node (&gMainProfileCtxt.bleRegstedProfileList, &node);
	while (node != &gMainProfileCtxt.bleRegstedProfileList)
	{
		profileContext = (tProfileInterfaceContext *)node;
        if((profileContext->profileID == profileID) &&
		   (profileContext->callback_func != NULL))
        {
            profileContext->callback_func ( response->hciNotifPkt.eventCode, 
                                            response );
			break;
        }
        /* get the next node "registered profile context" in the list */
        list_get_next_node (node, &node);
	}
}

/**
 * BLE_Profile_Notify_State_Change 
 * 
 * This function is used to notify all the
 * registered profiles about the state change at
 * BLE main profile. The opcode is set to NO-OP to
 * tell the profiles that it is only to inform them
 * of the state change and they can take action depending
 * on their current state
 */
void BLE_Profile_Notify_State_Change (void)
{
    tHciResponsePacket response;
    response.hciNotifPkt.eventCode = HCI_EVT_CMD_COMPLETE;
    response.evtPkt.commandComplete.numHCICommandPackets = 1;
    response.evtPkt.commandComplete.commandOpcode = HCI_CMD_NO_OPERATION;
    
    BLE_Profile_Notify_To_All_Profiles (&response);
}
