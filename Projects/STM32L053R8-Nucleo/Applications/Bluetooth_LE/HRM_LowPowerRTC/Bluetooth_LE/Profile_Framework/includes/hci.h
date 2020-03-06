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
*   FILENAME        -  hhci.h
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
*   Comments: 
*
*******************************************************************************
*  File Description 
*  ---------------------
* This file have Host HCI API declarations which are exposed to upper layers.
******************************************************************************/
#ifndef _HHCI_H_
#define _HHCI_H_

/*******************************************************************************
 * Include Files
******************************************************************************/
#include "hal_types.h"
#include "ble_status.h"

/*******************************************************************************
 * Macro Declarations
******************************************************************************/
#define HCI_PACKET_SIZE				(255)
#define HCI_PACKET_HDR_SIZE     	(4)
#define HCI_CMD_PACKET_HDR_SIZE		(3)
#define HCI_EVT_PACKET_HDR_SIZE		(2)
#define HCI_ACL_PACKET_HDR_SIZE		(4)
#define HCI_PACKET_TYPE_SIZE	    (1)

#define CMD_OPCODE_OFFSET                (0)
#define CMD_PARAM_TOTAL_LENGTH_OFFSET    (2)
#define EVT_CODE_OFFSET                  (0)
#define EVT_PARAM_TOTAL_LENGTH_OFFSET    (1)
#define ACL_DATA_HANDLE_OFFSET           (0)
#define ACL_DATA_TOTAL_LENGTH_OFFSET	 (2)	

/* HCI Command Opcodes */
/* 
 * The BLE Controller supports all following (masked and unmasked) commands 
 * from the host. In the host stack source code, implementation corresponding 
 * to all commands are present for the accomplishment of the implementation. 
 * But here only those HCI command are unmasked which are being used by the 
 * profiles layer and all other are maked. It is dont to keep the host stack 
 * size as small as possible in the final compiled stack. While compiling the
 * host stack, code corresponding to only those command which are unmasked, 
 * will compile. 
 */

#define HCI_CMD_NO_OPERATION							(0x0000)
/* Group: Link control commands */
#define HCI_LINK_CTL_CMD_DISCONNECT                    (0x0406)
#define HCI_LINK_CTL_CMD_READ_REMOTE_VERSION           (0x041D) 

/* Group: Host controller and baseband commands */
#define HCI_HOST_CTL_CMD_SET_EVENT_MASK                (0x0C01)
#define HCI_HOST_CTL_CMD_RESET                         (0x0C03) 
#define HCI_HOST_CTL_CMD_READ_TRANSMIT_POWER_LEVEL     (0x0C2D) 
#define HCI_HOST_CTL_CMD_SET_CONTROLLER_TO_HOST_FC     (0x0C31) 
#define HCI_HOST_CTL_CMD_HOST_BUFFER_SIZE              (0x0C33) 
#define HCI_HOST_CTL_CMD_HOST_NUM_COMP_PKTS            (0x0C35) 

/* Group: Informational parameters */
#define HCI_INFO_PARAM_CMD_READ_LOCAL_VERSION          (0x1001) 
#define HCI_INFO_PARAM_CMD_READ_LOCAL_COMMANDS         (0x1002) 
#define HCI_INFO_PARAM_CMD_READ_LOCAL_FEATURES         (0x1003) 
#define HCI_INFO_PARAM_CMD_READ_BD_ADDR                (0x1009) 

/* Group: Status parameters */
#define HCI_STATUS_PARAM_CMD_READ_RSSI                 (0x1405)

