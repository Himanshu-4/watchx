#ifndef _GATT_SERVER_H_
#define _GATT_SERVER_H_

#include "ble_config.h"
#include "system.h"


/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///////// defines the device information profile data 

//////////// define the length of the characteristics value 
//// battery level percentage 
#define BATT_LEVEL_PERCENTAGE_USER_DESCRIPTOR "Percentage 0-100"

#define BATT_SVC_BATT_LVL_CHAR_MAX_LEN 4 

#define BATT_SVC_BATT_CUSTOM_DATA_CHAR_MAX_LEN 30


enum _BLE_GATT_SERVER_ERRORS_
{
    ble_gatt_server_ok =0x00,

    ble_gatt_op_timeout = NRF_BLE_GATT_SERVER_ERR_BASE,
    
};

/// @brief preinit the gatt server 
/// @param  void 
void ble_gatt_server_pre_init(void);


#endif