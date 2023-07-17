#include "ble_peer_info.h"

#include "ble_gatt_client.h"


/// @brief this is to init the ble peer device info 
/// @param conn_handle
/// @return succ/failure of the function 
uint32_t  ble_peer_device_init(uint16_t conn_handle)
{

    uint32_t err =0;
     // discover some services 
    ble_service_struct_t gap_srv =
    {
        .ble_service.uuid.type = BLE_UUID_TYPE_BLE,
        .ble_service.uuid.uuid = BLE_UUID_GAP,
    };

    ble_service_struct_t gatt_srv = 
    {
        .ble_service.uuid.type = BLE_UUID_TYPE_BLE,
        .ble_service.uuid.uuid = BLE_UUID_GATT
    };

    

    err = gatt_client_discover_service(conn_handle, &gap_srv);
    NRF_LOG_INFO("re %d,ha %x,%x",err, gap_srv.ble_service.handle_range.start_handle,
    gap_srv.ble_service.handle_range.end_handle);

}