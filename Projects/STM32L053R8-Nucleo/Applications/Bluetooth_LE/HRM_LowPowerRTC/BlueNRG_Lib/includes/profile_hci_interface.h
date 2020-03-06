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
*   FILENAME        -  profile_hci_interface.h
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
#ifndef _PROFILE_HCI_INTERFACE_H_
#define _PROFILE_HCI_INTERFACE_H_

/*******************************************************************************
 * Include Files
******************************************************************************/


/*******************************************************************************
 * Macro Declarations
******************************************************************************/
#define HCI_PROFILE_CMD_INTERFACE_FREE     (0x00)
#define HCI_PROFILE_CMD_INTERFACE_BUSY     (0x01)

/*******************************************************************************
* type definitions
*******************************************************************************/
typedef uint8 tHciCmdInterfaceStatus;
/*******************************************************************************
*  Function prototypes
*******************************************************************************/
void Profile_HCI_Interface_Init (void);

tBleStatus Profile_HCI_Interface_Set_Status ( uint16 opcode , uint8 sflag );

void Profile_HCI_Interface_Reset_GATT_Procedure_Status (void);

tHciCmdInterfaceStatus Profile_HCI_Interface_Get_Status(uint16 opcode);

void Profile_HCI_Interface_Set_GATT_Procedure_Status(uint16 opcode);

tHciCommandOpcode Profile_HCI_Interface_Get_Current_GATT_Procedure_Opcode ( void );

#ifdef HCI_LINK_CTL_CMD_DISCONNECT
tBleStatus HCI_Disconnect_Cmd(uint8 profileID,uint16 Connection_Handle,uint8 Reason);
#endif

#ifdef HCI_LINK_CTL_CMD_READ_REMOTE_VERSION
tBleStatus HCI_Send_Read_Remote_Version_Info_Cmd(uint8 profileID,uint16 Connection_Handle );
#endif

#ifdef HCI_HOST_CTL_CMD_SET_EVENT_MASK
tBleStatus HCI_Set_Event_Mask_Cmd (uint8 profileID, uint8 * EventMask );
#endif

#ifdef HCI_HOST_CTL_CMD_RESET
tBleStatus HCI_Reset_Cmd (uint8 profileID);
#endif

#ifdef HCI_HOST_CTL_CMD_READ_TRANSMIT_POWER_LEVEL
tBleStatus HCI_Read_Trmt_Power_Level_Cmd (uint8 profileID,uint16 Connection_Handle,uint8 Type);
#endif

#ifdef SET_TX_POWER_COMMAND /* TBR */
#ifdef HCI_VENDOR_CMD_SET_TX_POWER_LEVEL
tBleStatus HCI_LE_Set_TX_Power_Level_Cmd(uint8 profileID,uint8 en_high_power, uint8 pa_level);
#endif 
#endif 

#ifdef SET_L2CAP_CONN_UPD_REQ_COMMAND /* TBR */
#ifdef HCI_VENDOR_CMD_L2CAP_CONN_UPD_REQ 
tBleStatus HCI_L2CAP_Connection_Parameter_Update_Request(uint8 profileID,uint16 conn_handle, uint16 interval_min,
							 uint16 interval_max, uint16 slave_latency,
							 uint16 timeout_multiplier);
#endif
#endif

#ifdef HCI_HOST_CTL_CMD_SET_CONTROLLER_TO_HOST_FC
tBleStatus HCI_Set_Contr_Host_Flow_Control_Cmd (uint8 profileID,uint8 Flow_Control_Enable);
#endif

#ifdef HCI_HOST_CTL_CMD_HOST_BUFFER_SIZE
tBleStatus HCI_Host_Buffer_Size_Cmd ( uint8 profileID,
										 uint16 Host_ACL_Data_Pkt_Length, 
                                         uint8 Host_Synchronous_Data_Pkt_Length, 
                                         uint16 Host_Total_Num_ACL_Data_Pkt, 
                                         uint16 Host_Total_Num_Synchronous_Data_Pkt );
