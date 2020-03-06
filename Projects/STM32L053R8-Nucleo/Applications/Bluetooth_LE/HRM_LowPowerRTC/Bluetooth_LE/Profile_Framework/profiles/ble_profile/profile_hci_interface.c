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
*   FILENAME        -  profile_hci_interface.c
*
*   COMPILER        -  gnu gcc
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      20/04/2012
*   $Revision$:  first version
*   $Author$:    
*   Comments: 
*
*******************************************************************************
*  File Description 
*  ---------------------
* 
******************************************************************************/

/*******************************************************************************
 * Include Files
******************************************************************************/
#include <host_config.h>
#include <hal_types.h>
#include <ble_status.h>
#include <hci.h>
#include <profile_hci_interface.h>
#include <host_osal.h>
#include <debug.h>
#include <list.h>
#include <ble_profile.h>
/*******************************************************************************
 * Macro Declarations
******************************************************************************/

/*******************************************************************************
* type definitions
*******************************************************************************/


typedef void * (* PTR_TO_ARRAY_OF_VOID_PTRS)[];

/******************************************************************************
 * Local Variable
******************************************************************************/
static tHciCommandOpcode gHciSupportedCommandList[HCI_NUM_OF_COMMANDS_SUPPORTED] = {

                                        #ifdef HCI_LINK_CTL_CMD_DISCONNECT
                                            HCI_LINK_CTL_CMD_DISCONNECT,
                                        #endif
                                        #ifdef HCI_LINK_CTL_CMD_READ_REMOTE_VERSION
                                            HCI_LINK_CTL_CMD_READ_REMOTE_VERSION,
                                        #endif
                                        #ifdef HCI_HOST_CTL_CMD_SET_EVENT_MASK
                                            HCI_HOST_CTL_CMD_SET_EVENT_MASK,
                                        #endif
                                        #ifdef HCI_HOST_CTL_CMD_RESET
                                            HCI_HOST_CTL_CMD_RESET,
                                        #endif
                                        #ifdef HCI_HOST_CTL_CMD_READ_TRANSMIT_POWER_LEVEL
                                            HCI_HOST_CTL_CMD_READ_TRANSMIT_POWER_LEVEL,
                                        #endif
                                        #ifdef HCI_HOST_CTL_CMD_SET_CONTROLLER_TO_HOST_FC
                                            HCI_HOST_CTL_CMD_SET_CONTROLLER_TO_HOST_FC,
                                        #endif
                                        #ifdef HCI_HOST_CTL_CMD_HOST_BUFFER_SIZE
                                            HCI_HOST_CTL_CMD_HOST_BUFFER_SIZE,
                                        #endif
                                        #ifdef HCI_HOST_CTL_CMD_HOST_NUM_COMP_PKTS
                                            HCI_HOST_CTL_CMD_HOST_NUM_COMP_PKTS,
                                        #endif
                                        #ifdef HCI_INFO_PARAM_CMD_READ_LOCAL_VERSION
                                            HCI_INFO_PARAM_CMD_READ_LOCAL_VERSION,
                                        #endif
                                        #ifdef HCI_INFO_PARAM_CMD_READ_LOCAL_COMMANDS
                                            HCI_INFO_PARAM_CMD_READ_LOCAL_COMMANDS,
                                        #endif
                                        #ifdef HCI_INFO_PARAM_CMD_READ_LOCAL_FEATURES
                                            HCI_INFO_PARAM_CMD_READ_LOCAL_FEATURES,
                                        #endif
                                        #ifdef HCI_INFO_PARAM_CMD_READ_BD_ADDR
                                            HCI_INFO_PARAM_CMD_READ_BD_ADDR,
                                        #endif
                                        #ifdef HCI_STATUS_PARAM_CMD_READ_RSSI
                                            HCI_STATUS_PARAM_CMD_READ_RSSI,
                                        #endif
                                        #ifdef HCI_LE_CTL_SET_EVENT_MASK
                                            HCI_LE_CTL_SET_EVENT_MASK,
                                        #endif
                                        #ifdef HCI_LE_CTL_READ_BUFFER_SIZE
                                            HCI_LE_CTL_READ_BUFFER_SIZE,
                                        #endif
                                        #ifdef HCI_LE_CTL_READ_LOCAL_SUPPORTED_FEATURE
                                            HCI_LE_CTL_READ_LOCAL_SUPPORTED_FEATURE,
                                        #endif
                                        #ifdef HCI_LE_CTL_SET_RANDOM_ADDRESS
                                            HCI_LE_CTL_SET_RANDOM_ADDRESS,
                                        #endif
                                        #ifdef HCI_LE_CTL_SET_ADV_PARAMETERS
                                            HCI_LE_CTL_SET_ADV_PARAMETERS,
                                        #endif
                                        #ifdef HCI_LE_CTL_READ_ADV_CHANNEL_TX_POWER
                                            HCI_LE_CTL_READ_ADV_CHANNEL_TX_POWER,
                                        #endif
                                        #ifdef HCI_LE_CTL_SET_ADV_DATA
                                            HCI_LE_CTL_SET_ADV_DATA,
                                        #endif
                                        #ifdef HCI_LE_CTL_SET_SCAN_RESPONSE
                                            HCI_LE_CTL_SET_SCAN_RESPONSE,
                                        #endif
                                        #ifdef HCI_LE_CTL_SET_ADVERTIZE_ENABLE
                                            HCI_LE_CTL_SET_ADVERTIZE_ENABLE,
                                        #endif
                                        #ifdef HCI_LE_CTL_READ_WHITE_LIST_SIZE
                                            HCI_LE_CTL_READ_WHITE_LIST_SIZE,
                                        #endif
                                        #ifdef HCI_LE_CTL_CLEAR_WHILE_LIST
                                            HCI_LE_CTL_CLEAR_WHILE_LIST,
                                        #endif
                                        #ifdef HCI_LE_CTL_ADD_DEVICE_TO_WHILE_LIST
                                            HCI_LE_CTL_ADD_DEVICE_TO_WHILE_LIST,
                                        #endif
                                        #ifdef HCI_LE_CTL_REMOVE_DEVICE_FROM_WHITE_LIST
                                            HCI_LE_CTL_REMOVE_DEVICE_FROM_WHITE_LIST,
                                        #endif
                                        #ifdef HCI_LE_CTL_READ_CHANNEL_MAP
                                            HCI_LE_CTL_READ_CHANNEL_MAP,
                                        #endif
                                        #ifdef HCI_LE_CTL_ENCRYPT
                                            HCI_LE_CTL_ENCRYPT,
                                        #endif
                                        #ifdef HCI_LE_CTL_RAND
                                            HCI_LE_CTL_RAND,
                                        #endif
                                        #ifdef HCI_LE_CTL_LTK_REQ_REPLY
                                            HCI_LE_CTL_LTK_REQ_REPLY,
                                        #endif
                                        #ifdef HCI_LE_CTL_LTK_REQ_NEGATIVE_REPLY
                                            HCI_LE_CTL_LTK_REQ_NEGATIVE_REPLY,
                                        #endif
                                        #ifdef HCI_LE_CTL_READ_SUPPORTED_STATES
                                            HCI_LE_CTL_READ_SUPPORTED_STATES,
                                        #endif
                                        #ifdef HCI_LE_CTL_RECEIVER_TEST
                                            HCI_LE_CTL_RECEIVER_TEST,
                                        #endif
                                        #ifdef HCI_LE_CTL_TRANSMITTER_TEST
                                            HCI_LE_CTL_TRANSMITTER_TEST,
                                        #endif
                                        #ifdef HCI_LE_CTL_TEST_END
                                            HCI_LE_CTL_TEST_END,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE
                                            HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_SET_LIMITED_DISCOVERABLE
                                            HCI_VENDOR_CMD_GAP_SET_LIMITED_DISCOVERABLE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE
                                            HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_SET_DIRECT_CONNECTABLE
                                            HCI_VENDOR_CMD_GAP_SET_DIRECT_CONNECTABLE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_SET_SET_IO_CAPABILITY
                                            HCI_VENDOR_CMD_GAP_SET_SET_IO_CAPABILITY,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_SET_AUTHENTICATION_REQ
                                            HCI_VENDOR_CMD_GAP_SET_AUTHENTICATION_REQ,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_SET_AUTHORIZATION_REQ
                                            HCI_VENDOR_CMD_GAP_SET_AUTHORIZATION_REQ,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_PASS_KEY_RESP
                                            HCI_VENDOR_CMD_GAP_PASS_KEY_RESP,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_AUTHORIZATION_RESP
                                            HCI_VENDOR_CMD_GAP_AUTHORIZATION_RESP,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_INIT
                                            HCI_VENDOR_CMD_GAP_INIT,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_SET_NON_CONNECTABLE
                                            HCI_VENDOR_CMD_GAP_SET_NON_CONNECTABLE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_SET_UNDIRECTED_CONNECTABLE
                                            HCI_VENDOR_CMD_GAP_SET_UNDIRECTED_CONNECTABLE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_SLAVE_SECURITY_REQUEST
                                            HCI_VENDOR_CMD_GAP_SLAVE_SECURITY_REQUEST,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_UPDATE_ADV_DATA
                                            HCI_VENDOR_CMD_GAP_UPDATE_ADV_DATA,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_DELETE_AD_TYPE
                                            HCI_VENDOR_CMD_GAP_DELETE_AD_TYPE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_GET_SEC_LEVEL
                                            HCI_VENDOR_CMD_GAP_GET_SEC_LEVEL,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_SET_EVT_MASK
                                            HCI_VENDOR_CMD_GAP_SET_EVT_MASK,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GAP_CONFIGURE_WHITELIST
                                            HCI_VENDOR_CMD_GAP_CONFIGURE_WHITELIST,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_SERVER_INIT
                                            HCI_VENDOR_CMD_GATT_SERVER_INIT,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_ADD_SERVICE
                                            HCI_VENDOR_CMD_GATT_ADD_SERVICE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_INC_SERVICE
                                            HCI_VENDOR_CMD_GATT_INC_SERVICE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_ADD_CHAR
                                            HCI_VENDOR_CMD_GATT_ADD_CHAR,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_ADD_CHAR_DESC
                                            HCI_VENDOR_CMD_GATT_ADD_CHAR_DESC,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL
                                            HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_DEL_CHAR
                                            HCI_VENDOR_CMD_GATT_DEL_CHAR,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_DEL_SERVICE
                                            HCI_VENDOR_CMD_GATT_DEL_SERVICE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_DEL_INC_SERVICE
                                            HCI_VENDOR_CMD_GATT_DEL_INC_SERVICE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_SET_EVT_MASK
                                            HCI_VENDOR_CMD_GATT_SET_EVT_MASK,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_EXCHG_CONFIG   
                                            HCI_VENDOR_CMD_GATT_EXCHG_CONFIG,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_FIND_INFO
                                            HCI_VENDOR_CMD_GATT_FIND_INFO,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_FIND_ATTR_BY_TYPE_VAL
                                            HCI_VENDOR_CMD_GATT_FIND_ATTR_BY_TYPE_VAL,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_READ_ATTR_BY_TYPE
                                            HCI_VENDOR_CMD_GATT_READ_ATTR_BY_TYPE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_READ_ATTR_BY_GROUP_TYPE
                                            HCI_VENDOR_CMD_GATT_READ_ATTR_BY_GROUP_TYPE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_PREPARE_WRITE
                                            HCI_VENDOR_CMD_GATT_PREPARE_WRITE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_EXECUTE_WRITES
                                            HCI_VENDOR_CMD_GATT_EXECUTE_WRITES,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_DISC_ALL_PRIMARY_SERVICES
                                            HCI_VENDOR_CMD_GATT_DISC_ALL_PRIMARY_SERVICES,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_DISC_PRIMARY_SERVICE_BY_UUID
                                            HCI_VENDOR_CMD_GATT_DISC_PRIMARY_SERVICE_BY_UUID,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_FIND_INCLUDED_SERVICES
                                            HCI_VENDOR_CMD_GATT_FIND_INCLUDED_SERVICES,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_DISC_ALL_CHARAC_OF_A_SERVICE
                                            HCI_VENDOR_CMD_GATT_DISC_ALL_CHARAC_OF_A_SERVICE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_DISC_CHARAC_BY_UUID
                                            HCI_VENDOR_CMD_GATT_DISC_CHARAC_BY_UUID,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_DISC_ALL_CHAR_DESCRIPTORS
                                            HCI_VENDOR_CMD_GATT_DISC_ALL_CHAR_DESCRIPTORS,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_READ_CHARACTERISTIC_VAL
                                            HCI_VENDOR_CMD_GATT_READ_CHARACTERISTIC_VAL,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_READ_CHARAC_USING_UUID
                                            HCI_VENDOR_CMD_GATT_READ_CHARAC_USING_UUID,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_READ_LONG_CHARAC_VAL
                                            HCI_VENDOR_CMD_GATT_READ_LONG_CHARAC_VAL,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_READ_MULTIPLE_CHARAC_VALUES
                                            HCI_VENDOR_CMD_GATT_READ_MULTIPLE_CHARAC_VALUES,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_WRITE_CHARAC_VAL
                                            HCI_VENDOR_CMD_GATT_WRITE_CHARAC_VAL,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_WRITE_LONG_CHARAC_VAL
                                            HCI_VENDOR_CMD_GATT_WRITE_LONG_CHARAC_VAL,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_WRITE_CHARAC_RELIABLE
                                            HCI_VENDOR_CMD_GATT_WRITE_CHARAC_RELIABLE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_READ_CHARAC_DESCRIPTOR
                                            HCI_VENDOR_CMD_GATT_READ_CHARAC_DESCRIPTOR,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_READ_LONG_CHARAC_DESCRIPTOR
                                            HCI_VENDOR_CMD_GATT_READ_LONG_CHARAC_DESCRIPTOR,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_WRITE_CHARAC_DESCRIPTOR
                                            HCI_VENDOR_CMD_GATT_WRITE_CHARAC_DESCRIPTOR,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_WRITE_LONG_CHARAC_DESCRIPTOR
                                            HCI_VENDOR_CMD_GATT_WRITE_LONG_CHARAC_DESCRIPTOR,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_WRITE_WITHOUT_RESPONSE
                                            HCI_VENDOR_CMD_GATT_WRITE_WITHOUT_RESPONSE,
                                        #endif
                                        #ifdef HCI_VENDOR_CMD_GATT_SIGNED_WRITE_WITHOUT_RESPONSE
                                            HCI_VENDOR_CMD_GATT_SIGNED_WRITE_WITHOUT_RESPONSE,
                                        #endif
#ifdef SET_TX_POWER_COMMAND /* TBR */
                                        #ifdef HCI_VENDOR_CMD_SET_TX_POWER_LEVEL
                                            HCI_VENDOR_CMD_SET_TX_POWER_LEVEL,
                                        #endif
#endif
                                            
#ifdef SET_L2CAP_CONN_UPD_REQ_COMMAND /* TBR */
                                        #ifdef HCI_VENDOR_CMD_L2CAP_CONN_UPD_REQ 
                                            HCI_VENDOR_CMD_L2CAP_CONN_UPD_REQ,
                                        #endif
#endif
                                                                            };


