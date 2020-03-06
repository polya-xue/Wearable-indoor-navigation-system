/*******************************************************************************/
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
*   FILENAME        -  ble_profile.h
*
*   COMPILER        -  EWARM (gnu gcc)
*
********************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      09/03/2012
*   $Revision$:  first version
*   $Author$:    
*   Comments:    BLE Main Profile Header File
*
********************************************************************************
*
*  File Description 
*  ---------------------
*  
*
*******************************************************************************/
#ifndef _BLE_PROFILE_H_
#define _BLE_PROFILE_H_

/*******************************************************************************
 * Macro
*******************************************************************************/
/* profile IDs */
#define PID_BLE_PROFILE					       (0x01)
#define PID_HEART_RATE_SENSOR				   (0x02)
#define PID_PROXIMITY_MONITOR				   (0x03)
#define PID_PROXIMITY_REPORTER				   (0x04)
#define PID_TIME_SERVER				   		   (0x05)
#define PID_TIME_CLIENT			    		   (0x06)
#define PID_HID_DEVICE						   (0x07)
#define PID_FIND_ME_TARGET				   	   (0x08)
#define PID_FIND_ME_LOCATOR				   	   (0x09)
#define PID_PHONE_ALERT_CLIENT				   (0x0A)
#define PID_BLOOD_PRESSURE_SENSOR			   (0x0B)
#define PID_HEALTH_THERMOMETER			       (0x0C)
#define PID_ALERT_NOTIFICATION_CLIENT		   (0x0D)
#define PID_ALERT_NOTIFICATION_SERVER		   (0x0E)

/* Added PID for Glucose sensor profile role */
#define PID_GLUCOSE_SENSOR		   (0x0F)


/* minimum encryption key size */
#define MIN_ENCRYPTION_KEY_SIZE					(10)

/* UUID types */
#define UUID_TYPE_16                      (0x01)
#define UUID_TYPE_128                     (0x02)

/* service type */
#define PRIMARY_SERVICE                   (0x01)
#define SECONDARY_SERVICE                 (0x02)

/* disconnection codes */
#define DISCONNECT_REASON_RMT_USR_TERM_CONN	   (0x13)

/* IO capabilities */
typedef uint8 tIoCapability;
#define IO_CAP_DISPLAY_ONLY				(0x00)
#define IO_CAP_DISPLAY_YES_NO			(0x01)
#define KEYBOARD_ONLY					(0x02)
#define IO_CAP_NO_INPUT_NO_OUTPUT		(0x03)
#define IO_CAP_KEYBOARD_DISPLAY			(0x04)

#define NO_SECURITY_REQUIRED			(0x00)
#define SLAVE_INITIATE_SECURITY			(0x01)
#define WAIT_FOR_PAIRING_CMPLT			(0x02)

/* BIT MASK FOR CHARACTERISTIC PROPERTIES ACCORDING
 */ 
#define BROADCAST                        (0x01)
#define READ                             (0x02)
#define WRITE_WITHOUT_RESPONSE           (0x04)
#define WRITE                            (0x08)
#define NOTIFY                           (0x10)
#define INDICATE                         (0x20)
#define AUTHENTICATED_SIGNED_WRITE       (0x40)
#define EXTENDED_PROPERTIES              (0x80)

/* SECURITY PERMISSIONS FOR ATTRIBUTES */
#define ATTR_PERMISSION_NONE                      (0x00)
#define ATTR_PERMISSION_AUTHENTICATE_TO_READ      (0x01)
#define ATTR_PERMISSION_AUTHORIZE_TO_READ         (0x02)
#define ATTR_PERMISSION_ENCRYPTED_READ            (0x04)
#define ATTR_PERMISSION_AUTHENTICATE_TO_WRITE     (0x08)
#define ATTR_PERMISSION_AUTHORIZE_TO_WRITE        (0x10)
#define ATTR_PERMISSION_ENCRYPTED_WRITE           (0x20)


/* ATTRIBUTE ACCESS PERMISSIONS */
#define ATTR_NO_ACCESS                    (0x00)
#define ATTR_ACCESS_READ_ONLY             (0x01)
#define ATTR_ACCESS_WRITE_ONLY            (0x02)
#define ATTR_ACCESS_READ_WRITE            (0x03)

/* GATT EVENT MASK OPTION */
#define GATT_DONT_NOTIFY_EVENTS                       (0x00)
#define GATT_NOTIFY_ATTRIBUTE_WRITE                   (0x01)
#define GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP  (0x02)
#define GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP   (0x04)
#define GATT_NOTIFY_ALL_EVENTS                        (0xFF)

