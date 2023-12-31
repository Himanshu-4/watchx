#include "ble_peer_info.h"

#include "ble_gatt_client.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/// deifne some services that are used in some general info from iphone

#define BLE_UUID_SERVICE_DEVICE_INFO 0x180A
#define BLE_UUID_CHAR_MANUFACTURER_NAME 0x2A29
#define BLE_UUID_CHAR_MODEL_NUMBER 0x2A24

#define BLE_UUID_SERVICE_BATTERY 0x180F
#define BLE_UUID_CHAR_BATTERY_LEVEL 0x2A19

#define BLE_UUID_SERVICE_CURRENT_TIME 0x1805
#define BLE_UUID_CHAR_CURRENT_TIME 0x2A2B
#define BLE_UUID_CHAR_LOCAL_TIME 0x2A0F

/// @brief this contains the
static ble_peer_device_info_struct_t peer_dev_info;

/// @brief this is to init the ble peer device info
/// @param conn_handle
/// @return succ/failure of the function
uint32_t ble_peer_device_init(uint16_t conn_handle)
{

    uint32_t err = 0;
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    /// GAP service
    // discover some services
    ble_service_struct_t ble_gap_srvc =
        {
            .ble_service.uuid.type = BLE_UUID_TYPE_BLE,
            .ble_service.uuid.uuid = BLE_UUID_GAP,
        };
    ble_char_struct_t gap_char_dev_name =
        {
            .characterstic.uuid.type = BLE_UUID_TYPE_BLE,
            .characterstic.uuid.uuid = BLE_UUID_GAP_CHARACTERISTIC_DEVICE_NAME};
    ble_char_struct_t gap_char_dev_app =
        {
            .characterstic.uuid.type = BLE_UUID_TYPE_BLE,
            .characterstic.uuid.uuid = BLE_UUID_GAP_CHARACTERISTIC_APPEARANCE};

    err = gatt_client_discover_service(conn_handle, &ble_gap_srvc);
    NRF_ASSERT(err);
    // NRF_LOG_INFO("s %x,%x,%x,%x",
    //              ble_gap_srvc.ble_service.uuid.uuid,
    //              ble_gap_srvc.ble_service.uuid.type,
    //              ble_gap_srvc.ble_service.handle_range.start_handle,
    //              ble_gap_srvc.ble_service.handle_range.end_handle);

    err = gatt_client_discover_chars(conn_handle, &ble_gap_srvc, &gap_char_dev_name);
    NRF_ASSERT(err);
    // NRF_LOG_INFO("c %x,%d,%d,%d,%d,%d",
    //              gap_char_dev_name.characterstic.uuid.uuid,
    //              gap_char_dev_name.characterstic.uuid.type,
    //              gap_char_dev_name.characterstic.char_props.indicate,
    //              gap_char_dev_name.characterstic.char_props.read,
    //              gap_char_dev_name.characterstic.char_props.write,
    //              gap_char_dev_name.characterstic.char_props.notify);

    err = gatt_client_discover_chars(conn_handle, &ble_gap_srvc, &gap_char_dev_app);
    NRF_ASSERT(err);
    // NRF_LOG_INFO("c %x,%d,%d,%d,%d,%d",
    //              gap_char_dev_app.characterstic.uuid.uuid,
    //              gap_char_dev_app.characterstic.uuid.type,
    //              gap_char_dev_app.characterstic.char_props.indicate,
    //              gap_char_dev_app.characterstic.char_props.read,
    //              gap_char_dev_app.characterstic.char_props.write,
    //              gap_char_dev_app.characterstic.char_props.notify);

    peer_dev_info.gap.ble_gap_device_name_char_handle = gap_char_dev_name.characterstic.handle_value;
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    /// GATT service
    ble_service_struct_t ble_gatt_srvc =
        {
            .ble_service.uuid.type = BLE_UUID_TYPE_BLE,
            .ble_service.uuid.uuid = BLE_UUID_GATT};
    ble_char_struct_t gatt_char_srvc_change =
        {
            .characterstic.uuid.type = BLE_UUID_TYPE_BLE,
            .characterstic.uuid.uuid = BLE_UUID_GATT_CHARACTERISTIC_SERVICE_CHANGED};
    ble_char_desc_struct_t char_srvc_change_cccd = // properties are only indication enabled
        {
            .descriptor.uuid.type = BLE_UUID_TYPE_BLE,
            .descriptor.uuid.uuid = BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG,
        };

    err = gatt_client_discover_service(conn_handle, &ble_gatt_srvc);
    NRF_ASSERT(err);
    // NRF_LOG_INFO("s %x,%x,%x,%x",
    //              ble_gatt_srvc.ble_service.uuid.uuid,
    //              ble_gatt_srvc.ble_service.uuid.type,
    //              ble_gatt_srvc.ble_service.handle_range.start_handle,
    //              ble_gatt_srvc.ble_service.handle_range.end_handle);

    err = gatt_client_discover_chars(conn_handle, &ble_gatt_srvc, &gatt_char_srvc_change);
    NRF_ASSERT(err);
    // NRF_LOG_INFO("c %x,%d,%d,%d,%d,%d",
    //              gatt_char_srvc_change.characterstic.uuid.uuid,
    //              gatt_char_srvc_change.characterstic.uuid.type,
    //              gatt_char_srvc_change.characterstic.char_props.indicate,
    //              gatt_char_srvc_change.characterstic.char_props.read,
    //              gatt_char_srvc_change.characterstic.char_props.write,
    //              gatt_char_srvc_change.characterstic.char_props.notify);

    // discover the char descriptor
    err = gatt_client_discover_char_desc(conn_handle, &gatt_char_srvc_change, &char_srvc_change_cccd);
    NRF_ASSERT(err);

    uint16_t notifval = INDICATION_ENABLE;
    /// try to write the notification
    err = gattc_client_char_desc_write(conn_handle, &char_srvc_change_cccd, u8_ptr & notifval, sizeof(notifval));
    NRF_ASSERT(err);

    peer_dev_info.gatt.ble_Gatt_srvc_cgd_char_handle = gatt_char_srvc_change.characterstic.handle_value;
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    /// DEVICE Info service
    ble_service_struct_t ble_device_info_srvc =
        {
            .ble_service.uuid.type = BLE_UUID_TYPE_BLE,
            .ble_service.uuid.uuid = BLE_UUID_SERVICE_DEVICE_INFO};
    ble_char_struct_t dev_info_manufacturer_name_char =
        {
            .characterstic.uuid.type = BLE_UUID_TYPE_BLE,
            .characterstic.uuid.uuid = BLE_UUID_CHAR_MANUFACTURER_NAME};
    ble_char_struct_t dev_info_manufacturer_model_char =
        {
            .characterstic.uuid.type = BLE_UUID_TYPE_BLE,
            .characterstic.uuid.uuid = BLE_UUID_CHAR_MODEL_NUMBER};

    err = gatt_client_discover_service(conn_handle, &ble_device_info_srvc);
    NRF_ASSERT(err);
    // NRF_LOG_INFO("s %x,%x,%x,%x",
    //              ble_device_info_srvc.ble_service.uuid.uuid,
    //              ble_device_info_srvc.ble_service.uuid.type,
    //              ble_device_info_srvc.ble_service.handle_range.start_handle,
    //              ble_device_info_srvc.ble_service.handle_range.end_handle);

    err = gatt_client_discover_chars(conn_handle, &ble_device_info_srvc, &dev_info_manufacturer_name_char);
    NRF_ASSERT(err);
    // NRF_LOG_INFO("c %x,%d,%d,%d,%d,%d",
    //              dev_info_manufacturer_name_char.characterstic.uuid.uuid,
    //              dev_info_manufacturer_name_char.characterstic.uuid.type,
    //              dev_info_manufacturer_name_char.characterstic.char_props.indicate,
    //              dev_info_manufacturer_name_char.characterstic.char_props.read,
    //              dev_info_manufacturer_name_char.characterstic.char_props.write,
    //              dev_info_manufacturer_name_char.characterstic.char_props.notify);

    err = gatt_client_discover_chars(conn_handle, &ble_device_info_srvc, &dev_info_manufacturer_model_char);
    NRF_ASSERT(err);
    // NRF_LOG_INFO("c %x,%d,%d,%d,%d,%d",
    //              dev_info_manufacturer_model_char.characterstic.uuid.uuid,
    //              dev_info_manufacturer_model_char.characterstic.uuid.type,
    //              dev_info_manufacturer_model_char.characterstic.char_props.indicate,
    //              dev_info_manufacturer_model_char.characterstic.char_props.read,
    //              dev_info_manufacturer_model_char.characterstic.char_props.write,
    //              dev_info_manufacturer_model_char.characterstic.char_props.notify);

    peer_dev_info.dev_info.ble_gatt_device_info_srvc_char_handle = dev_info_manufacturer_name_char.characterstic.handle_value;
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    /// battey  service
    ble_service_struct_t ble_battery_srvc =
        {
            .ble_service.uuid.type = BLE_UUID_TYPE_BLE,
            .ble_service.uuid.uuid = BLE_UUID_SERVICE_BATTERY};
    ble_char_struct_t batt_info_batt_level_char =
        {
            .characterstic.uuid.type = BLE_UUID_TYPE_BLE,
            .characterstic.uuid.uuid = BLE_UUID_CHAR_BATTERY_LEVEL};
    ble_char_desc_struct_t batt_level_char_cccd = // properties are only indication enabled
        {
            .descriptor.uuid.type = BLE_UUID_TYPE_BLE,
            .descriptor.uuid.uuid = BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG,
        };

    err = gatt_client_discover_service(conn_handle, &ble_battery_srvc);
    NRF_ASSERT(err);
    // NRF_LOG_INFO("s %x,%x,%x,%x",
    //              ble_battery_srvc.ble_service.uuid.uuid,
    //              ble_battery_srvc.ble_service.uuid.type,
    //              ble_battery_srvc.ble_service.handle_range.start_handle,
    //              ble_battery_srvc.ble_service.handle_range.end_handle);

    err = gatt_client_discover_chars(conn_handle, &ble_battery_srvc, &batt_info_batt_level_char);
    NRF_ASSERT(err);
    // NRF_LOG_INFO("c %x,%d,%d,%d,%d,%d",
    //              batt_info_batt_level_char.characterstic.uuid.uuid,
    //              batt_info_batt_level_char.characterstic.uuid.type,
    //              batt_info_batt_level_char.characterstic.char_props.indicate,
    //              batt_info_batt_level_char.characterstic.char_props.read,
    //              batt_info_batt_level_char.characterstic.char_props.write,
    //              batt_info_batt_level_char.characterstic.char_props.notify);

    // discover the char descriptor
    err = gatt_client_discover_char_desc(conn_handle, &batt_info_batt_level_char, &batt_level_char_cccd);
    NRF_ASSERT(err);

    uint16_t b_notifval = NOTIFICATION_ENABLE;
    /// try to write the notification
    err = gattc_client_char_desc_write(conn_handle, &batt_level_char_cccd, u8_ptr & b_notifval, sizeof(notifval));
    NRF_ASSERT(err);

    // // read the descriptoer value
    // err = gattc_client_char_desc_read(conn_handle, &batt_level_char_cccd, u8_ptr & b_notifval, sizeof(notifval));
    // NRF_ASSERT(err);
    peer_dev_info.batt_val.ble_battery_level_char_handle = batt_info_batt_level_char.characterstic.handle_value;

    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    /// current time service
    ble_service_struct_t ble_current_time_srvc =
        {
            .ble_service.uuid.type = BLE_UUID_TYPE_BLE,
            .ble_service.uuid.uuid = BLE_UUID_SERVICE_CURRENT_TIME};
    ble_char_struct_t current_time_value_char =
        {
            .characterstic.uuid.type = BLE_UUID_TYPE_BLE,
            .characterstic.uuid.uuid = BLE_UUID_CHAR_CURRENT_TIME};
    ble_char_desc_struct_t current_time_value_cccd = // properties are only indication enabled
        {
            .descriptor.uuid.type = BLE_UUID_TYPE_BLE,
            .descriptor.uuid.uuid = BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG,
        };
    ble_char_struct_t local_time_value_char =
        {
            .characterstic.uuid.type = BLE_UUID_TYPE_BLE,
            .characterstic.uuid.uuid = BLE_UUID_CHAR_LOCAL_TIME};

    err = gatt_client_discover_service(conn_handle, &ble_current_time_srvc);
    NRF_ASSERT(err);
    // NRF_LOG_INFO("s %x,%x,%x,%x",
    //              ble_current_time_srvc.ble_service.uuid.uuid,
    //              ble_current_time_srvc.ble_service.uuid.type,
    //              ble_current_time_srvc.ble_service.handle_range.start_handle,
    //              ble_current_time_srvc.ble_service.handle_range.end_handle);

    err = gatt_client_discover_chars(conn_handle, &ble_current_time_srvc, &current_time_value_char);
    NRF_ASSERT(err);
    // NRF_LOG_INFO("c %x,%d,%d,%d,%d,%d",
    //              current_time_value_char.characterstic.uuid.uuid,
    //              current_time_value_char.characterstic.uuid.type,
    //              current_time_value_char.characterstic.char_props.indicate,
    //              current_time_value_char.characterstic.char_props.read,
    //              current_time_value_char.characterstic.char_props.write,
    //              current_time_value_char.characterstic.char_props.notify);

    err = gatt_client_discover_chars(conn_handle, &ble_current_time_srvc, &local_time_value_char);
    NRF_ASSERT(err);
    // NRF_LOG_INFO("c %x,%d,%d,%d,%d,%d",
    //              local_time_value_char.characterstic.uuid.uuid,
    //              local_time_value_char.characterstic.uuid.type,
    //              local_time_value_char.characterstic.char_props.indicate,
    //              local_time_value_char.characterstic.char_props.read,
    //              local_time_value_char.characterstic.char_props.write,
    //              local_time_value_char.characterstic.char_props.notify);

    // discover the char descriptor
    err = gatt_client_discover_char_desc(conn_handle, &current_time_value_char, &current_time_value_cccd);
    NRF_ASSERT(err);

    uint16_t t_notifval = NOTIFICATION_ENABLE;
    /// try to write the notification
    err = gattc_client_char_desc_write(conn_handle, &current_time_value_cccd, u8_ptr & t_notifval, sizeof(notifval));
    NRF_ASSERT(err);

    peer_dev_info.dev_time.ble_dev_time_info_char_handle = current_time_value_char.characterstic.handle_value;
    // // read the descriptoer value
    // err = gattc_client_char_desc_read(conn_handle, &current_time_value_cccd, u8_ptr & t_notifval, sizeof(notifval));
    // NRF_ASSERT(err);

    // NRF_LOG_INFO("t-%d", t_notifval);

    /// discover the chars

    peer_dev_info.ble_peer_Device_inited = BLE_PPER_DEV_INITED;
    peer_dev_info.conn_handle = conn_handle;

    return err;
}

