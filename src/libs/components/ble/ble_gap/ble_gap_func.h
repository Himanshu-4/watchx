#ifndef BLE_GAP_SECURITY_H
#define BLE_GAP_SECURITY_H

#include "ble_softdevice_init.h"

#include "system.h"


///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

//////////////// BLE advertisement related macros 
/**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#define BLE_ADV_INTERVAL 100  //// advertisemnt interval in terms of 0.625 millisec 
#define BLE_ADV_DURATION 6000  // only advertise for 1 minute // in milliseconds                                        /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define BLE_MAX_ADV_EVENTS 0



#define DEVICE_NAME BLE_DEVICE_GAP_ADVERTISEMENT_NAME          /**< Name of device. Will be included in the advertising data. */


#define MIN_CONN_INTERVAL MSEC_TO_UNITS(BLE_MIN_CONNECTION_INTERVAL , UNIT_1_25_MS) /**< Minimum acceptable connection interval (0.4 seconds). */
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(BLE_MAX_CONNECTION_INTERVAL , UNIT_1_25_MS) /**< Maximum acceptable connection interval (0.65 second). */
#define SLAVE_LATENCY  BLE_CONNECTION_SLAVE_LATENCY                            
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(BLE_CONNECTION_SUPERVISION_TIMEOUT, UNIT_10_MS)  /**< Connection supervisory time-out (4 seconds). */




/// @brief preinit the gap so that it can the BLE GAP properly
/// @param  void 
void ble_gap_pre_init(void);


enum _BLE_GAP_RELATED_ERRORS_
{
    ble_gap_ok = 0x00,

    ble_gap_err_timeout,

    /// error regarding conn handle 
    ble_gap_err_conn_handle_invalid,

    ble_gap_err_max_Device_limit_reached,
    ble_gap_err_device_index_invalid ,

    ble_gap_err_instance_already_inited,
    ble_gap_err_instnace_init_falied,
    ble_gap_err_instnace_not_inited

};
/////////////////////////////////////////////////////////////////
///////////////// add the callback ////////////////////////////

enum _BLE_GAP_CALLBACKS_
{
    ble_gap_evt_connected,
    ble_gap_evt_disconnected,
    ble_gap_evt_timeout,
    ble_gap_evt_sec_procedure_cmpt,
    ble_gap_max_callback_supp

};

enum _BLE_GAP_SECURITY_PARAMS_TYPE_
{
    ble_gap_security_param1,
    ble_gap_security_param2,
    ble_gap_security_param3,
    ble_gap_security_max_params_supported

};

/// @brief this is to init the gap instance for the connection  
/// @param index 
/// @return succ/failure 
uint32_t ble_gap_instance_init(uint8_t index );

/// @brief this is to deinit the gap instnace for this conn handle 
/// @param index 
/// @return succ/failure 
uint32_t ble_gap_instance_deinit(uint8_t index);

/// @brief this is to get the connection handle of the connected device 
/// @param  index of the device 
/// @return the conection handle , 
uint16_t ble_gap_get_conn_handle(uint8_t index );


////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////   callback regarding functions 

typedef void (*ble_gap_procdeure_callbacks)(void *param , ble_gap_evt_t const  * gap_evt);

/// @brief this is to add the callback to the particular callback 
/// @param callback_type 
/// @param callbacks 
void ble_gap_add_callback(uint8_t callback_type, ble_gap_procdeure_callbacks callbacks);


/// @brief this is to remove the gap callbacks 
/// @param callback_type
void ble_gap_remove_callback( uint8_t callback_type);


/// @brief this is to disconnect the device and also remove the connection handle from the connected device array 
/// @param conn_handle
void ble_gap_disconnect(uint16_t conn_handle);


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

/// @brief this function is used to advertise the ble device
/// @param  void
/// @return succ/failure of the function 
uint32_t  ble_gap_start_advertise(void);

/// @brief this is to stop the advertisement process 
/// @param  void 
/// @return succ/failure of the function
uint32_t  ble_gap_stop_advertise(void);

/// @brief this function will delete all the bonds 
/// @param  void 
void ble_gap_delete_bonds(void);

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// function declarations ///////////////////////////////

/// @brief init the gap security procedure
/// @param index 
/// @param sec_param_type 
void ble_gap_security_init(uint8_t index, uint8_t sec_param_type);



typedef PACKED_STRUCT _BLE_GAP_FUNCTION_INSTANCE_
{
    /// @brief this is a flag to make sure that gap instnace is inited 
    uint8_t ble_gap_instnace_inited;

    /// @brief the connection handle of the index 
    uint16_t ble_gap_conn_handle;

    /// @brief security index to use in the conn handle 
    uint8_t ble_gap_security_param_index;

    /// @brief  keyset of the device 
    ble_gap_lesc_p256_pk_t public_key_device;
    ble_gap_lesc_dhkey_t private_key_device;

    /// @brief keyset of the peer
    ble_gap_lesc_p256_pk_t public_key_peer;
    ble_gap_lesc_dhkey_t private_key_peer;
    

    ble_gap_sec_keyset_t key_set;    

    /// @brief  32 bytes shared secret 
    // ble_gap_lesc_dhkey_t peer_dhkey;


    //// this is to init the gap function instnaces 
}ble_gap_inst_Struct_t;



#endif