#ifndef _BLE_CLIENT_H
#define _BLE_CLIENT_H


#include "ble_softdevice_init.h"

#define BLE_CLIENT_INITED 0x10UL

#define BLE_CLIENT_DEINIT 0x00

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
////////////////// cccd value of the characteristics
#define NOTIFICATION_AND_INDICATION_DISABLE 0
#define NOTIFICATION_ENABLE 1
#define INDICATION_ENABLE 2
#define NOTIFICATION_AND_INDICATION_EANBLE 3


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////// error codes for client

/// @brief these are the error code of the clients
enum _CLIENT_ERR_CODE_
{
    ble_client_ok = 0x00,
    /// @brief not found related error code
    ble_client_err_conn_handle_not_found = NRF_BLE_GATT_CLIENT_ERR_BASE,
    ble_client_err_srvc_not_found,
    ble_client_err_char_not_found,
    ble_client_err_desc_not_found,

    /// @brief ble operation failed err code
    ble_client_err_write_op_failed,
    ble_client_err_read_op_failed,
    ble_client_err_read_size_exceed,
    ble_client_err_ex_mtu_failed,
    
    /// @brief invalid state, max limit reached , timeout errors
    ble_client_err_timeout,
    ble_client_mutex_not_avialble,
    ble_client_err_invalid_state,
    ble_client_err_max_limit_reached,
    ble_client_err_client_not_inited,

};

/////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////// data structure  declarations /////////////////

/// @brief define the service strucutre that is gona pass in the functions 
typedef struct _BLE_GATT_SERVICE_
{
    ble_gattc_service_t ble_service;
} ble_service_struct_t;

/// @brief define the ble gatt characteristic 
typedef struct _BLE_GATT_CHAR_
{
    /// @brief the characterstics deintaion , its uuidprop, its handles  
    ble_gattc_char_t characterstic;

} ble_char_struct_t;

/// @brief define the ble gatt char descriptor 
typedef struct _BLE_GATT_DESC_
{
    /// @brief the char descritpor  
    ble_gattc_desc_t descriptor;
} ble_char_desc_struct_t;
/////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////// function declarations /////////////////

////// this is preinit the gatt client module in int main function
uint32_t gatt_client_pre_init(void);

//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////// this is to init  the gatt client module and functionality regarding gatt client connection

/// @brief init the gatt client module
/// @param  BLE_connnection handle
uint32_t gatt_client_init(uint16_t conn_handle);

/// @brief this is to deinit the gatt client module ,you can call it at gap_disconnecct
/// @param conn_handle
/// @return succ/failure of function
uint32_t gatt_client_deinit(uint16_t conn_handle);


enum _CALLBACK_TYPE_
{
    ble_gatt_client_notif_callback =0x00,
    ble_gatt_client_indic_callback,
    ble_gatt_client_error_callback,
    ble_gatt_client_timeout_callback,
    ble_gatt_client_max_callbacks_supp
};

/// @brief callback handler type 
typedef void (*gatt_client_callback_handler)(ble_gattc_evt_t const *gattc_event);

/// @brief this is to add the gatt client timeout callback
/// @param gatt_client_callback_handler
void ble_gatt_client_add_callback(uint8_t callback_type,gatt_client_callback_handler callback);

/// @brief this function is used to remove the callback from the gatt client 
/// @param callback_type 
void ble_gatt_client_remove_callback(uint8_t callback_type);


//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////// this is to discover the att database of the server


/// @brief for searching the gatt server service
/// @param conn_hand
/// @param number how many services you want to discover 
/// @return succ/failure of function
uint32_t gatt_client_explore_service(uint16_t conn_handle , uint8_t num);

/// @brief for searching the gatt server service
/// @param conn_hand
/// @param ble_service_struct_t pointer 
/// @return succ/failure of function
uint32_t gatt_client_discover_service(uint16_t conn_hand ,ble_service_struct_t * service_struct );

/// @brief to discover the gatt client characteristics of the server, at this time only one by one char discv is avaialble 
/// @param conn_hand
/// @param service_struct
/// @param char_struct
/// @return succ/failure of function
uint32_t gatt_client_discover_chars(uint16_t conn_hand , ble_service_struct_t * service_struct , ble_char_struct_t * char_struct);

/// @brief for seraching the gatt client char descriptor discovery
/// @param conn_hand
/// @param char_struct
/// @param desc_uuid
/// @return succ/failure of function
uint32_t gatt_client_discover_char_desc(uint16_t conn_hand , ble_char_struct_t * char_struct , ble_char_desc_struct_t * desc_struct);

/// @brief this is to set the server mtu of the connection handle
/// @param conn_hand
/// @param mtu
/// @return succ/failure of function
uint32_t gatt_client_set_server_mtu(uint16_t conn_hand, uint16_t mtu);

//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////// /// char operations  specific functions

/// @brief this is to read the char value of the service, or u can read descriptor value
/// @param conn_hand
/// @param ble_char structure 
/// @param buffer 
/// @param size of the buffer 
/// @return succ/failure of function
uint32_t gatt_client_char_read(uint16_t conn_hand , ble_char_struct_t *char_struct, uint8_t *buff, uint8_t size );

#define CHAR_WRITTE_NO_RSP 0
#define CHAR_WRITE_WITH_RSP 1

/// @brief this is to write a value to the char ,descriptor
/// @param conn_hand
/// @param write_type
/// @param buff
/// @param size
/// @return succ/failure of func
uint32_t gatt_client_char_write(uint16_t conn_hand, ble_char_struct_t *char_struct,  uint8_t write_type, uint8_t const *buff, uint16_t size);

/// @brief this is to write a value to the gatt client descriptor 
/// @param conn_hand 
/// @param desc_struct 
/// @param buff 
/// @param size 
/// @return succ/Failure of func 
uint32_t gattc_client_char_desc_write(uint16_t conn_hand , ble_char_desc_struct_t *desc_struct, uint8_t const *buff, uint16_t size);

/// @brief this to read the char descriptor value 
/// @param conn_hand 
/// @param desc_struct 
/// @param buff 
/// @param size 
/// @return succ/Failure of the func 
uint32_t gattc_client_char_desc_read(uint16_t conn_hand , ble_char_desc_struct_t *desc_struct, uint8_t *buff, uint16_t size);


/// @todo implemntation pending 

/// @brief this is to write a long value to the char
/// @param conn_hand
/// @param buff
/// @param size
/// @return succ/failure of function
uint32_t gatt_client_char_long_write(uint16_t conn_hand, ble_char_struct_t *char_struct, uint8_t const  *buff, uint16_t size);

/// @brief this is to do reliable write to the characteristics of service
/// @param conn_hand
/// @param buff
/// @param size
/// @return succ/faliure of func
uint32_t gatt_client_char_reliable_write(uint16_t conn_hand, ble_char_struct_t *char_struct, uint8_t const *buff, uint16_t size);

/////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////// client declaration strucutre  /////////////////

/// @brief this structure is the genric form of client
typedef PACKED_STRUCT _BLE_CLIENT_DATA_STRUCT_
{
    /// @brief this is to ensure client is inited properly
    uint8_t client_inited;

    /// @brief the gatt client connection handle
    uint16_t conn_handle;
}
ble_client_struct;

#endif