#ifndef _BLE_PEER_INFO_H
#define _BLE_PEER_INFO_H

#include "ble_softdevice_init.h"



/// @brief this is to init the ble peer device info 
/// @param conn_handle 
/// @return succ/failure of the function 
uint32_t  ble_peer_device_init(uint16_t conn_handle);



#endif