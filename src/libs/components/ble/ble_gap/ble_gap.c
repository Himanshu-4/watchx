
#include "ble_gap_func.h"

#include "nrf_ran_gen.h"

// #include "tinycrypt/ecc_dh.h"
#include "uECC.h"

//// make a mutex for encryption process
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/////////////////////// static variables  ////////////////////////////////
////////// create a mutex for the functions

// use semaphore spi handle
static SemaphoreHandle_t ble_gap_mutex_handle;

// these are used to create a semaphore buffer
static StaticSemaphore_t ble_gap_semphr_buffer = {0};

/// @brief this is to get the task handle and
volatile xTaskHandle ble_gap_taskhandle;

////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////   GAP related functions  implimented here

/// @brief check for valid index if not then return with err
#define CHECK_INDEX(x)                  \
    if (x >= BLE_GAP_MAX_NO_OF_DEVICES) \
    return nrf_ERR_INVALID_PARAM

//// check for instnace init
#define CHECK_INIT(x)                                                   \
    if (gap_inst[x].ble_gap_instnace_inited != BLE_GAP_INSTANCE_INITED) \
    return ble_gap_err_instnace_not_inited

//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

////////// extern variables 

extern const ble_gap_sec_params_t gap_sec_param[ble_gap_security_max_params_supported];


/// @brief EXtern functions  

extern void ble_advertise_pre_init(void);

extern int random_number_gen(uint8_t *dest, unsigned size);


//==========================================================================================================
///=============================== Global variables here ===================================================
//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief  the callbacks associated with this conn handle
volatile ble_gap_callback_struct_t GAP_Callbacks[ble_gap_max_callback_supp] = {{NULL}};

/// define the instance here
volatile ble_gap_inst_Struct_t gap_inst[BLE_GAP_MAX_NO_OF_DEVICES] = {0};

// static uint8_t public_key_device[BLE_GAP_LESC_P256_PK_LEN] = {0};

// static uint8_t private_key_device[BLE_GAP_LESC_DHKEY_LEN] = {0};

/// @brief keyset of the device
static ble_gap_lesc_p256_pk_t public_key_device;
static uint8_t private_key_device[32];

/// @brief keyset of the peer
static ble_gap_lesc_p256_pk_t public_key_peer;
static uint8_t private_key_peer[32];

/// @brief encryption keys and identity information for device
static ble_gap_enc_key_t device_en_keys;
static ble_gap_id_key_t device_id_keys;

/// @brief encryption keys and identity information for peer
static ble_gap_enc_key_t peer_en_keys;
static ble_gap_id_key_t peer_id_keys;


//==========================================================================================================
///=============================== function defination  here ===================================================
//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////

/// @brief preinit the gap so that it can the BLE GAP properly
/// @param  void
/// @return succ/failure
void ble_gap_pre_init(void)
{
    ble_advertise_pre_init();

    /// set the rng function
    uECC_set_rng(random_number_gen);

    //// the pool capacity of the Random number is 64
    /// instantize the gap instance

    for (int i = 0; i < BLE_MAX_DEVICE_SUPPORTED; i++)
    {
        memset(u8_ptr & gap_inst[i], 0, sizeof(ble_gap_inst_Struct_t));
    }

    for (int i = 0; i < BLE_MAX_DEVICE_SUPPORTED; i++)
    {
        gap_inst[i].ble_gap_conn_handle = BLE_CONN_HANDLE_INVALID;
    }

    /// create the mutex for gap handlers
    ble_gap_mutex_handle = xSemaphoreCreateMutexStatic(&ble_gap_semphr_buffer);

    if (ble_gap_mutex_handle == NULL)
    {
        NRF_LOG_ERROR("null gap mutex");
    }
    xSemaphoreGive(ble_gap_mutex_handle);
}

/// @brief this is to init the gap instance for the connection
/// @param conn_handle
/// @param index
/// @return succ/failure
uint32_t ble_gap_instance_init(uint8_t index, uint8_t pairing_type)
{
    CHECK_INDEX(index);
    if (gap_inst[index].ble_gap_instnace_inited)
        return ble_gap_err_instance_already_inited;

    //// take the mutex
    if (xSemaphoreTake(ble_gap_mutex_handle, BLE_GAP_API_MUTEX_TIMEOUT) != pdPASS)
    {
        return ble_gap_err_mutex_not_avial;
    }

    /// init the instnace
    memset(u8_ptr & gap_inst[index], 0, sizeof(ble_gap_inst_Struct_t));
    gap_inst[index].ble_gap_conn_handle = BLE_CONN_HANDLE_INVALID;

    gap_inst[index].ble_gap_instnace_inited = BLE_GAP_INSTANCE_INITED;
    gap_inst[index].ble_gap_pairing_type = pairing_type;

    /// the pairing keys to store
    if (pairing_type == PAIRING_TYPE_LEGACY)
    {
        /// when done store the keys pointer in keyset
        /// store the device keys
        gap_inst[index].key_set.keys_own.p_pk = &public_key_device;
        gap_inst[index].private_key_device = NULL;

        gap_inst[index].key_set.keys_own.p_enc_key = &device_en_keys;
        gap_inst[index].key_set.keys_own.p_id_key = &device_id_keys;
        gap_inst[index].key_set.keys_own.p_sign_key = NULL;

        //// store the peer keys
        gap_inst[index].key_set.keys_peer.p_pk = &public_key_peer;
        gap_inst[index].private_key_peer = NULL;

        gap_inst[index].key_set.keys_peer.p_enc_key = &peer_en_keys;
        gap_inst[index].key_set.keys_peer.p_id_key = &peer_id_keys;
        gap_inst[index].key_set.keys_peer.p_sign_key = NULL;

        gap_inst[index].ble_gap_security_param_index = ble_gap_security_param2;
    }

    else
    {
        /// when done store the keys pointer in keyset
        /// store the device keys
        gap_inst[index].key_set.keys_own.p_pk = &public_key_device;
        gap_inst[index].private_key_device = private_key_device;

        gap_inst[index].key_set.keys_own.p_enc_key = &device_en_keys;
        gap_inst[index].key_set.keys_own.p_id_key = NULL;
        gap_inst[index].key_set.keys_own.p_sign_key = NULL;

        //// store the peer keys
        gap_inst[index].key_set.keys_peer.p_pk = &public_key_peer;
        gap_inst[index].private_key_peer = NULL;

        gap_inst[index].key_set.keys_peer.p_enc_key = NULL;
        gap_inst[index].key_set.keys_peer.p_id_key = &peer_id_keys;
        gap_inst[index].key_set.keys_peer.p_sign_key = NULL;

        gap_inst[index].ble_gap_security_param_index = ble_gap_security_param1;
    }

    /// give back the mutex
    xSemaphoreGive(ble_gap_mutex_handle);
    return ble_gap_ok;
}

