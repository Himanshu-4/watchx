#ifndef BLE_GAP_SECURITY_H
#define BLE_GAP_SECURITY_H

#include "ble_softdevice_init.h"
////////// do have to perform bonfing
#define BLE_GAP_SEC_PARAM_BOND 1

//// no io capabilites of the device
#define BLE_GAP_SEC_PARAM_IO_CAPS BLE_GAP_IO_CAPS_NONE

////////// disable the generation of keypress notification
#define BLE_GAP_SEC_PARAM_KEYPRESS 0

///// the low energy secure connection is a ble 5.0 security feature
#define BLE_GAP_SEC_PARAM_LESC_SUPPORT 0

///// we dont need man in middle attack protection for now
#define BLE_GAP_SEC_PARAM_MITM 0

/// the device have no out of band support
#define BLE_GAP_SEC_PARAM_OOB_SUPPORT 0

/////// the key size
#define BLE_GAP_SEC_PARAM_MAX_KEY_SIZE 7
#define BLE_GAP_SEC_PARAM_MIN_KEY_SIZE 16

#define BLE_GAP_SEC_PARAM_LTK 1
#define BLE_GAP_SEC_PARAM_IRK 1

#define BLE_GAP_SEC_PARAM_DERV_LINK_FROM_LTK 1
#define BLE_GAP_SEC_PARAM_CONN_SIGNATURE_RESOLVING_KEY 0

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// function declarations ///////////////////////////////

#define BLE_GAP_LESC_TYPE_PAIRING 10UL
#define BLE_GAP_LEGACY_TYPE_PAIRING 20UL

typedef PACKED_STRUCT _GAP_SEC_PROC_INIT_
{
    uint8_t gap_sec_pairing_type : 1; // do we require lesc pairing
    uint8_t bond_formation : 1;
}
ble_gap_sec_params;

/// @brief init the gap security procedure
/// @param conn_handle
void ble_gap_security_init(uint16_t conn_handle, ble_gap_sec_params *sec_param);

/// @brief this function will wait for the
/// @return
uint16_t ble_gap_get_conn_handle();



///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

//////////////// BLE advertisement related macros 
/**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#define BLE_ADV_INTERVAL 100  //// advertisemnt interval in terms of 0.625 millisec 
#define BLE_ADV_DURATION 6000  // only advertise for 1 minute // in milliseconds                                        /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define BLE_MAX_ADV_EVENTS 0



#define DEVICE_NAME "WATCHX"          /**< Name of device. Will be included in the advertising data. */


#define MIN_CONN_INTERVAL MSEC_TO_UNITS(BLE_MIN_CONNECTION_INTERVAL , UNIT_1_25_MS) /**< Minimum acceptable connection interval (0.4 seconds). */
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(BLE_MAX_CONNECTION_INTERVAL , UNIT_1_25_MS) /**< Maximum acceptable connection interval (0.65 second). */
#define SLAVE_LATENCY  BLE_CONNECTION_SLAVE_LATENCY                            
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(BLE_CONNECTION_SUPERVISION_TIMEOUT, UNIT_10_MS)  /**< Connection supervisory time-out (4 seconds). */



/// @brief preinit the gap so that it can the BLE GAP properly
/// @param  void 
void ble_gap_pre_init(void);



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

typedef void (*ble_gap_procdeure_callbacks)(void *param , ble_gap_evt_t * gap_evt);

/// @brief this is to add the callback to the particular callback 
/// @param callback_type 
/// @param conn_handle 
/// @param callbacks 
void ble_gap_add_callback(uint8_t callback_type, uint16_t conn_handle, ble_gap_procdeure_callbacks callbacks);

/// @brief this is to disconnect the
/// @param conn_handle
void ble_gap_disconnect(uint16_t conn_handle);

/// @brief this function is used to advertise the ble device
/// @param  void
uint32_t  ble_gap_start_advertise(void);


uint32_t  ble_gap_stop_advertise(void);

/// @brief this function will delete all the bonds 
/// @param  void 
void ble_gap_delete_bonds(void);


#endif