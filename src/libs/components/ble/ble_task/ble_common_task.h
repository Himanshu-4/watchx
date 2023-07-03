#ifndef _BLE_COMMON_TASK_
#define _BLE_COMMON_TASK_


//// include the ble specific library 
#include "ble_ams.h"
#include "ble_ancs.h"


#define BLE_COMMON_TASK_NAME "ble_common"

/// @brief this is to init the ble common task in static memory 
/// @param param 
void ble_common_task_pre_init(void *param);






#endif