#endif

#ifdef HCI_HOST_CTL_CMD_HOST_NUM_COMP_PKTS
tBleStatus HCI_Host_Nbr_Completed_Pkts_Cmd ( uint8 profileID,
												uint8 Number_Of_Handles,
                                                uint16 * Connection_Handle,
                                                uint16 * Host_Num_Of_Completed_Pkt );
#endif

#ifdef HCI_INFO_PARAM_CMD_READ_LOCAL_VERSION
tBleStatus HCI_Read_Local_Version_Info_Cmd (uint8 profileID);
#endif

#ifdef HCI_INFO_PARAM_CMD_READ_LOCAL_COMMANDS
tBleStatus HCI_Read_Local_Supported_Commands_Cmd (uint8 profileID);
#endif

#ifdef HCI_INFO_PARAM_CMD_READ_LOCAL_FEATURES
tBleStatus HCI_Read_Local_Supported_Features_Cmd (uint8 profileID);
#endif

#ifdef HCI_INFO_PARAM_CMD_READ_BD_ADDR
tBleStatus HCI_Read_BD_ADDR_Cmd (uint8 profileID);
#endif

#ifdef HCI_STATUS_PARAM_CMD_READ_RSSI
tBleStatus HCI_Read_RSSI_Cmd (uint8 profileID,uint16 Handle);
#endif

#ifdef HCI_LE_CTL_SET_EVENT_MASK
tBleStatus HCI_LE_Set_Event_Mask_Cmd (uint8 profileID,uint8 * EventMask);
#endif

#ifdef HCI_LE_CTL_READ_BUFFER_SIZE
tBleStatus HCI_LE_Read_Buffer_Size_Cmd (uint8 profileID);
#endif

#ifdef HCI_LE_CTL_READ_LOCAL_SUPPORTED_FEATURE
tBleStatus HCI_LE_Read_Local_Supp_Features_Cmd (uint8 profileID);
#endif

#ifdef HCI_LE_CTL_SET_RANDOM_ADDRESS
tBleStatus HCI_LE_Set_Random_Address_Cmd (uint8 profileID,uint8 * Random_Address);
#endif

#ifdef HCI_LE_CTL_SET_ADV_PARAMETERS
tBleStatus HCI_LE_Set_Adv_Parameters_Cmd ( uint8 profileID,
											  uint16 Advertising_Interval_Min, 
                                              uint16 Advertising_Interval_Max, 
                                              uint8 Advertising_Type, 
                                              uint8 Own_Address_Type, 
                                              uint8 Direct_Address_Type, 
                                              uint8 * Direct_Address, 
                                              uint8 Advertising_Channer_Map, 
                                              uint8 Adrevtising_Filter_Policy );
#endif
                              
#ifdef HCI_LE_CTL_READ_ADV_CHANNEL_TX_POWER
tBleStatus HCI_LE_Read_Adv_Chnl_Tx_Power_Cmd (uint8 profileID);
#endif

#ifdef HCI_LE_CTL_SET_ADV_DATA
tBleStatus HCI_LE_Set_Adv_Data_Cmd (uint8 profileID,
									   uint8 Advertising_Data_Length, 
                                       uint8 * Advertising_Data );
#endif

#ifdef HCI_LE_CTL_SET_SCAN_RESPONSE
tBleStatus HCI_LE_Set_Scan_Resp_Data_Cmd ( uint8 profileID,
											  uint8 Scan_Response_Data_Length, 
                                              uint8 * Scan_Response_Data);
#endif
                        
#ifdef HCI_LE_CTL_SET_ADVERTIZE_ENABLE
tBleStatus HCI_LE_Set_Adv_Enable_Cmd (uint8 profileID,uint8 Advertising_Enable);
#endif

#ifdef HCI_LE_CTL_READ_WHITE_LIST_SIZE
tBleStatus HCI_LE_Read_White_List_Size_Cmd (uint8 profileID);
#endif

