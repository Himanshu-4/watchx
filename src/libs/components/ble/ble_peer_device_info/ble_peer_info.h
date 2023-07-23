#ifndef _BLE_PEER_INFO_H
#define _BLE_PEER_INFO_H

#include "ble_softdevice_init.h"

//// deifne some services that are used in some general info from iphone 

#define BLE_UUID_SERVICE_DEVICE_INFO  0x180A
#define BLE_UUID_CHAR_MANUFACTURER_NAME 0x2A29
#define BLE_UUID_CHAR_MODEL_NUMBER  0x2A24

#define BLE_UUID_SERVICE_BATTERY    0x180F 
#define BLE_UUID_CHAR_BATTERY_LEVEL 0x2A19

#define BLE_UUID_SERVICE_CURRENT_TIME   0x1805
#define BLE_UUID_CHAR_CURRENT_TIME  0x2A2B
#define BLE_UUID_CHAR_LOCAL_TIME    0x2A0F

enum _BLE_PEER_DEVICE_INFO_ERRORS_
{
    ble_peer_dev_ok =0x00,
    ble_peer_dev_info_op_timeout = NRF_BLE_PEER_DEV_ERR_BASE,
    
};


/// @brief this is to init the ble peer device info 
/// @param conn_handle 
/// @return succ/failure of the function 
uint32_t  ble_peer_device_init(uint16_t conn_handle);


/// @todo 

/// @brief 
/// @param param 
/// @param evt 
void ble_peer_Device_indication_handler(void *param, ble_gattc_evt_t *evt);

/// @brief 
/// @param param 
/// @param evt 
void ble_peer_Device_notification_handler(void *param, ble_gattc_evt_t *evt);

#endif