static tHciCmdInterfaceStatus hciCmdInterfaceStatus[HCI_NUM_OF_COMMANDS_SUPPORTED] = {HCI_PROFILE_CMD_INTERFACE_FREE};

static tHciCmdInterfaceStatus gattClientProcedureStatus = HCI_PROFILE_CMD_INTERFACE_FREE;
static tHciCommandOpcode runningGATTProcedureOpcode ;

extern void *hostdbgfile;
extern void *profiledbgfile;

/*******************************************************************************
 * Local Function Prototypes
*******************************************************************************/
static tBleStatus Profile_HCI_Interface_Send_Command(uint8 profileID,
														uint16 opcode,
														uint8 numOfCmdParam,
														void * (* cmdParamPtrArray)[]);

tBleStatus Profile_HCI_Interface_Send_GATT_Procedure_Command(uint8 profileID, 
																uint16 opcode,
																uint8 numOfCmdParam,
																void * (* cmdParamPtrArray)[] );

extern void BLE_Profile_Set_State(tProfileState state);
/*******************************************************************************
 * Function definitions 
*******************************************************************************/
void Profile_HCI_Interface_Init (void)
{
    /* Initialize all interface status flags with default values */
    uint8 cmdIndex = 0;
    
    for (cmdIndex = 0 ; cmdIndex < HCI_NUM_OF_COMMANDS_SUPPORTED ; cmdIndex++)
    {
        /* save the command interface status */
        hciCmdInterfaceStatus[cmdIndex] = HCI_PROFILE_CMD_INTERFACE_FREE;
    }
	
	runningGATTProcedureOpcode = HCI_CMD_NO_OPERATION;
}

tHciCmdInterfaceStatus Profile_HCI_Interface_Get_Status ( uint16 opcode )
{
    uint8 cmdIndex = 0;
    uint8 retval = BLE_STATUS_UNKNOWN_COMMAND;

    for (cmdIndex = 0 ; cmdIndex < HCI_NUM_OF_COMMANDS_SUPPORTED ; cmdIndex++)
    {
        if (gHciSupportedCommandList[cmdIndex] == opcode)
        {
            break;
        }
    }

    if (cmdIndex == HCI_NUM_OF_COMMANDS_SUPPORTED)
    {
        return (retval);
    }

    retval = hciCmdInterfaceStatus[cmdIndex];
  
    return (retval);
}


tBleStatus Profile_HCI_Interface_Set_Status ( uint16 opcode , uint8 sflag )
{
    uint8 cmdIndex = 0;
    
    if ((sflag != HCI_PROFILE_CMD_INTERFACE_BUSY) && (sflag != HCI_PROFILE_CMD_INTERFACE_FREE))
    {
        return (BLE_STATUS_INVALID_PARAMS);
    }
    
    for (cmdIndex = 0 ; cmdIndex < HCI_NUM_OF_COMMANDS_SUPPORTED ; cmdIndex++)
    {
        if (gHciSupportedCommandList[cmdIndex] == opcode)
        {
            break;
        }
    }
    
    if (cmdIndex == HCI_NUM_OF_COMMANDS_SUPPORTED)
    {
        return (BLE_STATUS_INVALID_PARAMS);
    }
    
    /* save the command interface status */
    hciCmdInterfaceStatus[cmdIndex] = sflag;
  
    return (BLE_STATUS_SUCCESS);
}

tBleStatus Profile_HCI_Interface_Send_Command(uint8 profileID,
												 uint16 opcode,
                                                 uint8 numOfCmdParam,
                                                 void * (* cmdParamPtrArray)[] )
{
    HCI_MESG_DBG ( hostdbgfile, "Sending command %04X through HCI \n", opcode );
	
    return HCI_Send_Command(profileID,opcode, numOfCmdParam, cmdParamPtrArray );
}


tBleStatus Profile_HCI_Interface_Send_GATT_Procedure_Command(uint8 profileID, 
																uint16 opcode,
																uint8 numOfCmdParam,
																void * (* cmdParamPtrArray)[] )
{
	tBleStatus retval = BLE_STATUS_FAILED;

    HCI_MESG_DBG ( hostdbgfile, "Sending command %04X through HCI \n", opcode );
	retval = HCI_Send_Command (profileID, opcode, numOfCmdParam, cmdParamPtrArray );

    if ( retval == BLE_STATUS_SUCCESS)
    {
		gattClientProcedureStatus = HCI_PROFILE_CMD_INTERFACE_BUSY;
		runningGATTProcedureOpcode = opcode;
    }

    return (retval);
}

void Profile_HCI_Interface_Set_GATT_Procedure_Status(uint16 opcode)
{
	gattClientProcedureStatus = HCI_PROFILE_CMD_INTERFACE_BUSY;
	runningGATTProcedureOpcode = opcode;
}

void Profile_HCI_Interface_Reset_GATT_Procedure_Status(void)
{
	gattClientProcedureStatus = HCI_PROFILE_CMD_INTERFACE_FREE;
	runningGATTProcedureOpcode = HCI_CMD_NO_OPERATION;
}


tHciCommandOpcode Profile_HCI_Interface_Get_Current_GATT_Procedure_Opcode ( void )
{
	return (runningGATTProcedureOpcode);
}

/* ++++++++++++++++++++++ Group: Link control commands +++++++++++++++++++++ */
/* HCI_LINK_CTL_CMD_DISCONNECT */
#ifdef HCI_LINK_CTL_CMD_DISCONNECT
tBleStatus HCI_Disconnect_Cmd(uint8 profileID,uint16 Connection_Handle,uint8 Reason )
{
    void * cmdParam[2] = {NULL};
    uint16 connHandle = Connection_Handle;
    uint8 reason = Reason; 

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_Disconnect_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_Disconnect_Cmd \n" );

    cmdParam[0] = (void *)&connHandle;
    cmdParam[1] = (void *)&reason;

	HCI_MESG_DBG ( hostdbgfile, "Disconnect Param: %04X %02X \n", connHandle, reason );

    return (Profile_HCI_Interface_Send_Command(profileID,HCI_LINK_CTL_CMD_DISCONNECT, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif

/* HCI_LINK_CTL_CMD_READ_REMOTE_VERSION */
#ifdef HCI_LINK_CTL_CMD_READ_REMOTE_VERSION
tBleStatus HCI_Send_Read_Remote_Version_Info_Cmd(uint8 profileID,uint16 Connection_Handle)
{
    void * cmdParam[1] = {NULL};
    uint16 connHandle = Connection_Handle;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_Send_Read_Remote_Version_Info_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_Send_Read_Remote_Version_Info_Cmd \n" );

    cmdParam[0] = (void *)&connHandle;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LINK_CTL_CMD_READ_REMOTE_VERSION, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif



/* Group: Host controller and baseband commands */

/* HCI_HOST_CTL_CMD_SET_EVENT_MASK */
#ifdef HCI_HOST_CTL_CMD_SET_EVENT_MASK
tBleStatus HCI_Set_Event_Mask_Cmd(uint8 profileID,uint8 * EventMask )
{
    void * cmdParam[1] = {NULL};
    if (EventMask == NULL)
    {
        return (BLE_STATUS_NULL_PARAM);
    }

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_Set_Event_Mask_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_Set_Event_Mask_Cmd \n" );
	HCI_MESG_DATA (hostdbgfile, EventMask, 8);

    cmdParam[0] = (void *)EventMask;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_HOST_CTL_CMD_SET_EVENT_MASK, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_HOST_CTL_CMD_RESET */
#ifdef HCI_HOST_CTL_CMD_RESET
tBleStatus HCI_Reset_Cmd(uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_Reset_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_Reset_Cmd \n" );

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_HOST_CTL_CMD_RESET, 0, NULL ));
}
#endif


/* HCI_HOST_CTL_CMD_READ_TRANSMIT_POWER_LEVEL */
#ifdef HCI_HOST_CTL_CMD_READ_TRANSMIT_POWER_LEVEL
tBleStatus HCI_Read_Trmt_Power_Level_Cmd(uint8 profileID,uint16 Connection_Handle,uint8 Type)
{
    void * cmdParam[10] = {NULL};

    uint16 connectionHandle = Connection_Handle;
    uint8 type = Type;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_Read_Trmt_Power_Level_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_Read_Trmt_Power_Level_Cmd \n" );

    cmdParam[0] = (void *)&connectionHandle;
    cmdParam[1] = (void *)&type;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_HOST_CTL_CMD_READ_TRANSMIT_POWER_LEVEL, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_HOST_CTL_CMD_SET_CONTROLLER_TO_HOST_FC */
#ifdef HCI_HOST_CTL_CMD_SET_CONTROLLER_TO_HOST_FC
tBleStatus HCI_Set_Contr_Host_Flow_Control_Cmd(uint8 profileID,uint8 Flow_Control_Enable)
{
    void * cmdParam[1] = {NULL};
    uint8 flowControlEnable = Flow_Control_Enable;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_Set_Contr_Host_Flow_Control_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_Set_Contr_Host_Flow_Control_Cmd \n" );

    cmdParam[0] = (void *)&flowControlEnable;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_HOST_CTL_CMD_SET_CONTROLLER_TO_HOST_FC, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_HOST_CTL_CMD_HOST_BUFFER_SIZE */
#ifdef HCI_HOST_CTL_CMD_HOST_BUFFER_SIZE
tBleStatus HCI_Host_Buffer_Size_Cmd(uint8 profileID,uint16 Host_ACL_Data_Pkt_Length, 
                                     uint8 Host_Synchronous_Data_Pkt_Length, 
                                     uint16 Host_Total_Num_ACL_Data_Pkt, 
                                     uint16  Host_Total_Num_Synchronous_Data_Pkt )
{
    void * cmdParam[4] = {NULL};

    uint16 hostACLDataPktLength = Host_ACL_Data_Pkt_Length;
    uint8 hostSynchronousDataPktLength = Host_Synchronous_Data_Pkt_Length;
    uint16 hostTotalNumACLDataPkt = Host_Total_Num_ACL_Data_Pkt;
    uint16 hostTotalNumSynchronousDataPkt = Host_Total_Num_Synchronous_Data_Pkt;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_Host_Buffer_Size_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_Host_Buffer_Size_Cmd \n" );

    cmdParam[0] = (void *)&hostACLDataPktLength;
    cmdParam[1] = (void *)&hostSynchronousDataPktLength;
    cmdParam[2] = (void *)&hostTotalNumACLDataPkt;
    cmdParam[3] = (void *)&hostTotalNumSynchronousDataPkt;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_HOST_CTL_CMD_HOST_BUFFER_SIZE, 4, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_HOST_CTL_CMD_HOST_NUM_COMP_PKTS */
#ifdef HCI_HOST_CTL_CMD_HOST_NUM_COMP_PKTS
tBleStatus HCI_Host_Nbr_Completed_Pkts_Cmd(uint8 profileID,uint8 Number_Of_Handles,  /* can this be 0 also ?? */
                                            uint16 * Connection_Handle, /* can this be NULL also ?? */
                                            uint16 * Host_Num_Of_Completed_Pkt /* can this be NULL also ?? */
                                            )
{
    void * cmdParam[3] = {NULL};
    uint8 numOfHandles = Number_Of_Handles;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_Host_Nbr_Completed_Pkts_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_Host_Nbr_Completed_Pkts_Cmd \n" );

    if ((Number_Of_Handles == 0) || (Connection_Handle == NULL) || (Host_Num_Of_Completed_Pkt == NULL))
    {
        return (BLE_STATUS_INVALID_PARAMS);
    }

    cmdParam[0] = (void *)&numOfHandles;
    cmdParam[1] = (void *)Connection_Handle;
    cmdParam[2] = (void *)Host_Num_Of_Completed_Pkt;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_HOST_CTL_CMD_HOST_NUM_COMP_PKTS, 3, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* Group: Informational parameters */

/* HCI_INFO_PARAM_CMD_READ_LOCAL_VERSION */
#ifdef HCI_INFO_PARAM_CMD_READ_LOCAL_VERSION
tBleStatus HCI_Read_Local_Version_Info_Cmd(uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_Read_Local_Version_Info_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_Read_Local_Version_Info_Cmd \n" );

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_INFO_PARAM_CMD_READ_LOCAL_VERSION, 0, NULL ));
}
#endif


/* HCI_INFO_PARAM_CMD_READ_LOCAL_COMMANDS */
#ifdef HCI_INFO_PARAM_CMD_READ_LOCAL_COMMANDS
tBleStatus HCI_Read_Local_Supported_Commands_Cmd(uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_Read_Local_Supported_Commands_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_Read_Local_Supported_Commands_Cmd \n" );

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_INFO_PARAM_CMD_READ_LOCAL_COMMANDS, 0, NULL ));
}
#endif


/* HCI_INFO_PARAM_CMD_READ_LOCAL_FEATURES */
#ifdef HCI_INFO_PARAM_CMD_READ_LOCAL_FEATURES
tBleStatus HCI_Read_Local_Supported_Features_Cmd(uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_Read_Local_Supported_Features_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_Read_Local_Supported_Features_Cmd \n" );

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_INFO_PARAM_CMD_READ_LOCAL_FEATURES, 0, NULL ));
}
#endif


/* HCI_INFO_PARAM_CMD_READ_BD_ADDR */
#ifdef HCI_INFO_PARAM_CMD_READ_BD_ADDR
tBleStatus HCI_Read_BD_ADDR_Cmd(uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_Read_BD_ADDR_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_Read_BD_ADDR_Cmd \n" );

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_INFO_PARAM_CMD_READ_BD_ADDR, 0, NULL ));
}
#endif


