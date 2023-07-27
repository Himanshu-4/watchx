
#include "ble_gap_func.h"

#include "uECC.h"

#include "nrf_ran_gen.h"



///// define here the key pair 

static uint8_t public_key[BLE_GAP_LESC_P256_PK_LEN] = {0};

static uint8_t private_key[BLE_GAP_LESC_DHKEY_LEN] = {0};


////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////   GAP related functions  implimented here

/// @brief check for valid index if not then return with err
#define CHECK_INDEX(x)                  \
    if (x >= BLE_GAP_MAX_NO_OF_DEVICES) \
    return nrf_ERR_INVALID_PARAM    \

//// check for instnace init
#define CHECK_INIT(x)                                                   \
    if (gap_inst[x].ble_gap_instnace_inited != BLE_GAP_INSTANCE_INITED) \
    return ble_gap_err_instnace_not_inited  \


//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////// extern here

extern const ble_gap_sec_params_t gap_sec_param[ble_gap_security_max_params_supported];

extern void ble_advertise_pre_init(void);

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  global variables here


/// @brief  the callbacks associated with this conn handle
volatile ble_gap_callback_struct_t GAP_Callbacks[ble_gap_max_callback_supp] = {{NULL}};

/// define the instance here

 ble_gap_inst_Struct_t gap_inst[BLE_GAP_MAX_NO_OF_DEVICES] = {0};

/// @brief this is to init the gap instance for the connection
/// @param conn_handle
/// @param index
/// @return succ/failure
uint32_t ble_gap_instance_init(uint8_t index)
{
    CHECK_INDEX(index);
    if (gap_inst[index].ble_gap_instnace_inited)
        return ble_gap_err_instance_already_inited;

    /// init the instnace
    memset(u8_ptr &gap_inst[index], 0, sizeof(ble_gap_inst_Struct_t));
    gap_inst[index].ble_gap_conn_handle = BLE_CONN_HANDLE_INVALID;

    gap_inst[index].ble_gap_instnace_inited = BLE_GAP_INSTANCE_INITED;

    uint32_t ret =0;

    /// when done store the keys pointer in keyset 

    /// copy the key pair 
    memcpy(gap_inst[index].public_key_device.pk, public_key,BLE_GAP_LESC_P256_PK_LEN );
    memcpy(gap_inst[index].private_key_device.key,  private_key, BLE_GAP_LESC_DHKEY_LEN);

    gap_inst[index].key_set.keys_own.p_pk = (ble_gap_lesc_p256_pk_t *)  gap_inst[index].public_key_device.pk;
    gap_inst[index].key_set.keys_peer.p_pk = (ble_gap_lesc_p256_pk_t *) gap_inst[index].public_key_peer.pk;
    
    /// check for a valid public key 
    ret = uECC_valid_public_key(gap_inst[index].key_set.keys_own.p_pk->pk ,uECC_secp256r1());
    NRF_ASSERT(ret);

    return ret;
}

/// @brief this is to deinit the gap instnace for this conn handle
/// @param conn_handle
/// @return succ/failure
uint32_t ble_gap_instance_deinit(uint8_t index)
{
    CHECK_INDEX(index);
    {;;;;;;}
     /// it is just here  for fun 😻😁🤦‍♀️🤦
    memset((uint8_t *)&gap_inst[index], 0, sizeof(ble_gap_inst_Struct_t));
    gap_inst[index].ble_gap_conn_handle = BLE_CONN_HANDLE_INVALID;

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
    if (gap_inst[index].ble_gap_instnace_inited != BLE_GAP_INSTANCE_INITED)
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
void ble_gap_add_callback(uint8_t callback_type, ble_gap_procdeure_callbacks callbacks , void * param)
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


/// @brief preinit the gap so that it can the BLE GAP properly
/// @param  void
/// @return succ/failure
void ble_gap_pre_init(void)
{
    ble_advertise_pre_init();

    /// instantize the gap instance

    for (int i = 0; i < BLE_MAX_DEVICE_SUPPORTED; i++)
    {
        memset(u8_ptr & gap_inst[i], 0, sizeof(ble_gap_inst_Struct_t));
    }

    for (int i = 0; i < BLE_MAX_DEVICE_SUPPORTED; i++)
    {
        gap_inst[i].ble_gap_conn_handle = BLE_CONN_HANDLE_INVALID;
    }
}

/// @brief this function is used to init the security procedure
/// @param conn_handle
/// @param sec_param_type
uint32_t ble_gap_security_init(uint8_t index, uint8_t sec_param_type)
{
    CHECK_INDEX(index);
    CHECK_INIT(index);

    if (gap_inst[index].ble_gap_conn_handle == BLE_CONN_HANDLE_INVALID)
        return ble_gap_err_conn_handle_invalid;

    gap_inst[index].ble_gap_security_param_index = sec_param_type;

    uint32_t ret_code = sd_ble_gap_authenticate(gap_inst[index].ble_gap_conn_handle, &gap_sec_param[sec_param_type]);
    NRF_ASSERT(ret_code);

    /// this will exit when the crypto operations are completed and link is autheticated
    return ble_gap_ok;
}



static int random_number_gen(uint8_t *dest, unsigned size)
{
    uint8_t ret = nrf_rng_fill_buff(dest, size);
    return (ret == nrf_OK)?(1):(0);
}


/// @brief this function is used to generate the key pair value 
/// @param  void 
void ble_gap_genreate_keypair(void)
{
    nrf_rng_init();
    /// set the rng function
    uECC_set_rng(random_number_gen);

    /// genertate the key pair 
    uint8_t ret = uECC_make_key(public_key, private_key ,uECC_secp256r1());
    if(ret !=1)
    {
        NRF_LOG_ERROR("keyfailed %d",ret);
    }
}