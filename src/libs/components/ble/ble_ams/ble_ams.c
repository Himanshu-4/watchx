#include "ble_ams.h"

/// include the gatt client
#include "ble_gatt_client.h"

#include "memory_manager/kernel_mem_manager.h"

#include "string.h"

//// define the enum for the gatt notification callback in the entity update
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//// macros

#define BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE 3

#define BLE_AMS_SERVICE_16BIT_UUID 0x502B

#define BLE_AMS_CHAR_REMOTE_CMD_16BIT_UUID 0x81d8
#define BLE_AMS_CHAR_ENTITY_UPDATE_16BIT_UUID 0xabce
#define BLE_AMS_CHAR_ENTITY_ATTRIBUTE_16BIT_UUID 0xf38c

enum __ENTITY_UPDATE_CHAR_NOTIF_CB_FORMAT_
{
    ble_ams_index_entity_id,
    ble_ams_index_att_id,
    ble_ams_index_entity_update_flag,
    ble_ams_index_data_value, // this data value can be in variable size
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//// in the global vars to store the volume, duration, etc
static volatile float player_volume = 0;

static volatile uint8_t playbackstate;

static volatile float track_duration = 1, elapsed_time = 0, playbackrate;

static volatile uint8_t q_att_index = 0, att_total_items_q = 0, q_att_shuffle_mode, q_att_repeat_mode;

/////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////// defining the global struct

static volatile ble_ams_struct_t ble_ams_handler = {0};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// static function here ////////////////////////////////////////////////////////////

static void ble_ams_seperate_playbackinfo(const char *str)
{
    uint16_t index = 0;
    //// the loop will be run 3 times as there are 3 commas
    uint16_t lenofstr = strlen(str);

    for (int i = 0; i < 3; i++)
    {
        char temp[20] = {0};
        int temp_index = 0;

        while (str[index] != ',' && index < lenofstr)
        {
            temp[temp_index++] = str[index++];
        }
        index++;
        if (i == 0) // first is the playback state
        {
            playbackstate = atoi(temp);
        }
        else if (i == 1) /// after that is the playback rate
        {
            playbackrate = atof(temp);
        }
        else if (i == 2) ///// after that is the elapsed time
        {
            elapsed_time = atof(temp);
        }
    }
}

/// @brief this function is used to init the ble apple media services
static void ble_ams_service_init()
{

    /** @brief 128-bit service UUID for the Apple Media Service. */
    const ble_uuid128_t ble_apple_media_service_uuid128 =
        {
            .uuid128 =
                {
                    //  89 D3 50 2B 0F 36 43 3A 8E F4 C5 02 AD 55 F8 DC
                    0xDC, 0xF8, 0x55, 0xAD, 0x02, 0xc5, 0xf4, 0x8e,
                    0x3a, 0x43, 0x36, 0x0f, 0x2b, 0x50, 0xd3, 0x89}};

    /** @brief 128-bit AMS remote command  characteristic UUID. */
    const ble_uuid128_t ble_ams_remote_command_char_uuid128 =
        {
            .uuid128 =
                {
                    //  9B 3C 81 D8 57 B1 4A 8A B8 DF 0E 56 F7 CA 51 C2 (writeable, notifiable)
                    0xc2, 0x51, 0xca, 0xf7, 0x56, 0x0e, 0xdf, 0xb8,
                    0x8a, 0x4a, 0xb1, 0x57, 0xd8, 0x81, 0x3c, 0x9b}};

    /**@brief 128-bit ams entity update char characteristics  UUID. */
    const ble_uuid128_t ble_ams_entity_update_char_uuid128 =
        {
            {// 2F 7C AB CE 80 8D 41 1F 9A 0C BB 92 BA 96 C1 02(writeable with response, notifiable)
             0x02, 0xc1, 0x96, 0xBA, 0x92, 0xBB, 0x0c, 0x9A,
             0x1F, 0x41, 0x8D, 0x80, 0xce, 0xab, 0x7c, 0x2f}};

    /**@brief 128-bit ams entity attribute characteristics  UUID. */
    const ble_uuid128_t ble_ams_entity_attribute_char_uuid128 =
        {
            {// C6 B2 F3 8C 23 AB 46 D8 A6 AB A3 A8 70 BB D5 D7 (readable, writeable)
             0xd7, 0xd5, 0xbb, 0x70, 0xa8, 0xa3, 0xab, 0xa6,
             0xd8, 0x46, 0xab, 0x23, 0x8c, 0xf3, 0xb2, 0xc6}};

    ///////// first make sure that all the data is 0 for the services
    memset((uint8_t *)&ble_ams_handler.ams_srvc_char, 0, sizeof(ble_ams_services_struct_t));

    uint32_t err_code = 0;
    //// the apple media service id
    err_code = sd_ble_uuid_vs_add(&ble_apple_media_service_uuid128, (uint8_t *)&ble_ams_handler.ams_srvc_char.ams_service.ble_service.uuid.type);
    NRF_ASSERT(err_code);

    err_code = sd_ble_uuid_vs_add(&ble_ams_remote_command_char_uuid128, (uint8_t *)&ble_ams_handler.ams_srvc_char.ams_control_point_char.characterstic.uuid.type);
    NRF_ASSERT(err_code);

    err_code = sd_ble_uuid_vs_add(&ble_ams_entity_update_char_uuid128, (uint8_t *)&ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.uuid.type);
    NRF_ASSERT(err_code);

    err_code = sd_ble_uuid_vs_add(&ble_ams_entity_attribute_char_uuid128, (uint8_t *)&ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.characterstic.uuid.type);
    NRF_ASSERT(err_code);

    /// assignt the service and chars uuid
    ble_ams_handler.ams_srvc_char.ams_service.ble_service.uuid.uuid = BLE_AMS_SERVICE_16BIT_UUID;

    ble_ams_handler.ams_srvc_char.ams_control_point_char.characterstic.uuid.uuid = BLE_AMS_CHAR_REMOTE_CMD_16BIT_UUID;
    ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.uuid.uuid = BLE_AMS_CHAR_ENTITY_UPDATE_16BIT_UUID;
    ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.characterstic.uuid.uuid = BLE_AMS_CHAR_ENTITY_ATTRIBUTE_16BIT_UUID;

    //// assignt the descritptor
    ble_ams_handler.ams_srvc_char.ams_control_point_desc.descriptor.uuid.type = BLE_UUID_TYPE_BLE;
    ble_ams_handler.ams_srvc_char.ams_control_point_desc.descriptor.uuid.uuid = BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG;

    //// assignt the descritptor
    ble_ams_handler.ams_srvc_char.ams_entity_update_desc.descriptor.uuid.type = BLE_UUID_TYPE_BLE;
    ble_ams_handler.ams_srvc_char.ams_entity_update_desc.descriptor.uuid.uuid = BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG;

    ble_ams_handler.ams_srvc_char.ams_entity_attribute_desc.descriptor.uuid.type = BLE_UUID_TYPE_BLE;
    ble_ams_handler.ams_srvc_char.ams_entity_attribute_desc.descriptor.uuid.uuid = BLE_UUID_DESCRIPTOR_CHAR_EXT_PROP;

    // NRF_LOG_INFO("ams_type %d,%d,%d,%d",ble_ams_handler.ams_srvc_char.ams_service.ble_service.uuid.type,
    // ble_ams_handler.ams_srvc_char.ams_control_point_char.characterstic.uuid.type , ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.uuid.type ,
    // ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.characterstic.uuid.type);
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

//// create a kernel memory instance to hold the data from the notification handler into this memory
/// this is useful because it can give us compile time memory consumption , which in our case is useful because now run time
///// consumption can be minimized

KERNEL_MEM_INSTANTISE(ble_ams_mem_inst, ble_ams_mem_pool, BLE_AMS_MEM_SIZE, ble_ams_memory_mutex);

/// @brief this is to init the ams at a coonection event
/// @param  connection_handle
uint32_t ble_ams_init(uint16_t conn_handle)
{

    uint32_t err = 0;
    /// init the kernel memory here
    kernel_mem_init(&ble_ams_mem_inst, ble_ams_mem_pool, BLE_AMS_MEM_SIZE, &ble_ams_memory_mutex, BLE_AMS_MUTEX_TIMEOUT);

    if ((conn_handle == BLE_CONN_HANDLE_INVALID))
        return nrf_ERR_INVALID_PARAM;

    /// it is asssumed that the gatt client module is inited and working succesfully
    ble_ams_handler.ble_ams_instance_inited = BLE_AMS_INSTANCE_DEINITED;

    /// assign the conn handle
    ble_ams_handler.conn_handle = conn_handle;

    //// search for the ams service
    err = gatt_client_discover_service(conn_handle, (ble_service_struct_t *)&ble_ams_handler.ams_srvc_char.ams_service);
    NRF_ASSERT(err);

    // serach the service if present
    if (err != nrf_OK)
    {
        return nrf_ERR_OPERATION_FAILED;
    }

    /// discover control point char and desc
    err = gatt_client_discover_chars(conn_handle, (ble_service_struct_t *)&ble_ams_handler.ams_srvc_char.ams_service, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_control_point_char);
    NRF_ASSERT(err);
    // discover the client char config descriptor
    err = gatt_client_discover_char_desc(conn_handle, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_control_point_char, (ble_char_desc_struct_t *)&ble_ams_handler.ams_srvc_char.ams_control_point_desc);
    NRF_ASSERT(err);

    // NRF_LOG_INFO("c %x,%d, %d%d%d%d%d%d %x,%x",
    //              ble_ams_handler.ams_srvc_char.ams_control_point_char.characterstic.uuid.uuid,
    //              ble_ams_handler.ams_srvc_char.ams_control_point_char.characterstic.uuid.type,
    //              ble_ams_handler.ams_srvc_char.ams_control_point_char.characterstic.char_props.indicate,
    //              ble_ams_handler.ams_srvc_char.ams_control_point_char.characterstic.char_props.notify,
    //              ble_ams_handler.ams_srvc_char.ams_control_point_char.characterstic.char_props.read,
    //              ble_ams_handler.ams_srvc_char.ams_control_point_char.characterstic.char_props.write,
    //              ble_ams_handler.ams_srvc_char.ams_control_point_char.characterstic.char_props.write_wo_resp,
    //              ble_ams_handler.ams_srvc_char.ams_control_point_char.characterstic.char_props.auth_signed_wr,
    //              ble_ams_handler.ams_srvc_char.ams_control_point_char.characterstic.handle_decl,
    //              ble_ams_handler.ams_srvc_char.ams_control_point_char.characterstic.handle_value);

    /// disconver entity update char and desc
    err = gatt_client_discover_chars(conn_handle, (ble_service_struct_t *)&ble_ams_handler.ams_srvc_char.ams_service, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_update_char);
    NRF_ASSERT(err);

    err = gatt_client_discover_char_desc(conn_handle, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_update_char, (ble_char_desc_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_update_desc);
    NRF_ASSERT(err);
    // NRF_LOG_INFO("c %x,%d, %d%d%d%d%d%d %x,%x",
    //              ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.uuid.uuid,
    //              ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.uuid.type,
    //              ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.char_props.indicate,
    //              ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.char_props.notify,
    //              ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.char_props.read,
    //              ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.char_props.write,
    //              ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.char_props.write_wo_resp,
    //              ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.char_props.auth_signed_wr,
    //              ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.handle_decl,
    //              ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.handle_value);
    ///// discover the client char config descriptor
   
    /// discover the entity attribute  char and desc
    err = gatt_client_discover_chars(conn_handle, (ble_service_struct_t *)&ble_ams_handler.ams_srvc_char.ams_service, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_attribute_char);
    NRF_ASSERT(err);
    
    //// discover the char descriptor extended properties
    err = gatt_client_discover_char_desc(conn_handle, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_attribute_char, (ble_char_desc_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_attribute_desc);
    NRF_ASSERT(err);
    
    // NRF_LOG_INFO("c %x,%d, %d%d%d%d%d%d %x,%x",
    //              ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.characterstic.uuid.uuid,
    //              ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.characterstic.uuid.type,
    //              ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.characterstic.char_props.indicate,
    //              ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.characterstic.char_props.notify,
    //              ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.characterstic.char_props.read,
    //              ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.characterstic.char_props.write,
    //              ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.characterstic.char_props.write_wo_resp,
    //              ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.characterstic.char_props.auth_signed_wr,
    //              ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.characterstic.handle_decl,
    //              ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.characterstic.handle_value);

    const uint16_t notif_en_data = NOTIFICATION_ENABLE;
    /// now here suscribe for the notification for the gatt char
    /// suscribe the gatt notication of remote cmd and entity update char
    err = gattc_client_char_desc_write(conn_handle, (ble_char_desc_struct_t *)&ble_ams_handler.ams_srvc_char.ams_control_point_desc, u8_ptr & notif_en_data, sizeof(notif_en_data));
    NRF_ASSERT(err);
    err = gattc_client_char_desc_write(conn_handle, (ble_char_desc_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_update_desc, u8_ptr & notif_en_data, sizeof(notif_en_data));
    NRF_ASSERT(err);

    //// clear the cmd supported  values
    memset((uint8_t *)&ble_ams_handler.cmds, 0, sizeof(ble_ams_handler.cmds));

    /// suscribe for the differnt entity attributes

    //// subscribe all the attribute for player entity like playername ,player playbacinfo ,player volume
    const uint8_t entity_id_player[] = {ble_ams_entityid_player, ble_ams_player_attribute_name, ble_ams_player_attribute_playbackinfo, ble_ams_player_attribute_volume};

    //// subscribe all the attribute for the entity  queue like q properties
    const uint8_t entity_id_queue[] = {ble_ams_entityid_queue, ble_ams_queue_attribute_index, // give the q present index
                                       ble_ams_queue_attribute_byte_count, ble_ams_queue_attribute_shuffle_mode, ble_ams_queue_attribute_repeat_mode};

    ///// subscribe all the track attributes like name, duration , artist name
    const uint8_t entity_id_track[] = {ble_ams_entityid_Track, ble_ams_track_attribute_artist, ble_ams_track_attribute_album,
                                       ble_ams_track_attribute_title, ble_ams_track_attribute_duration};

    /////// write this to the entty update char
    err = gatt_client_char_write(conn_handle, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_update_char, CHAR_WRITE_WITH_RSP, entity_id_player, sizeof(entity_id_player));
    NRF_ASSERT(err);

    err = gatt_client_char_write(conn_handle, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_update_char, CHAR_WRITE_WITH_RSP, entity_id_queue, sizeof(entity_id_queue));
    NRF_ASSERT(err);

    err = gatt_client_char_write(conn_handle, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_update_char, CHAR_WRITE_WITH_RSP, entity_id_track, sizeof(entity_id_track));
    NRF_ASSERT(err);

    ble_ams_handler.ble_ams_instance_inited = BLE_AMS_INSTANCE_INITED;

    /// make all the data to zero
    return nrf_OK;
}

/// @brief this is to deinit the ams at disconnection, ble disbale
/// @param  void
uint32_t ble_ams_deinit(void)
{
    NRF_LOG_WARNING("ams deinit");

    //// clear the cmd supported  values
    memset((uint8_t *)&ble_ams_handler.cmds.ams_supp_cmds, 0, sizeof(ble_ams_handler.cmds.ams_supp_cmds));

    ble_ams_handler.ble_ams_instance_inited = BLE_AMS_INSTANCE_DEINITED;
    ble_ams_handler.conn_handle = BLE_CONN_HANDLE_INVALID;
    //// clear the
    return nrf_OK;
}

/// @brief this function gives the attribute string name
/// @param attributeindex
/// @return string containg attribute , NULL if none
char *ble_ams_get_attribute_name(ble_ams_attribute_name index)
{
    if (ble_ams_handler.ble_ams_instance_inited != BLE_AMS_INSTANCE_INITED)
    {
        return NULL;
    }
    if (index >= ble_ams_attribute_index_misc)
    {
        return NULL;
    }
    return (char *)kernel_mem_get_Data_ptr(&ble_ams_mem_inst, index);
}

/// @brief ble_ams_execute cmd is the function used to execute a specific cmd in media player
/// @param cmd_id
/// @return succ/Failure of the cmd
uint32_t ble_ams_execute_cmd(ble_ams_media_cmds cmd_id)
{
    if (ble_ams_handler.ble_ams_instance_inited != BLE_AMS_INSTANCE_INITED)
    {
        return nrf_ERR_INVALID_STATE;
    }

    if (cmd_id >= ble_ams_cmd_rfus)
    {
        return nrf_ERR_INVALID_PARAM;
    }
    if (ble_ams_handler.cmds.ams_supp_cmds[cmd_id] == 0)
    {
        return nrf_ERR_OPERATION_FAILED;
    }

    uint8_t err = 0;
    uint8_t cmd = cmd_id;
    /// we can execute the cmds
    err = gatt_client_char_write(ble_ams_handler.conn_handle, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_control_point_char, CHAR_WRITE_WITH_RSP, &cmd, 1);
    NRF_ASSERT(err);

    return nrf_OK;
}

/// @brief get the playback state @ref _BLE_AMS_PLAYBACK_STATE_
/// @param  playstate
/// @return succ/faliure
uint32_t ble_ams_get_playback_State(uint8_t *playstate)
{
    if (ble_ams_handler.ble_ams_instance_inited != BLE_AMS_INSTANCE_INITED)
    {
        return nrf_ERR_INVALID_STATE;
    }
    /// give the playback state
    *playstate = playbackstate;
    return nrf_OK;
}

/// @brief get the playback rate in the integer fromat  playback rate 1.2x 1.5x 2.3x etc
/// @param the playback rate in float value
/// @return succ/faliure
uint32_t ble_ams_get_playbackrate(float *rate)
{
    if (ble_ams_handler.ble_ams_instance_inited != BLE_AMS_INSTANCE_INITED)
    {
        return nrf_ERR_INVALID_STATE;
    }

    *rate = playbackrate;
    return nrf_OK;
}

/// @brief returns the volume of the media in percentage
/// @param  volume pointer
/// @return succ/filure
uint32_t ble_ams_get_volume(uint8_t *volume)
{
    if (ble_ams_handler.ble_ams_instance_inited != BLE_AMS_INSTANCE_INITED)
    {
        return nrf_ERR_INVALID_STATE;
    }

    *volume = (uint8_t)(player_volume * 100);
    return nrf_OK;
}

/// @brief geive the elapsed time in seconds
/// @param  void
/// @return returns the elpased time in seconds
uint32_t ble_ams_get_elapsed_time(uint32_t *time)
{
    if (ble_ams_handler.ble_ams_instance_inited != BLE_AMS_INSTANCE_INITED)
    {
        return nrf_ERR_INVALID_STATE;
    }

    *time = (uint32_t)elapsed_time;
    return nrf_OK;
}

/// @brief get the total time of the track
/// @param  void
/// @return returns the track time in seconds
uint32_t ble_ams_get_track_time(uint32_t *track_time)
{
    if (ble_ams_handler.ble_ams_instance_inited != BLE_AMS_INSTANCE_INITED)
    {
        return nrf_ERR_INVALID_STATE;
    }
    *track_time = (uint32_t)track_duration;
    return nrf_OK;
}

/// @brief get the q attribute like q index , repeat mode @ref ble_ams_q_att_data
/// @param  ble_ams_q_att_data
/// @param ble_ams_QUeue data
/// @return returns cmd specific  @ref _BLE_AMS_SHUFFLE_MODE_  @ref _BLE_AMS_REPEAT_MODE_
uint32_t ble_ams_get_Queue_attribute(ble_ams_q_att_data index, uint8_t *data)
{
    if (ble_ams_handler.ble_ams_instance_inited != BLE_AMS_INSTANCE_INITED)
    {
        return nrf_ERR_INVALID_STATE;
    }

    switch (index)
    {
    case ble_ams_queue_attribute_index:
    {
        *data = q_att_index;
    }
    break;

    case ble_ams_queue_attribute_byte_count:
    {
        *data = att_total_items_q;
    }
    break;

    case ble_ams_queue_attribute_shuffle_mode:
    {
        *data = q_att_shuffle_mode;
    }
    break;
    case ble_ams_queue_attribute_repeat_mode:
    {
        *data = q_att_repeat_mode;
    }
    break;

    default:
    {
        *data = 0;
    }
    break;
    }
    return nrf_OK;
}


/// @brief this function is used for debugg purpose and to print the apple media info 
/// @param  void
void ble_ams_print_media_info(void)
{
    /// print the strings first 
    delay(50);
    NRF_LOG_INFO("p %s",ble_ams_get_attribute_name(ble_ams_attribute_index_mediaplayer));
    delay(50);
    NRF_LOG_INFO("a %s",ble_ams_get_attribute_name(ble_ams_attribute_index_artist_name));
    delay(50);
    NRF_LOG_INFO("t %d",ble_ams_get_attribute_name(ble_ams_attribute_index_track_name));
    delay(50);
    NRF_LOG_INFO("a %s",ble_ams_get_attribute_name(ble_ams_attribute_index_album_name));

    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief this is the apple media service handler where
/// @param param
/// @param ble_evt
bool ble_ams_client_event_handler(ble_gattc_evt_t const *evt)
{
    /// match the connection handle
    if (evt->conn_handle != ble_ams_handler.conn_handle)
    {
        NRF_LOG_ERROR("invalid conn");
        return 0;
    }
    //// search for the char handle

    //// handle the remote cmd char
    if (evt->params.hvx.handle == ble_ams_handler.ams_srvc_char.ams_control_point_char.characterstic.handle_value)
    {
        NRF_LOG_INFO("cp len %d",evt->params.hvx.len);
        if (evt->params.hvx.len <= 2)
        {
            /// based on the length we get err and cmd sets
            NRF_LOG_ERROR("%d %d %d", evt->params.hvx.len, evt->params.hvx.data[0],evt->params.hvx.data[1]);
        }
        else
        {
            uint8_t total_cmds = evt->params.hvx.len;
            //// here you get the cmd supported in the ams control char
            for (uint8_t i = 0; i < total_cmds; i++)
            {
                /// put the cmd in the
                ble_ams_handler.cmds.ams_supp_cmds[evt->params.hvx.data[i]] = 1;
            }
        }
    }
    /// handle the entity update char , here we recieve the differnt entity attributes value
    else if (evt->params.hvx.handle == ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.handle_value)
    {

        if(evt->params.hvx.len <= BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE)
        {
            NRF_LOG_ERROR("eu len %d %d,%d,%d",evt->params.hvx.len,evt->params.hvx.data[0],
            evt->params.hvx.data[1],evt->params.hvx.data[2]);
            return 0;
        }
        ///////// it is assumed that the notification recvd is in the format of the entity_update notif cb @ref __ENTITY_UPDATE_CHAR_NOTIF_CB_FORMAT_
        /// switch between differnt entity id
        uint8_t entity_id = evt->params.hvx.data[ble_ams_index_entity_id];
        uint8_t att_id = evt->params.hvx.data[ble_ams_index_att_id];
        uint8_t trunc = evt->params.hvx.data[ble_ams_index_entity_update_flag];
        
        NRF_LOG_INFO("e %d a %d fg %d l %d",entity_id, att_id,trunc,evt->params.hvx.len);
        delay(200);
        char str[200];
        memcpy(str,&evt->params.hvx.data[ble_ams_index_data_value], (evt->params.hvx.len - BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE) );
        
        NRF_LOG_INFO("%s",str);
        memset(str,0,200);
        uint8_t err = 0;
    }
    //     switch (entity_id)
    //     {

    //         ////////////////////////////////////////////////////////////////////////////////////////////////////
    //         ////////////////////////////////////////////////////////////////////////////////////////////////////
    //         ////////////////////////////////////////// entity id media player info
    //     case ble_ams_entityid_player:
    //     {
    //         //// switch between the att id
    //         switch (att_id)
    //         {
    //         case ble_ams_player_attribute_name:
    //         {
    //             uint8_t err = kernel_mem_add_data(&ble_ams_mem_inst, ble_ams_attribute_index_mediaplayer, &evt->params.hvx.data[ble_ams_index_data_value], evt->params.hvx.len - BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE);
    //             /// either add it or modify it
    //             if (err == KERNEL_MEM_ERR_UID_ALRDY_PRESENT)
    //             {
    //                 //// uid already present , have to modify it
    //                 kernel_mem_modify_data(&ble_ams_mem_inst, ble_ams_attribute_index_mediaplayer, &evt->params.hvx.data[ble_ams_index_data_value], evt->params.hvx.len - BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE);
    //             }
    //         }
    //         break;

    //         case ble_ams_player_attribute_playbackinfo:
    //         {
    //             ///
    //             ble_ams_seperate_playbackinfo((char *)(char *)&evt->params.hvx.data[ble_ams_index_data_value]);
    //         }
    //         break;

    //         case ble_ams_player_attribute_volume:
    //         {
    //             /// convert the string volume to float
    //             player_volume = atof((char *)&evt->params.hvx.data[ble_ams_index_data_value]);
    //         }
    //         break;
    //         default:
    //             NRF_LOG_ERROR("unk att id");
    //             break;
    //         }
    //     }
    //     break;

    //         /////////////// ./////////////////////////////////////////////////////////////////////////////////////
    //         //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //         /////////////////////////////////////////////////////// ams entity id queue
    //     case ble_ams_entityid_queue:
    //     {
    //         switch (att_id)
    //         {
    //         case ble_ams_queue_attribute_index:
    //         {
    //             q_att_index = atoi((char *)&evt->params.hvx.data[ble_ams_index_data_value]);
    //         }
    //         break;
    //         case ble_ams_queue_attribute_byte_count:
    //         {
    //             att_total_items_q = atoi((char *)&evt->params.hvx.data[ble_ams_index_data_value]);
    //         }
    //         break;

    //         case ble_ams_queue_attribute_shuffle_mode:
    //         {
    //             q_att_shuffle_mode = atoi((char *)&evt->params.hvx.data[ble_ams_index_data_value]);
    //         }
    //         break;

    //         case ble_ams_queue_attribute_repeat_mode:
    //         {
    //             q_att_repeat_mode = atoi((char *)&evt->params.hvx.data[ble_ams_index_data_value]);
    //         }
    //         break;
    //         default:
    //             NRF_LOG_ERROR("unk att id");
    //             break;
    //         }
    //     }
    //     break;

    //         //////////////////////////////////////////////////////////////////////////////////////////////////////
    //         ///////////////////////////////////////////////////////////////////////////////////////////////////////
    //         ///////////////////////////////////// ams entity id track /////////////////////////////////////////////

    //     case ble_ams_entityid_Track:
    //     {
    //         switch (att_id)
    //         {

    //         case ble_ams_track_attribute_artist:
    //         {
    //             err = kernel_mem_add_data(&ble_ams_mem_inst, ble_ams_attribute_index_artist_name, &evt->params.hvx.data[ble_ams_index_data_value], evt->params.hvx.len - BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE);
    //             /// either add it or modify it
    //             if (err == KERNEL_MEM_ERR_UID_ALRDY_PRESENT)
    //             {
    //                 //// uid already present , have to modify it
    //                 kernel_mem_modify_data(&ble_ams_mem_inst, ble_ams_attribute_index_artist_name, &evt->params.hvx.data[ble_ams_index_data_value], evt->params.hvx.len - BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE);
    //             }
    //         }
    //         break;
    //         case ble_ams_track_attribute_album:
    //         {

    //             err = kernel_mem_add_data(&ble_ams_mem_inst, ble_ams_attribute_index_album_name, &evt->params.hvx.data[ble_ams_index_data_value], evt->params.hvx.len - BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE);
    //             /// either add it or modify it
    //             if (err == KERNEL_MEM_ERR_UID_ALRDY_PRESENT)
    //             {
    //                 //// uid already present , have to modify it
    //                 kernel_mem_modify_data(&ble_ams_mem_inst, ble_ams_attribute_index_album_name, &evt->params.hvx.data[ble_ams_index_data_value], evt->params.hvx.len - BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE);
    //             }
    //         }
    //         break;
    //         case ble_ams_track_attribute_title:
    //         {

    //             err = kernel_mem_add_data(&ble_ams_mem_inst, ble_ams_attribute_index_track_name, &evt->params.hvx.data[ble_ams_index_data_value], evt->params.hvx.len - BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE);
    //             /// either add it or modify it
    //             if (err == KERNEL_MEM_ERR_UID_ALRDY_PRESENT)
    //             {
    //                 //// uid already present , have to modify it
    //                 kernel_mem_modify_data(&ble_ams_mem_inst, ble_ams_attribute_index_track_name, &evt->params.hvx.data[ble_ams_index_data_value], evt->params.hvx.len - BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE);
    //             }
    //         }
    //         break;
    //         case ble_ams_track_attribute_duration:
    //         {
    //             track_duration = atof((char *)&evt->params.hvx.data[ble_ams_index_data_value]);
    //         }
    //         break;

    //         default:
    //             NRF_LOG_ERROR("unk att id");
    //             break;
    //         }
    //     }
    //     break;

    //     default:
    //         /// unknown entity id
    //         NRF_LOG_ERROR("unknown entity id");
    //         break;
    //     }
    // }

    return 0;
}