/* Group: Status parameters */

/* HCI_STATUS_PARAM_CMD_READ_RSSI */
#ifdef HCI_STATUS_PARAM_CMD_READ_RSSI
tBleStatus HCI_Read_RSSI_Cmd(uint8 profileID,uint16 Handle)
{
    void * cmdParam[1] = {NULL};
    uint16 handle = Handle;
    
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_Read_RSSI_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_Read_RSSI_Cmd \n" );

    cmdParam[0] = (void *)&handle;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_STATUS_PARAM_CMD_READ_RSSI, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* Group: LE controller commands */
/* HCI_LE_CTL_SET_EVENT_MASK */
#ifdef HCI_LE_CTL_SET_EVENT_MASK
tBleStatus HCI_LE_Set_Event_Mask_Cmd(uint8 profileID,uint8 * EventMask)
{
    void * cmdParam[1] = {NULL};

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Set_Event_Mask_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Set_Event_Mask_Cmd \n" );
	HCI_MESG_DATA (hostdbgfile, EventMask, 8);

    if (EventMask == NULL)
    {
        return (BLE_STATUS_NULL_PARAM);
    }
    
    cmdParam[0] = (void *)EventMask;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_SET_EVENT_MASK, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_LE_CTL_READ_BUFFER_SIZE */
#ifdef HCI_LE_CTL_READ_BUFFER_SIZE
tBleStatus HCI_LE_Read_Buffer_Size_Cmd(uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Read_Buffer_Size_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Read_Buffer_Size_Cmd \n" );

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_READ_BUFFER_SIZE, 0, NULL ));
}
#endif


/* HCI_LE_CTL_READ_LOCAL_SUPPORTED_FEATURE */
#ifdef HCI_LE_CTL_READ_LOCAL_SUPPORTED_FEATURE
tBleStatus HCI_LE_Read_Local_Supp_Features_Cmd(uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Read_Local_Supp_Features_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Read_Local_Supp_Features_Cmd \n" );

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_READ_LOCAL_SUPPORTED_FEATURE, 0, NULL));
}
#endif


/* HCI_LE_CTL_SET_RANDOM_ADDRESS */
#ifdef HCI_LE_CTL_SET_RANDOM_ADDRESS
tBleStatus HCI_LE_Set_Random_Address_Cmd(uint8 profileID,uint8 * Random_Address)
{
    void * cmdParam[1] = {NULL};

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Set_Random_Address_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Set_Random_Address_Cmd \n" );

    if (Random_Address == NULL)
    {
        return (HCI_RESULT_FAILURE);
    }
    
    cmdParam[0] = (void *)Random_Address;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_SET_RANDOM_ADDRESS, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_LE_CTL_SET_ADV_PARAMETERS */
#ifdef HCI_LE_CTL_SET_ADV_PARAMETERS
tBleStatus HCI_LE_Set_Adv_Parameters_Cmd(uint8 profileID, uint16 Advertising_Interval_Min, 
                                          uint16 Advertising_Interval_Max, 
                                          uint8 Advertising_Type, 
                                          uint8 Own_Address_Type, 
                                          uint8 Direct_Address_Type, 
                                          uint8 * Direct_Address, 
                                          uint8 Advertising_Channer_Map, 
                                          uint8 Adrevtising_Filter_Policy )
{
    void * cmdParam[8] = {NULL};
    uint16 advertisingIntervalMin = Advertising_Interval_Min;
    uint16 advertisingIntervalMax = Advertising_Interval_Max;
    uint8 advertisingType = Advertising_Type;
    uint8 ownAddressType = Own_Address_Type; 
    uint8 directAddressType = Direct_Address_Type;
    uint8 advertisingChannerMap = Advertising_Channer_Map;
    uint8 adrevtisingFilterPolicy = Adrevtising_Filter_Policy;
    
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Set_Adv_Parameters_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Set_Adv_Parameters_Cmd \n" );

    if (Direct_Address == NULL)
    {
        return (HCI_RESULT_FAILURE);
    }

    cmdParam[0] = (void *)&advertisingIntervalMin;
    cmdParam[1] = (void *)&advertisingIntervalMax;
    cmdParam[2] = (void *)&advertisingType;
    cmdParam[3] = (void *)&ownAddressType;
    cmdParam[4] = (void *)&directAddressType;
    cmdParam[5] = (void *)Direct_Address;
    cmdParam[6] = (void *)&advertisingChannerMap;
    cmdParam[7] = (void *)&adrevtisingFilterPolicy;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_SET_ADV_PARAMETERS, 8, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_LE_CTL_READ_ADV_CHANNEL_TX_POWER */
#ifdef HCI_LE_CTL_READ_ADV_CHANNEL_TX_POWER
tBleStatus HCI_LE_Read_Adv_Chnl_Tx_Power_Cmd(uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Read_Adv_Chnl_Tx_Power_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Read_Adv_Chnl_Tx_Power_Cmd \n" );

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_READ_ADV_CHANNEL_TX_POWER, 0, NULL ));
}
#endif


/* HCI_LE_CTL_SET_ADV_DATA */
#ifdef HCI_LE_CTL_SET_ADV_DATA
tBleStatus HCI_LE_Set_Adv_Data_Cmd(uint8 profileID,uint8 Advertising_Data_Length,  /* can it be 0 also ??*/
                                    uint8 * Advertising_Data /* can it be NULL also ??*/
                                    )
{
    void * cmdParam[2] = {NULL};
    uint8 advertisingDataLength = Advertising_Data_Length;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Set_Adv_Data_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Set_Adv_Data_Cmd \n" );

    if (Advertising_Data == NULL)
    {
        return (HCI_RESULT_FAILURE);
    }
    
    cmdParam[0] = (void *)&advertisingDataLength;
    cmdParam[1] = (void *)Advertising_Data;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_SET_ADV_DATA, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_LE_CTL_SET_SCAN_RESPONSE */
#ifdef HCI_LE_CTL_SET_SCAN_RESPONSE
tBleStatus HCI_LE_Set_Scan_Resp_Data_Cmd(uint8 profileID,uint8 Scan_Response_Data_Length, 
                                          uint8 * Scan_Response_Data)
{
    void * cmdParam[2] = {NULL};
    uint8 scanResponseDataLength = Scan_Response_Data_Length;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Set_Scan_Resp_Data_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Set_Scan_Resp_Data_Cmd \n" );

    if (Scan_Response_Data == NULL)
    {
        return 0x01;
    }
    
    cmdParam[0] = (void *)&scanResponseDataLength;
    cmdParam[1] = (void *)Scan_Response_Data;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_SET_SCAN_RESPONSE, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_LE_CTL_SET_ADVERTIZE_ENABLE */
#ifdef HCI_LE_CTL_SET_ADVERTIZE_ENABLE
tBleStatus HCI_LE_Set_Adv_Enable_Cmd(uint8 profileID,uint8 Advertising_Enable)
{
    void * cmdParam[1] = {NULL};
    uint8 advertisingEnable = Advertising_Enable;
    
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Set_Adv_Enable_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Set_Adv_Enable_Cmd \n" );

    cmdParam[0] = (void *)&advertisingEnable;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_SET_ADVERTIZE_ENABLE, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_LE_CTL_READ_WHITE_LIST_SIZE */
#ifdef HCI_LE_CTL_READ_WHITE_LIST_SIZE
tBleStatus HCI_LE_Read_White_List_Size_Cmd(uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Read_White_List_Size_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Read_White_List_Size_Cmd \n" );

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_READ_WHITE_LIST_SIZE, 0, NULL));
}
#endif


/* HCI_LE_CTL_CLEAR_WHILE_LIST */
#ifdef HCI_LE_CTL_CLEAR_WHILE_LIST
tBleStatus HCI_LE_Clear_White_List_Cmd(uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Clear_White_List_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Clear_White_List_Cmd \n" );

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_CLEAR_WHILE_LIST, 0, NULL));
}
#endif


/* HCI_LE_CTL_ADD_DEVICE_TO_WHILE_LIST */
#ifdef HCI_LE_CTL_ADD_DEVICE_TO_WHILE_LIST
tBleStatus HCI_LE_Add_Dev_To_White_List_Cmd(uint8 profileID,uint8 Address_Type,
                                             uint8 * Address )
{
    void * cmdParam[2] = {NULL};
    uint8 addressType = Address_Type;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Add_Dev_To_White_List_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Add_Dev_To_White_List_Cmd \n" );

    if (Address == NULL)
    {
        return (BLE_STATUS_NULL_PARAM);
    }

    cmdParam[0] = (void *)&addressType;
    cmdParam[1] = (void *)Address;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_ADD_DEVICE_TO_WHILE_LIST, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_LE_CTL_REMOVE_DEVICE_FROM_WHITE_LIST */
#ifdef HCI_LE_CTL_REMOVE_DEVICE_FROM_WHITE_LIST
tBleStatus HCI_LE_Rem_Dev_From_White_List_Cmd(uint8 profileID,uint8 Address_Type,
                                               uint8 * Address )
{
    void * cmdParam[2] = {NULL};
    uint8 addressType = Address_Type;
    
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Rem_Dev_From_White_List_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Rem_Dev_From_White_List_Cmd \n" );

    if (Address == NULL)
	{
        return (HCI_RESULT_FAILURE);
    }

    cmdParam[0] = (void *)&addressType;
    cmdParam[1] = (void *)Address;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_REMOVE_DEVICE_FROM_WHITE_LIST, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_LE_CTL_READ_CHANNEL_MAP */
#ifdef HCI_LE_CTL_READ_CHANNEL_MAP
tBleStatus HCI_LE_Read_Channel_Map_Cmd(uint8 profileID,uint16 Connection_Handle)
{
    void * cmdParam[1] = {NULL};
    uint16 connectionHandle = Connection_Handle;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Read_Channel_Map_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Read_Channel_Map_Cmd \n" );

    cmdParam[0] = (void *)&connectionHandle;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_READ_CHANNEL_MAP, 1, &cmdParam));
}
#endif


