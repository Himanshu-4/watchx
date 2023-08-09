#ifndef BLE_GAP_SECURITY_H
#define BLE_GAP_SECURITY_H

#include "ble_softdevice_init.h"




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

    ble_gap_err_timeout = NRF_BLE_GAP_ERR_BASE,

    ble_gap_err_mutex_not_avial,
    /// error regarding conn handle 
    ble_gap_err_conn_handle_invalid,

    ble_gap_err_max_Device_limit_reached,
    ble_gap_err_device_index_invalid ,

    ble_gap_err_instance_already_inited,
    ble_gap_err_instnace_init_falied,
    ble_gap_err_instnace_not_inited,

    ble_gap_err_dh_key_cal_failed

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

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
///////// define the ble gap instnace inits and deinit flags 
#define BLE_GAP_INSTANCE_INITED 1
#define BLE_GAP_INSTNACE_DEINITED 0

#define PAIRING_TYPE_LEGACY 1
#define PAIRING_TYPE_LESC 2

/// @brief this is to init the gap instance for the connection  
/// @param index 
/// @return succ/failure 
uint32_t ble_gap_instance_init(uint8_t index , uint8_t pairing_type );

/// @brief this is to deinit the gap instnace for this conn handle 
/// @param index 
/// @return succ/failure 
uint32_t ble_gap_instance_deinit(uint8_t index);

/// @brief this is to get the connection handle of the connected device 
/// @param  index of the device 
/// @return the conection handle , 
uint16_t ble_gap_get_conn_handle(uint8_t index );

/// @brief this function is to set the connection handle of that index 
/// @param index 
/// @param conn_handle 
/// @return 
uint32_t ble_gap_set_conn_handle(uint8_t index, uint16_t conn_handle);


/// @brief this is to remove the connection handle 
/// @param index 
/// @return 
uint32_t ble_gap_remove_conn_handle(uint8_t index);

/// @brief this will return a valid index based on the conn handle 
/// @param conn_handle 
/// @return index 
uint8_t ble_gap_get_gap_index(uint16_t conn_handle);

/// @brief this function will print you the keys that are stored only for debugging purpose  
/// @param index 
uint32_t ble_gap_print_keys(uint8_t index);

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////   callback regarding functions 

typedef void (*ble_gap_procdeure_callbacks)(void *param , ble_gap_evt_t const  * gap_evt);

typedef PACKED_STRUCT _BLE_GAP_CALLBACK_TYPES_
{
    ble_gap_procdeure_callbacks callback;
    void * callback_param; 
}ble_gap_callback_struct_t;


/// @brief this is to add the callback to the particular callback 
/// @param callback_type 
/// @param callbacks 
void ble_gap_add_callback(uint8_t callback_type, ble_gap_procdeure_callbacks callbacks, void *param);


/// @brief this is to remove the gap callbacks 
/// @param callback_type
void ble_gap_remove_callback( uint8_t callback_type);


/// @brief this is to disconnect the device and also remove the connection handle from the connected device array 
/// @param conn_handle
void ble_gap_disconnect(uint16_t conn_handle);


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

#define BLE_ADVERTISE_WITH_FAST_PAIR 10
#define BLE_ADVERTISE_WITHOUT_FAST_PAIR 20

/// @brief this function is used to start a ble advertisement 
/// @param type 
/// @return succ/failure 
uint32_t ble_gap_start_advertise(uint8_t type);

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
/// @return succ/failure  
uint32_t ble_gap_security_init(uint8_t index);



typedef struct _BLE_GAP_FUNCTION_INSTANCE_
{
    /// @brief this is a flag to make sure that gap instnace is inited 
    uint8_t ble_gap_instnace_inited;

    /// @brief the connection handle of the index 
    uint16_t ble_gap_conn_handle;

    /// @brief security index to use in the conn handle 
    uint8_t ble_gap_pairing_type; //// this is to switch bw legacy and lesc pairing 
    uint8_t ble_gap_security_param_index;

    ble_gap_sec_keyset_t key_set;    

    /// @brief  32 bytes shared secret 
    uint8_t * private_key_peer;
    uint8_t * private_key_device;

    //// this is to init the gap function instnaces 
}ble_gap_inst_Struct_t;



#endif