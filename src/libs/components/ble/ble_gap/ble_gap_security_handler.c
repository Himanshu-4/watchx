#include "ble_gap_func.h"

#include "FreeRTOS.h"
#include "task.h"
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
int random_number_gen(uint8_t *dest, unsigned size)
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


/// @brief this function is handle the authentication status of the data
/// @param p_ble_evt
static void nrf_handle_authentication_status(ble_evt_t const *p_ble_evt)
{

    NRF_LOG_INFO("au %d,e %d,b %d,l %d,s1 %x,s2 %x,ko %x,kp %x",
                 p_ble_evt->evt.gap_evt.params.auth_status.auth_status,
                 p_ble_evt->evt.gap_evt.params.auth_status.error_src,
                 p_ble_evt->evt.gap_evt.params.auth_status.bonded,
                 p_ble_evt->evt.gap_evt.params.auth_status.lesc,

                 //// the sm1 level
                 ((p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv1 << 3) |
                  (p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv2 << 2) |
                  (p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv3 << 1) |
                  (p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv4)),

                 /// sm2 level
                 ((p_ble_evt->evt.gap_evt.params.auth_status.sm2_levels.lv1 << 3) |
                  (p_ble_evt->evt.gap_evt.params.auth_status.sm2_levels.lv2 << 2) |
                  (p_ble_evt->evt.gap_evt.params.auth_status.sm2_levels.lv3 << 1) |
                  (p_ble_evt->evt.gap_evt.params.auth_status.sm2_levels.lv4)),

                 /// kdist owner
                 ((p_ble_evt->evt.gap_evt.params.auth_status.kdist_own.enc << 3) |
                  (p_ble_evt->evt.gap_evt.params.auth_status.kdist_own.id << 2) |
                  (p_ble_evt->evt.gap_evt.params.auth_status.kdist_own.link << 1) |
                  (p_ble_evt->evt.gap_evt.params.auth_status.kdist_own.sign)),

                 /// kdist peer
                 ((p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer.enc << 3) |
                  (p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer.id << 2) |
                  (p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer.link << 1) |
                  (p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer.sign)));

    /// get the index from the conn handle
    uint8_t index = ble_gap_get_gap_index(p_ble_evt->evt.gap_evt.conn_handle);

    /// make sure we dont overconnect like to cross the max concurrent device limit 
    if (index < BLE_MAX_DEVICE_SUPPORTED)
    {
        // create a new instnace for bond structure 
        ble_gap_store_bond_info_struct_t new_Device_bond_info = {0};
        /// copy the bond info 
        memcpy( &new_Device_bond_info.peer_id_info, gap_inst[index].key_set.keys_peer.p_id_key, sizeof(ble_gap_id_key_t));
        memcpy(&new_Device_bond_info.dev_enc_key, gap_inst[index].key_set.keys_own.p_enc_key, sizeof(ble_gap_enc_key_t));
        
        uint8_t total_uid_presnt = nvs_Get_total_no_of_uid();

        uint32_t err = nvs_add_data(++total_uid_presnt, u8_ptr &new_Device_bond_info ,sizeof(new_Device_bond_info)); 
        if(err != nrf_OK)
        {
            NRF_LOG_ERROR("add nvs %d",err);
        }
    }
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