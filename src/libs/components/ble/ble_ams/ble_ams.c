#include "ble_ams.h"

/// include the gatt client
#include "ble_gatt_client.h"

#include "memory_manager/kernel_mem_manager.h"

#include "string.h"

//// create a kernel memory instance to hold the data from the notification handler into this memory
/// this is useful because it can give us compile time memory consumption , which in our case is useful because now run time
///// consumption can be minimized

/** @brief 128-bit service UUID for the Apple Media Service. */
static ble_uuid128_t const ble_apple_media_service_uuid128 =
    {
        .uuid128 =
            {
                //  89 D3 50 2B 0F 36 43 3A 8E F4 C5 02 AD 55 F8 DC
                0xDC, 0xF8, 0x55, 0xAD, 0x02, 0xc5, 0xf4, 0x8e,
                0x3a, 0x43, 0x36, 0x0f, 0x2b, 0x50, 0xd3, 0x89}};

/** @brief 128-bit AMS remote command  characteristic UUID. */
static ble_uuid128_t const ble_ams_remote_command_char_uuid128 =
    {
        .uuid128 =
            {
                //  9B 3C 81 D8 57 B1 4A 8A B8 DF 0E 56 F7 CA 51 C2 (writeable, notifiable)
                0xc2, 0x51, 0xca, 0xf7, 0x56, 0x0e, 0xdf, 0xb8,
                0x8a, 0x4a, 0xb1, 0x57, 0xd8, 0x81, 0x3c, 0x9b}};

/**@brief 128-bit ams entity update char characteristics  UUID. */
static ble_uuid128_t const ble_ams_entity_update_char_uuid128 =
    {
        {// 2F 7C AB CE 80 8D 41 1F 9A 0C BB 92 BA 96 C1 02(writeable with response, notifiable)
         0x02, 0xc1, 0x96, 0xBA, 0x92, 0xBB, 0x0c, 0x9A,
         0x1F, 0x41, 0x8D, 0x80, 0xce, 0xab, 0x7c, 0x2f}};

/**@brief 128-bit ams entity attribute characteristics  UUID. */
static ble_uuid128_t const ble_ams_entity_attribute_char_uuid128 =
    {
        {// C6 B2 F3 8C 23 AB 46 D8 A6 AB A3 A8 70 BB D5 D7 (readable, writeable)
         0xd7, 0xd5, 0xbb, 0x70, 0xa8, 0xa3, 0xab, 0xa6,
         0xd8, 0x46, 0xab, 0x23, 0x8c, 0xf3, 0xb2, 0xc6}};

/////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////// defining the global struct

static volatile ble_ams_struct_t ble_ams_handler = {0};

