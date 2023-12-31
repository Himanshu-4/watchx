#ifndef _BLE_CONFIG_H
#define _BLE_CONFIG_H


///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
////////////// this is the config file for the ble related things 

///////// define the error bases 
#define NRF_BLE_GAP_ERR_BASE (NRF_BLE_ERR_BASE + 0x00UL)

#define NRF_BLE_GATT_CLIENT_ERR_BASE (NRF_BLE_ERR_BASE + 0x20UL)

#define NRF_BLE_GATT_SERVER_ERR_BASE (NRF_BLE_ERR_BASE + 0x40UL)

#define NRF_BLE_AMS_ERR_BASE (NRF_BLE_ERR_BASE + 0x60UL)

#define NRF_BLE_ANCS_ERR_BASE (NRF_BLE_ERR_BASE + 0x70UL)

#define NRF_BLE_PEER_DEV_ERR_BASE (NRF_BLE_ERR_BASE + 0x80UL)
//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//======================= BLE COMMON  CONFIG =============================================

#define BLE_MAX_DEVICE_SUPPORTED 1
// #define BLE_MAX_DEVICE_SUPPORTED 3 


//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//======================= BLE SOFTDEVICE  CONFIG =============================================

#define NRF_SDH_BLE_EVT_BUF_SIZE   300


#define  NRF_SDH_CLOCK_LF_SRC  NRF_CLOCK_LF_SRC_XTAL
#define  NRF_SDH_CLOCK_LF_RC_CTIV   0
#define  NRF_SDH_CLOCK_LF_RC_TEMP_CTIV  0
#define  NRF_SDH_CLOCK_LF_ACCURACY   7


///////// define the connection configuration 
#define NRF_SOFTDEVICE_DEFAULT_CONFIG_TAG  1

/////////////// below here are the defined configuration to set with sd_ble_cfg_set 
//////// ========= BLE connection specific configuration


//// GAP connection configuration, cfg_id is @ref BLE_CONN_CFG_GAP.
////The number of concurrent connections the application can create with this configuration.
#define NRF_BLE_TOTAL_LINK_COUNT   BLE_MAX_DEVICE_SUPPORTED

///The time set aside for this connection on every connection interval in 1.25 ms units.
// #define NRF_BLE_GAP_EVENT_LENGTH_TIME   6 // 7.5 msec 
#define NRF_BLE_GAP_EVENT_LENGTH_TIME   24 // 30 msec 

/// GATTS connection configuration, cfg_id is @ref BLE_CONN_CFG_GATTS.
/// Minimum guaranteed number of Handle Value Notifications that can be queued for transmission.The default value is @ref BLE_GATTS_HVN_TX_QUEUE_SIZE_DEFAULT */
#define NRF_BLE_GATT_S_HVN_TX_Q_SIZE  1

///======= gatt mtu conn config
///Maximum size of ATT packet the SoftDevice can send or receive. 
/// GATT connection configuration, cfg_id is @ref BLE_CONN_CFG_GATT.
#define NRF_BLE_GATT_MTU_CONFIG  250

//////======= gatt client conn config 
///The guaranteed minimum number of Write without Response that can be queued for transmission.
///  GATTC connection configuration, cfg_id is @ref BLE_CONN_CFG_GATTC
#define NRF_BLE_GATTC_WRITE_CMD_TX_Q_SIZE  1

/// l2cap conn configuration are pending 

//////////======== global common configuration 

/// Vendor Specific base UUID configuration, cfg_id is @ref BLE_COMMON_CFG_VS_UUID.
#define NRF_BLE_NO_OF_VENDOR_SPECIFIC_UUID 10


////////========== global gap configuration 

/// Central Address Resolution characteristic include configuration,cfg_id is @ref BLE_GAP_CFG_CAR_INCL_CONFIG. */
#define NRF_BLE_GAP_CHAR_CENTRAL_ADDR_RESOLUTION_INCLUDE  0

/// Peripheral Preferred Connection Parameters characteristic include configuration, cfg_id is @ref BLE_GAP_CFG_PPCP_INCL_CONFIG. */
#define NRF_BLE_GAP_CHAR_PERIPHERAL_PREFERD_CONN_INCLUDE    0

///  Device name configuration, cfg_id is @ref BLE_GAP_CFG_DEVICE_NAME. 
#define NRF_BLE_GAP_CONFIG_DEVICE_NAME  "WATCHX"

//// ===Role count configuration, cfg_id is @ref BLE_GAP_CFG_ROLE_COUNT.