/* Group: LE controller commands */
#define HCI_LE_CTL_SET_EVENT_MASK                      (0x2001)
/* #define HCI_LE_CTL_READ_BUFFER_SIZE                    (0x2002) */
/* #define HCI_LE_CTL_READ_LOCAL_SUPPORTED_FEATURE        (0X2003) */
/* #define HCI_LE_CTL_SET_RANDOM_ADDRESS                  (0X2005) */
/* #define HCI_LE_CTL_SET_ADV_PARAMETERS                  (0X2006) */
/* #define HCI_LE_CTL_READ_ADV_CHANNEL_TX_POWER           (0X2007) */
/* #define HCI_LE_CTL_SET_ADV_DATA                        (0X2008) */
#define HCI_LE_CTL_SET_SCAN_RESPONSE                   (0X2009) 
/* #define HCI_LE_CTL_SET_ADVERTIZE_ENABLE                (0X200A) */
/* #define HCI_LE_CTL_READ_WHITE_LIST_SIZE                (0X200F) */
/* #define HCI_LE_CTL_CLEAR_WHILE_LIST                    (0X2010) */
 #define HCI_LE_CTL_ADD_DEVICE_TO_WHILE_LIST            (0X2011) 
/* #define HCI_LE_CTL_REMOVE_DEVICE_FROM_WHITE_LIST       (0X2012) */
/* #define HCI_LE_CTL_READ_CHANNEL_MAP                    (0X2015) */
/* #define HCI_LE_CTL_ENCRYPT                             (0X2017) */
/* #define HCI_LE_CTL_RAND                                (0X2018) */
/* #define HCI_LE_CTL_LTK_REQ_REPLY                       (0X201A) */
/* #define HCI_LE_CTL_LTK_REQ_NEGATIVE_REPLY              (0X201B) */
/* #define HCI_LE_CTL_READ_SUPPORTED_STATES               (0X201C) */
/* #define HCI_LE_CTL_RECEIVER_TEST                       (0X201D) */
/* #define HCI_LE_CTL_TRANSMITTER_TEST                    (0X201E) */
/* #define HCI_LE_CTL_TEST_END                            (0X201F) */

/* -------------- Vendor Specific HCI Commands -------------- */

/* Group: GAP vendor Specific commands */
#define HCI_VENDOR_CMD_GAP_SET_NON_DISCOVERABLE        (0xFC81)
#define HCI_VENDOR_CMD_GAP_SET_LIMITED_DISCOVERABLE    (0xFC82)
#define HCI_VENDOR_CMD_GAP_SET_DISCOVERABLE            (0xFC83)
/* #define HCI_VENDOR_CMD_GAP_SET_DIRECT_CONNECTABLE      (0xFC84) */
#define HCI_VENDOR_CMD_GAP_SET_SET_IO_CAPABILITY       (0xFC85)
#define HCI_VENDOR_CMD_GAP_SET_AUTHENTICATION_REQ      (0xFC86)
/* #define HCI_VENDOR_CMD_GAP_SET_AUTHORIZATION_REQ       (0xFC87) */
#define HCI_VENDOR_CMD_GAP_PASS_KEY_RESP               (0xFC88)
#define HCI_VENDOR_CMD_GAP_AUTHORIZATION_RESP          (0xFC89) 
#define HCI_VENDOR_CMD_GAP_INIT                        (0xFC8A)
/* #define HCI_VENDOR_CMD_GAP_SET_NON_CONNECTABLE         (0xFC8B) */
#define HCI_VENDOR_CMD_GAP_SET_UNDIRECTED_CONNECTABLE  (0xFC8C)
#define HCI_VENDOR_CMD_GAP_SLAVE_SECURITY_REQUEST      (0xFC8D)
/* #define HCI_VENDOR_CMD_GAP_UPDATE_ADV_DATA             (0xFC8E) */
/* #define HCI_VENDOR_CMD_GAP_DELETE_AD_TYPE              (0xFC8F) */
/* #define HCI_VENDOR_CMD_GAP_GET_SEC_LEVEL               (0xFC90) */
#define HCI_VENDOR_CMD_GAP_SET_EVT_MASK                (0xFC91)
#define HCI_VENDOR_CMD_GAP_CONFIGURE_WHITELIST         (0xFC92)