/* HCI_LE_CTL_ENCRYPT */
#ifdef HCI_LE_CTL_ENCRYPT
tBleStatus HCI_LE_Encrypt_Cmd(uint8 profileID,uint8 Key[16], 
                               uint8 Plain_Text_Data[16])
{
    void * cmdParam[2] = {NULL};

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Encrypt_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Encrypt_Cmd \n" );

    if ((Key == NULL) || (Plain_Text_Data == NULL))
    {
        return (HCI_RESULT_FAILURE);
    }
        
    cmdParam[0] = (void *)Key;
    cmdParam[1] = (void *)Plain_Text_Data;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_ENCRYPT, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_LE_CTL_RAND */
#ifdef HCI_LE_CTL_RAND
tBleStatus HCI_LE_Rand_Cmd(uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Rand_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Rand_Cmd \n" );

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_RAND, 0, NULL ));
}
#endif


/* HCI_LE_CTL_LTK_REQ_REPLY */
#ifdef HCI_LE_CTL_LTK_REQ_REPLY
tBleStatus HCI_LE_LTK_Req_Reply_Cmd(uint8 profileID,uint16 Connection_Handle,
                                     uint8 * Long_Term_Key )
{
    void * cmdParam[2] = {NULL};
    uint16 connectionHandle = Connection_Handle;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_LTK_Req_Reply_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_LTK_Req_Reply_Cmd \n" );

    if (Long_Term_Key == NULL)
    {
        return (HCI_RESULT_FAILURE);
    }

    cmdParam[0] = (void *)&connectionHandle;
    cmdParam[1] = (void *)Long_Term_Key;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_LTK_REQ_REPLY, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_LE_CTL_LTK_REQ_NEGATIVE_REPLY */
#ifdef HCI_LE_CTL_LTK_REQ_NEGATIVE_REPLY
tBleStatus HCI_LE_LTK_Req_Neg_Reply_Cmd(uint8 profileID,uint16 Connection_Handle)
{
    void * cmdParam[1] = {NULL};
    uint16 connectionHandle = Connection_Handle;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_LTK_Req_Neg_Reply_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_LTK_Req_Neg_Reply_Cmd \n" );

    cmdParam[0] = (void *)&connectionHandle;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_LTK_REQ_NEGATIVE_REPLY, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_LE_CTL_READ_SUPPORTED_STATES */
#ifdef HCI_LE_CTL_READ_SUPPORTED_STATES
tBleStatus HCI_LE_Read_Supported_States_Cmd(uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Read_Supported_States_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Read_Supported_States_Cmd \n" );

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_READ_SUPPORTED_STATES, 0, NULL ));
}
#endif


/* HCI_LE_CTL_RECEIVER_TEST */
#ifdef HCI_LE_CTL_RECEIVER_TEST
tBleStatus HCI_LE_Receiver_Test_Cmd(uint8 profileID,uint8 Rx_Frequency)
{
    void * cmdParam[10] = {NULL};
    uint8 rxFrequency = Rx_Frequency;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Receiver_Test_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Receiver_Test_Cmd \n" );

    cmdParam[0] = (void *)&rxFrequency;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_RECEIVER_TEST, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_LE_CTL_TRANSMITTER_TEST */
#ifdef HCI_LE_CTL_TRANSMITTER_TEST
tBleStatus HCI_LE_Transmitter_Test_Cmd(uint8 profileID,uint8 Tx_Frequency, 
                                        uint8 Length_Of_Text_Data, 
                                        uint8 Packet_Payload )
{
    void * cmdParam[3] = {NULL};
    uint8 txFrequency = Tx_Frequency;
    uint8 lengthOfTextData = Length_Of_Text_Data;
    uint8 packetPayload = Packet_Payload;
                            
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Transmitter_Test_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Transmitter_Test_Cmd \n" );

    cmdParam[0] = (void *)&txFrequency;
    cmdParam[1] = (void *)&lengthOfTextData;
    cmdParam[2] = (void *)&packetPayload;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_TRANSMITTER_TEST, 3, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_LE_CTL_TEST_END */
#ifdef HCI_LE_CTL_TEST_END
tBleStatus HCI_LE_Test_End_Cmd(uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Test_End_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Test_End_Cmd \n" );

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_LE_CTL_TEST_END, 0, NULL));
}
#endif


/* Group: GAP vendor Specific commands */
/* HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE */
#ifdef HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE
tBleStatus HCI_GAP_Set_Non_Discoverable_Cmd (uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Set_Non_Discoverable_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_Set_Non_Discoverable_Cmd \n" );

	 if((BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE) &&
	    (BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_DISABLING_ADVERTISING))
	{
		/* if the device is already advertising because of
		* another profile then this command should not be
		* issued agin but the profile also should not treat
		* this as failure
		*/
		PROFILE_MESG_DBG(profiledbgfile, "BLE_PROFILE_STATE_DISABLING_ADVERTISING\n");
		return BLE_STATUS_ACTION_DISALLOWED;
	}
	else if(BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE)
	{
		BLE_Profile_Set_State(BLE_PROFILE_STATE_DISABLING_ADVERTISING);
	}
    return (Profile_HCI_Interface_Send_Command (profileID,HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE, 0, NULL ));
}
#endif


