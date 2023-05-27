#ifndef _GATT_SERVER_H_
#define _GATT_SERVER_H_




/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///////// defines the device information profile data 

//////////// define the length of the characteristics value 
//// battery level percentage 
#define BATT_LEVEL_PERCENTAGE_USER_DESCRIPTOR "Percentage 0-100"

#define BATT_SVC_BATT_LVL_CHAR_MAX_LEN 4 

#define BATT_SVC_BATT_CUSTOM_DATA_CHAR_MAX_LEN 30


/// @brief preinit the gatt server 
/// @param  void 
void ble_gatt_server_pre_init(void);


#endif