#ifdef HCI_LE_CTL_CLEAR_WHILE_LIST
tBleStatus HCI_LE_Clear_White_List_Cmd (uint8 profileID);
#endif

#ifdef HCI_LE_CTL_ADD_DEVICE_TO_WHILE_LIST
tBleStatus HCI_LE_Add_Dev_To_White_List_Cmd ( uint8 profileID,
												 uint8 Address_Type,
                                                 uint8 * Address );
#endif
                                       
#ifdef HCI_LE_CTL_REMOVE_DEVICE_FROM_WHITE_LIST
tBleStatus HCI_LE_Rem_Dev_From_White_List_Cmd ( uint8 profileID,
												   uint8 Address_Type,
                                                   uint8 * Address );
#endif

#ifdef HCI_LE_CTL_READ_CHANNEL_MAP
tBleStatus HCI_LE_Read_Channel_Map_Cmd (uint8 profileID,uint16 Connection_Handle);
#endif

#ifdef HCI_LE_CTL_ENCRYPT
tBleStatus HCI_LE_Encrypt_Cmd ( uint8 profileID,
								   uint8 Key[16], 
                                   uint8 Plain_Text_Data[16]);
#endif

#ifdef HCI_LE_CTL_RAND
tBleStatus HCI_LE_Rand_Cmd (uint8 profileID);
#endif

#ifdef HCI_LE_CTL_LTK_REQ_REPLY
tBleStatus HCI_LE_LTK_Req_Reply_Cmd ( uint8 profileID,
										 uint16 Connection_Handle,
                                         uint8 * Long_Term_Key );
#endif

#ifdef HCI_LE_CTL_LTK_REQ_NEGATIVE_REPLY
tBleStatus HCI_LE_LTK_Req_Neg_Reply_Cmd (uint8 profileID,uint16 Connection_Handle);
#endif

#ifdef HCI_LE_CTL_READ_SUPPORTED_STATES
tBleStatus HCI_LE_Read_Supported_States_Cmd (uint8 profileID);
#endif

#ifdef HCI_LE_CTL_RECEIVER_TEST
tBleStatus HCI_LE_Receiver_Test_Cmd (uint8 profileID,uint8 Rx_Frequency);
#endif

#ifdef HCI_LE_CTL_TRANSMITTER_TEST
tBleStatus HCI_LE_Transmitter_Test_Cmd ( uint8 profileID,
											uint8 Tx_Frequency, 
                                            uint8 Length_Of_Text_Data, 
                                            uint8 Packet_Payload );
#endif

#ifdef HCI_LE_CTL_TEST_END
tBleStatus HCI_LE_Test_End_Cmd (uint8 profileID);
#endif

#ifdef HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE
tBleStatus HCI_GAP_Set_Non_Discoverable_Cmd (uint8 profileID);
#endif
                                            
#ifdef HCI_VENDOR_CMD_GAP_SET_LIMITED_DISCOVERABLE
tBleStatus HCI_GAP_Set_Limited_Discoverable_Cmd ( uint8 profileID,
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
                                                     uint16 Slave_Conn_Interval_Max );
#endif
                                            
#ifdef HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE
tBleStatus HCI_GAP_Set_Discoverable_Cmd ( uint8 profileID,
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
                                             uint16 Slave_Conn_Interval_Max );
#endif
                                            
#ifdef HCI_VENDOR_CMD_GAP_SET_DIRECT_CONNECTABLE
tBleStatus HCI_GAP_Set_DirectConnectable_Cmd ( uint8 profileID,
												  uint8 Own_Address_Type, 
                                                  uint8 Initiator_Address_Type, 
                                                  uint8 * Initiator_Direct_Address );
#endif

#ifdef HCI_VENDOR_CMD_GAP_SET_SET_IO_CAPABILITY
tBleStatus HCI_GAP_Set_IOCapability_Cmd (uint8 profileID,uint8 IO_Capability);
#endif