/* HCI_VENDOR_CMD_GAP_SET_LIMITED_DISCOVERABLE */
#ifdef HCI_VENDOR_CMD_GAP_SET_LIMITED_DISCOVERABLE
tBleStatus HCI_GAP_Set_Limited_Discoverable_Cmd (uint8 profileID,
													uint8 Advertising_Event_Type,
                                                     uint16 Advertising_Interval_Min,
                                                     uint16 Advertising_Interval_Max,
                                                     uint8 Address_Type,
                                                     uint8 Adv_Filter_Policy,
                                                     uint8 Local_Name_Length,  /* can this be 0 also ?? */
                                                     uint8 * Local_Name,       /* can this be NULL also ?? */
                                                     uint8 Service_UUID_Length,  /* can this be 0 also ?? */
                                                     uint8 *Service_Uuid_List,   /* can this be NULL also ?? */
                                                     uint16 Slave_Conn_Interval_Min,
                                                     uint16 Slave_Conn_Interval_Max )
{
    void * cmdParam[11] = {NULL};

    uint8 numofparam = 0;
    uint8 advertisingEventType = Advertising_Event_Type;
    uint16 advertisingIntervalMin = Advertising_Interval_Min;
    uint16 advertisingIntervalMax = Advertising_Interval_Max;
    uint8 addressType = Address_Type;
    uint8 advertisingFilterPolicy = Adv_Filter_Policy;
    uint8 localNameLength = Local_Name_Length;
/*    uint8 * Local_Name; */
    uint8 serviceUuidLength = Service_UUID_Length;
/*    uint8 *Service_Uuid_List; */
    uint16 slaveCommIntervalMin = Slave_Conn_Interval_Min;
    uint16 slaveCommIntervalMax = Slave_Conn_Interval_Max;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Set_Limited_Discoverable_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_Set_Limited_Discoverable_Cmd \n" );

	/* check the main profile state. If it is already in advertising
	 * state, then this command should not be issued
	 */
    if((BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE) &&
	    ((BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_ADVERTISING) ||
		(BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_ENABLING_ADVERTISING)))
	{
		/* if the device is already advertising because of
		* another profile then this command should not be
		* issued agin but the profile also should not treat
		* this as failure
		*/
		PROFILE_MESG_DBG(profiledbgfile, "BLE_STATUS_ALREADY_ADVERTISING\n");
		return BLE_STATUS_ALREADY_ADVERTISING;
	}
	
	/* set the state to indicate this command has been issued */
	BLE_Profile_Set_State(BLE_PROFILE_STATE_ENABLING_ADVERTISING);
	
    cmdParam[numofparam] = (void *)&advertisingEventType;
    numofparam++;
    cmdParam[numofparam] = (void *)&advertisingIntervalMin;
    numofparam++;
    cmdParam[numofparam] = (void *)&advertisingIntervalMax;
    numofparam++;
    cmdParam[numofparam] = (void *)&addressType;
    numofparam++;
    cmdParam[numofparam] = (void *)&advertisingFilterPolicy;
    numofparam++;
    cmdParam[numofparam] = (void *)&localNameLength;
    numofparam++;
    if (localNameLength)
    {
        HCI_MESG_DBG(hostdbgfile,"Local_Name_Length = %d\n",localNameLength);
        cmdParam[numofparam] = (void *)Local_Name;
        numofparam++;
    }
    cmdParam[numofparam] = (void *)&serviceUuidLength;
    numofparam++;
    if (serviceUuidLength)
    {
        HCI_MESG_DBG(hostdbgfile,"Service_UUID_Length = %d\n",serviceUuidLength);
        cmdParam[numofparam] = (void *)Service_Uuid_List;
        numofparam++;
    }
    cmdParam[numofparam] = (void *)&slaveCommIntervalMin;
    numofparam++;
    cmdParam[numofparam] = (void *)&slaveCommIntervalMax;
    numofparam++;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_GAP_SET_LIMITED_DISCOVERABLE, numofparam, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif


/* HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE */
#ifdef HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE
tBleStatus HCI_GAP_Set_Discoverable_Cmd(uint8 profileID,
										  uint8 Advertising_Event_Type,
                                          uint16 Advertising_Interval_Min,
                                          uint16 Advertising_Interval_Max,
                                          uint8 Address_Type,
                                          uint8 Adv_Filter_Policy,
                                          uint8 Local_Name_Length,
                                          uint8 * Local_Name,
                                          uint8 Service_UUID_Length,
                                          uint8 *Service_Uuid_List,
                                          uint16 Slave_Conn_Interval_Min,
                                          uint16 Slave_Conn_Interval_Max)
{
    void * cmdParam[11] = {NULL};

    uint8 numofparam = 0;
    uint8 advertisingEventType = Advertising_Event_Type;
    uint16 advertisingIntervalMin = Advertising_Interval_Min;
    uint16 advertisingIntervalMax = Advertising_Interval_Max;
    uint8 addressType = Address_Type;
    uint8 advertisingFilterPolicy = Adv_Filter_Policy;
    uint8 localNameLength = Local_Name_Length;
/*    uint8 * Local_Name; */
    uint8 serviceUuidLength = Service_UUID_Length;
/*    uint8 *Service_Uuid_List; */
    uint16 slaveCommIntervalMin = Slave_Conn_Interval_Min;
    uint16 slaveCommIntervalMax = Slave_Conn_Interval_Max;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Set_Discoverable_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_Set_Discoverable_Cmd \n" );

	/* check the main profile state. If it is already in advertising
	 * state, then this command should not be issued
	 */
    if((BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE) &&
	    ((BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_ADVERTISING) ||
		(BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_ENABLING_ADVERTISING)))
	{
		/* if the device is already advertising because of
		* another profile then this command should not be
		* issued agin but the profile also should not treat
		* this as failure
		*/
		PROFILE_MESG_DBG(profiledbgfile, "BLE_STATUS_ALREADY_ADVERTISING\n");
		return BLE_STATUS_ALREADY_ADVERTISING;
	}
	
	/* set the state to indicate this command has been issued */
	BLE_Profile_Set_State(BLE_PROFILE_STATE_ENABLING_ADVERTISING);
	
	/* if the command was issued wrongly by some profile in some
	 * other state, then it is not the responsibility of the main
	 * profile
	 */ 
	cmdParam[numofparam] = (void *)&advertisingEventType;
	numofparam++;
	cmdParam[numofparam] = (void *)&advertisingIntervalMin;
	numofparam++;
	cmdParam[numofparam] = (void *)&advertisingIntervalMax;
	numofparam++;
	cmdParam[numofparam] = (void *)&addressType;
	numofparam++;
	cmdParam[numofparam] = (void *)&advertisingFilterPolicy;
	numofparam++;
	cmdParam[numofparam] = (void *)&localNameLength;
	numofparam++;
	if (localNameLength)
	{
		HCI_MESG_DBG(hostdbgfile,"Local_Name_Length = %d\n",localNameLength);
		cmdParam[numofparam] = (void *)Local_Name;
		numofparam++;
	}
	cmdParam[numofparam] = (void *)&serviceUuidLength;
	numofparam++;
	if (serviceUuidLength)
	{
		HCI_MESG_DBG(hostdbgfile,"Service_UUID_Length = %d\n",serviceUuidLength);
		cmdParam[numofparam] = (void *)Service_Uuid_List;
		numofparam++;
	}
	cmdParam[numofparam] = (void *)&slaveCommIntervalMin;
	numofparam++;
	cmdParam[numofparam] = (void *)&slaveCommIntervalMax;
	numofparam++;
	
	return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE, numofparam, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif


/* HCI_VENDOR_CMD_GAP_SET_DIRECT_CONNECTABLE */
#ifdef HCI_VENDOR_CMD_GAP_SET_DIRECT_CONNECTABLE
tBleStatus HCI_GAP_Set_DirectConnectable_Cmd(uint8 profileID, uint8 Own_Address_Type, 
                                              uint8 Initiator_Address_Type, 
                                              uint8 * Initiator_Direct_Address )
{
    void * cmdParam[3] = {NULL};
    uint8 ownAddressType = Own_Address_Type;
    uint8 initiatorAddressType = Initiator_Address_Type;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Set_DirectConnectable_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_Set_DirectConnectable_Cmd \n" );

    if (Initiator_Direct_Address == NULL)
    {
        return (BLE_STATUS_INVALID_PARAMS);
    }
	/* check the main profile state. If it is already in advertising
	 * state, then this command should not be issued
	 */
    if((BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE) &&
	    ((BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_ADVERTISING) ||
		(BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_ENABLING_ADVERTISING)))
	{
		/* if the device is already advertising because of
		* another profile then this command should not be
		* issued agin but the profile also should not treat
		* this as failure
		*/
		PROFILE_MESG_DBG(profiledbgfile, "BLE_STATUS_ALREADY_ADVERTISING\n");
		return BLE_STATUS_ALREADY_ADVERTISING;
	}
	
	/* set the state to indicate this command has been issued */
	BLE_Profile_Set_State(BLE_PROFILE_STATE_ENABLING_ADVERTISING);
	
    cmdParam[0] = (void *)&ownAddressType;
    cmdParam[1] = (void *)&initiatorAddressType;
    cmdParam[2] = (void *)Initiator_Direct_Address;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_GAP_SET_DIRECT_CONNECTABLE, 3, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif


/* HCI_VENDOR_CMD_GAP_SET_SET_IO_CAPABILITY */
#ifdef HCI_VENDOR_CMD_GAP_SET_SET_IO_CAPABILITY
tBleStatus HCI_GAP_Set_IOCapability_Cmd (uint8 profileID, uint8 IO_Capability )
{
    void * cmdParam[10] = {NULL};
    uint8 ioCapability = IO_Capability;
    
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Set_IOCapability_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_Set_IOCapability_Cmd \n" );

    cmdParam[0] = (void *)&ioCapability;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_GAP_SET_SET_IO_CAPABILITY, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_VENDOR_CMD_GAP_SET_AUTHENTICATION_REQ */
#ifdef HCI_VENDOR_CMD_GAP_SET_AUTHENTICATION_REQ
tBleStatus HCI_GAP_Set_Authentication_Req_Cmd (uint8 profileID, uint8  MIMT_Mode,
                                                uint8  OOB_Enable, 
                                                uint8 * OOB_Data, /* uint8 OOB_Data[16] */
                                                uint8  Encryption_Key_Size_Min, 
												uint8  Encryption_Key_Size_Max, 
                                                uint8  Use_Fixed_Pin, 
                                                uint32 Fixed_Pin, 
                                                uint8  Bonding_Mode) 
{
    void * cmdParam[10] = {NULL};

    uint8 mimtMode = MIMT_Mode;
    uint8 oobEnable = OOB_Enable;
/*    uint8 OOB_Data[16] */
    uint8 encryptionKeySizeMin = Encryption_Key_Size_Min;
	uint8 encryptionKeySizeMax = Encryption_Key_Size_Max;
    uint8 useFixedPin = Use_Fixed_Pin;
    uint32 fixedPin = Fixed_Pin;
    uint8 bondingMode = Bonding_Mode;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Set_Authentication_Req_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_Set_Authentication_Req_Cmd \n" );

	HCI_MESG_DBG (hostdbgfile, "Authenq Req params: %02X %02X %02X %02X %02X %08X %02X \n", 
                  mimtMode, oobEnable, encryptionKeySizeMin, encryptionKeySizeMax, 
				  useFixedPin, fixedPin, bondingMode);

    cmdParam[0] = (void *)&mimtMode;
    cmdParam[1] = (void *)&oobEnable;
    cmdParam[2] = (void *)OOB_Data;
    cmdParam[3] = (void *)&encryptionKeySizeMin;
	cmdParam[4] = (void *)&encryptionKeySizeMax;
    cmdParam[5] = (void *)&useFixedPin;
    cmdParam[6] = (void *)&fixedPin;
    cmdParam[7] = (void *)&bondingMode;
    
    return (Profile_HCI_Interface_Send_Command (profileID,HCI_VENDOR_CMD_GAP_SET_AUTHENTICATION_REQ, 8, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_VENDOR_CMD_GAP_SET_AUTHORIZATION_REQ */
#ifdef HCI_VENDOR_CMD_GAP_SET_AUTHORIZATION_REQ
tBleStatus HCI_GAP_Set_Authorization_Req_Cmd(uint8 profileID, uint8 Authorization_Enable )
{
    void * cmdParam[1] = {NULL};
    uint8 authorizationEnable = Authorization_Enable;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Set_Authorization_Req_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_Set_Authorization_Req_Cmd \n" );

    cmdParam[0] = (void *)&authorizationEnable;
    
    HCI_MESG_DBG(hostdbgfile,"HCI_GAP_Set_Authorization_Req_Cmd\n");
    return (Profile_HCI_Interface_Send_Command (profileID,HCI_VENDOR_CMD_GAP_SET_AUTHORIZATION_REQ, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_VENDOR_CMD_GAP_PASS_KEY_RESP */
#ifdef HCI_VENDOR_CMD_GAP_PASS_KEY_RESP
tBleStatus HCI_GAP_PassKey_Response_Cmd(uint8 profileID, uint16 Conn_Handle, uint32 Pass_Key )
{
    void * cmdParam[1] = {NULL};
    uint16 connHandle = Conn_Handle;
    uint32 passKey = Pass_Key;
    
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_PassKey_Response_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_PassKey_Response_Cmd \n" );
	HCI_MESG_DBG(hostdbgfile,"Pass Key: %08X \n", passKey);

    cmdParam[0] = (void *)&connHandle;
    cmdParam[1] = (void *)&passKey;

    return (Profile_HCI_Interface_Send_Command (profileID,HCI_VENDOR_CMD_GAP_PASS_KEY_RESP, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_VENDOR_CMD_GAP_AUTHORIZATION_RESP */
#ifdef HCI_VENDOR_CMD_GAP_AUTHORIZATION_RESP
tBleStatus HCI_GAP_Set_Authorization_Resp_Cmd(uint8 profileID, uint8 Authorize )
{
    void * cmdParam[1] = {NULL};
    uint8 authorize = Authorize;
    
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Set_Authorization_Resp_Cmd \n" );
    HCI_MESG_DBG(hostdbgfile,"HCI_GAP_Set_Authorization_Resp_Cmd \n");

    cmdParam[0] = (void *)&authorize;

    return (Profile_HCI_Interface_Send_Command (profileID,HCI_VENDOR_CMD_GAP_AUTHORIZATION_RESP, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_VENDOR_CMD_GAP_INIT */
#ifdef HCI_VENDOR_CMD_GAP_INIT
tBleStatus HCI_GAP_Init_Cmd(uint8 profileID, uint8 GAP_Role )
{
    void * cmdParam[1] = {NULL};
    uint8 role = GAP_Role;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Init_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_Init_Cmd \n" );

    cmdParam[0] = (void *)&role;
    
    return (Profile_HCI_Interface_Send_Command (profileID,HCI_VENDOR_CMD_GAP_INIT,1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_VENDOR_CMD_GAP_SET_NON_CONNECTABLE */
#ifdef HCI_VENDOR_CMD_GAP_SET_NON_CONNECTABLE
tBleStatus HCI_GAP_Set_NonConnectable_Cmd(uint8 profileID, uint8 Advertising_Event_Type )
{
    void * cmdParam[1] = {NULL};
    uint8 advertisingEventType = Advertising_Event_Type;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Set_NonConnectable_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_Set_NonConnectable_Cmd \n" );

    cmdParam[0] = (void *)&advertisingEventType;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_GAP_SET_NON_CONNECTABLE, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif


/* HCI_VENDOR_CMD_GAP_SET_UNDIRECTED_CONNECTABLE */
#ifdef HCI_VENDOR_CMD_GAP_SET_UNDIRECTED_CONNECTABLE
tBleStatus HCI_GAP_Set_Undirected_Connectable_Cmd(uint8 profileID,
													 uint8 Adv_Filter_Policy, 
                                                     uint8 Own_Address_Type )
{
    void * cmdParam[2] = {NULL};
    uint8 advtFilterPolicy = Adv_Filter_Policy;
    uint8 ownAddressType = Own_Address_Type;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Set_Undirected_Connectable_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_Set_Undirected_Connectable_Cmd \n" );

    /* check the main profile state. If it is already in advertising
	 * state, then this command should not be issued
	 */
    if((BLE_Profile_Get_Main_State() == BLE_PROFILE_STATE_CONNECTABLE) &&
	    ((BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_ADVERTISING) || 
		(BLE_Profile_Get_Sub_State() == BLE_PROFILE_STATE_ENABLING_ADVERTISING)))
	{
		/* if the device is already advertising because of
		* another profile then this command should not be
		* issued agin but the profile also should not treat
		* this as failure
		*/
		PROFILE_MESG_DBG(profiledbgfile, "BLE_STATUS_ALREADY_ADVERTISING\n");
		return BLE_STATUS_ALREADY_ADVERTISING;
	}
	
	/* set the state to indicate this command has been issued */
	BLE_Profile_Set_State(BLE_PROFILE_STATE_ENABLING_ADVERTISING);
	
    cmdParam[0] = (void *)&advtFilterPolicy;
    cmdParam[1] = (void *)&ownAddressType;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_GAP_SET_UNDIRECTED_CONNECTABLE, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif


/* HCI_VENDOR_CMD_GAP_SLAVE_SECURITY_REQUEST */
#ifdef HCI_VENDOR_CMD_GAP_SLAVE_SECURITY_REQUEST
tBleStatus HCI_GAP_Slave_Security_Req_Cmd(uint8 profileID,
											uint8 Bonding_Mode, 
                                            uint8 MIMT_Mode )
{
    void * cmdParam[2] = {NULL};
    uint8 bondingMode = Bonding_Mode;
    uint8 mimtMode = MIMT_Mode;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Slave_Security_Req_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_Slave_Security_Req_Cmd \n" );
	HCI_MESG_DBG(hostdbgfile,"Slave Security: %02x %02X \n", bondingMode, mimtMode);

    cmdParam[0] = (void *)&bondingMode;
    cmdParam[1] = (void *)&mimtMode;
    
    return (Profile_HCI_Interface_Send_Command (profileID,HCI_VENDOR_CMD_GAP_SLAVE_SECURITY_REQUEST, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_VENDOR_CMD_GAP_UPDATE_ADV_DATA */
#ifdef HCI_VENDOR_CMD_GAP_UPDATE_ADV_DATA
tBleStatus HCI_GAP_Update_Advertising_Data(uint8 profileID,
											  uint8 Advertising_Data_Length, 
                                              uint8 * Advertising_Data )
{
    void * cmdParam[2] = {NULL};
    uint8 advertisingDataLength = Advertising_Data_Length;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Update_Advertising_Data \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_Update_Advertising_Data \n" );

    if (Advertising_Data == NULL)
    {
        return (HCI_RESULT_FAILURE);
    }

    cmdParam[0] = (void *)&advertisingDataLength;
    cmdParam[1] = (void *)Advertising_Data;
    
    return (Profile_HCI_Interface_Send_Command (profileID,HCI_VENDOR_CMD_GAP_UPDATE_ADV_DATA, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_VENDOR_CMD_GAP_DELETE_AD_TYPE */
#ifdef HCI_VENDOR_CMD_GAP_DELETE_AD_TYPE
tBleStatus HCI_GAP_Delete_AD_Type(uint8 profileID,uint8 AD_Type)
{
    void * cmdParam[1] = {NULL};
    uint8 adType = AD_Type;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Delete_AD_Type \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_Delete_AD_Type \n" );

    cmdParam[0] = (void *)&adType;
    
    return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_GAP_DELETE_AD_TYPE, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_VENDOR_CMD_GAP_GET_SEC_LEVEL */
#ifdef HCI_VENDOR_CMD_GAP_GET_SEC_LEVEL
tBleStatus HCI_GAP_Get_Security_Level(uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Get_Security_Level \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_Get_Security_Level \n" );

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_GAP_GET_SEC_LEVEL, 0, NULL));
}
#endif


/* HCI_VENDOR_CMD_GAP_SET_EVT_MASK */
#ifdef HCI_VENDOR_CMD_GAP_SET_EVT_MASK
tBleStatus HCI_GAP_Set_Event_Mask_Cmd(uint8 profileID, uint16 Event_Mask )
{
    void * cmdParam[1] = {NULL};
    uint16 eventMask = Event_Mask;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Set_Event_Mask_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_Set_Event_Mask_Cmd \n" );
	HCI_MESG_DBG (hostdbgfile, "GAP Event Mask Param: %08X \n", eventMask);

    cmdParam[0] = (void *)&eventMask;
    
    return (Profile_HCI_Interface_Send_Command (profileID,HCI_VENDOR_CMD_GAP_SET_EVT_MASK, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_VENDOR_CMD_GAP_CONFIGURE_WHITELIST */
#ifdef HCI_VENDOR_CMD_GAP_CONFIGURE_WHITELIST
tBleStatus HCI_GAP_Configure_WhiteList_Cmd (uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GAP_Configure_WhiteList_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GAP_Configure_WhiteList_Cmd \n" );

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_GAP_CONFIGURE_WHITELIST, 0, NULL ));
}
#endif


/* Group: GATT vendor Specific commands */
/* HCI_VENDOR_CMD_GATT_SERVER_INIT */
#ifdef HCI_VENDOR_CMD_GATT_SERVER_INIT
tBleStatus HCI_GATT_Init_Cmd (uint8 profileID)
{
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Init_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Init_Cmd \n" );

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_GATT_SERVER_INIT, 0, NULL ));
}
#endif


/* HCI_VENDOR_CMD_GATT_ADD_SERVICE */
#ifdef HCI_VENDOR_CMD_GATT_ADD_SERVICE
tBleStatus HCI_GATT_Add_Service_Cmd(uint8 profileID, 
									  uint8 UUID_Type, 
                                      uint8 * UUID, 
                                      uint8 Service_Type, 
                                      uint8 Max_Attribute_Records )
{
    void * cmdParam[4] = {NULL};

    uint8 uuidType = UUID_Type;
    uint8 serviceType = Service_Type;
    uint8 maxAttributeRecords = Max_Attribute_Records;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Add_Service_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Add_Service_Cmd \n" );
	HCI_MESG_DBG (hostdbgfile, " Add Service params: %02X %04X %02X %02X \n", 
	              uuidType, (uint16)(*(uint16 *)UUID), serviceType , maxAttributeRecords );

    cmdParam[0] = (void *)&uuidType;
    cmdParam[1] = (void *)UUID;
    cmdParam[2] = (void *)&serviceType;
    cmdParam[3] = (void *)&maxAttributeRecords;
    
    return (Profile_HCI_Interface_Send_Command (profileID,HCI_VENDOR_CMD_GATT_ADD_SERVICE, 4, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_VENDOR_CMD_GATT_INC_SERVICE */
#ifdef HCI_VENDOR_CMD_GATT_INC_SERVICE
tBleStatus HCI_GATT_Include_Service_Cmd(uint8 profileID,
                                         uint16 Service_Handle, 
                                         uint16 Include_Start_Handle, 
                                         uint16 Include_End_Handle, 
                                         uint8 Included_UUID_Type, /* 2 bytes or 16 bytes */ 
                                         uint8 * Included_UUID )
{
    void * cmdParam[5] = {NULL};

    uint16 serviceHandle = Service_Handle;
    uint16 includeStartHandle = Include_Start_Handle;
    uint16 includeEndHandle = Include_End_Handle;
    uint8 includeUUIDType = Included_UUID_Type;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Include_Service_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Include_Service_Cmd \n" );

    if (Included_UUID == NULL)
    {
        return BLE_STATUS_NULL_PARAM;
    }

    cmdParam[0] = (void *)&serviceHandle;
    cmdParam[1] = (void *)&includeStartHandle;
    cmdParam[2] = (void *)&includeEndHandle;
    cmdParam[3] = (void *)&includeUUIDType;
    cmdParam[4] = (void *)Included_UUID;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_GATT_INC_SERVICE, 5, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_VENDOR_CMD_GATT_ADD_CHAR */
#ifdef HCI_VENDOR_CMD_GATT_ADD_CHAR
tBleStatus HCI_GATT_Add_Characterisitic_Cmd ( uint8 profileID,
												 uint16 Service_Handle, 
                                                 uint8 Char_UUID_Type, /* 2 bytes or 16 bytes */
                                                 uint8 * Char_UUID, 
                                                 uint8 Char_Value_Length, 
                                                 uint8 Char_Properties, 
                                                 uint8 Security_Permissions, 
                                                 uint8 Gatt_Event_Mask,
                                                 uint8 Encryption_Key_Size,
                                                 uint8 Is_Variable )
{
	void * cmdParam[9] = {NULL};

    uint16 serviceHandle = Service_Handle;
    uint8 charUuidType = Char_UUID_Type;
    uint8 charValueLen = Char_Value_Length;
    uint8 charProperties = Char_Properties;
    uint8 securityPermission = Security_Permissions;
    uint8 gattEventMask = Gatt_Event_Mask;
    uint8 encryptionKeySize = Encryption_Key_Size;
    uint8 isVariable = Is_Variable;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Add_Characterisitic_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Add_Characterisitic_Cmd \n" );
	HCI_MESG_DBG (hostdbgfile, " Add Char params: %04X %02X %04X %02X %02X %02X %02X \n", serviceHandle, 
	charUuidType, (uint16)(*(uint16 *)Char_UUID), charValueLen, charProperties, securityPermission, gattEventMask);

    if (Char_UUID == NULL)
    {
        return (BLE_STATUS_NULL_PARAM);
    }

    cmdParam[0] = (void *)&serviceHandle;
    cmdParam[1] = (void *)&charUuidType;
    cmdParam[2] = (void *)Char_UUID;
    cmdParam[3] = (void *)&charValueLen;
    cmdParam[4] = (void *)&charProperties;
    cmdParam[5] = (void *)&securityPermission;
    cmdParam[6] = (void *)&gattEventMask;
    cmdParam[7] = (void *)&encryptionKeySize;
    cmdParam[8] = (void *)&isVariable;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_GATT_ADD_CHAR, 9, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_VENDOR_CMD_GATT_ADD_CHAR_DESC */
#ifdef HCI_VENDOR_CMD_GATT_ADD_CHAR_DESC
tBleStatus HCI_GATT_Add_Char_Descriptor_Cmd ( uint8 profileID,
												 uint16 Serv_Handle, 
                                                 uint16 Char_Handle, 
                                                 uint8 Char_Desc_UUID_Type,  /* 2 bytes or 16 bytes */
                                                 uint8 * Char_Desc_UUID, 
                                                 uint8 Desc_Value_Max_Length, 
                                                 uint8 Char_Desc_Value_Length, /* can it be zero also ?? */
                                                 uint8 * Char_Desc_Value, /* in case of 0 length, it will be NULL */
                                                 uint8 Security_Permissions, 
                                                 uint8 Access_Permissions, 
                                                 uint8 Gatt_Event_Mask,
                                                 uint8 Encryption_Key_Size, 
                                                 uint8 Is_Variable )
{
    void * cmdParam[15] = {NULL};
    uint16 serviceHandle = Serv_Handle;
    uint16 charHandle = Char_Handle;
    uint8  charDescUuidType = Char_Desc_UUID_Type;
    uint8  descValueMaxLen = Desc_Value_Max_Length;
    uint8  charDescValueLen = Char_Desc_Value_Length;
    uint8  securityPermissions = Security_Permissions;
    uint8  accessPermissions = Access_Permissions;
    uint8  gattEventMask = Gatt_Event_Mask;
    uint8 encryptionKeySize = Encryption_Key_Size;
    uint8 isVariable = Is_Variable;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Add_Char_Descriptor_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Add_Char_Descriptor_Cmd \n" );

    if ((Char_Desc_UUID == NULL) || (Char_Desc_Value == NULL))
    {
        return (BLE_STATUS_NULL_PARAM);
    }

    cmdParam[0] = (void *)&serviceHandle;
    cmdParam[1] = (void *)&charHandle;
    cmdParam[2] = (void *)&charDescUuidType;
    cmdParam[3] = (void *)Char_Desc_UUID;
    cmdParam[4] = (void *)&descValueMaxLen;
    cmdParam[5] = (void *)&charDescValueLen;
    cmdParam[6] = (void *)Char_Desc_Value;
    cmdParam[7] = (void *)&securityPermissions;
    cmdParam[8] = (void *)&accessPermissions;
    cmdParam[9] = (void *)&gattEventMask;
    cmdParam[10] = (void *)&encryptionKeySize;
    cmdParam[11] = (void *)&isVariable;

    return ( Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_GATT_ADD_CHAR_DESC, 8, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL */
#ifdef HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL
tBleStatus HCI_GATT_Update_CharVal_Cmd ( uint8 profileID,
											uint16 Serv_Handle, 
                                            uint16 Char_Handle, 
                                            uint8 Char_Value_Offset,
                                            uint8 Char_Value_Length, /* can it be 0 also ?*/
                                            uint8 * Char_Value )
{
    void * cmdParam[5] = {NULL};

    uint16 serviceHandle = Serv_Handle;
    uint16 charHandle = Char_Handle;
    uint8 charValueOffset = Char_Value_Offset;
    uint8 charValueLen = Char_Value_Length;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Update_CharVal_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Update_CharVal_Cmd \n" );

    if (Char_Value == NULL)
    {
        return (BLE_STATUS_NULL_PARAM);
    }

    cmdParam[0] = (void *)&serviceHandle;
    cmdParam[1] = (void *)&charHandle;
    cmdParam[2] = (void *)&charValueOffset;
    cmdParam[3] = (void *)&charValueLen;
    cmdParam[4] = (void *)Char_Value;

    return ( Profile_HCI_Interface_Send_Command(profileID, HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL, 5, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif
    

/* HCI_VENDOR_CMD_GATT_DEL_CHAR */
#ifdef HCI_VENDOR_CMD_GATT_DEL_CHAR
tBleStatus HCI_GATT_Delete_Characteristic_Cmd(uint8 profileID, 
												 uint16 Service_Handle, 
                                                 uint16 Char_Handle )
{
    void * cmdParam[2] = {NULL};
    uint16 serviceHandle = Service_Handle;
    uint16 charHandle = Char_Handle;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Delete_Characteristic_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Delete_Characteristic_Cmd \n" );

    cmdParam[0] = (void *)&serviceHandle;
    cmdParam[1] = (void *)&charHandle;

    return (Profile_HCI_Interface_Send_Command(profileID, HCI_VENDOR_CMD_GATT_DEL_CHAR, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif


/* HCI_VENDOR_CMD_GATT_DEL_SERVICE */
#ifdef HCI_VENDOR_CMD_GATT_DEL_SERVICE
tBleStatus HCI_GATT_Delete_Service_Cmd(uint8 profileID,uint16 Service_Handle)
{
    void * cmdParam[10] = {NULL};
    uint16 serviceHandle = Service_Handle;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Delete_Service_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Delete_Service_Cmd \n" );

    cmdParam[0] = (void *)&serviceHandle;

    return (Profile_HCI_Interface_Send_Command(profileID, HCI_VENDOR_CMD_GATT_DEL_SERVICE, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif


/* HCI_VENDOR_CMD_GATT_DEL_INC_SERVICE */
#ifdef HCI_VENDOR_CMD_GATT_DEL_INC_SERVICE
tBleStatus HCI_GATT_Delete_Include_Service_Cmd(uint8 profileID,
											     uint16 Service_Handle, 
                                                 uint16 Include_Handle )
{
    void * cmdParam[2] = {NULL};
    uint16 serviceHandle = Service_Handle;
    uint16 includeHandle = Include_Handle;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Delete_Include_Service_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Delete_Include_Service_Cmd \n" );

    cmdParam[0] = (void *)&serviceHandle;
    cmdParam[1] = (void *)&includeHandle;

    return (Profile_HCI_Interface_Send_Command(profileID, HCI_VENDOR_CMD_GATT_DEL_INC_SERVICE, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif



/* HCI_VENDOR_CMD_GATT_SET_EVT_MASK */
#ifdef HCI_VENDOR_CMD_GATT_SET_EVT_MASK
tBleStatus HCI_GATT_Set_Event_Mask_Cmd(uint8 profileID,uint32 Event_Mask)
{
    void * cmdParam[1] = {NULL};
    uint32 eventMask = Event_Mask;
    
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Set_Event_Mask_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Set_Event_Mask_Cmd \n" );
	HCI_MESG_DBG (hostdbgfile, "GATT Event Mask Param: %08X \n", eventMask);

    cmdParam[0] = (void *)&eventMask;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_GATT_SET_EVT_MASK, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif


/* HCI_VENDOR_CMD_GATT_EXCHG_CONFIG    */
#ifdef HCI_VENDOR_CMD_GATT_EXCHG_CONFIG
tBleStatus HCI_GATT_Exchange_Configuration_Cmd( uint8 profileID , uint16 conn_handle)
{
	void * cmdParam[1] = {NULL};
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Exchange_Configuration_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Exchange_Configuration_Cmd \n" );
    cmdParam[0] = (void *)&conn_handle;
    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_EXCHG_CONFIG, 1,  (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam) );
}
#endif


/* HCI_VENDOR_CMD_GATT_FIND_INFO */
#ifdef HCI_VENDOR_CMD_GATT_FIND_INFO
tBleStatus HCI_GATT_Find_Information_Cmd ( uint8 profileID,
											uint16 Conn_Handle,
											  uint16 Start_Handle,
                                              uint16 End_Handle )
{
    void * cmdParam[3] = {NULL};
    uint16 startHandle = Start_Handle;
    uint16 endHandle = End_Handle;
	uint16 connHandle = Conn_Handle;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Find_Information_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Find_Information_Cmd \n" );

	cmdParam[0] = (void *)&connHandle;
    cmdParam[1] = (void *)&startHandle;
    cmdParam[2] = (void *)&endHandle;
	
    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_FIND_INFO, 3, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif


/* HCI_VENDOR_CMD_GATT_FIND_ATTR_BY_TYPE_VAL */
#ifdef HCI_VENDOR_CMD_GATT_FIND_ATTR_BY_TYPE_VAL
tBleStatus HCI_GATT_Find_Attr_By_Type_Value_Cmd ( uint8 profileID,
													uint16 Conn_Handle,
													 uint16 Start_Handle,
                                                     uint16 End_Handle, 
                                                     uint16 UUID, 
                                                     uint8 Attribute_Value_Length, 
                                                     uint8 * Attribute_Value )
{
    void * cmdParam[11] = {NULL};

    uint16 startHandle = Start_Handle;
    uint16 endHandle = End_Handle;
    uint16 uuid = UUID;
    uint8 attributeValueLen = Attribute_Value_Length;
	uint16 connHandle = Conn_Handle;

    if (Attribute_Value == NULL)
    {
        return (BLE_STATUS_NULL_PARAM);
    }

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Find_Attr_By_Type_Value_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Find_Attr_By_Type_Value_Cmd \n" );

    cmdParam[0] = (void *)&connHandle;
	cmdParam[1] = (void *)&startHandle;
    cmdParam[2] = (void *)&endHandle;
    cmdParam[3] = (void *)&uuid;
    cmdParam[4] = (void *)&attributeValueLen;
    cmdParam[5] = (void *)Attribute_Value;
	

    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_FIND_ATTR_BY_TYPE_VAL, 6, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif

#ifdef HCI_VENDOR_CMD_GATT_READ_ATTR_BY_TYPE
tBleStatus HCI_GATT_Read_Attribute_By_Type_Cmd ( uint8 profileID,
													uint16 Conn_Handle,
												    uint16 Start_Handle,
                                                    uint16 End_Handle, 
                                                    uint8 UUID_Type, 
                                                    uint8 * UUID )
{
    void * cmdParam[11] = {NULL};

    uint16 startHandle = Start_Handle;
    uint16 endHandle = End_Handle;
    uint8 uuidType = UUID_Type;
	uint16 connHandle = Conn_Handle;

    if (UUID == NULL)
    {
        return (BLE_STATUS_NULL_PARAM);
    }

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Read_Attribute_By_Type_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Read_Attribute_By_Type_Cmd \n" );

    cmdParam[0] = (void *)&connHandle;
	cmdParam[1] = (void *)&startHandle;
    cmdParam[2] = (void *)&endHandle;
    cmdParam[3] = (void *)&uuidType;
    cmdParam[4] = (void *)UUID;
	

    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_READ_ATTR_BY_TYPE, 5, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif

#ifdef HCI_VENDOR_CMD_GATT_READ_ATTR_BY_GROUP_TYPE
tBleStatus HCI_GATT_Read_Attribute_By_Group_Cmd ( uint8 profileID,
													uint16 Conn_Handle,
													 uint16 Start_Handle,
                                                     uint16 End_Handle, 
                                                     uint8 UUID_Type, 
                                                     uint8 * UUID )
{
    void * cmdParam[11] = {NULL};

    uint16 startHandle = Start_Handle;
    uint16 endHandle = End_Handle;
    uint8 uuidType = UUID_Type;
	uint16 connHandle = Conn_Handle;

    if (UUID == NULL)
    {
        return (BLE_STATUS_NULL_PARAM);
    }

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Read_Attribute_By_Group_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Read_Attribute_By_Group_Cmd \n" );

    cmdParam[0] = (void *)&connHandle;
	cmdParam[1] = (void *)&startHandle;
    cmdParam[2] = (void *)&endHandle;
    cmdParam[3] = (void *)&uuidType;
    cmdParam[4] = (void *)UUID;
	

    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_READ_ATTR_BY_GROUP_TYPE, 5, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif

#ifdef HCI_VENDOR_CMD_GATT_PREPARE_WRITE
tBleStatus HCI_GATT_Prepare_Write_Cmd ( uint8 profileID,
											uint16 Conn_Handle,
										   uint16 Attr_Handle, 
                                           uint16 Value_Offset, 
                                           uint8 Attr_Value_Len, 
                                           uint8 * Attr_Value )
{
    void * cmdParam[11] = {NULL};

    uint16 attrHandle = Attr_Handle;
    uint16 valueOffset = Value_Offset;
    uint8 attValueLen = Attr_Value_Len;
	uint16 connHandle = Conn_Handle

    if ( (Attr_Value_Len == 0) || (Attr_Value == NULL) )
    {
        return (BLE_STATUS_NULL_PARAM);
    }

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Prepare_Write_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Prepare_Write_Cmd \n" );

    cmdParam[0] = (void *)connHandle;
	cmdParam[1] = (void *)&attrHandle;
    cmdParam[2] = (void *)&valueOffset;
    cmdParam[3] = (void *)&attValueLen;
    cmdParam[4] = (void *)Attr_Value;
	

    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_PREPARE_WRITE, 5, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif

#ifdef HCI_VENDOR_CMD_GATT_EXECUTE_WRITES
tBleStatus HCI_GATT_Execute_Write_Cmd(uint8 profileID,uint16 Conn_Handle,uint8 Execute)
{
    void * cmdParam[10] = {NULL};

    uint8 execute = Execute;
	uint16 connHandle = Conn_Handle;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Execute_Write_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Execute_Write_Cmd \n" );

    
	cmdParam[0] = (void *)&connHandle;
	cmdParam[1] = (void *)&execute;

    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_EXECUTE_WRITES, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif

#ifdef HCI_VENDOR_CMD_GATT_CONFORM_INDICATION
tBleStatus HCI_GATT_Conform_Indication_Cmd( uint8 profileID, uint16 Conn_Handle)
{
	
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Conform_Indication_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Conform_Indication_Cmd \n" );
	void * cmdParam[10] = {NULL};
	uint16 connHandle = Conn_Handle;
	cmdParam[0] = (void *)&connHandle;
    return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_GATT_CONFORM_INDICATION, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam) ));
}
#endif

#ifdef HCI_VENDOR_CMD_GATT_WRITE_RESP
tBleStatus HCI_GATT_Write_Response_Cmd ( uint8 profileID,
										 uint16 Conn_Handle, 
											uint16 Att_Handle, 
                                            uint8 Status, 
                                            uint8 Error_Code, 
                                            uint8 Att_Value_Length, 
                                            uint8 * Att_Value )
{
    void * cmdParam[6] = {NULL};

    uint16 attHandle = Att_Handle;
    uint8  status = Status;
    uint8  errorCode = Error_Code;
    uint8  attValue = Att_Value_Length;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Write_Response_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Write_Response_Cmd \n" );

    cmdParam[0] = (void *)&Conn_Handle;
    cmdParam[1] = (void *)&attHandle;
    cmdParam[2] = (void *)&status;
    cmdParam[3] = (void *)&errorCode;
    cmdParam[4] = (void *)&attValue;
    cmdParam[5] = (void *)Att_Value;

    return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_GATT_WRITE_RESP, 6, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif

#ifdef HCI_VENDOR_CMD_GATT_ALLOW_READ
tBleStatus HCI_GATT_Allow_Read_Cmd ( uint8 profileID, uint16 Conn_Handle)
{
	void * cmdParam[1] = {NULL};
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Allow_Read_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Allow_Read_Cmd \n" );
    cmdParam[0] = (void *)&Conn_Handle;
    return (Profile_HCI_Interface_Send_Command ( profileID,HCI_VENDOR_CMD_GATT_ALLOW_READ, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif

/* HCI_VENDOR_CMD_GATT_DISC_ALL_PRIMARY_SERVICES */
#ifdef HCI_VENDOR_CMD_GATT_DISC_ALL_PRIMARY_SERVICES
tBleStatus HCI_GATT_Discover_All_Primary_Service_Cmd ( uint8 profileID ,uint16 connHandle)
{
	void * cmdParam[1] = {NULL};
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Discover_All_Primary_Service_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Discover_All_Primary_Service_Cmd \n" );
    cmdParam[0] = (void *)&connHandle;
    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_DISC_ALL_PRIMARY_SERVICES, 1, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif

/* HCI_VENDOR_CMD_GATT_DISC_PRIMARY_SERVICE_BY_UUID */
#ifdef HCI_VENDOR_CMD_GATT_DISC_PRIMARY_SERVICE_BY_UUID
tBleStatus HCI_GATT_Discover_Primary_Service_By_UUID_Cmd ( uint8 profileID,
															uint16 Conn_Handle,
															  uint8 UUID_Type, 
                                                              uint8 * UUID )
{
    void * cmdParam[3] = {NULL};
    uint8 uuidType = UUID_Type;
    
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Discover_Primary_Service_By_UUID_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Discover_Primary_Service_By_UUID_Cmd \n" );
	HCI_MESG_DBG (hostdbgfile, " Discover Service params: %02X %04X \n", 
	              uuidType, (uint16)(*(uint16 *)UUID) );

    cmdParam[0] = (void *)&Conn_Handle;
	cmdParam[1] = (void *)&uuidType;
    cmdParam[2] = (void *)UUID;
	

    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_DISC_PRIMARY_SERVICE_BY_UUID, 3, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif

/* HCI_VENDOR_CMD_GATT_FIND_INCLUDED_SERVICES */
#ifdef HCI_VENDOR_CMD_GATT_FIND_INCLUDED_SERVICES
tBleStatus HCI_GATT_Find_Included_Service_Cmd ( uint8 profileID,
													uint16 Conn_Handle,
												   uint16 Start_Handle, 
                                                   uint16 End_Handle )
{
    void * cmdParam[3] = {NULL};
    uint16 startHandle = Start_Handle;
    uint16 endHandle = End_Handle;
	uint16 connHandle = Conn_Handle;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Find_Included_Service_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Find_Included_Service_Cmd \n" );
	HCI_MESG_DBG ( hostdbgfile, " Discover all chars of a service params: %04X %04X \n", 
	              Start_Handle, End_Handle );

    cmdParam[0] = (void *)&connHandle;
	cmdParam[1] = (void *)&startHandle;
    cmdParam[2] = (void *)&endHandle;
	

    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_FIND_INCLUDED_SERVICES, 3, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif

/* HCI_VENDOR_CMD_GATT_DISC_ALL_CHARAC_OF_A_SERVICE */
#ifdef HCI_VENDOR_CMD_GATT_DISC_ALL_CHARAC_OF_A_SERVICE
tBleStatus HCI_GATT_Discover_All_Chars_Of_A_Service_Cmd ( uint8 profileID,
														  uint16 Conn_Handle, 
															 uint16 Start_Handle, 
                                                             uint16 End_Handle )
{
    void * cmdParam[3] = {NULL};
    uint16 startHandle = Start_Handle;
    uint16 endHandle = End_Handle;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Discover_All_Chars_Of_A_Service_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Discover_All_Chars_Of_A_Service_Cmd \n" );
	HCI_MESG_DBG ( hostdbgfile, " Discover all chars of a service params: %04X %04X \n", 
	              Start_Handle, End_Handle );
    cmdParam[0] = (void *)&Conn_Handle;
    cmdParam[1] = (void *)&startHandle;
    cmdParam[2] = (void *)&endHandle;

    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_DISC_ALL_CHARAC_OF_A_SERVICE, 3, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));

}
#endif

/* HCI_VENDOR_CMD_GATT_DISC_CHARAC_BY_UUID */
#ifdef HCI_VENDOR_CMD_GATT_DISC_CHARAC_BY_UUID
tBleStatus HCI_GATT_Discover_Char_By_UUID_Cmd ( uint8 profileID,
												  uint16 Conn_Handle,
												   uint16 Start_Handle, 
                                                   uint16 End_Handle, 
                                                   uint8 UUID_Type, 
                                                   uint8 * UUID )
{
    void * cmdParam[10] = {NULL};

    uint16 startHandle = Start_Handle;
    uint16 endHandle = End_Handle;
    uint8 uuidType = UUID_Type;
	uint16 connHandle = Conn_Handle;

    if (UUID == NULL)
    {
        return (BLE_STATUS_NULL_PARAM);
    }

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Discover_Char_By_UUID_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Discover_Char_By_UUID_Cmd \n" );

    cmdParam[0] = (void *)&connHandle;
	cmdParam[1] = (void *)&startHandle;
    cmdParam[2] = (void *)&endHandle;
    cmdParam[3] = (void *)&uuidType;
    cmdParam[4] = (void *)UUID;
	
	

    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_DISC_CHARAC_BY_UUID, 5, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif

/* HCI_VENDOR_CMD_GATT_DISC_ALL_CHAR_DESCRIPTORS */
#ifdef HCI_VENDOR_CMD_GATT_DISC_ALL_CHAR_DESCRIPTORS
tBleStatus HCI_GATT_Discover_All_Char_Descriptor_Cmd ( uint8 profileID,
                                                       uint16 Conn_Handle, 
														  uint16 Start_Handle, 
                                                          uint16 End_Handle )
{
    void * cmdParam[3] = {NULL};
    uint16 startHandle = Start_Handle;
    uint16 endHandle = End_Handle;
    
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Discover_All_Char_Descriptor_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Discover_All_Char_Descriptor_Cmd \n" );
	HCI_MESG_DBG ( hostdbgfile, "Discover all char descriptor params: %04X %04X \n", 
                                 Start_Handle, End_Handle );
    cmdParam[0] = (void *)&Conn_Handle;
    cmdParam[1] = (void *)&startHandle;
    cmdParam[2] = (void *)&endHandle;

    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_DISC_ALL_CHAR_DESCRIPTORS, 3, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));

}
#endif

/* HCI_VENDOR_CMD_GATT_READ_CHARACTERISTIC_VAL */
#ifdef HCI_VENDOR_CMD_GATT_READ_CHARACTERISTIC_VAL
tBleStatus HCI_GATT_Read_Char_Value_Cmd ( uint8 profileID,uint16 Conn_Handle,uint16 Attr_Handle )
{
    void * cmdParam[2] = {NULL};
    uint16 attrHandle = Attr_Handle;
    
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Read_Char_Value_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Read_Char_Value_Cmd \n" );
	HCI_MESG_DBG ( hostdbgfile, "Read Char Value params: %04X \n", Attr_Handle );

    cmdParam[0] = (void *)&Conn_Handle;
    cmdParam[1] = (void *)&attrHandle;
    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_READ_CHARACTERISTIC_VAL, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif

#ifdef HCI_VENDOR_CMD_GATT_READ_CHARAC_USING_UUID
    #error "This is a new command added !! "
#endif

#ifdef HCI_VENDOR_CMD_GATT_READ_LONG_CHARAC_VAL
    #error "This is a new command added !! "
#endif

#ifdef HCI_VENDOR_CMD_GATT_READ_MULTIPLE_CHARAC_VALUES
    #error "This is a new command added !! "
#endif

/* HCI_VENDOR_CMD_GATT_WRITE_CHARAC_VAL */
#ifdef HCI_VENDOR_CMD_GATT_WRITE_CHARAC_VAL
tBleStatus HCI_GATT_Write_Char_Value_Cmd ( uint8 profileID,
                                           uint16 Conn_Handle,
											  uint16 Attribute_Handle, 
                                              uint8 Attr_Value_Len, 
                                              uint8 * Attr_Value )
{
    void * cmdParam[4] = {NULL};
    uint16 attributeHandle = Attribute_Handle;
    uint8 attrValueLen = Attr_Value_Len;
    
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Write_Char_Value_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Write_Char_Value_Cmd \n" );
	HCI_MESG_DBG ( hostdbgfile, " Write Char Value params: %04X %02X \n", 
                                  Attribute_Handle, Attr_Value_Len );
    cmdParam[0] = (void *)&Conn_Handle;
    cmdParam[1] = (void *)&attributeHandle;
    cmdParam[2] = (void *)&attrValueLen;
    cmdParam[3] = (void *)Attr_Value;

    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_WRITE_CHARAC_VAL, 4, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif


#ifdef HCI_VENDOR_CMD_GATT_WRITE_LONG_CHARAC_VAL
    #error "This is a new command added !! "
#endif

#ifdef HCI_VENDOR_CMD_GATT_WRITE_CHARAC_RELIABLE
    #error "This is a new command added !! "
#endif

#ifdef HCI_VENDOR_CMD_GATT_READ_CHARAC_DESCRIPTOR
tBleStatus HCI_GATT_Read_Char_Descriptor_Cmd ( uint8 profileID,uint16 Conn_Handle,uint16 Attribute_Handle )
{
    void * cmdParam[2] = {NULL};
    uint16 attributeHandle = Attribute_Handle;
	uint16 connHandle = Conn_Handle;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Read_Char_Descriptor_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Read_Char_Descriptor_Cmd \n" );
	HCI_MESG_DBG ( hostdbgfile, " Read Char Descriptor params: %04X \n", Attribute_Handle );

    
	cmdParam[0] = (void *)&connHandle;
	cmdParam[1] = (void *)&attributeHandle;

    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_READ_CHARAC_DESCRIPTOR, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif

#ifdef HCI_VENDOR_CMD_GATT_READ_LONG_CHARAC_DESCRIPTOR
    #error "This is a new command added !! "
#endif

#ifdef HCI_VENDOR_CMD_GATT_WRITE_CHARAC_DESCRIPTOR
tBleStatus HCI_GATT_Write_Char_Descriptor_Cmd ( uint8 profileID,
                                                uint16 Conn_Handle,
												   uint16 Attribute_Handle, 
                                                   uint8 Attr_Value_Len, 
                                                   uint8 * Attr_Value  )
{
    void * cmdParam[4] = {NULL};
    uint16 attributeHandle = Attribute_Handle;
    uint8 attrValueLen = Attr_Value_Len;
    
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Write_Char_Descriptor_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Write_Char_Descriptor_Cmd \n" );
	HCI_MESG_DBG (hostdbgfile, " Write Char Descriptor params: %04X %02X \n", 
                                  Attribute_Handle, Attr_Value_Len );
    cmdParam[0] = (void *)&Conn_Handle;
    cmdParam[1] = (void *)&attributeHandle;
    cmdParam[2] = (void *)&attrValueLen;
    cmdParam[3] = (void *)Attr_Value;

    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_WRITE_CHARAC_DESCRIPTOR, 4, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif

#ifdef HCI_VENDOR_CMD_GATT_WRITE_LONG_CHARAC_DESCRIPTOR
    #error "This is a new command added !! "
#endif

/* HCI_VENDOR_CMD_GATT_WRITE_WITHOUT_RESPONSE */
#ifdef HCI_VENDOR_CMD_GATT_WRITE_WITHOUT_RESPONSE
tBleStatus HCI_GATT_Write_Without_Response_Cmd ( uint8 profileID,
                                                 uint16 Conn_Handle,
													uint16 Attribute_Handle, 
                                                    uint8 Attr_Value_Len, 
                                                    uint8 * Attr_Value )
{
    void * cmdParam[4] = {NULL};
    uint16 attributeHandle = Attribute_Handle;
    uint8 attrValueLen = Attr_Value_Len;
    
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Write_Without_Response_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Write_Without_Response_Cmd \n" );
	HCI_MESG_DBG (hostdbgfile, " Write Char Value params: %04X %02X \n", 
                                  Attribute_Handle, Attr_Value_Len );
    cmdParam[0] = (void *)&Conn_Handle;
    cmdParam[1] = (void *)&attributeHandle;
    cmdParam[2] = (void *)&attrValueLen;
    cmdParam[3] = (void *)Attr_Value;

    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_WRITE_WITHOUT_RESPONSE, 4, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif


#ifdef HCI_VENDOR_CMD_GATT_SIGNED_WRITE_WITHOUT_RESPONSE
tBleStatus HCI_GATT_Signed_Write_Without_Response_Cmd ( uint8 profileID,
													uint16 Conn_Handle,
													uint16 Attribute_Handle, 
                                                    uint8 Attr_Value_Len, 
                                                    uint8 * Attr_Value )
{
    void * cmdParam[5] = {NULL};
    uint16 attributeHandle = Attribute_Handle;
    uint8 attrValueLen = Attr_Value_Len;
    
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_GATT_Signed_Write_Without_Response_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_GATT_Signed_Write_Without_Response_Cmd \n" );
	HCI_MESG_DBG (hostdbgfile, " Write Char Value params: %04X %02X \n", 
                                  Attribute_Handle, Attr_Value_Len );

    cmdParam[0] = (void *)&Conn_Handle;
	cmdParam[1] = (void *)&attributeHandle;
    cmdParam[2] = (void *)&attrValueLen;
    cmdParam[3] = (void *)Attr_Value;
	

    return (Profile_HCI_Interface_Send_GATT_Procedure_Command (profileID, HCI_VENDOR_CMD_GATT_SIGNED_WRITE_WITHOUT_RESPONSE, 4, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam ));
}
#endif

#ifdef SET_TX_POWER_COMMAND /* TBR */

#ifdef  HCI_VENDOR_CMD_SET_TX_POWER_LEVEL
tBleStatus HCI_LE_Set_TX_Power_Level_Cmd(uint8 profileID,uint8 en_high_power, uint8 pa_level)
{
    void * cmdParam[2] = {NULL};
    uint8 power = en_high_power;
    uint8 pa = pa_level;

    PROFILE_MESG_DBG ( profiledbgfile, "HCI_LE_Set_TX_Power_Level_Cmd \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_LE_Set_TX_Power_Level_Cmd \n" );

    cmdParam[0] = (void *)&power;
    cmdParam[1] = (void *)&pa;
    
    return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_SET_TX_POWER_LEVEL, 2, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif 
#endif 

#ifdef SET_L2CAP_CONN_UPD_REQ_COMMAND /* TBR */

#ifdef HCI_VENDOR_CMD_L2CAP_CONN_UPD_REQ 
tBleStatus HCI_L2CAP_Connection_Parameter_Update_Request(uint8 profileID,uint16 conn_handle, uint16 interval_min,
							 uint16 interval_max, uint16 slave_latency,
							 uint16 timeout_multiplier)
{
  
    void * cmdParam[5] = {NULL};
    uint16 connectionHandle = conn_handle;
    uint16 int_min = interval_min;
    uint16 int_max = interval_max;
    uint16 latency = slave_latency;
    uint16 timeout = timeout_multiplier;
    
    PROFILE_MESG_DBG ( profiledbgfile, "HCI_L2CAP_Connection_Parameter_Update_Request \n" );
    HCI_MESG_DBG ( hostdbgfile, "HCI_L2CAP_Connection_Parameter_Update_Request \n" );
    
    cmdParam[0] = (void *)&connectionHandle;
    cmdParam[1] = (void *)&int_min;
    cmdParam[2] = (void *)&int_max;
    cmdParam[3] = (void *)&latency;
    cmdParam[4] = (void *)&timeout;
    
    return (Profile_HCI_Interface_Send_Command (profileID, HCI_VENDOR_CMD_L2CAP_CONN_UPD_REQ, 10, (PTR_TO_ARRAY_OF_VOID_PTRS)&cmdParam));
}
#endif
#endif