/* IS VALUE LENGTH OF CHARACTERISTIC VARIABLE ?? */
#define CHAR_VALUE_LEN_CONSTANT           (0x00)
#define CHAR_VALUE_LEN_VARIABLE           (0x01)

/* advertising event types */
#define ADV_IND                 (0x00)
#define ADV_DIRECT_IND          (0x01)
#define ADV_NOCONN_IND          (0x02)

/* address types */
#define PUBLIC_ADDR     (0)
#define RANDOM_ADDR     (1)

/* advertising filter policy */
#define NO_WHITE_LIST_USE           (0X00)
#define WHITE_LIST_FOR_ONLY_SCAN    (0X01)  
#define WHITE_LIST_FOR_ONLY_CONN    (0X02)
#define WHITE_LIST_FOR_ALL          (0X03) 

/* AD types */
#define AD_TYPE_16_BIT_SERV_UUID				(0x02)

#define CHAR_EXTENDED_PROPERTIES_DESCRIPTOR_LENGTH         (2)
#define CLIENT_CHAR_CONFIG_DESCRIPTOR_LENGTH               (2)
#define SERVER_CHAR_CONFIG_DESCRIPTOR_LENGTH               (2)
#define CHAR_PRESENTATION_FORMAT_DESCRIPTOR_LENGTH         (7)
#define CHAR_AGGREGATE_FORMAT_DESCRIPTOR_LENGTH            (2)
#define REPORT_REFERENCE_DESCRIPTOR_LENGTH                 (2)
#define ATTRIBUTE_OPCODE_HANDLE_VALUE_NOTIFICATION         (0x1B)


/* Status values for write permission response */
#define GATT_WRITE_RESP_STATUS_NO_ERROR            (0x00)
#define GATT_WRITE_RESP_STATUS_VALUE_ERROR         (0x01)

/* advertising timeout is 30 seconds */
#define BLE_FAST_CONN_ADV_TIMEOUT	   (30)

/**
 * bit masks for the characteristics of the device information
 * service
 */ 
#define SYSTEM_ID_CHAR_MASK                                 (0x01)
#define MODEL_NUMBER_STRING_CHAR_MASK                       (0x02)
#define SERIAL_NUMBER_STRING_CHAR_MASK                      (0x04)
#define FIRMWARE_REVISION_STRING_CHAR_MASK                  (0x08)
#define HARDWARE_REVISION_STRING_CHAR_MASK                  (0x10)
#define SOFTWARE_REVISION_STRING_CHAR_MASK                  (0x20)
#define MANUFACTURER_NAME_STRING_CHAR_MASK                  (0x40)
#define IEEE_CERTIFICATION_CHAR_MASK                        (0x80)
#define PNP_ID_CHAR_MASK                                    (0x100)

#define MAX_DIS_LEN			(9)

/* device information service characteristic lengths */
#define SYSTEM_ID_LEN_MAX                                 (8)
#define MODEL_NUMBER_STRING_LEN_MAX                       (32)
#define SERIAL_NUMBER_STRING_LEN_MAX                      (32)
#define FIRMWARE_REVISION_STRING_LEN_MAX                  (32)
#define HARDWARE_REVISION_STRING_LEN_MAX                  (32)
#define SOFTWARE_REVISION_STRING_LEN_MAX                  (32)
#define MANUFACTURER_NAME_STRING_LEN_MAX                  (32)
#define IEEE_CERTIFICATION_LEN_MAX                        (32)
#define PNP_ID_LEN_MAX                                    (7)

/** 
 * error codes for device
 * information service
 */
#define DIS_COULD_NOT_BE_ADDED					(0x01)
#define DIS_ADD_MODEL_NUM_CHAR_FAILED			(0x02)
#define DIS_ADD_SERIAL_NUM_CHAR_FAILED			(0x03)
#define DIS_ADD_FIRMWARE_REV_CHAR_FAILED		(0x04)
#define DIS_ADD_HARDWARE_REV_CHAR_FAILED		(0x05)
#define DIS_ADD_SOFTWARE_REV_CHAR_FAILED		(0x06)
#define DIS_ADD_MANUFACTURER_NAME_CHAR_FAILED	(0x07)
#define DIS_ADD_IEEE_CERTIFICATION_CHAR_FAILED	(0x08)
#define DIS_ADD_PNP_ID_CHAR_FAILED				(0x09)