/* Group: GATT vendor Specific commands */
#define HCI_VENDOR_CMD_GATT_SERVER_INIT                (0xFD01)
#define HCI_VENDOR_CMD_GATT_ADD_SERVICE                (0xFD02)
#define HCI_VENDOR_CMD_GATT_INC_SERVICE                (0xFD03) 
#define HCI_VENDOR_CMD_GATT_ADD_CHAR                   (0xFD04)
#define HCI_VENDOR_CMD_GATT_ADD_CHAR_DESC              (0xFD05)
#define HCI_VENDOR_CMD_GATT_UPD_CHAR_VAL               (0xFD06)
/* #define HCI_VENDOR_CMD_GATT_DEL_CHAR                   (0xFD07) */
/* #define HCI_VENDOR_CMD_GATT_DEL_SERVICE                (0xFD08) */
/* #define HCI_VENDOR_CMD_GATT_DEL_INC_SERVICE            (0xFD09) */
#define HCI_VENDOR_CMD_GATT_SET_EVT_MASK               (0xFD0A)
#define HCI_VENDOR_CMD_GATT_EXCHG_CONFIG               (0xFD0B)
/* #define HCI_VENDOR_CMD_GATT_FIND_INFO                  (0xFD0C) */
/* #define HCI_VENDOR_CMD_GATT_FIND_ATTR_BY_TYPE_VAL      (0xFD0D) */
/* #define HCI_VENDOR_CMD_GATT_READ_ATTR_BY_TYPE          (0xFD0E) */
/* #define HCI_VENDOR_CMD_GATT_READ_ATTR_BY_GROUP_TYPE    (0xFD0F) */
        /* #define HCI_VENDOR_CMD_GATT_READ_REQ                   () */
        /* #define HCI_VENDOR_CMD_GATT_READ_BLOB_REQ              () */
        /* #define HCI_VENDOR_CMD_GATT_READ_MULTIPLE_ATTR_VAL     () */
        /* #define HCI_VENDOR_CMD_GATT_WRITE_REQ                  () */
        /* #define HCI_VENDOR_CMD_GATT_WRITE_CMD                  () */
        /* #define HCI_VENDOR_CMD_GATT_SIGNED_WRITE_CMD           () */
/* #define HCI_VENDOR_CMD_GATT_PREPARE_WRITE              (0xFD10) */
/* #define HCI_VENDOR_CMD_GATT_EXECUTE_WRITES             (0xFD11) */

#ifdef SET_TX_POWER_COMMAND /* TBR */

#define HCI_VENDOR_CMD_SET_TX_POWER_LEVEL (0xFC0F)
   
#endif 

#ifdef SET_L2CAP_CONN_UPD_REQ_COMMAND /* TBR */

#define HCI_VENDOR_CMD_L2CAP_CONN_UPD_REQ (0xFD81)

#endif

/* GATT client procedures */
#define HCI_VENDOR_CMD_GATT_DISC_ALL_PRIMARY_SERVICES         (0xFD12)
#define HCI_VENDOR_CMD_GATT_DISC_PRIMARY_SERVICE_BY_UUID      (0xFD13)
/* #define HCI_VENDOR_CMD_GATT_FIND_INCLUDED_SERVICES            (0xFD14) */
#define HCI_VENDOR_CMD_GATT_DISC_ALL_CHARAC_OF_A_SERVICE      (0xFD15)
/* #define HCI_VENDOR_CMD_GATT_DISC_CHARAC_BY_UUID               (0xFD16) */
#define HCI_VENDOR_CMD_GATT_DISC_ALL_CHAR_DESCRIPTORS         (0xFD17)
#define HCI_VENDOR_CMD_GATT_READ_CHARACTERISTIC_VAL           (0xFD18)
/* #define HCI_VENDOR_CMD_GATT_READ_CHARAC_USING_UUID            (0xFD19) */
/* #define HCI_VENDOR_CMD_GATT_READ_LONG_CHARAC_VAL              (0xFD1A) */
/* #define HCI_VENDOR_CMD_GATT_READ_MULTIPLE_CHARAC_VALUES       (0xFD1B) */
#define HCI_VENDOR_CMD_GATT_WRITE_CHARAC_VAL                  (0xFD1C)
/* #define HCI_VENDOR_CMD_GATT_WRITE_LONG_CHARAC_VAL             (0xFD1D) */
/* #define HCI_VENDOR_CMD_GATT_WRITE_CHARAC_RELIABLE             (0xFD1E) */
/* #define HCI_VENDOR_CMD_GATT_WRITE_LONG_CHARAC_DESCRIPTOR      (0xFD1F) */
/* #define HCI_VENDOR_CMD_GATT_READ_LONG_CHARAC_DESCRIPTOR       (0xFD20) */
#define HCI_VENDOR_CMD_GATT_WRITE_CHARAC_DESCRIPTOR           (0xFD21)
#define HCI_VENDOR_CMD_GATT_READ_CHARAC_DESCRIPTOR            (0xFD22)
#define HCI_VENDOR_CMD_GATT_WRITE_WITHOUT_RESPONSE            (0xFD23)
#define HCI_VENDOR_CMD_GATT_SIGNED_WRITE_WITHOUT_RESPONSE     (0xFD24) 
/* #define HCI_VENDOR_CMD_GATT_CONFORM_INDICATION             (0xFD25) */
#define HCI_VENDOR_CMD_GATT_WRITE_RESP                     (0xFD26)
#define HCI_VENDOR_CMD_GATT_ALLOW_READ                     (0xFD27)



