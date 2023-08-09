
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
////////// extern here

extern const ble_gap_sec_params_t gap_sec_param[ble_gap_security_max_params_supported];

extern void ble_advertise_pre_init(void);

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
///=============================== static function here ===================================================
//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////// keypair generaion function

/// @note the softdevice random pool is 64 bytes only we have to split the transaction into 32 bit 

/// @brief random number generator function used to genrate entropy
/// @param dest
/// @param size
/// @return success or failure
static int random_number_gen(uint8_t *dest, unsigned size)
{
    uint8_t split_no = ((size)/16) + (((size)%16)==0?(0):(1));
    uint8_t bytes_avial = 0;
    uint8_t index =0;

         /// do a retreies here to check softdevice for radnom number 
    for (uint8_t i = 0; i < BLE_GAP_RANDOM_NUM_GEN_RETIRES; i++)
    {
        /// check that if we are done with giving all the data 
        if(index >= split_no)
        {
            return 1;
        }

        /// check for avialable radnom bytes in softdevice buffer  
        sd_rand_application_bytes_available_get(&bytes_avial);
        if ( bytes_avial <= 16)
        {
            delay(BLE_GAP_RANDOM_NUM_GEN_WAIT_TIME_MSEC);
            continue;
        }
        
        /// get the data from softdevice random buffer 
        uint8_t ret = sd_rand_application_vector_get((dest + (index *16)), (((size)%16)==0?(16):((size)%16)));
        if(ret != nrf_OK)
        {
            continue;
        }
        /// increment the index 
        index++;
    }

    /// failed to get random numbers
    NRF_LOG_ERROR("random fail");
    return  0;
}


/// @brief this function is to generate a pairing key for legacy pairing
/// @param  void
static void ble_gap_genrate_legacy_keypair(uint8_t index)
{
    /// generate the legacy temporary pairing key

    random_number_gen(gap_inst[index].key_set.keys_own.p_pk->pk,
                      sizeof(gap_inst[index].key_set.keys_own.p_pk->pk));
    // nrf_rng_init();
    // nrf_rng_fill_buff(public_key_device.pk, sizeof(public_key_device));
}

/// @brief this function is used to generate the key pair value
/// @param  void
static void ble_gap_genreate_lesc_keypair(uint8_t index)
{

    //genertate the key pair
    uint8_t ret = uECC_make_key(gap_inst[index].key_set.keys_own.p_pk->pk, gap_inst[index].private_key_device, uECC_secp256r1());
    if (ret != 1)
    {
        NRF_LOG_ERROR("mkey");
    }
    /// check for a valid public key
    ret = uECC_valid_public_key(gap_inst[index].key_set.keys_own.p_pk->pk, uECC_secp256r1());
    if (ret != 1)
    {
        NRF_LOG_ERROR("pbk_dev");
    }
}


/// @brief this is to start the dhkey calculation
/// @param conn_handle
/// @return err code
static uint32_t nrf_start_dhkey_calculation(uint8_t index)
{
    /// get the index from the conn handle
    uint32_t err_code = 0;
    ble_gap_lesc_dhkey_t peer_dh_key;

    // first check that is the public key of peer is valid or not
    // err_code = uECC_valid_public_key(gap_inst[index].key_set.keys_peer.p_pk->pk, uECC_secp256r1());
    err_code = uECC_valid_public_key(u8_ptr gap_inst[index].key_set.keys_peer.p_pk->pk, uECC_secp256r1());
    if (err_code != 1)
    {
        NRF_LOG_ERROR("peerkey");
        return nrf_ERR_OPERATION_FAILED;
    }

    /// compute here the shared secret from our private key and peer public key
    err_code = uECC_shared_secret(u8_ptr gap_inst[index].key_set.keys_peer.p_pk->pk, u8_ptr gap_inst[index].private_key_device,
                                  peer_dh_key.key, uECC_secp256r1());
    if (err_code != 1)
    {
        NRF_LOG_ERROR("dhkey");
        return nrf_ERR_OPERATION_FAILED;
    }

    err_code = sd_ble_gap_lesc_dhkey_reply(gap_inst[index].ble_gap_conn_handle, &peer_dh_key);
    NRF_ASSERT(err_code);

    return err_code;
}

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

/// @brief this function is used to init the security procedure
/// @param conn_handle
/// @param sec_param_type
uint32_t ble_gap_security_init(uint8_t index)
{
    CHECK_INDEX(index);
    CHECK_INIT(index);

    if (gap_inst[index].ble_gap_conn_handle == BLE_CONN_HANDLE_INVALID)
        return ble_gap_err_conn_handle_invalid;

    //// take the mutex
    if (xSemaphoreTake(ble_gap_mutex_handle, BLE_GAP_API_MUTEX_TIMEOUT) != pdPASS)
    {
        return ble_gap_err_mutex_not_avial;
    }

    //// return code
    uint32_t ret = 0;

    /// get the task handle and wait for notification
    ble_gap_taskhandle = xTaskGetCurrentTaskHandle();

    /// send an authentication request to the peer device
    ret = sd_ble_gap_authenticate(gap_inst[index].ble_gap_conn_handle, &gap_sec_param[gap_inst[index].ble_gap_security_param_index]);
    NRF_ASSERT(ret);
    if (ret != nrf_OK)
    {
        goto return_mech;
    }

    //// switch between the lesc and legacy keypair algorithms
    if (gap_inst[index].ble_gap_pairing_type == PAIRING_TYPE_LESC)
    {
        ble_gap_genreate_lesc_keypair(index);

        /// wait from the notification about the dhkey request
        if (xTaskNotifyWait(0, U32_MAX, &ret, BLE_GAP_API_TASK_NOTIF_TIMEOUT) != pdPASS)
        {
            ret = ble_gap_err_timeout;
            goto return_mech;
        }

        /// check for the return
        if (ret != nrf_OK)
        {
            goto return_mech;
        }

        ret = nrf_start_dhkey_calculation(index);

        if (ret != nrf_OK)
        {
            ret = ble_gap_err_dh_key_cal_failed;
            goto return_mech;
        }
        /// wait for the notification from the connection security update event
        if (xTaskNotifyWait(0, U32_MAX, &ret, BLE_GAP_API_TASK_NOTIF_TIMEOUT) != pdPASS)
        {
            ret = ble_gap_err_timeout;
            goto return_mech;
        }
        else
        {
            NRF_LOG_INFO("succ conn");
        }
    }
    else
    {
        ble_gap_genrate_legacy_keypair(index);
    }

//// return mechanism
return_mech:
    /// nullify the task handle
    ble_gap_taskhandle = NULL;

    /// give back the mutex
    xSemaphoreGive(ble_gap_mutex_handle);

    return ret;
}