/* Little Endian buffer to Controller Byte order conversion */
#define LE_TO_NRG_16(ptr)  (uint16) ( ((uint16) \
                                           *((uint8 *)ptr)) | \
                                          ((uint16) \
                                           *((uint8 *)ptr + 1) << 8 ) )

#define LE_TO_NRG_32(ptr)   (tHalUint32) ( ((uint32) \
                                           *((uint8 *)ptr)) | \
                                           ((uint32) \
                                            *((uint8 *)ptr + 1) << 8)  | \
                                           ((uint32) \
                                            *((uint8 *)ptr + 2) << 16) | \
                                           ((uint32) \
                                            *((uint8 *)ptr + 3) << 24) )
											
/* Store Value into a buffer in Little Endian Format */
#define STORE_LE_16(buf, val)    ( ((buf)[0] =  (uint8) (val)    ) , \
                                   ((buf)[1] =  (uint8) (val>>8) ) )

#define STORE_LE_32(buf, val)    ( ((buf)[0] =  (uint8) (val)     ) , \
                                   ((buf)[1] =  (uint8) (val>>8)  ) , \
                                   ((buf)[2] =  (uint8) (val>>16) ) , \
                                   ((buf)[3] =  (uint8) (val>>24) ) ) 

/*******************************************************************************
* BLE PROFILE STATES
*******************************************************************************/ 

/* BLE Profile task main states */
#define BLE_PROFILE_STATE_UNINIT               (0x00)
#define BLE_PROFILE_STATE_INIT_BLE             (0x10)
#define BLE_PROFILE_STATE_INIT_PROFILE         (0x20)
#define BLE_PROFILE_STATE_CONNECTABLE          (0x30)
#define BLE_PROFILE_STATE_CONNECTING           (0x40)
#define BLE_PROFILE_STATE_CONNECTED            (0x50)
#define BLE_PROFILE_INVALID                    (0xFF)

/* ble profile sub states under BLE_PROFILE_STATE_INIT_BLE main state */
#define BLE_PROFILE_STATE_DETECT_BLE                (0x11)
#define BLE_PROFILE_STATE_INITIALIZING_GATT         (0x12)
#define BLE_PROFILE_STATE_INITIALIZING_GAP          (0x13)
#define BLE_PROFILE_STATE_SETTING_IO_CAPABILITY     (0x14)
#define BLE_PROFILE_STATE_SETTING_AUTH_REQ          (0x15)
#define BLE_PROFILE_STATE_SETTING_EVENT_MASK        (0x16)
#define BLE_PROFILE_STATE_SETTING_LE_EVENT_MASK     (0x17)
#define BLE_PROFILE_STATE_SETTING_GAP_EVENT_MASK    (0x18)
#define BLE_PROFILE_STATE_SETTING_GATT_EVENT_MASK   (0x19)
#define BLE_PROFILE_STATE_CONFIGURING_WHITELIST     (0x1A)
#define BLE_PROFILE_STATE_MAIN_PROFILE_INIT_DONE    (0x1B)
#define BLE_PROFILE_STATE_INITIALIZATION_ERROR      (0x1F)

/* device information addition states */
#define BLE_PROFILE_STATE_ADD_DEVICE_INFORMATION_SERVICE	 (0x21)
#define BLE_PROFILE_STATE_DIS_ADD_SYSTEM_ID_CHAR	    	 (0x23)
#define BLE_PROFILE_STATE_DIS_ADD_MODEL_NUM_STR_CHAR	     (0x24)
#define BLE_PROFILE_STATE_DIS_ADD_SERIAL_NUM_STR_CHAR	     (0x25)
#define BLE_PROFILE_STATE_DIS_ADD_FIRMWARE_REV_STR_CHAR	     (0x26)
#define BLE_PROFILE_STATE_DIS_ADD_HARDWARE_REV_STR_CHAR	     (0x27)
#define BLE_PROFILE_STATE_DIS_ADD_SOFTWARE_REV_STR_CHAR	     (0x28)
#define BLE_PROFILE_STATE_DIS_ADD_MANUFACTURER_NAME_STR_CHAR (0x29)
#define BLE_PROFILE_STATE_DIS_ADD_IEEE_CERTIFICATION_CHAR	 (0x2A)
#define BLE_PROFILE_STATE_DIS_ADD_PNP_ID_CHAR	    	 	 (0x2B)