#define HCI_NUM_OF_COMMANDS_SUPPORTED                  (50)



/* ---- HCI Event Codes ---- */
#define HCI_EVT_DISCONN_COMPLETE                   (0x05)
#define HCI_EVT_ENCRYPT_CHANGE                     (0x08)
#define HCI_EVT_READ_REMOTE_VERSION_COMPLETE       (0x0C)
#define HCI_EVT_CMD_COMPLETE                       (0x0E)
#define HCI_EVT_CMD_STATUS                         (0x0F)
#define HCI_EVT_HARDWARE_ERROR                     (0x10)
#define HCI_EVT_NUM_COMP_PKTS                      (0x13)
#define HCI_EVT_DATA_BUFFER_OVERFLOW               (0x1A)
#define HCI_EVT_ENCRYPTION_KEY_REFRESH_COMPLETE    (0x30)

#define HCI_EVT_LE_META_EVENT                      (0x3E)
#define HCI_EVT_LE_CONN_COMPLETE                     (0x01)
#define HCI_EVT_LE_CONN_UPDATE_COMPLETE              (0x03)
#define HCI_EVT_LE_LTK_REQUEST                       (0x05)


#define HCI_EVT_VENDOR_SPECIFIC                   (0xFF)

/* HCI vendor specific event */
#define HCI_EVT_BLUE_INITIALIZED                      (0x0001)

/* GAP Vendor specific event codes */
#define HCI_EVT_BLUE_GAP_SET_LIMITED_DISCOVERABLE     (0x0400)
#define HCI_EVT_BLUE_GAP_PAIRING_CMPLT                (0x0401)
#define HCI_EVT_BLUE_GAP_PASS_KEY_REQUEST             (0x0402)
#define HCI_EVT_BLUE_GAP_AUTHORIZATION_REQUEST        (0x0403)
#define HCI_EVT_BLUE_GAP_SLAVE_SECURITY_INITIATED     (0X0404)

/* L2CAP vendor specific event codes */
#define HCI_EVT_BLUE_L2CAP_CONN_UPD_RESP			  (0x0800)
#define HCI_EVT_BLUE_L2CAP_PROCEDURE_TIMEOUT          (0x0801)


/* vendor specific debug events */
#define HCI_EVT_BLUE_DEBUG							  (0x1000)

/* GATT vendor specific event codes */
#define HCI_EVT_BLUE_GATT_ATTRIBUTE_MODIFIED          (0x0C01)
#define HCI_EVT_BLUE_GATT_PROCEDURE_TIMEOUT           (0x0C02)
#define HCI_EVT_BLUE_EXCHANGE_MTU_RESP		  		  (0x0C03)
#define HCI_EVT_BLUE_ATT_FIND_INFORMATION_RESP		  (0x0C04)
#define HCI_EVT_BLUE_ATT_FIND_BY_TYPE_VAL_RESP		  (0x0C05)