#ifdef HCI_VENDOR_CMD_GAP_SET_AUTHENTICATION_REQ
tBleStatus HCI_GAP_Set_Authentication_Req_Cmd ( uint8 profileID,
												   uint8  MIMT_Mode,
                                                   uint8  OOB_Enable, 
                                                   uint8 * OOB_Data, /* uint8 OOB_Data[16] */
                                                   uint8  Encryption_Key_Size_Min, 
                                                   uint8  Encryption_Key_Size_Max, 
                                                   uint8  Use_Fixed_Pin, 
                                                   uint32 Fixed_Pin, 
                                                   uint8  Bonding_Mode);
#endif
                                                       
#ifdef HCI_VENDOR_CMD_GAP_SET_AUTHORIZATION_REQ /*TODO Check*/
tBleStatus HCI_GAP_Set_Authorization_Req_Cmd (uint8 profileID,uint8 Authorization_Enable);
#endif

#ifdef HCI_VENDOR_CMD_GAP_PASS_KEY_RESP
tBleStatus HCI_GAP_PassKey_Response_Cmd (uint8 profileID,uint16 Conn_Handle, uint32 Pass_Key);
#endif

#ifdef HCI_VENDOR_CMD_GAP_AUTHORIZATION_RESP
tBleStatus HCI_GAP_Set_Authorization_Resp_Cmd (uint8 profileID,uint8 Authorize);
#endif

#ifdef HCI_VENDOR_CMD_GAP_INIT
tBleStatus HCI_GAP_Init_Cmd (uint8 profileID,uint8 GAP_Role);
#endif

#ifdef HCI_VENDOR_CMD_GAP_SET_NON_CONNECTABLE
tBleStatus HCI_GAP_Set_NonConnectable_Cmd (uint8 profileID, uint8 Advertising_Event_Type );
#endif

#ifdef HCI_VENDOR_CMD_GAP_SET_UNDIRECTED_CONNECTABLE
tBleStatus HCI_GAP_Set_Undirected_Connectable_Cmd (  uint8 profileID,
														uint8 Adv_Filter_Policy, 
                                                        uint8 Own_Address_Type );
#endif

#ifdef HCI_VENDOR_CMD_GAP_SLAVE_SECURITY_REQUEST
tBleStatus HCI_GAP_Slave_Security_Req_Cmd (uint8 profileID, 
											  uint8 Bonding_Mode, 
                                              uint8 MIMT_Mode );
#endif

#ifdef HCI_VENDOR_CMD_GAP_UPDATE_ADV_DATA
tBleStatus HCI_GAP_Update_Advertising_Data ( uint8 profileID,
												uint8 Advertising_Data_Length, 
                                                uint8 * Advertising_Data );
#endif

#ifdef HCI_VENDOR_CMD_GAP_DELETE_AD_TYPE
tBleStatus HCI_GAP_Delete_AD_Type (uint8 profileID, uint8 AD_Type );
#endif

#ifdef HCI_VENDOR_CMD_GAP_GET_SEC_LEVEL
tBleStatus HCI_GAP_Get_Security_Level (uint8 profileID);
#endif

#ifdef HCI_VENDOR_CMD_GAP_SET_EVT_MASK
tBleStatus HCI_GAP_Set_Event_Mask_Cmd (uint8 profileID, uint16 Event_Mask );
#endif

#ifdef HCI_VENDOR_CMD_GAP_CONFIGURE_WHITELIST
tBleStatus HCI_GAP_Configure_WhiteList_Cmd (uint8 profileID);
#endif

#ifdef HCI_VENDOR_CMD_GATT_SERVER_INIT
tBleStatus HCI_GATT_Init_Cmd (uint8 profileID);
#endif

#ifdef HCI_VENDOR_CMD_GATT_ADD_SERVICE
tBleStatus HCI_GATT_Add_Service_Cmd (uint8 profileID, 
										uint8 UUID_Type, 
                                         uint8 * UUID, 
                                         uint8 Service_Type, 
                                         uint8 Max_Attribute_Records );
#endif