#define BLE_PROFILE_STATE_DIS_UPD_SYSTEM_ID_CHAR	    	 (0x23)
#define BLE_PROFILE_STATE_DIS_UPD_MODEL_NUM_STR_CHAR	     (0x24)
#define BLE_PROFILE_STATE_DIS_UPD_SERIAL_NUM_STR_CHAR	     (0x25)
#define BLE_PROFILE_STATE_DIS_UPD_FIRMWARE_REV_STR_CHAR	     (0x26)
#define BLE_PROFILE_STATE_DIS_UPD_HARDWARE_REV_STR_CHAR	     (0x27)
#define BLE_PROFILE_STATE_DIS_UPD_SOFTWARE_REV_STR_CHAR	     (0x28)
#define BLE_PROFILE_STATE_DIS_UPD_MANUFACTURER_NAME_STR_CHAR (0x29)
#define BLE_PROFILE_STATE_DIS_UPD_IEEE_CERTIFICATION_CHAR	 (0x2A)
#define BLE_PROFILE_STATE_DIS_UPD_PNP_ID_CHAR	    	 	 (0x2B)

/* ble profile sub states under BLE_PROFILE_STATE_CONNECTABLE main state */
#define BLE_PROFILE_STATE_CONNECTABLE_IDLE					     (0x31)
#define BLE_PROFILE_STATE_ENABLING_ADVERTISING					 (0x32)
#define BLE_PROFILE_STATE_ADVERTISING                            (0x33)
#define BLE_PROFILE_STATE_DISABLING_ADVERTISING					 (0x34)


/* ble profile sub states under BLE_PROFILE_STATE_CONNECTING main state */
#define BLE_PROFILE_STATE_WAIT_FOR_PAIRING						 (0x39)
#define BLE_PROFILE_STATE_PAIRING_STARTED                        (0x40)
#define BLE_PROFILE_STATE_SLAVE_SECURITY_REQUESTING              (0x41)
#define BLE_PROFILE_STATE_SLAVE_SECURITY_REQUESTED               (0x42)
#define BLE_PROFILE_STATE_SLAVE_SECURITY_INITIATED               (0x43)
#define BLE_PROFILE_STATE_PASS_KEY_REQUESTED                     (0x44)
#define BLE_PROFILE_STATE_PASS_KEY_RESPONDED                     (0x45)
#define BLE_PROFILE_STATE_PAIRING_COMPLETE_WAITING               (0x47)
#define BLE_PROFILE_STATE_PAIRING_COMPLETE                       (0x48)


/* ble profile sub states under BLE_PROFILE_STATE_CONNECTED main state */
#define BLE_PROFILE_STATE_CONNECTED_IDLE                         (0x51)
#define BLE_PROFILE_STATE_CMD_DISCONNECT                         (0x54)
#define BLE_PROFILE_STATE_DISCONNECTING                          (0x55)

#define BLE_PROFILE_STATE_EXCHANGE_CONFIG                        (0x56)
#define BLE_PROFILE_STATE_EXCHANGING_CONFIG                      (0x57)


/*******************************************************************************
 * type definitions 
*******************************************************************************/
/**
 * security parameters structure
 */ 
typedef struct _tSecurityParams
{
    /**
	 * IO capability of the device
	 */ 
	tIoCapability ioCapability;
	
    /**
	 * Authentication requirement of the device
	 * Man In the Middle protection required?
	 */ 
	uint8 mitm_mode;
	
    /**
	 * bonding mode of the device
	 */ 
	uint8 bonding_mode;
	
	/**
	 * Flag to tell whether OOB data has 
	 * to be used during the pairing process
	 */ 
	uint8 OOB_Data_Present; 
	
	/**
	 * OOB data to be used in the pairing process if 
	 * OOB_Data_Present is set to TRUE
	 */ 
    uint8 OOB_Data[16]; 
	
	/**
	 * this variable indicates whether to use a fixed pin 
	 * during the pairing process or a passkey has to be 
	 * requested to the application during the pairing process
	 * 0 implies use fixed pin and 1 implies request for passkey
	 */ 
	uint8 Use_Fixed_Pin; 
    
	/**
	 * minimum encryption key size requirement
	 */ 
    uint8 encryptionKeySizeMin;
    
	/**
	 * maximum encryption key size requirement
	 */ 
    uint8 encryptionKeySizeMax;
	
	/**
	 * fixed pin to be used in the pairing process if
	 * Use_Fixed_Pin is set to 1
	 */ 
	uint32 Fixed_Pin;
	
	/**
	 * this flag indicates whether the host has to initiate
	 * the security, wait for pairing or does not have any security
     * requirements.\n
	 * 0x00 : no security required
	 * 0x01 : host should initiate security by sending the slave security
	 *        request command
	 * 0x02 : host need not send the clave security request but it
	 * has to wait for paiirng to complete before doing any other
	 * processing
	 */ 
	uint8 initiateSecurity;
}tSecurityParams;


