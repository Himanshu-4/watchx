#ifndef BLE_AMS_H
#define BLE_AMS_H

#include "ble_softdevice_init.h"

//////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////// this  is the database table of the apple media service 
typedef PACKED_STRUCT _BLE_AMS_SERVICES_STRUCT_
{
    /// @brief the apple media service
    ble_gattc_service_t ams_service;

    /// @brief control point of the Apple media service 
    ble_gattc_char_t ams_control_point_cahr;
    ble_gattc_desc_t ams_control_point_desc;

    /// @brief entity update for the apple media service
    ble_gattc_char_t ams_entity_update_char;
    ble_gattc_desc_t ams_entity_update_desc;

    /// @brief entity attribute for the apple media service 
    ble_gattc_char_t ams_entity_attribute_char;
    ble_gattc_desc_t ams_entity_attribute_desc;

}ble_ams_services_struct_t;



typedef PACKED_STRUCT _BLE_AMS_STRUCT_ 
{
    /// @brief contains the structure for the apple media service att table 
    ble_ams_services_struct_t ams_srvc_char;
    ble_ams_supported_cmdsets cmds;



}ble_ams_struct_t;



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
enum _BLE_AMS_SUPPORTED_RREMOTE_CMDS_
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
};

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


#define ENTITY_UPDATE_FLAG_TRUNCATED_TRUE 1
#define ENTITY_UPDATE_FLAG_TRUNCATED_FALSE 0


enum _BLE_AMS_SUPPORTED_QUEUE_ATTRIBUTE_IDS_
{
    ble_ams_queue_attribute_index,
    ble_ams_queue_attribute_byte_count,
    ble_ams_queue_attribute_shuffle_mode,
    ble_ams_queue_attribute_repeat_mode,
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


/// @brief this is the apple media service handler where 
/// @param param 
void ble_ams_client_event_handler(void *param ,ble_gattc_evt_t *evt);


#endif