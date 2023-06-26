#ifndef _BLE_CLIENT_H
#define _BLE_CLIENT_H


#include "ble_softdevice_init.h"

#define BLE_CLIENT_INITED 0x10UL

#define BLE_CLIENT_DEINIT 0x00

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
////////////////// cccd value of the characteristics
#define NOTIFICATION_ENABLED 1
#define NOTIFICATION_AND_INDICATION_DISABLED 0
#define INDICATION_ENABLED 2


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
    ble_client_err_conn_handle_not_found,
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
    /// @brief 
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

typedef void (*gatt_client_timeout_handler)(void *param);
/// @brief this is to add the gatt client timeout callback
/// @param conn_hand
/// @param parameter pointer
/// @param gatt_client_timeout_handler
/// @return succ/failure of function
uint32_t gatt_client_add_timeout_callback(uint16_t conn_hand, gatt_client_timeout_handler handler, void *param);


////////////////////////////// add the error handler 

typedef void (*gatt_client_error_handler)(void *param , uint16_t err_code);
/// @brief this is to call when an errr is occured during gatt operations  
/// @param conn_handle 
/// @param parameter pointer
/// @param handler 
/// @return succ/failure of the function 
uint32_t gatt_client_add_err_handler_callback(uint16_t conn_handle, gatt_client_error_handler handler , void *param );


//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////// this is to discover the att database of the server

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



/// @brief this the typedef to add callback for the gatt notification
typedef void (*gatt_client_notif_callbaclk)(void *param, ble_gattc_evt_t *evt );

/// @brief this is to add the notification callback for the notification
/// @note this function will be called inside the ble event handler task of the sd , which have a higher priority 
//// and limited stack size please dont use memory intensive  task inside this handler 
/// @param conn_hand
/// @param callback
/// @param pointer_param 
/// @return succ/failure of function
uint32_t gatt_client_add_notif_callback(uint16_t conn_hand, gatt_client_notif_callbaclk callback , void *param);

/// @brief this the typedef to add callback for the gatt indication
typedef void (*gatt_client_indc_callbaclk)(void *param,  ble_gattc_evt_t *evt);

/// @brief this is to add the indication callback to the client handlers
/// @note this function will be called inside the ble event handler task of the sd , which have a higher priority 
//// and limited stack size please dont use memory intensive  task inside this handler 
/// @param conn_hand
/// @param callback
/// @param pointer_param 
/// @return succ/failure of function
uint32_t gatt_client_add_indication_callback(uint16_t conn_hand, gatt_client_indc_callbaclk callback , void *param);


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
uint32_t gatt_client_char_write(uint16_t conn_hand, ble_char_struct_t *char_struct,  uint8_t write_type, uint8_t *buff, uint16_t size);

/// @brief this is to write a value to the gatt client descriptor 
/// @param conn_hand 
/// @param desc_struct 
/// @param buff 
/// @param size 
/// @return succ/Failure of func 
uint32_t gattc_client_char_desc_write(uint16_t conn_hand , ble_char_desc_struct_t *desc_struct, uint8_t *buff, uint16_t size);

/// @brief this to read the char descriptor value 
/// @param conn_hand 
/// @param desc_struct 
/// @param buff 
/// @param size 
/// @return succ/Failure of the func 
uint32_t gattc_client_char_desc_read(uint16_t conn_hand , ble_char_desc_struct_t *desc_struct, uint8_t *buff, uint16_t size);

/// @brief this is to write a long value to the char
/// @param conn_hand
/// @param buff
/// @param size
/// @return succ/failure of function
uint32_t gatt_client_char_long_write(uint16_t conn_hand, ble_char_struct_t *char_struct, uint8_t *buff, uint16_t size);

/// @brief this is to do reliable write to the characteristics of service
/// @param conn_hand
/// @param buff
/// @param size
/// @return succ/faliure of func
uint32_t gatt_client_char_reliable_write(uint16_t conn_hand, ble_char_struct_t *char_struct, uint8_t *buff, uint16_t size);

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

    /////////// assign the functions callbacks

    /// @brief the gatt client notification callback
    gatt_client_notif_callbaclk client_notif_handler;
    void *client_notif_hand_param;

    /// @brief gatt client indication handler
    gatt_client_indc_callbaclk client_ind_handler;
    void *client_indi_hand_param;

    /// @brief the gatt client timeout handler
    gatt_client_timeout_handler client_timeout_handler;
    void *client_timeout_hand_param;

    gatt_client_error_handler client_err_handler;
    void * client_err_hand_param;
}
ble_client_struct;

#endif