typedef HCI_READ_EVENT_NOTIFY_TYPE PROFILE_CALLBACK_FUNC_TYPE;

typedef uint8 tStateAgreementVote;

/**
 * profile context structure which
 * every profile has to regiater with the main
 * profile
 */ 
typedef struct _tProfileInterfaceContext
{
	/**
	 * list node
	 * for internal usage
	 */ 
    tListNode currentNode;

    /* each profile that is supported by the BlueNRG is 
	 * assigned a unique ID. The IDs are listed in ble_profile.h
	 * When a profile is registered with the main profile, it has
	 * to supply a valid ID. This will be used in all other
	 * operations after the profile is registered to identify the
	 * actions of the profile
	 */ 
    uint8 profileID;

   /**
    * callback function to be called by the main profile
	* to notify the profile about the events received at HCI
	*/ 
    PROFILE_CALLBACK_FUNC_TYPE callback_func;

    /**
	 * this variable is set when the profile has completed
	 * its initialization and is ready to start advertsing when 
	 * commanded by the user
	 */ 
    tStateAgreementVote voteForAdvertisableState;
	
	/**
	 * bitmask of the device information service
	 * characteristics required\n
	 */ 
	uint16 DISCharRequired;
}tProfileInterfaceContext;

/**
 * global profile context
 * contains the variables common to all 
 * profiles
 */ 
typedef struct _tBLEProfileGlobalContext{
	
	/**
	 * security requirements of the host
	 */ 
    tSecurityParams bleSecurityParam;
	
    /**
     * gap service handle
     */
	uint16 gapServiceHandle;
	
	/**
	 * device name characteristic handle
	 */ 
	uint16 devNameCharHandle;
	
	/**
	 * appearance characteristic handle
	 */ 
	uint16 appearanceCharHandle;
	
	/**
	 * connection handle of the current active connection
	 * When not in connection, the handle is set to 0xFFFF
	 */ 
    uint16 connectionHandle;

    /**
	 * length of the UUID list to be used while advertising
	 */ 
    uint8 advtServUUIDlen;
	
	/**
	 * the UUID list to be used while advertising
	 */ 
    uint8 advtServUUID[100];

}tBLEProfileGlobalContext;


/**
 * profile state
 */ 
typedef uint8 tProfileState;

/**
 * notofication event
 */
typedef uint8 tNotificationEvent;

/**
 * notification callback to the application
 */ 
typedef void (* BLE_CALLBACK_FUNCTION_TYPE)(tNotificationEvent event,uint8 evtLen,uint8* evtData);

/**
 * DIS callback
 */ 
typedef void (* DIS_CALLBACK)(uint8 status);

extern tBLEProfileGlobalContext gBLEProfileContext;
/*******************************************************************************
 * Function Prototypes
*******************************************************************************/

/**
 * BLE_Profile_Init
 * 
 * @param[in] tSecurityParams : Security requirements of the host
 * @param[in] BLE_CALLBACK_FUNCTION_TYPE : callback to be called
 *            by the profile to notify the application
 * 
 * Initializes the data structures required to run any of the BLE profiles.
 * Also initializes the HCI
 */ 
tBleStatus BLE_Profile_Init (tSecurityParams * securityParameters, 
							 BLE_CALLBACK_FUNCTION_TYPE bleCallbackFunc );

/**
 * BLE_Profile_Get_Main_State
 * 
 * @return main state of the profile state machine
 */ 
tProfileState BLE_Profile_Get_Main_State (void);

/**
 * BLE_Profile_Get_Sub_State
 * 
 * @return sub state of the profile state machine
 */ 
tProfileState BLE_Profile_Get_Sub_State (void);

/**
 * BLE_Profile_Register_Profile
 * 
 * @param[in] profileContext : profile context of the profile 
 *            to be registered
 * 
 * Registers the profile context with the main profile. The profile ID
 * will be used to monitor the commands sent by the profile at the HCI.
 * Notifications of the events received via HCI will be sent only to
 * registered profiles via the callback registered by the profile
 * with the main profile
 * 
 * @return BLE_STATUS_SUCCESS if the profile is successfully registered
 *         BLE_STATUS_FAILED if the profile registration failed
 */ 
