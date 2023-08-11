#include "ble_gap_func.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// #include "tinycrypt/ecc_dh.h"
#include "uECC.h"

#include "nvs.h"

////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////  Macro GAP related functions  implimented here

/// @brief check for valid index if not then return with err
#define CHECK_INDEX(x)                  \
    if (x >= BLE_GAP_MAX_NO_OF_DEVICES) \
    return nrf_ERR_INVALID_PARAM

//// check for instnace init
#define CHECK_INIT(x)                                                   \
    if (gap_inst[x].ble_gap_instnace_inited != BLE_GAP_INSTANCE_INITED) \
    return ble_gap_err_instnace_not_inited

//==========================================================================================================
///=============================== extern variable here ===================================================
//////////////////////////////////////////////////////////////////////////////////////////////////

/// define the instance here
extern ble_gap_inst_Struct_t gap_inst[BLE_GAP_MAX_NO_OF_DEVICES];

// use mutex handle
extern SemaphoreHandle_t ble_gap_mutex_handle;

extern const ble_gap_sec_params_t gap_sec_param[ble_gap_security_max_params_supported];

//==========================================================================================================
///=============================== GLOBAL variable here ===================================================
//////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief this is to get the task handle and
volatile xTaskHandle ble_gap_taskhandle;

volatile ble_evt_t *ble_gap_Evt;

//==========================================================================================================
///=============================== function declarations  here ===================================================
//////////////////////////////////////////////////////////////////////////////////////////////////

/// @note the softdevice random pool is 64 bytes only we have to split the transaction into 32 bit
/// @brief random number generator function used to genrate entropy
/// @param dest
/// @param size
/// @return success or failure
int random_number_gen(uint8_t *dest, unsigned size);
//==========================================================================================================
///=============================== static function here ===================================================
//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////// keypair generaion function

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

    // genertate the key pair
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
        NRF_LOG_ERROR("peerkey inv");
        return nrf_ERR_OPERATION_FAILED;
    }

    /// compute here the shared secret from our private key and peer public key
    err_code = uECC_shared_secret(u8_ptr gap_inst[index].key_set.keys_peer.p_pk->pk, u8_ptr gap_inst[index].private_key_device,
                                  peer_dh_key.key, uECC_secp256r1());
    if (err_code != 1)
    {
        NRF_LOG_ERROR("dhkey fail");
        return nrf_ERR_OPERATION_FAILED;
    }

    err_code = sd_ble_gap_lesc_dhkey_reply(gap_inst[index].ble_gap_conn_handle, &peer_dh_key);
    NRF_ASSERT(err_code);

    return err_code;
}

//==========================================================================================================
///=============================== function definations here ===================================================
//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @note the softdevice random pool is 64 bytes only we have to split the transaction into 32 bit