#define HCI_EVT_BLUE_ATT_READ_BY_TYPE_RESP			  (0x0C06)
#define HCI_EVT_BLUE_ATT_READ_RESP					  (0x0C07)
#define HCI_EVT_BLUE_ATT_READ_BLOB_RESP				  (0x0C08)
#define HCI_EVT_BLUE_ATT_READ_MULTIPLE_RESP			  (0x0C09)
#define HCI_EVT_BLUE_ATT_READ_BY_GROUP_RESP           (0x0C0A)
#define HCI_EVT_BLUE_ATT_WRITE_RESP					  (0x0C0B)
#define HCI_EVT_BLUE_ATT_PREPARE_WRITE_RESP			  (0x0C0C)
#define HCI_EVT_BLUE_ATT_EXEC_WRITE_RESP			  (0x0C0D)
#define HCI_EVT_BLUE_GATT_INDICATION				  (0x0C0E)
#define HCI_EVT_BLUE_GATT_NOTIFICATION				  (0x0C0F)
#define HCI_EVT_BLUE_GATT_PROCEDURE_COMPLETE          (0x0C10)
#define HCI_EVT_BLUE_GATT_ERROR_RESP                  (0x0C11)
#define HCI_EVT_BLUE_GATT_DISC_READ_CHAR_BY_UUID_RESP (0x0C12)
#define HCI_EVT_BLUE_GATT_WRITE_PERMIT_REQ            (0x0C13)
#define HCI_EVT_BLUE_GATT_READ_PERMIT_REQ             (0x0C14)
#define HCI_EVT_BLUE_GATT_READ_MULTI_PERMIT_REQ       (0x0C15)
#define EVT_BLUE_GATT_PREP_WRITE_PERMIT_REQ           (0x0C16)

/* HCI Event Error Codes */


/*******************************************************************************
* type definitions
*******************************************************************************/
/*********************************************
 * HCI packet header type Declarations 
*********************************************/
typedef uint16 tHciCommandOpcode;
typedef uint8 tHciCommandLengthParam;

typedef uint8 tHciEventCode;
typedef uint8 tHciEventLengthParam;

typedef uint16 tHciACLPktHeader;
typedef uint16 tHciACLPayloadLength;


/*********************************************
 * Command Response Packet Type Declarations 
*********************************************/
typedef struct _tHciEventPacketFormat{
    tHciEventCode eventCode;
	tHciEventLengthParam eventParamLen;
}tHciEventPacketFormat;

typedef struct _tDisconnCompltNotifyPkt{
    uint8 status;
    uint16 connectionHandle;
    uint8 reason;
}tDisconnCompltNotifyPkt;

typedef struct _tEncryptionChangeNotifyPkt{
    uint8 status;
    uint16 connectionHandle;
    uint8 encryptionEnable;
}tEncryptionChangeNotifyPkt;

typedef struct _tReadRemoteVersionCompNotifyPkt{
    uint8 status;
    uint16 connectionHandle;
    uint8 version;
    uint16 manufacturerName;
    uint16 subversion;
}tReadRemoteVersionCompNotifyPkt;

/*********** command complete responses *****************/

typedef struct _tReadTxPowerLevelRespPkt{
    uint16 connectionHandle;
    uint8 txPowerLevel;
}tReadTxPowerLevelRespPkt;

typedef struct _tReadLocalVersionRespPkt{
    uint8 HCIVersion;
    uint16 HCIRevision;
    uint8 LMPVersion;
    uint16 manufacturerName;
    uint16 LMPSubversion;
}tReadLocalVersionRespPkt;

typedef struct _tReadLocalCommandsRespPkt{
    uint8 * supportedCommandBuff;  /* total 64 bytes */
}tReadLocalCommandsRespPkt;

typedef struct _tReadLocalFeaturesRespPkt{
    uint8 * LMPFeaturesBuff;  /* total 8 bytes */
}tReadLocalFeaturesRespPkt;

typedef struct _tReadBRAddressRespPkt{
    uint8 * BRAddressBuff;  /* total 6 bytes */
}tReadBRAddressRespPkt;

typedef struct _tReadRSSIRespPkt{
    uint16 handle;
    uint8 rssi;
}tReadRSSIRespPkt;

typedef struct _tReadBufferSizeRespPkt{
    uint16 leDataPacketLength;
    uint8 totalNumOfLEDataPackets;
}tReadBufferSizeRespPkt;

typedef struct _tReadLocalSupportedRespPkt{
    uint8 localFeatures[8];
}tReadLocalSupportedRespPkt;

