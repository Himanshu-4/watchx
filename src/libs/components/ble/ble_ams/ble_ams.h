#ifndef BLE_AMS_H
#define BLE_AMS_H

#include "ble_softdevice_init.h"
#include "ble_gatt_client.h"

#define NULL_STRING '\0'


enum _BLE_AMS_ERRORS_
{
    ble_ams_ok = 0x00,
    ble_ams_op_timeout = NRF_BLE_AMS_ERR_BASE,

};

//////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////// this  is the database table of the apple media service
typedef PACKED_STRUCT __ALIGNED(4) _BLE_AMS_SERVICES_STRUCT_
{
    /// @brief the apple media service
    ble_service_struct_t ams_service;

    /// @brief control point of the Apple media service
    ble_char_struct_t ams_control_point_char;
    ble_char_desc_struct_t ams_control_point_desc;

    /// @brief entity update for the apple media service
    ble_char_struct_t ams_entity_update_char;
    ble_char_desc_struct_t ams_entity_update_desc;

    /// @brief entity attribute for the apple media service
    ble_char_struct_t ams_entity_attribute_char;
    ble_char_desc_struct_t ams_entity_attribute_desc;
}
ble_ams_services_struct_t;

typedef PACKED_STRUCT __ALIGNED(4) _BLE_AMS_REMOTE_CMD_STRUCT_
{
    // so ther are  14 cmds in total that we are available with 
    bool ams_supp_cmds[14];
}
ble_ams_supported_cmdsets;


#define BLE_AMS_INSTANCE_INITED 0x10
#define BLE_AMS_INSTANCE_DEINITED 0x20

typedef struct __ALIGNED(4) _BLE_AMS_STRUCT_
{
    /// @brief contains the structure for the apple media service att table
    ble_ams_services_struct_t  ams_srvc_char;
    ble_ams_supported_cmdsets cmds;
    uint16_t conn_handle;
    uint8_t ble_ams_instance_inited;
}
ble_ams_struct_t;

//////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// enums for the AMS service

/// @brief err code for the when writing to any char anr reading the entity attribute
enum _BLE_AMS_ERROR_CODES_
{
    ble_ams_err_invalid_State = 0x0A,
    ble_ams_err_invalid_cmd,
    ble_ams_err_invalid_attribute,
};

/// @brief remote cmd supported by the AMS
typedef enum _BLE_AMS_SUPPORTED_RREMOTE_CMDS_
{
    ble_ams_cmd_play,
    ble_ams_cmd_pause,
    ble_ams_cmd_toogle_playpause,
    ble_ams_cmd_next_track,
    ble_ams_cmd_previous_track,
    ble_ams_cmd_volume_up,
    ble_ams_cmd_volue_down,
    ble_ams_cmd_advance_repeat_mode,
    ble_ams_cmd_advance_shuffle_mode,
    ble_ams_cmd_skip_forward,
    ble_ams_cmd_skip_backward,
    ble_ams_cmd_like_track,
    ble_ams_cmd_dislike_track,
    ble_ams_cmd_bookmark_track,
    ble_ams_cmd_rfus,
}ble_ams_media_cmds;

/// @brief this is the entity id for the player, q, and track
enum _BLE_AMS_SUPPORTED_ENTITY_IDS_
{
    ble_ams_entityid_player,
    ble_ams_entityid_queue,
    ble_ams_entityid_Track,
    ble_ams_entityid_rfus,
};

////// define the attribute id here

/// @brief the player attribute info
enum _BLE_AMS_SUPPORTED_PLAYER_ATTRIBUTE_IDS_
{
    ble_ams_player_attribute_name,
    ble_ams_player_attribute_playbackinfo,
    ble_ams_player_attribute_volume,
    ble_ams_player_attribute_rfus,
};

//// the playbackinfo have 3 string concatenate
//// playbackstate,playbackrate,elapsetime

/// @brief define the playback state
enum _BLE_AMS_PLAYBACK_STATE_
{
    ble_ams_playback_state_paused,
    ble_ams_playback_state_playing,
    ble_ams_playback_state_revinding,
    ble_ams_playback_state_fastforward
};

//// the playback rate is a string that reprsent the floating point value of rate

/// elapsed time  string that represent the floating point value

#define ENTITY_UPDATE_FLAG_TRUNCATED_TRUE 1
#define ENTITY_UPDATE_FLAG_TRUNCATED_FALSE 0