/// @brief this is to deinit the gap instnace for this conn handle
/// @param conn_handle
/// @return succ/failure
uint32_t ble_gap_instance_deinit(uint8_t index)
{
    CHECK_INDEX(index);

    //// take the mutex
    if (xSemaphoreTake(ble_gap_mutex_handle, BLE_GAP_API_MUTEX_TIMEOUT) != pdPASS)
    {
        return ble_gap_err_mutex_not_avial;
    }

    {
        ;
        ;
        ;
        ;
        ;
        ;
    }
    /// it is just here  for fun 😻😁🤦‍♀️🤦
    memset((uint8_t *)&gap_inst[index], 0, sizeof(ble_gap_inst_Struct_t));
    gap_inst[index].ble_gap_conn_handle = BLE_CONN_HANDLE_INVALID;

    // make the keys zero
    memset(&public_key_device, 0, sizeof(public_key_device));
    memset(private_key_device, 0, sizeof(private_key_device));

    /// @brief keyset of the peer
    memset(&public_key_peer, 0, sizeof(public_key_device));
    memset(private_key_peer, 0, sizeof(private_key_device));

    /// @brief encryption keys and identity information for device
    memset(&device_en_keys, 0, sizeof(device_en_keys));
    memset(&device_id_keys, 0, sizeof(device_id_keys));

    /// @brief encryption keys and identity information for peer
    memset(&peer_en_keys, 0, sizeof(peer_en_keys));
    memset(&peer_id_keys, 0, sizeof(peer_id_keys));

    /// give back the mutex
    xSemaphoreGive(ble_gap_mutex_handle);
    return ble_gap_ok;
}

/// @brief this function is to set the connection handle of that index
/// @param index
/// @param conn_handle
/// @return
uint32_t ble_gap_set_conn_handle(uint8_t index, uint16_t conn_handle)
{
    CHECK_INDEX(index);
    CHECK_INIT(index);
    /// set the connection handle
    gap_inst[index].ble_gap_conn_handle = conn_handle;

    return ble_gap_ok;
}

/// @brief
/// @param index
/// @return
uint32_t ble_gap_remove_conn_handle(uint8_t index)
{
    CHECK_INDEX(index);
    CHECK_INIT(index);
    /// set the connection handle
    gap_inst[index].ble_gap_conn_handle = BLE_CONN_HANDLE_INVALID;

    return ble_gap_ok;
}

/// @brief this will return a valid index based on the conn handle
/// @param conn_handle
/// @return index
uint8_t ble_gap_get_gap_index(uint16_t conn_handle)
{
    for (int i = 0; i < BLE_GAP_MAX_NO_OF_DEVICES; i++)
    {
        if (gap_inst[i].ble_gap_conn_handle == conn_handle)
        {
            return i;
        }
    }

    return BLE_GAP_MAX_NO_OF_DEVICES;
}

/// @brief this is to get the connection handle of the connected device
/// @param  index of the device
/// @return the conection handle
uint16_t ble_gap_get_conn_handle(uint8_t index)
{
    if (index >= BLE_GAP_MAX_NO_OF_DEVICES)
    {
        return BLE_CONN_HANDLE_INVALID;
    }
    // check if instnace is inited
    if (gap_inst[index].ble_gap_instnace_inited == BLE_GAP_INSTANCE_INITED)
    {
        return gap_inst[index].ble_gap_conn_handle;
    }
    return BLE_CONN_HANDLE_INVALID;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief this is to add the callback to the particular callback
/// @param callback_type
/// @param conn_handle
/// @param callbacks
void ble_gap_add_callback(uint8_t callback_type, ble_gap_procdeure_callbacks callbacks, void *param)
{
    if (callback_type > ble_gap_max_callback_supp)
        return;
    //// add the callback
    GAP_Callbacks[callback_type].callback = callbacks;
    GAP_Callbacks[callback_type].callback_param = param;
}

/// @brief this is to remove the gap callbacks
/// @param callback_type
void ble_gap_remove_callback(uint8_t callback_type)
{
    if (callback_type > ble_gap_max_callback_supp)
        return;
    ///// this is to set the callback
    //// add the callback
    GAP_Callbacks[callback_type].callback = NULL;
    GAP_Callbacks[callback_type].callback_param = NULL;
}
