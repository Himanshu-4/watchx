#ifndef _BLE_SOFTDEVICE_INIT_
#define _BLE_SOFTDEVICE_INIT_


///////////////////////////////////////////////////////
////////////// include ble common library /////////////
#include "ble_config.h"

// include the custom log here 
#include "nrf_custom_log.h"

#include "system.h"

//// include the softdevice s132 headers 
#include "ble_err.h"
#include "ble_gap.h"
#include "ble_gatt.h"
#include "ble_gatts.h"

#include "ble_hci.h"
#include "ble_l2cap.h"
#include "ble_ranges.h"
#include "ble_types.h"
#include "ble.h"

#include "nrf_error_sdm.h"
#include "nrf_error_soc.h"
#include "nrf_error.h"

#include "nrf_nvic.h"
#include "nrf_sd_def.h"
#include "nrf_sdm.h"
#include "nrf_soc.h"
#include "nrf_svc.h"



//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


#define BLE_UUID_16(a,b)          static const ble_uuid_t a =    \
{                                                                \
    .type = BLE_UUID_TYPE_BLE,                                   \
    .uuid = b,                                                   \
}                                                                \




void nrf_softdevice_init_and_start(void);




#endif