tBleStatus BLE_Profile_Register_Profile ( tProfileInterfaceContext * profileContext);

/**
 * BLE_Profile_Unregister_Profile
 * 
 * @param[in] profileContext : profile context of the profile 
 *            to be unregistered
 * 
 * Unregisters the profile context with the main profile. The profile ID
 * will be used to monitor the commands sent by the profile at the HCI.
 * Notifications of the events received via HCI will be sent only to
 * registered profiles via the callback registered by the profile
 * with the main profile
 * 
 * @return BLE_STATUS_SUCCESS if the profile is successfully registered
 *         BLE_STATUS_FAILED if the profile registration failed
 */ 
tBleStatus BLE_Profile_Unregister_Profile(tProfileInterfaceContext* profileContext);

/**
 * BLE_Profile_Vote_For_Advertisable_State
 * 
 * @param[in] profileContext : context of the profile which is voting
 * 
 * The host can command the controller to go into advertising state only when all
 * the profiles have completed their initialization in the scenario where there are
 * multiple profiles running. So each profile will notify the main profile that
 * it has completed its initialization. The main profile will allow the advertising
 * command only when all registered profiles notify that they have completed their
 * profile specific initialization
 * 
 * @return BLE_STATUS_SUCCESS if the voting was accepted else BLE_STATUS_FAILED
 */ 
tBleStatus BLE_Profile_Vote_For_Advertisable_State(tProfileInterfaceContext* profileContext);

/**
 * BLE_Profile_Disconnect
 * 
 * issues the disconnect command to the controller.
 * This command can be given by any of the profiles
 * or the application at any point in time when in
 * connection
 */ 
tBleStatus BLE_Profile_Disconnect(void);

/**
 * BLE_Profile_Change_Advertise_Sub_State
 * 
 * @param[in] profileContext : pointer to the profile context that is requesting
 *            the state change
 * @param[in] nextState : the state to which the main profile's sub state has to be changed
 * 
 * If the profile requesting the state change is a registered profile and 
 * the main profile is in a state to accept the change, the sub state is changed.
 * This function has to be called by the upper profiles to notify the main profile
 * about teh advertising status. This function has to be called with the nextState 
 * parameter set to BLE_PROFILE_STATE_CONNECTABLE_IDLE when advertising is disabled
 * and BLE_PROFILE_STATE_ADVERTISING when the advertising has been successfully enabled
 */ 
void BLE_Profile_Change_Advertise_Sub_State(tProfileInterfaceContext* profileContext, tProfileState nextState);

/**
 * BLE_Profile_Send_Pass_Key
 * 
 * @param[in] passkey : passkey to be sent to the controller
 * 
 * sends the passkey supplied by the user to the controller
 */ 
tBleStatus BLE_Profile_Send_Pass_Key(uint32 passkey);

tBleStatus BLE_Profile_Start_Configuration_Exchange(void);

/**
 * BLE_Profile_Add_Advertisment_Service_UUID
 * 
 * @param[in] servUUID : UUID of the service to be added in the
 *            advertisement data
 * 
 * adds the UUID specified in the advertisement data list
 */ 
void BLE_Profile_Add_Advertisment_Service_UUID(uint16 servUUID);

/**
 * BLE_Profile_Remove_Advertisment_Service_UUID
 * 
 * @param[in] servUUID : UUID of the service to be removed from the
 *            advertisement data
 * 
 * removes the UUID specified in the advertisement data list
 */ 
tBleStatus BLE_Profile_Remove_Advertisment_Service_UUID(uint16 servUUID);

/**
 * Is_Profile_Present
 * 
 * @param[in] profileID: profile ID of the profile which needs to be
 *            checked for its registration with the main profile
 * 
 * searched through the list of registered profiles to see if
 * there is a profile of the ID specified is present
 * 
 * @return BLE_STATUS_SUCCESS if the profile specified by profileID
 *         is registered with the main profile else BLE_STATUS_FAILED
 */ 
tBleStatus Is_Profile_Present(uint8 profileID);

/**
 * Profile_Process_Q
 * 
 * processes the main profile state machine
 * and also the timer Q
 * 
 */ 
int Profile_Process_Q(void);

#endif /* BLE_PROFILE_H_ */