typedef struct _tReadAdvtChnlTxPwrRespPkt{
    uint8 txPwrLevel;
}tReadAdvtChnlTxPwrRespPkt;

typedef struct _tReadWhiteListSizeRespPkt{
    uint8 whiteListSize;
}tReadWhiteListSizeRespPkt;

typedef struct _tReadChannelMapRespPkt{
    uint16 connectionHandle;
    uint8 leChannelMap[5];
}tReadChannelMapRespPkt;

typedef struct _tEncryptRespPkt{
    uint8 encryptedData[16];
}tEncryptRespPkt;

typedef struct _tRandomNumRespPkt{
    uint8 randomNum[8];
}tRandomNumRespPkt;

#ifdef HCI_LE_CTL_LTK_REQ_REPLY
typedef struct _tLTKReqReplyRespPkt{
    uint16 connectionHandle;
}tLTKReqReplyRespPkt;
#endif
#ifdef HCI_LE_CTL_LTK_REQ_NEGATIVE_REPLY
typedef struct _tLTKReqNegativeReplyRespPkt{
    uint16 connectionHandle;
}tLTKReqNegativeReplyRespPkt;
#endif

typedef struct _tReadSupportedStatesRespPkt{
    uint8 leStates[8];
}tReadSupportedStatesRespPkt;

#ifdef HCI_LE_CTL_TEST_END
typedef struct _tTestEndRespPkt{
    uint16 numOfPackets;
}tTestEndRespPkt;
#endif

typedef struct _tGapInitRespPkt{
    uint16 serviceHandle;
    uint16 devNameCharHandle;
    uint16 appearanceCharHandle;
}tGapInitRespPkt;

typedef struct _tGapGetSecurityLevelRespPkt{
    uint8 mimtProtectionRequired;
    uint8 bondingRequired;
    uint8 oobDataPresent;
    uint8 passkeyRequired;
}tGapGetSecurityLevelRespPkt;

typedef struct _tGattUpdateCharValRespPkt{
	uint16 servHandle;
	uint16 charHandle;
}tGattUpdateCharValRespPkt;

typedef struct _tGattAddServiceRespPkt{
    uint16 serviceHandle;
}tGattAddServiceRespPkt;

typedef struct _tGattIncludeServiceRespPkt{
    uint16 includedHandle;
}tGattIncludeServiceRespPkt;

typedef struct _tGattAddCharRespPkt{
    uint16 charHandle;
}tGattAddCharRespPkt;

typedef struct _tGattAddCharDescRespPkt{
  uint16 charDescHandle;
}tGattAddCharDescRespPkt;

typedef union _tCmdCmpltPktFormat
{
	tReadTxPowerLevelRespPkt readTxPowerLevelResp;
    tReadLocalVersionRespPkt readLocalVersionResp;
    tReadLocalCommandsRespPkt readLocalCommandsResp;
    tReadLocalFeaturesRespPkt readLocalFeaturesResp;
    tReadBRAddressRespPkt readBRAddressResp;
    tReadRSSIRespPkt readRSSIResp;
    tReadBufferSizeRespPkt readBufferSizeResp;
    tReadLocalSupportedRespPkt readLocalSupportedFeatureResp;
    tReadAdvtChnlTxPwrRespPkt readAdvtChnlTxPwrResp;
    tReadWhiteListSizeRespPkt readWhiteListSizeResp;
    tReadChannelMapRespPkt readChannelMapResp;
    tEncryptRespPkt encryptResp;
    tRandomNumRespPkt randomNumResp;
#ifdef HCI_LE_CTL_LTK_REQ_REPLY
    tLTKReqReplyRespPkt ltkReqReplyResp;
#endif
#ifdef HCI_LE_CTL_LTK_REQ_NEGATIVE_REPLY
    tLTKReqNegativeReplyRespPkt ltkReqNegativeReplyResp;
#endif
    tReadSupportedStatesRespPkt readSupportedStatesResp;
#ifdef HCI_LE_CTL_TEST_END
    tTestEndRespPkt testEndResp;
#endif
    tGapInitRespPkt gapInitResp;
    tGapGetSecurityLevelRespPkt gapGetSecurityLevelResp;
    tGattAddServiceRespPkt gattAddServiceResp;
    tGattIncludeServiceRespPkt gattIncludeServiceResp;
    tGattAddCharRespPkt gattAddCharResp;
    tGattAddCharDescRespPkt gattAddCharDescResp;
	tGattUpdateCharValRespPkt gattUpdateCharValRespPkt;
}tCmdCmpltPktFormat;