////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static void ble_ams_service_init()
{

    ///////// first make sure that all the data is 0 for the services
    memset((uint8_t *)&ble_ams_handler.ams_srvc_char, 0, sizeof(ble_ams_services_struct_t));

    uint32_t err_code = 0;
    err_code = sd_ble_uuid_vs_add(&ble_apple_media_service_uuid128, (uint8_t *)&ble_ams_handler.ams_srvc_char.ams_service.ble_service.uuid.type);
    NRF_ASSERT(err_code);

    err_code = sd_ble_uuid_vs_add(&ble_ams_remote_command_char_uuid128, (uint8_t *)&ble_ams_handler.ams_srvc_char.ams_control_point_cahr.characterstic.uuid.type);
    NRF_ASSERT(err_code);

    err_code = sd_ble_uuid_vs_add(&ble_ams_entity_update_char_uuid128, (uint8_t *)&ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.uuid.type);
    NRF_ASSERT(err_code);

    err_code = sd_ble_uuid_vs_add(&ble_ams_entity_attribute_char_uuid128, (uint8_t *)&ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.characterstic.uuid.type);
    NRF_ASSERT(err_code);

    // NRF_LOG_INFO("ams_type %d,%d,%d,%d",ble_ams_handler.ams_srvc_char.ams_service.uuid.type,
    // ble_ams_handler.ams_srvc_char.ams_control_point_cahr.uuid.type , ble_ams_handler.ams_srvc_char.ams_entity_update_char.uuid.type ,
    // ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.uuid.type);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief this is the to be called when
/// @param
void ble_ams_pre_init(void)
{
    /// init the ble ams srvice
    ble_ams_service_init();
}

/// @brief this is to init the ams at a coonection event
/// @param  void
uint32_t ble_ams_init(uint16_t conn_handle)
{
    NRF_LOG_INFO("ams init");

    if ((conn_handle == BLE_CONN_HANDLE_INVALID) || (conn_handle == 0))
        return nrf_ERR_INVALID_PARAM;

    uint32_t err = 0;
    /// it is asssumed that the gatt client module is inited and working succesfully
    ble_ams_handler.ble_ams_instance_inited = BLE_AMS_INSTANCE_DEINITED;

    //// search for the ams service
    err = gatt_client_discover_service(conn_handle, (ble_service_struct_t *)&ble_ams_handler.ams_srvc_char.ams_service);
    NRF_ASSERT(err);

    // serach the service if present
    if (err == nrf_OK)
    {
        /// discover control point char and desc
        err = gatt_client_discover_chars(conn_handle, (ble_service_struct_t *)&ble_ams_handler.ams_srvc_char.ams_service, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_control_point_cahr);
        NRF_ASSERT(err);

        err = gatt_client_discover_char_desc(conn_handle, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_control_point_cahr, (ble_char_desc_struct_t *)&ble_ams_handler.ams_srvc_char.ams_control_point_desc);
        NRF_ASSERT(err);

        /// disconver entity update char and desc
        err = gatt_client_discover_chars(conn_handle, (ble_service_struct_t *)&ble_ams_handler.ams_srvc_char.ams_service, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_update_char);
        NRF_ASSERT(err);

        err = gatt_client_discover_char_desc(conn_handle, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_update_char, (ble_char_desc_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_update_desc);
        NRF_ASSERT(err);

        /// discover the entity attribute  char and desc
        err = gatt_client_discover_chars(conn_handle, (ble_service_struct_t *)&ble_ams_handler.ams_srvc_char.ams_service, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_attribute_char);
        NRF_ASSERT(err);

        err = gatt_client_discover_char_desc(conn_handle, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_attribute_char, (ble_char_desc_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_attribute_desc);
        NRF_ASSERT(err);

        ble_ams_handler.ble_ams_instance_inited = BLE_AMS_INSTANCE_INITED;
    }

    /// make all the data to zero

    return nrf_OK;
}

/// @brief this is to deinit the ams at disconnection, ble disbale
/// @param  void
uint32_t ble_ams_deinit(void)
{
    NRF_LOG_WARNING("ams deinit");

    return nrf_OK;
}

/// @brief this function gives the attribute string name
/// @param attributeindex
/// @return string containg attribute , NULL if none
char *ble_ams_get_attribute_name(ble_ams_attribute_name index)
{
    char *string = NULL_STRING;

    switch (index)
    {
    case ble_ams_attribute_index_mediaplayer:
    {
        // string = /// the pointer to the stored media player name
    }
    break;

    case ble_ams_attribute_index_artist_name:
    {
    }
    break;

    case ble_ams_attribute_index_track_name:
    {
    }
    break;

    case ble_ams_attribute_index_album_name:
    {
    }
    break;

    case ble_ams_attribute_index_misc:
    {
    }
    break;

    default:
        /// return the null string
        string = NULL_STRING;
        break;
    }

    return string;
}

/// @brief ble_ams_execute cmd is the function used to execute a specific cmd in media player
/// @param cmd_id
/// @return succ/Failure of the cmd
uint32_t ble_ams_execute_cmd(ble_ams_media_cmds cmd_id);

/// @brief get the playback state @ref _BLE_AMS_PLAYBACK_STATE_
/// @param  void
/// @return return the playback state
uint8_t ble_ams_get_playback_State(void);

/// @brief get the playback rate in the integer fromat
/// @param void
/// @return the float value of the playback rate 1.2x 1.5x 2.3x etc
float ble_ams_get_playbackrate(void);

/// @brief returns the volume of the media in percentage
/// @param  void
/// @return return between 0 to 100 , value should be  consider in %
uint8_t ble_ams_get_volume(void);

/// @brief geive the elapsed time in seconds
/// @param  void
/// @return returns the elpased time in seconds
uint32_t ble_ams_get_elapsed_time(void);

/// @brief get the total time of the track
/// @param  void
/// @return returns the track time in seconds
uint32_t ble_ams_get_track_time(void);

/// @brief get the q attribute like q index , repeat mode @ref ble_ams_q_att_data
/// @param  ble_ams_q_att_data
/// @return returns cmd specific  @ref _BLE_AMS_SHUFFLE_MODE_  @ref _BLE_AMS_REPEAT_MODE_
uint32_t ble_ams_get_Queue_attribute(ble_ams_q_att_data index);

/// @brief this is the apple media service handler where
/// @param param
/// @param ble_evt
void ble_ams_client_event_handler(void *param, ble_gattc_evt_t *evt)
{
}
