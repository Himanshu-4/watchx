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


}ble_ams_struct_t;







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