typedef struct _tCommandCompleteNotifyPkt{

    uint8 numHCICommandPackets;
    uint16 commandOpcode;
    uint8 status;
    tCmdCmpltPktFormat cmdCmpltPacket;
}tCommandCompleteNotifyPkt;
/**********************************************************************/
typedef struct _tCommandStatusNotifyPkt{
    uint8 status;
    uint8 numHCICommandPackets;
    uint16 commandOpcode;
}tCommandStatusNotifyPkt;

typedef struct _tHardwareErrorNotifyPkt{
    uint8 hardwareCode;
}tHardwareErrorNotifyPkt;

typedef struct _tNumOfCompletedPktNotifyPkt{
    uint8 numberOfHandles;
    uint16 * connectionHandleArrayPtr;
    uint16 * numOfCompletedPktArrPtr;
}tNumOfCompletedPktNotifyPkt;

typedef struct _tDataBuffOverflowNotifyPkt{
    uint8 linkType;
}tDataBuffOverflowNotifyPkt;

typedef struct _tEncrypKeyRefreshNotifyPkt{
    uint8 status;
    uint16 connectionHandle;
}tEncrypKeyRefreshNotifyPkt;

/************* LE Meta Event ***************************/

typedef struct _tLEConnectionCompleteNotifyPkt{
    uint8 status;
    uint16 connectionHandle;
    uint8 role;
    uint8 peerAddressType;
    uint8 peerAddress[6];
    uint16 connectionInterval;
    uint16 connectionLatency;
    uint16 supervisionTimeout;
    uint8 masterClockAccuracy;
}tLEConnectionCompleteNotifyPkt;

typedef struct _tLEConnectionUpdateCompNotifyPkt{
    uint8 status;
    uint16 connectionHandle;
    uint16 connectionInterval;
    uint16 connectionLatency;
    uint16 supervisionTimeout;
}tLEConnectionUpdateCompNotifyPkt;

typedef struct _tLELongTermKeyReqNotifyPkt{
    uint16 connectionHandle;
    uint8 randomNumber[8];
    uint16 encryptionDiversifier;
}tLELongTermKeyReqNotifyPkt;

typedef union _tLeMetaEvt{
    tLEConnectionCompleteNotifyPkt leConnectionComplete;
    tLEConnectionUpdateCompNotifyPkt leConnectionUpdateComp;
    tLELongTermKeyReqNotifyPkt leLTKRequest;
}tLeMetaEvt;

typedef struct _tLEEventNotifyPkt{
     uint8 subEventCode;
	 tLeMetaEvt leMetaEvtPkt;
}tLEEventNotifyPkt;

/**************** Vendor Specific Event packet *********************/

typedef struct _tVSL2capConnectionUpdateRespNotifyPkt{
    uint8 dataLength;
    uint8 * dataBuff;
}tVSL2capConnectionUpdateRespNotifyPkt;

typedef struct _tVSGattAttributeModifiedNotifyPkt{
    uint16 attributeHandle; 
    uint8 dataLength;
    uint8 * attData;
}tVSGattAttributeModifiedNotifyPkt;

typedef struct _tVSGattClientEventNotifyPkt{
    uint8 dataLength;
    uint8 * attData;
}tVSGattClientEventNotifyPkt;

typedef struct _tVSGattProcedureCompleteNotifyPkt{
    uint8 dataLength;
    uint8 * attData;
	uint16 procedureOpcode;
}tVSGattProcedureCompleteNotifyPkt;

typedef struct _tVSGattWritePermitReqNotifyPkt{
    uint16 attHandle;
    uint8 dataLength;
    uint8 * attData;
}tVSGattWritePermitReqNotifyPkt;

