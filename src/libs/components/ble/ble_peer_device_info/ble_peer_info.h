#ifndef _BLE_PEER_INFO_H
#define _BLE_PEER_INFO_H

#include "ble_softdevice_init.h"

// include the time library
#include "time_manager/kernel_time.h"

#define BLE_PPER_DEV_INITED 0x02
#define BLE_PEER_DEV_DEINITED 0x00

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
/// @param time
/// @return succ/failure of the fun
uint32_t ble_peer_get_Time_info(kernel_time_struct_t *time);

/// @brief to get the date information from the iphone
/// @param date
/// @return succ/failure
uint32_t ble_peer_get_date_info(kernel_date_struct_t *date);

/// @brief to get the battery information from the iphone
/// @param batt_soc
/// @return succ/failure of fun
uint32_t ble_peer_get_battery_info( uint8_t *batt_soc);

/// @brief to get the device name of the bluetooth
/// @param device_name
/// @param size (mem should be given by the user )
/// @return succ/failure of function
uint32_t ble_peer_get_device_name(char *device_name, uint16_t size);

#define BLE_SERVICE_CHANGED 1
#define BLE_SERVICE_NOT_CHANGED 0

/// @brief to get the servie changed indication notification
/// @return service changed indication
uint8_t ble_peer_get_service_change_ind(void);

typedef PACKED_STRUCT _BLE_GAP_SERVICE_HANDLES_
{
    uint16_t ble_gap_device_name_char_handle;
}
ble_Gap_service_handles;

typedef PACKED_STRUCT _BLE_GATT_SERVICE_HANDLES_
{
    uint16_t ble_Gatt_srvc_cgd_char_handle;
}
ble_gatt_service_handles;

typedef PACKED_STRUCT _BLE_DEVICE_INFORMATION_SERVICE_HANDLE_
{
    uint16_t ble_gatt_device_info_srvc_char_handle;
}
ble_device_info_service_handles;

typedef PACKED_STRUCT _BLE_BATTERY_VALUE_SERVICE_HANDLE_
{
    uint16_t ble_battery_level_char_handle;
}
ble_device_batt_val_service_handles;

typedef PACKED_STRUCT _BLE_DEVICE_TIME_SERVICE_HANDLE_
{
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
bool ble_peer_Device_notification_handler(ble_gattc_evt_t const *evt);

#endif