/// @brief this is to deinit the ble peer device info
/// @return succ/failure of function
uint32_t ble_peer_device_info_deinit()
{
    memcpy(&peer_dev_info, 0, sizeof(peer_dev_info));
    peer_dev_info.conn_handle = BLE_CONN_HANDLE_INVALID;
    return nrf_OK;
}

/// @brief to get the time information from the iphone
/// @param conn_handle
/// @param time
/// @return succ/failure of the fun
uint32_t ble_peer_get_Time_info(kernel_time_struct_t *time)
{
}

/// @brief to get the date information from the iphone
/// @param conn_handle
/// @param date
/// @return succ/failure
uint32_t ble_peer_get_date_info(kernel_date_struct_t *date)
{
}

/// @brief to get the battery information from the iphone
/// @param conn_handle
/// @return succ/failure of fun
uint8_t ble_peer_get_battery_info(void)
{
    if (peer_dev_info.ble_peer_Device_inited != BLE_PPER_DEV_INITED)
    {
        NRF_LOG_ERROR("peer not inited");
        return 0;
    }
    uint8_t soc = 0;
    ble_char_struct_t temp = {.characterstic.handle_value = peer_dev_info.batt_val.ble_battery_level_char_handle};
    uint32_t ret = 0;
    ret = gatt_client_char_read(peer_dev_info.conn_handle, &temp, &soc, 1);
    if (ret != nrf_OK)
    {
        NRF_LOG_ERROR("batt reading %d", ret);
        return 0;
    }

    return soc;
}