typedef struct _tVSGattReadPermitReqNotifyPkt{
    uint16 attHandle;
}tVSGattReadPermitReqNotifyPkt;

typedef struct _tVSGattReadMultiPermitReqNotifyPkt{
    uint16 attHandle[12];
}tVSGattReadMultiPermitReqNotifyPkt;

typedef uint8 tVSEvtBlueInitialized;
typedef uint8 tVSPairingStatus;
typedef uint8 tVSGattProcTimeout;

typedef struct _tVendorSpecificEvtNotifyPkt
{
	/* vendor specific event code */
    uint16 vsEventCode;
	union{
    tVSEvtBlueInitialized BlueNRGMode;
	tVSPairingStatus status;
	tVSGattProcTimeout gattRole;
    tVSL2capConnectionUpdateRespNotifyPkt l2capConnUpdateResp;
    tVSGattAttributeModifiedNotifyPkt gattAttrModified;
    tVSGattClientEventNotifyPkt gattClientProcedureResp;
    tVSGattProcedureCompleteNotifyPkt gattProcedureComplete;
    tVSGattWritePermitReqNotifyPkt gattWritePermitReq;
    tVSGattReadPermitReqNotifyPkt gattReadPermitReq;
	}vsEvtPkt;

}tVendorSpecificEvtNotifyPkt;

typedef union _tEvtPkt{
	tDisconnCompltNotifyPkt disconnComplt;
    tEncryptionChangeNotifyPkt encryptionChange;
    tReadRemoteVersionCompNotifyPkt readRemoteVersionComp;
    tCommandCompleteNotifyPkt commandComplete;
    tCommandStatusNotifyPkt commandStatus;
    tHardwareErrorNotifyPkt hardwareError;
    tNumOfCompletedPktNotifyPkt numCompletedPkt;
    tDataBuffOverflowNotifyPkt dataBuffOverflow;
    tEncrypKeyRefreshNotifyPkt encryptionKeyRefresh;
    tLEEventNotifyPkt leEvent;
    tVendorSpecificEvtNotifyPkt vendorSpecificEvt;
}tEvtPkt;

typedef struct _tHciResponsePacket{
    tHciEventPacketFormat hciNotifPkt;
    tEvtPkt  evtPkt;
}tHciResponsePacket;




typedef void (* HCI_READ_EVENT_NOTIFY_TYPE) ( uint8 profileID, 
                                              tHciResponsePacket * response);

typedef void (* HCI_READ_ACL_PACKET_NOTIFY_TYPE) ( uint16 aclHeader, 
                                                   uint16 aclPayloadLangth, 
                                                   uint8 * aclPayloadBuff );
/******************************************************************************
* Function Prototypes
******************************************************************************/

/**
 * HCI_Init 
 * 
 * Initializes the HCI RX and TX queues
 * Initializes the HTL layer
 * registers callback to the HTL
 */ 
tBleStatus HCI_Init (void);

/**
 * HCI_Send_Command 
 * 
 * @param[in] profileID : ID of the profile which is sending the command
 * @param[in] opcode : opcode of the command to be sent
 * @param[in] numOfCmdParam : command parameter length
 * @param[in] cmdParamPtrArray : pointer to the array of command paramters
 * 
 * forms the command packet and inserts it into the hciWritePktQueue to be
 * written to the spi by HCI_Process_Q
 */
tBleStatus HCI_Send_Command (uint8 profileID,
							    uint16 opcode,
                                uint8 numOfCmdParam,
                                void * (* cmdParamPtrArray)[]);

/**
 * HCI_Register_Rx_Event_Handler
 * 
 * @param[in] eventPktRecvHandler : the handler to be registered
 * 
 * This function will be called to notify the profile of the events
 * received at HCI
 */ 
tBleStatus HCI_Register_Rx_Event_Handler (HCI_READ_EVENT_NOTIFY_TYPE eventPktRecvHandler);

/**
 * HCI_Process_Q
 * 
 * 1. writes the pending command packets to spi 
 * 2. reads data from HTL if available
 * 3. processes the events read from HTL
 */ 
void HCI_Process_Q(void);

#endif /* _HHCI_H_ */