/// @brief define the queue attribute
typedef enum _BLE_AMS_SUPPORTED_QUEUE_ATTRIBUTE_IDS_
{
    ble_ams_queue_attribute_index, // give the q present index 
    ble_ams_queue_attribute_byte_count, // give the total no of item in q 
    ble_ams_queue_attribute_shuffle_mode,
    ble_ams_queue_attribute_repeat_mode,
    ble_ams_queue_attribute_rfus,
}ble_ams_q_att_data;

//// return of the cmd ble_ams_queue_attribute_shuffle_mode 
/// @brief define the shuffle mode
enum _BLE_AMS_SHUFFLE_MODE_
{
    ble_ams_shuffle_mode_constant,
    ble_ams_shuffle_mode_off,
    ble_ams_shuffle_mode_one,
    ble_ams_shuffle_mode_all,
    ble_ams_shuffle_mode_rfus,
};

//// return of the cmd ble_ams_queue_attribute_repeat_mode
/// @brief define the repeat mode
enum _BLE_AMS_REPEAT_MODE_
{
    ble_ams_repetat_mode_constant,
    ble_ams_repetat_mode_off,
    ble_ams_repetat_mode_one,
    ble_ams_repetat_mode_all,
    ble_ams_repeat_mode_rfus,
};

enum _BLE_AMS_SUPPORTED_TRACK_ATTRIBUTE_IDS_
{
    ble_ams_track_attribute_artist,
    ble_ams_track_attribute_album,
    ble_ams_track_attribute_title,
    ble_ams_track_attribute_duration,
    ble_ams_track_Attribute_rfus,
};

////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// function declarations  ///////////////////////////////////////

/// @brief this is to init the apple media service this should be called in main
/// @param  void
void ble_ams_pre_init(void);

/// @brief this is to init the ams at a coonection event
/// @param  conn_handle
uint32_t ble_ams_init(uint16_t conn_handle);

/// @brief this is to deinit the ams at disconnection, ble disbale
/// @param  void
uint32_t ble_ams_deinit(void);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/////////////// these are the player attribute

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
///////////// below API are used to get the string value from the attributes 
//// returns null string if attribute value is absent  

typedef enum _BLE_AMS_ATTRIBUTES_NAME_INDEX_
{
    ble_ams_attribute_index_mediaplayer =0x01,
    ble_ams_attribute_index_artist_name,
    ble_ams_attribute_index_track_name,
    ble_ams_attribute_index_album_name,
    ble_ams_attribute_index_misc,
}ble_ams_attribute_name;

/// @brief this function gives the attribute string name 
/// @param attributeindex
/// @return string containg attribute , NULL if none
char *ble_ams_get_attribute_name(ble_ams_attribute_name index);


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
///////////// below API are used to exectute the cmd on the media player 

/// @brief ble_ams_execute cmd is the function used to execute a specific cmd in media player  
/// @param cmd_id 
/// @return succ/Failure of the cmd 
uint32_t ble_ams_execute_cmd(ble_ams_media_cmds cmd_id);


/// @brief get the playback state @ref _BLE_AMS_PLAYBACK_STATE_
/// @param  playstate
/// @return succ/faliure
uint32_t ble_ams_get_playback_State(uint8_t *playstate);

/// @brief get the playback rate in the integer fromat  playback rate 1.2x 1.5x 2.3x etc
/// @param the playback rate in float value 
/// @return succ/faliure
uint32_t ble_ams_get_playbackrate(float * rate);

/// @brief returns the volume of the media in percentage
/// @param  volume pointer 
/// @return succ/filure 
uint32_t ble_ams_get_volume(uint8_t * volume);

/// @brief geive the elapsed time in seconds 
/// @param  void 
/// @return returns the elpased time in seconds 
uint32_t ble_ams_get_elapsed_time(uint32_t * elapse_time);

/// @brief get the total time of the track 
/// @param  void 
/// @return returns the track time in seconds 
uint32_t ble_ams_get_track_time(uint32_t * track_time);


/// @brief get the q attribute like q index , repeat mode @ref ble_ams_q_att_data
/// @param  ble_ams_q_att_data
/// @param ble_ams_QUeue data 
/// @return returns cmd specific  @ref _BLE_AMS_SHUFFLE_MODE_  @ref _BLE_AMS_REPEAT_MODE_
uint32_t ble_ams_get_Queue_attribute(ble_ams_q_att_data index , uint8_t *data);

//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
/// @brief this is the apple media service handler where
/// @param param
void ble_ams_client_event_handler(void *param, ble_gattc_evt_t const *evt);

#endif