// Maximum number of advertising sets
#define NRF_BLE_GAP_CONFIG_ROLE_COUNT_ADV_SET_COUNT  1

// Maximum number of connections concurrently acting as a peripheral.
#define NRF_BLE_GAP_CONFIG_ROLE_COUNT_PERIPH_ROLE_COUNT  BLE_MAX_DEVICE_SUPPORTED

//  Maximum number of connections concurrently acting as a central.
#define NRF_BLE_GAP_CONFIG_ROLE_COUNT_CENTRAL_ROLE_COUNT  0

//  Number of SMP instances shared between all connections acting as a central.
#define NRF_BLE_GAP_CONFIG_ROLE_COUNT_CENTRAL_SEC_COUNT  0

//  If set, the Quality of Service (QoS) channel survey module is available to the
#define NRF_BLE_GAP_CONFIG_ROLE_COUNT_QOS_CHANEL_SEURVEY_ROLE_COUNT 0


//================================================================
////////========== global gatt server  configuration 
/// cfg_id in @ref BLE_GATTS_CFGS series. 

/// @brief this is to  include the Service Changed characteristic in the Attribute Table
///  Include service changed characteristic, cfg_id is @ref BLE_GATTS_CFG_SERVICE_CHANGED.
#define NRF_BLE_GATT_SERVER_SERVICE_CHANGE  1

/// Attribute table size, cfg_id is @ref BLE_GATTS_CFG_ATTR_TAB_SIZE. 
#define NRF_BLE_GATT_ATTRIBUTE_TABLE_SIZE 512  ///  512 bytes 


//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//======================= BLE GAP CONFIG =============================================

/// @brief define how many bond information can be stored 
#define BLE_GAP_MAX_BOND_USERS_STORED 8

/////////////////// ble gap api specific 

#define BLE_GAP_API_MUTEX_TIMEOUT 300

#define BLE_GAP_API_TASK_NOTIF_TIMEOUT 1000

////////////////////////////////////////////////
/// this is used for cryptographic operations 

#define BLE_GAP_RANDOM_NUM_GEN_WAIT_TIME_MSEC 5

#define BLE_GAP_RANDOM_NUM_GEN_RETIRES 10

//////////////////////// ble connection param config 

#define BLE_MIN_CONNECTION_INTERVAL 100 //// in terms of milliseconds 
#define BLE_MAX_CONNECTION_INTERVAL 200 ////// in terms of millisec

// these packets the masters chooses to skip without risking connection /**< Slave latency. */ slave can 
#define BLE_CONNECTION_SLAVE_LATENCY 4

#define BLE_CONNECTION_SUPERVISION_TIMEOUT 4000

#define BLE_GAP_MAX_NO_OF_DEVICES BLE_MAX_DEVICE_SUPPORTED

/**< Name of device. Will be included in the advertising data. */
#define BLE_DEVICE_GAP_ADVERTISEMENT_NAME "WATCHX"

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//======================= BLE GATT server CONFIG =============================================

#define BLE_GATT_SERVER_RX_MTU 250


//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//======================= BLE GATT client CONFIG =============================================

#define BLE_NO_OF_CLIENT_SUPPORTED BLE_MAX_DEVICE_SUPPORTED

///// define the mutex wait time 
#define BLE_CLIENT_FUNCTIONS_MUTEX_WAIT_TIME 300

///////// define the wit time for task notification 
#define BLE_CLIENT_FUNCTIONS_CLIENT_WAIT_TIME 1000

#define BLE_CLIENT_MESSAGE_BUFFER_SIZE 200


//////////////////////////////////////////////////////////////////////////////////////
//============================================================================================
//======================= BLE ADDITIONAL  CONFIGs =============================================
//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//======================= BLE APPLE AMS CONFIG =============================================

/// @brief this is the number of bytes you want to allocate to the ble apple media service libary to hold the media data (track name , app name  etc )
#define BLE_AMS_MEM_SIZE 256 /// this much memory is enough for ble ams library 

#define BLE_AMS_MUTEX_TIMEOUT 500

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//======================= BLE APPLE ANCS CONFIG =============================================

#define BLE_ANCS_MEM_SIZE 1024 

#define BLE_ANCS_MUTEX_TIMEOUT 500
/// 1600 bytes for the ancs link list 
#define BLE_ANCS_LL_SIZE 800 


//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//======================= BLE APPLE PEER DEVICE INFO CONFIG =============================================



#endif 