#ifdef HCI_VENDOR_CMD_GATT_INC_SERVICE
tBleStatus HCI_GATT_Include_Service_Cmd (uint8 profileID, 
											uint16 Service_Handle, 
                                             uint16 Include_Start_Handle, 
                                             uint16 Include_End_Handle, 
                                             uint8 Included_UUID_Type,
                                             uint8 * Included_UUID );
#endif

#ifdef HCI_VENDOR_CMD_GATT_ADD_CHAR
tBleStatus HCI_GATT_Add_Characterisitic_Cmd ( uint8 profileID,
												 uint16 Service_Handle, 
                                                 uint8 Char_UUID_Type,
                                                 uint8 * Char_UUID, 
                                                 uint8 Char_Value_Length, 
                                                 uint8 Char_Properties, 
                                                 uint8 Security_Permissions, 
                                                 uint8 Gatt_Event_Mask,
                                                 uint8 Encryption_Key_Size,
                                                 uint8 Is_Variable );
#endif

#ifdef HCI_VENDOR_CMD_GATT_ADD_CHAR_DESC
tBleStatus HCI_GATT_Add_Char_Descriptor_Cmd ( uint8 profileID,
												 uint16 Serv_Handle, 
                                                 uint16 Char_Handle, 
                                                 uint8 Char_Desc_UUID_Type,
                                                 uint8 * Char_Desc_UUID, 
                                                 uint8 Desc_Value_Max_Length,
                                                 uint8 Char_Desc_Value_Length,
                                                 uint8 * Char_Desc_Value,
                                                 uint8 Security_Permissions, 
                                                 uint8 Access_Permissions, 
                                                 uint8 Gatt_Event_Mask, 
                                                 uint8 Encryption_Key_Size,
                                                 uint8 Is_Variable );
#endif

#ifdef HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL
tBleStatus HCI_GATT_Update_CharVal_Cmd ( uint8 profileID,
											uint16 Serv_Handle, 
                                            uint16 Char_Handle, 
                                            uint8 Char_Value_Offset, 
                                            uint8 Char_Value_Length, 
                                            uint8 * Char_Value );
#endif

#ifdef HCI_VENDOR_CMD_GATT_DEL_CHAR
tBleStatus HCI_GATT_Delete_Characteristic_Cmd ( uint8 profileID,
												   uint16 Service_Handle, 
                                                   uint16 Char_Handle );
#endif

#ifdef HCI_VENDOR_CMD_GATT_DEL_SERVICE
tBleStatus HCI_GATT_Delete_Service_Cmd (uint8 profileID, uint16 Service_Handle );
#endif

#ifdef HCI_VENDOR_CMD_GATT_DEL_INC_SERVICE
tBleStatus HCI_GATT_Delete_Include_Service_Cmd ( uint8 profileID,
													uint16 Service_Handle, 
                                                    uint16 Include_Handle );
#endif

#ifdef HCI_VENDOR_CMD_GATT_SET_EVT_MASK
tBleStatus HCI_GATT_Set_Event_Mask_Cmd (uint8 profileID,uint32 Event_Mask);
#endif

#ifdef HCI_VENDOR_CMD_GATT_EXCHG_CONFIG
tBleStatus HCI_GATT_Exchange_Configuration_Cmd ( uint8 profileID, uint16 connectionHandle );
#endif

#ifdef HCI_VENDOR_CMD_GATT_FIND_INFO
tBleStatus HCI_GATT_Find_Information_Cmd ( uint8 profileID,
											  uint16 Conn_Handle,
											  uint16 Start_Handle,
                                              uint16 End_Handle );
#endif

#ifdef HCI_VENDOR_CMD_GATT_FIND_ATTR_BY_TYPE_VAL
tBleStatus HCI_GATT_Find_Attr_By_Type_Value_Cmd (uint8 profileID,
													uint16 Conn_Handle, 
													uint16 Start_Handle,
                                                     uint16 End_Handle, 
                                                     uint16 UUID, 
                                                     uint8 Attribute_Value_Length, 
                                                     uint8 * Attribute_Value );
#endif

