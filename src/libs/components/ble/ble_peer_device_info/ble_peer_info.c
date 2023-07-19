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
    NRF_ASSERT(err);
    NRF_LOG_INFO("p %d,%x,%x,%x,%x",err, gap_srv.ble_service.uuid.uuid,
    gap_srv.ble_service.uuid.type,
     gap_srv.ble_service.handle_range.start_handle,
    gap_srv.ble_service.handle_range.end_handle);

    err = gatt_client_discover_service(conn_handle, &gatt_srv);
    NRF_ASSERT(err);
    NRF_LOG_INFO("p %d,%x,%x,%x,%x",err, gatt_srv.ble_service.uuid.uuid,
    gatt_srv.ble_service.uuid.type,
     gatt_srv.ble_service.handle_range.start_handle,
    gatt_srv.ble_service.handle_range.end_handle);

    delay(100);

    /// discover the chars

    ble_char_struct_t device_name = 
    {
        .characterstic.uuid.type = BLE_UUID_TYPE_BLE,
        .characterstic.uuid.uuid = BLE_UUID_GAP_CHARACTERISTIC_DEVICE_NAME 
    };

    ble_char_struct_t device_appr = 
    {
        .characterstic.uuid.type = BLE_UUID_TYPE_BLE,
        .characterstic.uuid.uuid = BLE_UUID_GAP_CHARACTERISTIC_APPEARANCE
    };

    ble_char_struct_t dev_pp =
    {
        .characterstic.uuid.type = BLE_UUID_TYPE_BLE,
        .characterstic.uuid.uuid = BLE_UUID_GAP_CHARACTERISTIC_PPCP,
    };

    ble_char_struct_t srvc_chg =
    {
        .characterstic.uuid.type = BLE_UUID_TYPE_BLE,
        .characterstic.uuid.uuid =BLE_UUID_GATT_CHARACTERISTIC_SERVICE_CHANGED
    };



    

    err = gatt_client_discover_chars(conn_handle, &gap_srv, &device_name);
    NRF_ASSERT(err);
    NRF_LOG_INFO("c %x,%d,%d,%x,%x,%d",device_name.characterstic.uuid.uuid,
    device_name.characterstic.uuid.type, device_name.characterstic.handle_decl,
    device_name.characterstic.handle_value,
    device_name.characterstic.char_props.read,device_name.characterstic.char_props.write 
    );
    
    err = gatt_client_discover_chars(conn_handle, &gap_srv, &device_appr);
    NRF_ASSERT(err);
    NRF_LOG_INFO("c %x,%d,%d,%x,%x,%d",device_appr.characterstic.uuid.uuid,
    device_appr.characterstic.uuid.type, device_appr.characterstic.handle_decl,
    device_appr.characterstic.handle_value,
    device_appr.characterstic.char_props.read,device_appr.characterstic.char_props.write 
    );

    
    err = gatt_client_discover_chars(conn_handle, &gap_srv, &dev_pp);
    NRF_ASSERT(err);
    
    err = gatt_client_discover_chars(conn_handle, &gatt_srv, &srvc_chg);
    NRF_ASSERT(err);
    NRF_LOG_INFO("s %x,%d,%d,%d,%d,%d",srvc_chg.characterstic.uuid.uuid ,
    srvc_chg.characterstic.uuid.type, srvc_chg.characterstic.char_props.indicate, 
    srvc_chg.characterstic.char_props.read, srvc_chg.characterstic.char_props.write,
    srvc_chg.characterstic.char_props.notify);
    
    return err;
}