/// @brief to get the device name of the bluetooth
/// @param conn_handle
/// @param device_name
/// @return succ/failure of function
uint32_t ble_peer_get_device_name(char *device_name, uint16_t size)
{
    if (peer_dev_info.ble_peer_Device_inited != BLE_PPER_DEV_INITED)
    {
        return nrf_ERR_INVALID_PARAM;
    }

    ble_char_struct_t temp = {.characterstic.handle_value = peer_dev_info.gap.ble_gap_device_name_char_handle};
    return gatt_client_char_read(peer_dev_info.conn_handle, &temp, u8_ptr device_name, size);
}

/// @brief to get the servie changed indication notification
/// @param conn_handle
/// @return service changed indication
uint8_t ble_peer_get_service_change_ind(void)
{
}

/// @brief Device indication handler
/// @param param
/// @param evt
void ble_peer_Device_indication_handler(ble_gattc_evt_t const *evt)
{
    /// handle the gatt service change indication
    if (evt->params.hvx.handle == peer_dev_info.gatt.ble_Gatt_srvc_cgd_char_handle)
    {
        /* code */
        NRF_LOG_INFO("srvc chgd %d %d", evt->params.hvx.len, evt->params.hvx.data[0]);
    }
}

/// @brief Device notification handler
/// @param param
/// @param evt
bool ble_peer_Device_notification_handler(ble_gattc_evt_t const *evt)
{
    /// handle the notification from time and batt profile
    if (evt->params.hvx.handle == peer_dev_info.batt_val.ble_battery_level_char_handle)
    {
        NRF_LOG_INFO("batt len %d, %d", evt->params.hvx.len, evt->params.hvx.data[0]);
    }
    else if (evt->params.hvx.handle == peer_dev_info.dev_time.ble_dev_time_info_char_handle)
    {
        NRF_LOG_INFO("devtime %d , %d", evt->params.hvx.len, evt->params.hvx.data[0]);
    }

    return 0;
}