#ifdef HCI_VENDOR_CMD_GATT_READ_ATTR_BY_TYPE
tBleStatus HCI_GATT_Read_Attribute_By_Type_Cmd ( uint8 profileID,
													uint16 Conn_Handle,
													uint16 Start_Handle,
                                                    uint16 End_Handle, 
                                                    uint8 UUID_Type, 
                                                    uint8 * UUID );
#endif

#ifdef HCI_VENDOR_CMD_GATT_READ_ATTR_BY_GROUP_TYPE
tBleStatus HCI_GATT_Read_Attribute_By_Group_Cmd ( uint8 profileID,
													uint16 Conn_Handle,
													 uint16 Start_Handle,
                                                     uint16 End_Handle, 
                                                     uint8 UUID_Type, 
                                                     uint8 * UUID );
#endif

#ifdef HCI_VENDOR_CMD_GATT_PREPARE_WRITE
tBleStatus HCI_GATT_Prepare_Write_Cmd ( uint8 profileID,
										 uint16 Conn_Handle,
										   uint16 Attr_Handle, 
                                           uint16 Value_Offset, 
                                           uint8 Attr_Value_Len, 
                                           uint8 * Attr_Value );
#endif

#ifdef HCI_VENDOR_CMD_GATT_EXECUTE_WRITES
tBleStatus HCI_GATT_Execute_Write_Cmd (uint8 profileID,uint16 Conn_Handle, uint8 Execute );
#endif

#ifdef HCI_VENDOR_CMD_GATT_CONFORM_INDICATION
tBleStatus HCI_GATT_Conform_Indication_Cmd ( uint8 profileID, uint16 Conn_Handle);
#endif

#ifdef HCI_VENDOR_CMD_GATT_WRITE_RESP
tBleStatus HCI_GATT_Write_Response_Cmd ( uint8 profileID,
										 uint16 Conn_Handle, 
											uint16 Handle, 
                                            uint8 Status, 
                                            uint8 Error_Code, 
                                            uint8 Att_Value_Length, 
                                            uint8 * Att_Value );
#endif

#ifdef HCI_VENDOR_CMD_GATT_ALLOW_READ
tBleStatus HCI_GATT_Allow_Read_Cmd ( uint8 profileID, uint16 Conn_Handle );
#endif

#ifdef HCI_VENDOR_CMD_GATT_DISC_ALL_PRIMARY_SERVICES
tBleStatus HCI_GATT_Discover_All_Primary_Service_Cmd ( uint8 profileID ,uint16 connHandle);
#endif

#ifdef HCI_VENDOR_CMD_GATT_DISC_PRIMARY_SERVICE_BY_UUID
tBleStatus HCI_GATT_Discover_Primary_Service_By_UUID_Cmd (uint8 profileID, 
															uint16 Conn_Handle,
															 uint8 UUID_Type, 
                                                              uint8 * UUID );
#endif

#ifdef HCI_VENDOR_CMD_GATT_FIND_INCLUDED_SERVICES
tBleStatus HCI_GATT_Find_Included_Service_Cmd ( uint8 profileID,
                                                uint16 Conn_Handle, 
												   uint16 Start_Handle, 
                                                   uint16 End_Handle );
#endif

#ifdef HCI_VENDOR_CMD_GATT_DISC_ALL_CHARAC_OF_A_SERVICE
tBleStatus HCI_GATT_Discover_All_Chars_Of_A_Service_Cmd ( uint8 profileID,
                                                          uint16 Conn_Handle,
														     uint16 Start_Handle, 
                                                             uint16 End_Handle );
#endif

#ifdef HCI_VENDOR_CMD_GATT_DISC_CHARAC_BY_UUID
tBleStatus HCI_GATT_Discover_Char_By_UUID_Cmd ( uint8 profileID,
												   uint16 Conn_Handle,
												   uint16 Start_Handle, 
                                                   uint16 End_Handle, 
                                                   uint8 UUID_Type, 
                                                   uint8 * UUID );
#endif