/// @brief this function will print you the keys that are stored only for debugging purpose  
/// @param index 
uint32_t ble_gap_print_keys(uint8_t index)
{
    CHECK_INDEX(index);
    CHECK_INIT(index);

    //// take the mutex
    if (xSemaphoreTake(ble_gap_mutex_handle, BLE_GAP_API_MUTEX_TIMEOUT) != pdPASS)
    {
        return ble_gap_err_mutex_not_avial;
    }

    ///// display the owner keys 
    NRF_LOG_INFO("Owner key");
    delay(10);

    printf("ltk len %d\r\n",gap_inst[index].key_set.keys_own.p_enc_key->enc_info.ltk_len);
    for (int i = 0; i < 16; i++)
    {
        printf("%x,",gap_inst[index].key_set.keys_own.p_enc_key->enc_info.ltk[i]);
    }
    printf("lesc %d,auth %d\r\n",gap_inst[index].key_set.keys_own.p_enc_key->enc_info.lesc,
    gap_inst[index].key_set.keys_own.p_enc_key->enc_info.auth);

    delay(100);

    printf("ediv %d\r\n",gap_inst[index].key_set.keys_own.p_enc_key->master_id.ediv);
    for (int  i = 0; i < sizeof(gap_inst[index].key_set.keys_own.p_enc_key->master_id.rand); i++)
    {
        printf("%x,",gap_inst[index].key_set.keys_own.p_enc_key->master_id.rand[i]);
    }
    printf("\r\n");

    delay(100);

    printf("irk->");

    for (int  i = 0; i < sizeof(gap_inst[index].key_set.keys_own.p_id_key->id_info.irk); i++)
    {
        printf("%x,",gap_inst[index].key_set.keys_own.p_id_key->id_info.irk[i]);
    }
    printf("\r\n");

    delay(100);
   printf("addridown %d,ty %d\r\n",gap_inst[index].key_set.keys_own.p_id_key->id_addr_info.addr_id_peer,
   gap_inst[index].key_set.keys_own.p_id_key->id_addr_info.addr_type);

    printf("addrown-");
   for (int  i = 0; i < 6; i++)
   {
    printf("%x,",gap_inst[index].key_set.keys_own.p_id_key->id_addr_info.addr[i]);
   }
   printf("\r\n");


    delay(100);

//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    NRF_LOG_INFO("peer keys");

        delay(10);

    printf("ltk len %d\r\n",gap_inst[index].key_set.keys_peer.p_enc_key->enc_info.ltk_len);
    for (int i = 0; i < 16; i++)
    {
        printf("%x,",gap_inst[index].key_set.keys_peer.p_enc_key->enc_info.ltk[i]);
    }
    printf("lesc %d,auth %d\r\n",gap_inst[index].key_set.keys_peer.p_enc_key->enc_info.lesc,
    gap_inst[index].key_set.keys_peer.p_enc_key->enc_info.auth);

    delay(100);

    printf("ediv %d\r\n",gap_inst[index].key_set.keys_peer.p_enc_key->master_id.ediv);
    for (int  i = 0; i < sizeof(gap_inst[index].key_set.keys_peer.p_enc_key->master_id.rand); i++)
    {
        printf("%x,",gap_inst[index].key_set.keys_peer.p_enc_key->master_id.rand[i]);
    }
    printf("\r\n");

    delay(100);

    printf("irk->");

    for (int  i = 0; i < sizeof(gap_inst[index].key_set.keys_peer.p_id_key->id_info.irk); i++)
    {
        printf("%x,",gap_inst[index].key_set.keys_peer.p_id_key->id_info.irk[i]);
    }
    printf("\r\n");

    delay(100);
   printf("addridpeer %d,ty %d\r\n",gap_inst[index].key_set.keys_peer.p_id_key->id_addr_info.addr_id_peer,
   gap_inst[index].key_set.keys_peer.p_id_key->id_addr_info.addr_type);

    printf("addrpeer-");
   for (int  i = 0; i < 6; i++)
   {
    printf("%x,",gap_inst[index].key_set.keys_peer.p_id_key->id_addr_info.addr[i]);
   }
   printf("\r\n");

    

    
    
    /// give back the mutex
    xSemaphoreGive(ble_gap_mutex_handle);
}