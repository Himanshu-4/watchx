#ifndef _BLE_PEER_INFO_H
#define _BLE_PEER_INFO_H

#include "ble_softdevice_init.h"

// include the time library
#include "time_manager/kernel_time.h"

//// deifne some services that are used in some general info from iphone

#define BLE_UUID_SERVICE_DEVICE_INFO 0x180A
#define BLE_UUID_CHAR_MANUFACTURER_NAME 0x2A29
#define BLE_UUID_CHAR_MODEL_NUMBER 0x2A24

#define BLE_UUID_SERVICE_BATTERY 0x180F
#define BLE_UUID_CHAR_BATTERY_LEVEL 0x2A19

#define BLE_UUID_SERVICE_CURRENT_TIME 0x1805
#define BLE_UUID_CHAR_CURRENT_TIME 0x2A2B
#define BLE_UUID_CHAR_LOCAL_TIME 0x2A0F

enum _BLE_PEER_DEVICE_INFO_ERRORS_
{
    ble_peer_dev_ok = 0x00,
    ble_peer_dev_info_op_timeout = NRF_BLE_PEER_DEV_ERR_BASE,

};

/// @brief this is to init the ble peer device info
/// @param conn_handle
/// @return succ/failure of the function
uint32_t ble_peer_device_init(uint16_t conn_handle);

/// @brief to get the time information from the iphone
/// @param conn_handle
/// @param time
/// @return succ/failure of the fun
uint32_t ble_peer_get_Time_info(uint16_t conn_handle, kernel_time_struct_t *time);

/// @brief to get the date information from the iphone
/// @param conn_handle
/// @param date
/// @return succ/failure
uint32_t ble_peer_get_date_info(uint16_t conn_handle, kernel_date_struct_t *date);

/// @brief to get the battery information from the iphone
/// @param conn_handle
/// @param batt_soc
/// @return succ/failure of fun
uint32_t ble_peer_get_battery_info(uint16_t conn_handle, uint8_t *batt_soc);

/// @brief to get the device name of the bluetooth
/// @param conn_handle
/// @param device_name
/// @return succ/failure of function
uint32_t ble_peer_get_device_name(uint16_t conn_handle, char *device_name);

#define BLE_SERVICE_CHANGED 1
#define BLE_SERVICE_NOT_CHANGED 0

/// @brief to get the servie changed indication notification
/// @param conn_handle
/// @return service changed indication
uint8_t ble_peer_get_service_change_ind(uint16_t conn_handle);

typedef PACKED_STRUCT _BLE_GAP_SERVICE_HANDLES_
{
    uint16_t ble_gap_service_handle;
    uint16_t ble_gap_device_name_char_handle;
}
ble_Gap_service_handles;

typedef PACKED_STRUCT _BLE_GATT_SERVICE_HANDLES_
{
    uint16_t ble_gatt_Service_handle;
    uint16_t ble_Gatt_srvc_cgd_char_handle;
}
ble_gatt_service_handles;

typedef PACKED_STRUCT _BLE_DEVICE_INFORMATION_SERVICE_HANDLE_
{
    uint16_t ble_gatt_device_info_serivce_handle;
    uint16_t ble_gatt_device_info_srvc_char_handle;
}
ble_device_info_service_handles;

typedef PACKED_STRUCT _BLE_BATTERY_VALUE_SERVICE_HANDLE_
{
    uint16_t ble_battery_level_Service_handle;
    uint16_t ble_battery_level_char_handle;
}
ble_device_batt_val_service_handles;

typedef PACKED_STRUCT _BLE_DEVICE_TIME_SERVICE_HANDLE_
{
    uint16_t ble_dev_time_info_Service_handle;
    uint16_t ble_dev_time_info_char_handle;
}
ble_device_time_service_handles;


///  @brief this contains the handles for the differnt operations in peer.c 
typedef PACKED_STRUCT _BLE_PEER_DEVICE_STRUCT_
{
    uint8_t ble_peer_Device_inited;

    uint16_t conn_handle;
    
    ble_Gap_service_handles gap;
    ble_gatt_service_handles gatt;
    ble_device_info_service_handles dev_info;
    ble_device_batt_val_service_handles batt_val;
    ble_device_time_service_handles dev_time;

}
ble_peer_device_info_struct_t;

/// @todo

/// @brief
/// @param param
/// @param evt
void ble_peer_Device_indication_handler(ble_gattc_evt_t const *evt);

/// @brief
/// @param param
/// @param evt
void ble_peer_Device_notification_handler(ble_gattc_evt_t const *evt);

#endif