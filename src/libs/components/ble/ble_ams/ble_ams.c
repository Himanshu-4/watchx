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

static volatile uint8_t playbackstate;

static volatile float player_volume = 0, track_duration = 0, elapsed_time = 0, playbackrate = 0;

/// @brief music queue attributes
static volatile uint8_t q_att_index = 0, att_total_items_q = 0, q_att_shuffle_mode, q_att_repeat_mode;

/////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////// defining the global instance

static ble_ams_struct_t ble_ams_handler = {0};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// static function here ////////////////////////////////////////////////////////////

/// @brief this function will seperate the playback info
/// @param str
/// @param lenofstr
static void ble_ams_seperate_playbackinfo(const char *str, uint16_t lenofstr)
{
    //// the loop will be run 3 times as there are 3 commas
    uint16_t index = 0, prev_index = 0;

    for (uint8_t i = 0; i < 3; i++)
    {
        uint16_t len_of_data = 0;

        while ((str[index] != ',') && (index < lenofstr))
        {
            len_of_data++;
            index++;
        }
        /// move from comma
        index++;
        if (i == 0)
        {
            playbackstate = str_to_int(str, len_of_data);
            prev_index = index;
        }
        else if (i == 1)
        {
            playbackrate = str_to_float(str + prev_index, len_of_data);
            prev_index = index;
        }
        else if (i == 2)
        {
            elapsed_time = str_to_float(str + prev_index, len_of_data);
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

   //// the apple media service id
    sd_ble_uuid_vs_add(&ble_apple_media_service_uuid128, (uint8_t *)&ble_ams_handler.ams_srvc_char.ams_service.ble_service.uuid.type);
    sd_ble_uuid_vs_add(&ble_ams_remote_command_char_uuid128, (uint8_t *)&ble_ams_handler.ams_srvc_char.ams_control_point_char.characterstic.uuid.type);
    sd_ble_uuid_vs_add(&ble_ams_entity_update_char_uuid128, (uint8_t *)&ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.uuid.type);
    sd_ble_uuid_vs_add(&ble_ams_entity_attribute_char_uuid128, (uint8_t *)&ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.characterstic.uuid.type);
    
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

    /// make zero out
    playbackstate = 0;
    player_volume = 0;
    track_duration = 0;
    elapsed_time = 0;
    playbackrate = 0;
    q_att_index = 0;
    att_total_items_q = 0;
    q_att_shuffle_mode = 0;
    q_att_repeat_mode = 0;

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

    /// suscribe for the differnt entity attributes

    //// subscribe all the attribute for player entity like playername ,player playbacinfo ,player volume
    const uint8_t entity_id_player[] = {ble_ams_entityid_player, ble_ams_player_attribute_name, ble_ams_player_attribute_playbackinfo, ble_ams_player_attribute_volume};

    //// subscribe all the attribute for the entity  queue like q properties
    const uint8_t entity_id_queue[] = {ble_ams_entityid_queue, ble_ams_queue_attribute_index, // give the q present index
                                       ble_ams_queue_attribute_byte_count, ble_ams_queue_attribute_shuffle_mode, ble_ams_queue_attribute_repeat_mode};

    ///// subscribe all the track attributes like name, duration , artist name
    const uint8_t entity_id_track[] = {ble_ams_entityid_Track, ble_ams_track_attribute_artist, ble_ams_track_attribute_album,
                                       ble_ams_track_attribute_title, ble_ams_track_attribute_duration};

    ble_ams_handler.ble_ams_instance_inited = BLE_AMS_INSTANCE_INITED;
    /////// write this to the entty update char
    err = gatt_client_char_write(conn_handle, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_update_char, CHAR_WRITE_WITH_RSP, entity_id_player, sizeof(entity_id_player));
    NRF_ASSERT(err);

    err = gatt_client_char_write(conn_handle, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_update_char, CHAR_WRITE_WITH_RSP, entity_id_queue, sizeof(entity_id_queue));
    NRF_ASSERT(err);

    err = gatt_client_char_write(conn_handle, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_entity_update_char, CHAR_WRITE_WITH_RSP, entity_id_track, sizeof(entity_id_track));
    NRF_ASSERT(err);

    /// make all the data to zero
    return nrf_OK;
}

/// @todo also zero out all the data stored in vars and strings
/// @brief this is to deinit the ams at disconnection, ble disbale
/// @param  void
uint32_t ble_ams_deinit(void)
{
    NRF_LOG_WARNING("ams deinit");

    //// clear the cmd supported  values
    memset((uint8_t *)&ble_ams_handler.cmds, 0, sizeof(ble_ams_handler.cmds));

    /// deinit the kernel m,emory
    kernel_mem_deinit(&ble_ams_mem_inst);

    /// make zero out
    playbackstate = 0;
    player_volume = 0;
    track_duration = 0;
    elapsed_time = 0;
    playbackrate = 0;
    q_att_index = 0;
    att_total_items_q = 0;
    q_att_shuffle_mode = 0;
    q_att_repeat_mode = 0;

    ble_ams_handler.ble_ams_instance_inited = BLE_AMS_INSTANCE_DEINITED;
    ble_ams_handler.conn_handle = BLE_CONN_HANDLE_INVALID;
    //// clear the
    return nrf_OK;
}

/// @brief this function gives the attribute string name
/// @param attributeindex
/// @return string containg attribute , NULL if none
const char *ble_ams_get_attribute_name(ble_ams_attribute_name index)
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
    uint8_t err = 0;

    switch (cmd_id)
    {
    case ble_ams_cmd_play:
        err = ble_ams_handler.cmds.ams_cmd_play;
        break;
    case ble_ams_cmd_pause:
        err = ble_ams_handler.cmds.ams_cmd_pause;
        break;
    case ble_ams_cmd_toogle_playpause:
        err = ble_ams_handler.cmds.ams_cmd_toogle_playpause;
        break;
    case ble_ams_cmd_next_track:
        err = ble_ams_handler.cmds.ams_cmd_next_track;
        break;
    case ble_ams_cmd_previous_track:
        err = ble_ams_handler.cmds.ams_cmd_previous_track;
        break;
    case ble_ams_cmd_volume_up:
        err = ble_ams_handler.cmds.ams_cmd_volume_up;
        break;
    case ble_ams_cmd_volue_down:
        err = ble_ams_handler.cmds.ams_cmd_volue_down;
        break;
    case ble_ams_cmd_advance_repeat_mode:
        err = ble_ams_handler.cmds.ams_cmd_advance_repeat_mode;
        break;
    case ble_ams_cmd_advance_shuffle_mode:
        err = ble_ams_handler.cmds.ams_cmd_advance_shuffle_mode;
        break;
    case ble_ams_cmd_skip_forward:
        err = ble_ams_handler.cmds.ams_cmd_skip_forward;
        break;
    case ble_ams_cmd_skip_backward:
        err = ble_ams_handler.cmds.ams_cmd_skip_backward;
        break;
    case ble_ams_cmd_like_track:
        err = ble_ams_handler.cmds.ams_cmd_like_track;
        break;
    case ble_ams_cmd_dislike_track:
        err = ble_ams_handler.cmds.ams_cmd_dislike_track;
        break;
    case ble_ams_cmd_bookmark_track:
        err = ble_ams_handler.cmds.ams_cmd_bookmark_track;
        break;
    default:
        err = 0;
        break;
    }
    if (err == 0)
    {
        return nrf_ERR_OPERATION_FAILED;
    }

    /// we can execute the cmds
    err = gatt_client_char_write(ble_ams_handler.conn_handle, (ble_char_struct_t *)&ble_ams_handler.ams_srvc_char.ams_control_point_char, CHAR_WRITE_WITH_RSP, &cmd_id, 1);
    NRF_ASSERT(err);

    return nrf_OK;
}

/// @brief get the playback state @ref _BLE_AMS_PLAYBACK_STATE_
/// @param  playbackinfo
/// @return value
uint32_t ble_ams_get_playback_info(uint8_t info_type)
{
    if (ble_ams_handler.ble_ams_instance_inited != BLE_AMS_INSTANCE_INITED)
    {
        return 0;
    }

    uint32_t data = 0;
    switch (info_type)
    {
    case ble_ams_info_state:
    {
        data = playbackstate;
    }
    break;
    case ble_ams_info_elapsed_time:
    {
        data = elapsed_time;
    }
    break;
    case ble_ams_info_track_duration:
    {
        data = track_duration;
    }
    break;
    case ble_ams_info_volume:
    {
        data = player_volume * 100;
    }
    break;
    default:
        data = 0;
        break;
    }

    return data;
}

/// @brief get the playback rate in the floatfromat  playback rate 1.2x 1.5x 2.3x etc if not inited 0
/// @return rate
float ble_ams_get_playbackrate(void)
{
    if (ble_ams_handler.ble_ams_instance_inited != BLE_AMS_INSTANCE_INITED)
    {
        return 0;
    }

    return playbackrate;
}

/// @brief get the q attribute like q index , repeat mode @ref ble_ams_q_att_data
/// @param  ble_ams_q_att_data
/// @return returns q atrtibute data
uint32_t ble_ams_get_Queue_attribute(ble_ams_q_att_data index)
{
    if (ble_ams_handler.ble_ams_instance_inited != BLE_AMS_INSTANCE_INITED)
    {
        return 0;
    }

    uint32_t data = 0;

    switch (index)
    {
    case ble_ams_queue_attribute_index:
    {
        data = q_att_index;
    }
    break;

    case ble_ams_queue_attribute_byte_count:
    {
        data = att_total_items_q;
    }
    break;

    case ble_ams_queue_attribute_shuffle_mode:
    {
        data = q_att_shuffle_mode;
    }
    break;
    case ble_ams_queue_attribute_repeat_mode:
    {
        data = q_att_repeat_mode;
    }
    break;
    default:
        data = 0;
        break;
    }

    return data;
}

/// @brief this function is used for debugg purpose and to print the apple media info
/// @param  void
void ble_ams_print_media_info(void)
{
    /// print the strings first
    const char *str = ble_ams_get_attribute_name(ble_ams_attribute_index_mediaplayer);
    NRF_LOG_INFO("p %s", ((str == NULL) ? ("nil") : (str)));
    delay(50);

    str = ble_ams_get_attribute_name(ble_ams_attribute_index_artist_name);
    NRF_LOG_INFO("a %s", ((str == NULL) ? ("nil") : (str)));
    delay(50);

    str = ble_ams_get_attribute_name(ble_ams_attribute_index_track_name);
    NRF_LOG_INFO("t %s", ((str == NULL) ? ("nil") : (str)));
    delay(50);

    str = ble_ams_get_attribute_name(ble_ams_attribute_index_album_name);
    NRF_LOG_INFO("a %s", ((str == NULL) ? ("nil") : (str)));
    delay(50);

    delay(50);
    NRF_LOG_INFO("s %d,et %d,td %d,v %d r %f", ble_ams_get_playback_info(ble_ams_info_state), ble_ams_get_playback_info(ble_ams_info_elapsed_time), ble_ams_get_playback_info(ble_ams_info_track_duration), ble_ams_get_playback_info(ble_ams_info_volume), ble_ams_get_playbackrate());

    NRF_LOG_INFO("qi %d qc %d qs %d qr %d", ble_ams_get_Queue_attribute(ble_ams_queue_attribute_index),
                 ble_ams_get_Queue_attribute(ble_ams_queue_attribute_byte_count),
                 ble_ams_get_Queue_attribute(ble_ams_queue_attribute_shuffle_mode),
                 ble_ams_get_Queue_attribute(ble_ams_queue_attribute_repeat_mode));


    static uint8_t cmd =0;

    NRF_LOG_INFO("cmd %d,rsp%d",cmd,ble_ams_execute_cmd(cmd));
    cmd++;
    if(cmd > ble_ams_cmd_bookmark_track)
    {
        cmd =0;
    }
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
        memset(u8_ptr & ble_ams_handler.cmds, 0, sizeof(ble_ams_handler.cmds));
        //// here you get the cmd supported in the ams control char
        for (uint8_t i = 0; i < evt->params.hvx.len; i++)
        {
            /// put the cmd in the supported coomands
            switch (evt->params.hvx.data[i])
            {
            case ble_ams_cmd_play:
                ble_ams_handler.cmds.ams_cmd_play = 1;
                break;
            case ble_ams_cmd_pause:
                ble_ams_handler.cmds.ams_cmd_pause = 1;
                break;
            case ble_ams_cmd_toogle_playpause:
                ble_ams_handler.cmds.ams_cmd_toogle_playpause = 1;
                break;
            case ble_ams_cmd_next_track:
                ble_ams_handler.cmds.ams_cmd_next_track = 1;
                break;
            case ble_ams_cmd_previous_track:
                ble_ams_handler.cmds.ams_cmd_previous_track = 1;
                break;
            case ble_ams_cmd_volume_up:
                ble_ams_handler.cmds.ams_cmd_volume_up = 1;
                break;
            case ble_ams_cmd_volue_down:
                ble_ams_handler.cmds.ams_cmd_volue_down = 1;
                break;
            case ble_ams_cmd_advance_repeat_mode:
                ble_ams_handler.cmds.ams_cmd_advance_repeat_mode = 1;
                break;
            case ble_ams_cmd_advance_shuffle_mode:
                ble_ams_handler.cmds.ams_cmd_advance_shuffle_mode = 1;
                break;
            case ble_ams_cmd_skip_forward:
                ble_ams_handler.cmds.ams_cmd_skip_forward = 1;
                break;
            case ble_ams_cmd_skip_backward:
                ble_ams_handler.cmds.ams_cmd_skip_backward = 1;
                break;
            case ble_ams_cmd_like_track:
                ble_ams_handler.cmds.ams_cmd_like_track = 1;
                break;
            case ble_ams_cmd_dislike_track:
                ble_ams_handler.cmds.ams_cmd_dislike_track = 1;
                break;
            case ble_ams_cmd_bookmark_track:
                ble_ams_handler.cmds.ams_cmd_bookmark_track = 1;
                break;
            default:
                NRF_LOG_ERROR("unsupp cmd");
                break;
            }
            // NRF_LOG_INFO("%d,%d",evt->params.hvx.data[i],
            // ble_ams_handler.cmds.ams_supp_cmds[evt->params.hvx.data[i]]);
        }
    }
    /// handle the entity update char , here we recieve the differnt entity attributes value
    else if (evt->params.hvx.handle == ble_ams_handler.ams_srvc_char.ams_entity_update_char.characterstic.handle_value)
    {
        char const *str = NULL;
        uint16_t strlen = 0;

        ///////// it is assumed that the notification recvd is in the format of the entity_update notif cb @ref __ENTITY_UPDATE_CHAR_NOTIF_CB_FORMAT_
        /// switch between differnt entity id
        uint8_t entity_id = evt->params.hvx.data[ble_ams_index_entity_id];
        uint8_t att_id = evt->params.hvx.data[ble_ams_index_att_id];
        uint8_t trunc = evt->params.hvx.data[ble_ams_index_entity_update_flag];

        if (evt->params.hvx.len > BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE)
        {
            str = (char *)&evt->params.hvx.data[ble_ams_index_data_value];
            strlen = evt->params.hvx.len - BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE;
        }
        // else the string would be null and len would be zero

        uint8_t err = 0;

        switch (entity_id)
        {

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////// entity id media player info
        case ble_ams_entityid_player:
        {
            //// switch between the att id
            switch (att_id)
            {
            case ble_ams_player_attribute_name:
            {
                if (strlen == 0)
                {
                    kernel_mem_delete_data(&ble_ams_mem_inst, ble_ams_attribute_index_mediaplayer);
                }
                else
                {
                    uint8_t err = kernel_mem_add_data(&ble_ams_mem_inst, ble_ams_attribute_index_mediaplayer,
                                                      u8_ptr str, strlen);
                    /// either add it or modify it
                    if (err == KERNEL_MEM_ERR_UID_ALRDY_PRESENT)
                    {
                        //// uid already present , have to modify it
                        kernel_mem_modify_data(&ble_ams_mem_inst, ble_ams_attribute_index_mediaplayer,
                                               u8_ptr str, strlen);
                    }
                }
            }
            break;

            case ble_ams_player_attribute_playbackinfo:
            {
                ble_ams_seperate_playbackinfo(str, strlen);
            }
            break;

            case ble_ams_player_attribute_volume:
            {
                player_volume = str_to_float(str, strlen);
            }
            break;
            default:
                NRF_LOG_ERROR("unk att id");
                break;
            }
        }
        break;

            /////////////// ./////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////// ams entity id queue
        case ble_ams_entityid_queue:
        {

            /// switch between different att id for the queu entity
            switch (att_id)
            {
            case ble_ams_queue_attribute_index:
            {
                q_att_index = str_to_int(str, strlen);
            }
            break;
            case ble_ams_queue_attribute_byte_count:
            {
                att_total_items_q = str_to_int(str, strlen);
            }
            break;

            case ble_ams_queue_attribute_shuffle_mode:
            {
                q_att_shuffle_mode = str_to_int(str, strlen);
            }
            break;

            case ble_ams_queue_attribute_repeat_mode:
            {
                q_att_repeat_mode = str_to_int(str, strlen);
            }
            break;
            default:
                NRF_LOG_ERROR("unk att id");
                break;
            }
        }
        break;

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////// ams entity id track /////////////////////////////////////////////

        case ble_ams_entityid_Track:
        {
            switch (att_id)
            {
                /// we dont add artist name and album name , because they can consume more memory
            case ble_ams_track_attribute_artist:
            {
                // err = kernel_mem_add_data(&ble_ams_mem_inst, ble_ams_attribute_index_artist_name, &evt->params.hvx.data[ble_ams_index_data_value], evt->params.hvx.len - BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE);
                // /// either add it or modify it
                // if (err == KERNEL_MEM_ERR_UID_ALRDY_PRESENT)
                // {
                //     //// uid already present , have to modify it
                //     kernel_mem_modify_data(&ble_ams_mem_inst, ble_ams_attribute_index_artist_name, &evt->params.hvx.data[ble_ams_index_data_value], evt->params.hvx.len - BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE);
                // }
            }
            break;
            case ble_ams_track_attribute_album:
            {

                // err = kernel_mem_add_data(&ble_ams_mem_inst, ble_ams_attribute_index_album_name, &evt->params.hvx.data[ble_ams_index_data_value], evt->params.hvx.len - BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE);
                // /// either add it or modify it
                // if (err == KERNEL_MEM_ERR_UID_ALRDY_PRESENT)
                // {
                //     //// uid already present , have to modify it
                //     kernel_mem_modify_data(&ble_ams_mem_inst, ble_ams_attribute_index_album_name, &evt->params.hvx.data[ble_ams_index_data_value], evt->params.hvx.len - BLE_AMS_ENTITY_UPDATE_META_DATA_SIZE);
                // }
            }
            break;
            case ble_ams_track_attribute_title:
            {
                if (strlen == 0)
                {
                    kernel_mem_delete_data(&ble_ams_mem_inst, ble_ams_attribute_index_track_name);
                }
                else
                {
                    err = kernel_mem_add_data(&ble_ams_mem_inst, ble_ams_attribute_index_track_name,
                                              u8_ptr str, strlen);
                    /// either add it or modify it
                    if (err == KERNEL_MEM_ERR_UID_ALRDY_PRESENT)
                    {
                        //// uid already present , have to modify it
                        kernel_mem_modify_data(&ble_ams_mem_inst, ble_ams_attribute_index_track_name,
                                               u8_ptr str, strlen);
                    }
                }
            }
            break;
            case ble_ams_track_attribute_duration:
            {
                track_duration = str_to_float(str, strlen);
            }
            break;

            default:
                NRF_LOG_ERROR("unk att id");
                break;
            }
        }
        break;

        default:
            /// unknown entity id
            NRF_LOG_ERROR("unknown entity id");
            break;
        }
    }

    return 0;
}