#ifdef HCI_VENDOR_CMD_GATT_DISC_ALL_CHAR_DESCRIPTORS
tBleStatus HCI_GATT_Discover_All_Char_Descriptor_Cmd ( uint8 profileID,
                                                       uint16 Conn_Handle,
														  uint16 Start_Handle, 
                                                          uint16 End_Handle );
#endif

#ifdef HCI_VENDOR_CMD_GATT_READ_CHARACTERISTIC_VAL
tBleStatus HCI_GATT_Read_Char_Value_Cmd (uint8 profileID, uint16 Conn_Handle,uint16 Attr_Handle );
#endif

#ifdef HCI_VENDOR_CMD_GATT_READ_CHARAC_USING_UUID
tBleStatus HCI_GATT_Read_Char_Using_UUID_Cmd (uint8 profileID);
#endif

#ifdef HCI_VENDOR_CMD_GATT_READ_LONG_CHARAC_VAL
tBleStatus HCI_GATT_Read_Long_Char_Value_Cmd (uint8 profileID);
#endif

#ifdef HCI_VENDOR_CMD_GATT_READ_MULTIPLE_CHARAC_VALUES
tBleStatus HCI_GATT_Read_Multiple_Char_Value_Cmd (uint8 profileID);
#endif

#ifdef HCI_VENDOR_CMD_GATT_WRITE_CHARAC_VAL
tBleStatus HCI_GATT_Write_Char_Value_Cmd ( uint8 profileID,
                                           uint16 Conn_Handle,
											  uint16 Attribute_Handle, 
                                              uint8 Attr_Value_Len, 
                                              uint8 * Attr_Value );
#endif

#ifdef HCI_VENDOR_CMD_GATT_WRITE_LONG_CHARAC_VAL
tBleStatus HCI_GATT_Write_Long_Char_Value_Cmd (uint8 profileID);
#endif

#ifdef HCI_VENDOR_CMD_GATT_WRITE_CHARAC_RELIABLE
tBleStatus HCI_GATT_Write_Char_Reliable_Cmd (uint8 profileID);
#endif

#ifdef HCI_VENDOR_CMD_GATT_READ_CHARAC_DESCRIPTOR
tBleStatus HCI_GATT_Read_Char_Descriptor_Cmd (uint8 profileID,uint16 Conn_Handle, uint16 Attribute_Handle );
#endif

#ifdef HCI_VENDOR_CMD_GATT_READ_LONG_CHARAC_DESCRIPTOR
tBleStatus HCI_GATT_Read_Long_Char_Descriptor_Cmd (uint8 profileID);
#endif

#ifdef HCI_VENDOR_CMD_GATT_WRITE_CHARAC_DESCRIPTOR
tBleStatus HCI_GATT_Write_Char_Descriptor_Cmd ( uint8 profileID,
                                                uint16 Conn_Handle,
												   uint16 Attribute_Handle, 
                                                   uint8 Attr_Value_Len, 
                                                   uint8 * Attr_Value );
#endif

#ifdef HCI_VENDOR_CMD_GATT_WRITE_LONG_CHARAC_DESCRIPTOR
tBleStatus HCI_GATT_Write_Long_Char_Descriptor_Cmd (uint8 profileID);
#endif

#ifdef HCI_VENDOR_CMD_GATT_WRITE_WITHOUT_RESPONSE
tBleStatus HCI_GATT_Write_Without_Response_Cmd(uint8 profileID,
                                               uint16 Conn_Handle,
											   uint16 Attribute_Handle, 
											   uint8 Attr_Value_Len, 
											   uint8 * Attr_Value);
#endif

#ifdef HCI_VENDOR_CMD_GATT_SIGNED_WRITE_WITHOUT_RESPONSE
tBleStatus HCI_GATT_Signed_Write_Without_Response_Cmd(uint8 profileID,
														uint16 Conn_Handle,
													  uint16 Attribute_Handle, 
                                                      uint8 Attr_Value_Len, 
                                                      uint8 * Attr_Value);
#endif


#endif /* _PROFILE_HCI_INTERFACE_H_ */