/// @brief random number generator function used to genrate entropy
/// @param dest
/// @param size
/// @return success or failure
int random_number_gen(uint8_t *dest, unsigned size)
{
    uint8_t split_no = ((size) / 16) + (((size) % 16) == 0 ? (0) : (1));
    uint8_t bytes_avial = 0;
    uint8_t index = 0;

    /// do a retreies here to check softdevice for radnom number
    for (uint8_t i = 0; i < BLE_GAP_RANDOM_NUM_GEN_RETIRES; i++)
    {
        /// check that if we are done with giving all the data
        if (index >= split_no)
        {
            return 1;
        }

        /// check for avialable radnom bytes in softdevice buffer
        sd_rand_application_bytes_available_get(&bytes_avial);
        if (bytes_avial <= 16)
        {
            delay(BLE_GAP_RANDOM_NUM_GEN_WAIT_TIME_MSEC);
            continue;
        }

        /// get the data from softdevice random buffer
        uint8_t ret = sd_rand_application_vector_get((dest + (index * 16)), (((size) % 16) == 0 ? (16) : ((size) % 16)));
        if (ret != nrf_OK)
        {
            continue;
        }
        /// increment the index
        index++;
    }

    /// failed to get random numbers
    NRF_LOG_ERROR("random fail");
    return 0;
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
    uint32_t notif_value = 0;

    /// get the task handle and wait for notification
    ble_gap_taskhandle = xTaskGetCurrentTaskHandle();

    //////////////////////////////////////////////////////////////////////////////////////////
    /// send an authentication request to the peer device
    ret = sd_ble_gap_authenticate(gap_inst[index].ble_gap_conn_handle, &gap_sec_param[gap_inst[index].ble_gap_security_param_index]);
    if (ret != nrf_OK)
    {
        NRF_ASSERT(ret);
        goto return_mech;
    }

    //// switch between the lesc and legacy keypair algorithms
    if (gap_inst[index].ble_gap_pairing_type == PAIRING_TYPE_LESC)
    {
        /// wait for the notification from the gap handler
        if (xTaskNotifyWait(0, U32_MAX, &notif_value, BLE_GAP_API_TASK_NOTIF_TIMEOUT) != pdPASS)
        {
            ret = ble_gap_err_timeout;
            goto return_mech;
        }

        /// check the notifcation value that if a new bond formation is required
        if (notif_value == BLE_SECEVT_SEC_PARAM_REQ)
        {
            //////////////////////////////////////////////////////////////////////////////////////////
            /// genrate the lesc keypair(public,private ) for the authentication process
            ble_gap_genreate_lesc_keypair(index);

            //////////////////////////////////////////////////////////////////////////////////////////
            /// wait for the notification from the gap handler
            if (xTaskNotifyWait(0, U32_MAX, &notif_value, BLE_GAP_API_TASK_NOTIF_TIMEOUT) != pdPASS)
            {
                ret = ble_gap_err_timeout;
                NRF_LOG_ERROR("dh key notif");
                goto return_mech;
            }
            if (notif_value != BLE_SECEVT_LESC_DHKEY_REQ)
            {
                ret = ble_gap_err_dh_key_cal_failed;
                NRF_LOG_ERROR("dh key not req by sd");
                goto return_mech;
            }
            /// handle the dhkey calculation event
            ret = nrf_start_dhkey_calculation(index);
            if (ret != nrf_OK)
            {
                ret = ble_gap_err_dh_key_cal_failed;
                goto return_mech;
            }

            //////////////////////////////////////////////////////////////////////////////////////////
            /// wait for the notification from the connection security update event
            if (xTaskNotifyWait(0, U32_MAX, &notif_value, BLE_GAP_API_TASK_NOTIF_TIMEOUT) != pdPASS)
            {
                ret = ble_gap_err_timeout;
                goto return_mech;
            }
            // if (notif_value != BLE_SECEVT_CONN_SEC_UPDATE)
            // {
            //     ret = ble_gap_err_security_init_failed;
            //     NRF_LOG_ERROR("security init failed");
            //     goto return_mech;
            // }

            //////////////////////////////////////////////////////////////////////////////////////////
            /// wait for the authentication event
            if (xTaskNotifyWait(0, U32_MAX, &notif_value, BLE_GAP_API_TASK_NOTIF_TIMEOUT) != pdPASS)
            {
                ret = ble_gap_err_timeout;
                goto return_mech;
            }
            if (notif_value != BLE_SECEVT_AUTH_STATUS)
            {
                ret = ble_gap_err_security_init_failed;
                NRF_LOG_ERROR("auth status failed");
                goto return_mech;
            }
            else
            {
                NRF_LOG_INFO("bond stored");
            }

            //////////////////////////////////////////////////////////////////////////////////////////
            //// now store the bond information
            uint8_t uid_to_store = nvs_Get_total_no_of_uid();

            if (uid_to_store >= BLE_GAP_MAX_BOND_USERS_STORED)
            {
                NRF_LOG_ERROR("no more bonds can be stored ");
                ble_gap_disconnect(gap_inst[index].ble_gap_conn_handle);
                gap_inst[index].ble_gap_conn_handle = BLE_CONN_HANDLE_INVALID;
                NRF_LOG_INFO("please delte some bonds ");
                goto return_mech;
            }

            ble_gap_store_bond_info_struct_t new_bond = {0};

            /// copy the bond info
            memcpy(&new_bond.dev_enc_key, gap_inst[index].key_set.keys_own.p_enc_key, sizeof(ble_gap_enc_key_t));
            memcpy(&new_bond.peer_id_info, gap_inst[index].key_set.keys_peer.p_id_key, sizeof(ble_gap_id_key_t));

            ret = nvs_add_data(++uid_to_store, u8_ptr & new_bond, sizeof(new_bond));
            if (ret != nrf_OK)
            {
                NRF_LOG_ERROR("add bondpair %d", ret);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////
        //// check that is  the peer already bonded
        else if (notif_value == BLE_SECEVT_SEC_INFO_REQ)
        {
            

            /// scan throught the differnt bonds stored and match the eddiv and rand
            if (ble_gap_Evt != NULL)
            {
                //// find the rand and ediv
                /// get the total no of store bond present
                uint8_t total_bond_present = nvs_Get_total_no_of_uid();
                ble_gap_store_bond_info_struct_t *store_bonds = NULL;

                for (uint8_t i = 1; i <= total_bond_present; i++)
                {
                    store_bonds = nvs_get_data_pointer(i);
                    //// make sure that it is not null
                    if (store_bonds != NULL)
                    {
                        //// comparing the ediv and random number
                        if (ble_gap_Evt->evt.gap_evt.params.sec_info_request.master_id.ediv ==
                            store_bonds->dev_enc_key.master_id.ediv)
                        {
                            /// compare the random number
                            uint64_t* rand_num_evt = (uint64_t*) ble_gap_Evt->evt.gap_evt.params.sec_info_request.master_id.rand;
                            uint64_t* rand_num_sotred = (uint64_t*) store_bonds->dev_enc_key.master_id.rand;

                            if ((*rand_num_sotred) == (*rand_num_evt))
                            {
                                NRF_LOG_INFO("found bond");

                                ///// reply the security peer
                                ret = sd_ble_gap_sec_info_reply(gap_inst[index].ble_gap_conn_handle, &store_bonds->dev_enc_key.enc_info, &store_bonds->peer_id_info.id_info, NULL);
                                NRF_ASSERT(ret);

                                /// goto found match and wait for the 
                                goto found_match;
                            }
                        }
                    }
                }
            }
            ret = ble_gap_err_bond_info_not_found;
            goto return_mech;


        found_match:
            //////////////////////////////////////////////////////////////////////////////////////////
            /// wait for the notification from the connection security update event
            if (xTaskNotifyWait(0, U32_MAX, &notif_value, BLE_GAP_API_TASK_NOTIF_TIMEOUT) != pdPASS)
            {
                ret = ble_gap_err_timeout;
                goto return_mech;
            }
            if (notif_value != BLE_SECEVT_CONN_SEC_UPDATE)
            {
                ret = ble_gap_err_security_init_failed;
                NRF_LOG_ERROR("security init failed");
                goto return_mech;
            }
        }
    }
    //// @note if a legacy pairing is required then handle here
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

/// @brief this function will delete all the bonds
/// @param  void
/// @return succ/fialure
uint32_t ble_gap_delete_bonds(void)
{

    //// take the mutex
    if (xSemaphoreTake(ble_gap_mutex_handle, BLE_GAP_API_MUTEX_TIMEOUT) != pdPASS)
    {
        return ble_gap_err_mutex_not_avial;
    }

    uint32_t ret = 0;

    /// check how many uids present
    ret = nvs_Get_total_no_of_uid();
    if (ret != 0)
    {
        NRF_LOG_WARNING("eraseing partition");
        ret = nvs_erase_partition();
    }

    /// give back the mutex
    xSemaphoreGive(ble_gap_mutex_handle);

    return ret;
}

/// @todo modify the function according to your need

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

    printf("ltk len %d\r\n", gap_inst[index].key_set.keys_own.p_enc_key->enc_info.ltk_len);
    for (int i = 0; i < 16; i++)
    {
        printf("%x,", gap_inst[index].key_set.keys_own.p_enc_key->enc_info.ltk[i]);
    }
    printf("lesc %d,auth %d\r\n", gap_inst[index].key_set.keys_own.p_enc_key->enc_info.lesc,
           gap_inst[index].key_set.keys_own.p_enc_key->enc_info.auth);

    delay(100);

    printf("ediv %d\r\n", gap_inst[index].key_set.keys_own.p_enc_key->master_id.ediv);
    for (int i = 0; i < sizeof(gap_inst[index].key_set.keys_own.p_enc_key->master_id.rand); i++)
    {
        printf("%x,", gap_inst[index].key_set.keys_own.p_enc_key->master_id.rand[i]);
    }
    printf("\r\n");

    delay(100);

    printf("irk->");

    for (int i = 0; i < sizeof(gap_inst[index].key_set.keys_own.p_id_key->id_info.irk); i++)
    {
        printf("%x,", gap_inst[index].key_set.keys_own.p_id_key->id_info.irk[i]);
    }
    printf("\r\n");

    delay(100);
    printf("addridown %d,ty %d\r\n", gap_inst[index].key_set.keys_own.p_id_key->id_addr_info.addr_id_peer,
           gap_inst[index].key_set.keys_own.p_id_key->id_addr_info.addr_type);

    printf("addrown-");
    for (int i = 0; i < 6; i++)
    {
        printf("%x,", gap_inst[index].key_set.keys_own.p_id_key->id_addr_info.addr[i]);
    }
    printf("\r\n");

    delay(100);

    //////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    NRF_LOG_INFO("peer keys");

    delay(10);

    printf("ltk len %d\r\n", gap_inst[index].key_set.keys_peer.p_enc_key->enc_info.ltk_len);
    for (int i = 0; i < 16; i++)
    {
        printf("%x,", gap_inst[index].key_set.keys_peer.p_enc_key->enc_info.ltk[i]);
    }
    printf("lesc %d,auth %d\r\n", gap_inst[index].key_set.keys_peer.p_enc_key->enc_info.lesc,
           gap_inst[index].key_set.keys_peer.p_enc_key->enc_info.auth);

    delay(100);

    printf("ediv %d\r\n", gap_inst[index].key_set.keys_peer.p_enc_key->master_id.ediv);
    for (int i = 0; i < sizeof(gap_inst[index].key_set.keys_peer.p_enc_key->master_id.rand); i++)
    {
        printf("%x,", gap_inst[index].key_set.keys_peer.p_enc_key->master_id.rand[i]);
    }
    printf("\r\n");

    delay(100);

    printf("irk->");

    for (int i = 0; i < sizeof(gap_inst[index].key_set.keys_peer.p_id_key->id_info.irk); i++)
    {
        printf("%x,", gap_inst[index].key_set.keys_peer.p_id_key->id_info.irk[i]);
    }
    printf("\r\n");

    delay(100);
    printf("addridpeer %d,ty %d\r\n", gap_inst[index].key_set.keys_peer.p_id_key->id_addr_info.addr_id_peer,
           gap_inst[index].key_set.keys_peer.p_id_key->id_addr_info.addr_type);

    printf("addrpeer-");
    for (int i = 0; i < 6; i++)
    {
        printf("%x,", gap_inst[index].key_set.keys_peer.p_id_key->id_addr_info.addr[i]);
    }
    printf("\r\n");

    /// give back the mutex
    xSemaphoreGive(ble_gap_mutex_handle);
}