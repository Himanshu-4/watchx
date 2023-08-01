////////// include the gap security params
#include "ble_gap_func.h"

#include "uECC.h"

#include "nvs.h"

//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// externs vars

extern const ble_gap_sec_params_t gap_sec_param[ble_gap_security_max_params_supported];

/// @brief callbacks for the BLE GAP events
extern volatile ble_gap_callback_struct_t GAP_Callbacks[ble_gap_max_callback_supp];

/// @brief the advertisement state of the device
extern volatile uint8_t ble_advertisement_State;

/// @brief ble gap instance for multidevice
extern ble_gap_inst_Struct_t gap_inst[BLE_GAP_MAX_NO_OF_DEVICES];

//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// static declarations here

static volatile uint16_t conn_handle = 0;

static ble_gap_addr_t connected_peer_addr = {0};

/// @brief this is to start the dhkey calculation
/// @param conn_handle
/// @return err code
static uint32_t nrf_start_dhkey_calculation(ble_evt_t const *p_ble_evt);

/// @brief this function is used to load the peripheral keys and reply to the phone
/// @param conn_handle
/// @return
static void nrf_handle_security_info_request(ble_evt_t const *p_ble_evt);

/// @brief this function is used to handle the security parameter request
/// @param p_ble_evt
static void nrf_handle_security_param_request(ble_evt_t const *p_ble_evt);

/// @brief this function is handle the authentication status of the data
/// @param p_ble_evt
static void nrf_handle_authentication_status(ble_evt_t const *p_ble_evt);

//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// ble gpa event handler
void ble_gap_event_handler(ble_evt_t const *p_ble_evt)
{

    uint32_t err_code = 0;
    switch (p_ble_evt->header.evt_id)
    {
    //////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////   BLE GAP events //////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    ///////////////       there are 22 gap events ////////////////////////////////////////////
    case BLE_GAP_EVT_CONNECTED:
    {
        // NRF_LOG_INFO("connected");
        conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        static const ble_gap_conn_params_t device_preferd_conn_params =
            {
                .min_conn_interval = MIN_CONN_INTERVAL,
                .max_conn_interval = MAX_CONN_INTERVAL,
                .slave_latency = SLAVE_LATENCY,
                .conn_sup_timeout = CONN_SUP_TIMEOUT

            };
        // init the connection parameter update request
        sd_ble_gap_conn_param_update(conn_handle,
                                     &device_preferd_conn_params);

        delay(50);
        memcpy(&connected_peer_addr, &p_ble_evt->evt.gap_evt.params.connected.peer_addr, sizeof(connected_peer_addr));
        // call the callback
        if (GAP_Callbacks[ble_gap_evt_connected].callback != NULL)
        {
            ////// pass the index to the callback
            GAP_Callbacks[ble_gap_evt_connected].callback(
                GAP_Callbacks[ble_gap_evt_connected].callback_param, &p_ble_evt->evt.gap_evt);
        }

        ////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////

        ble_advertisement_State = false;
    }
    break;

    case BLE_GAP_EVT_DISCONNECTED:
    {
        NRF_LOG_ERROR("dct %x", p_ble_evt->evt.gap_evt.params.disconnected.reason);
        // after disconnecting

        uint8_t index = ble_gap_get_gap_index(conn_handle);
        if (index < BLE_MAX_DEVICE_SUPPORTED)
        {
            /// remove the connection handle
            if (ble_gap_ok == ble_gap_remove_conn_handle(index))
            {
                /// only call the callback for a valid conn hadnle
                if (GAP_Callbacks[ble_gap_evt_disconnected].callback != NULL)
                {
                    GAP_Callbacks[ble_gap_evt_disconnected].callback(
                        GAP_Callbacks[ble_gap_evt_disconnected].callback_param, &p_ble_evt->evt.gap_evt);
                }
            }
        }
    }
    break;

    case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
    {
        NRF_LOG_INFO("PHYurequest.");
        ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_2MBPS,
                .tx_phys = BLE_GAP_PHY_2MBPS};
        err_code = sd_ble_gap_phy_update(conn_handle, &phys);
        APP_ERROR_CHECK(err_code);
    }
    break;

    case BLE_GAP_EVT_PHY_UPDATE:
    {
        // the phy update rwquest is complete
        // NRF_LOG_INFO("updated PHY");
    }
    break;

    // advertisement report updated
    case BLE_GAP_EVT_ADV_REPORT:
    {
        NRF_LOG_WARNING("adv report ");
    }
    break;

    case BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST:
    {
        NRF_LOG_INFO("req dl update");

        ///// enable the data length extension
        ble_gap_data_length_params_t const gap_dl_update =
            {
                .max_rx_octets = 251,
                .max_rx_time_us = 2120,
                .max_tx_octets = 251,
                .max_tx_time_us = 2120,

            };
        // NULL is used if the master doesnot support that dl request it store the values  whats actually support by master
        err_code = sd_ble_gap_data_length_update(conn_handle, &gap_dl_update, NULL);
    }
    break;

    case BLE_GAP_EVT_DATA_LENGTH_UPDATE:
    {
        NRF_LOG_INFO("dlupdated");
    }
    break;

    case BLE_GAP_EVT_SCAN_REQ_REPORT:
    {
        // NRF_LOG_INFO("scan req");
    }
    break;

    case BLE_GAP_EVT_ADV_SET_TERMINATED:
    {
        NRF_LOG_WARNING("adv set trm");
        ble_advertisement_State = false;
    }
    break;

    case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
    {
        NRF_LOG_INFO("conn parm update req");
    }
    break;

    case BLE_GAP_EVT_CONN_PARAM_UPDATE:
    {
        NRF_LOG_INFO("conprm udted");
    }
    break;

    case BLE_GAP_EVT_TIMEOUT:
    {
        /// disconnect the device and remove the conn handle
        ble_gap_disconnect(conn_handle);

        ////// call the callback

        if (GAP_Callbacks[ble_gap_evt_timeout].callback != NULL)
        {
            GAP_Callbacks[ble_gap_evt_timeout].callback(
                GAP_Callbacks[ble_gap_evt_disconnected].callback_param, &p_ble_evt->evt.gap_evt);
        }
    }
    break;

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////

        ////////////////// the gap security manager protocol

    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
    {
        NRF_LOG_WARNING("GAP_EVT_SEC_PARAMS_REQUEST");
        nrf_handle_security_param_request(p_ble_evt);
    }
    break;

    case BLE_GAP_EVT_SEC_INFO_REQUEST:
    {
        NRF_LOG_WARNING("GAP_EVT_SEC_INFO_REQUEST");
        nrf_handle_security_info_request(p_ble_evt);
    }
    break;

    case BLE_GAP_EVT_PASSKEY_DISPLAY:
    {
        NRF_LOG_WARNING("GAP_EVT_PASSKEY_DISPLAY");

        // /// get the passkkey
        // NRF_LOG_INFO("%d", p_ble_evt->evt.gap_evt.params.passkey_display.match_request);

        // // memcpy(passkey, p_ble_evt->evt.gap_evt.params.passkey_display.passkey, sizeof(passkey));
        // if (p_ble_evt->evt.gap_evt.params.passkey_display.match_request)
        // {
        //     err_code = sd_ble_gap_auth_key_reply(conn_handle, BLE_GAP_AUTH_KEY_TYPE_PASSKEY, NULL);
        //     NRF_ASSERT(err_code);
        // }
    }
    break;

    case BLE_GAP_EVT_KEY_PRESSED:
    {
        NRF_LOG_WARNING("GAP_EVT_KEY_PRESSED");
    }
    break;

    case BLE_GAP_EVT_AUTH_KEY_REQUEST:
    {
        NRF_LOG_WARNING("GAP_EVT_AUTH_KEY_REQUEST");
    }
    break;

    case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
    {
        NRF_LOG_WARNING("GAP_EVT_LESC_DHKEY_REQUEST");
        // get the dhkey
        // NRF_LOG_INFO("%d, %d", p_ble_evt->evt.gap_evt.params.lesc_dhkey_request.oobd_req, p_ble_evt->evt.gap_evt.params.lesc_dhkey_request.p_pk_peer->pk[0]);

        nrf_start_dhkey_calculation(p_ble_evt);
    }
    break;

    case BLE_GAP_EVT_CONN_SEC_UPDATE:
    {
        NRF_LOG_WARNING("GAP_EVT_CONN_SEC_UPDATE");

        /// log about the security status
        NRF_LOG_INFO("%d,%d,%d", p_ble_evt->evt.gap_evt.params.conn_sec_update.conn_sec.sec_mode.sm,
                     p_ble_evt->evt.gap_evt.params.conn_sec_update.conn_sec.sec_mode.lv, p_ble_evt->evt.gap_evt.params.conn_sec_update.conn_sec.encr_key_size);

        /// call the callback
        if (GAP_Callbacks[ble_gap_evt_sec_procedure_cmpt].callback != NULL)
        {
            GAP_Callbacks[ble_gap_evt_sec_procedure_cmpt].callback(
                GAP_Callbacks[ble_gap_evt_disconnected].callback_param, &p_ble_evt->evt.gap_evt);
        }
    }
    break;

    case BLE_GAP_EVT_AUTH_STATUS:
    {
        NRF_LOG_WARNING("GAP_EVT_AUTH_STATUS");

        nrf_handle_authentication_status(p_ble_evt);
    }
    break;

    case BLE_GAP_EVT_SEC_REQUEST:
    {
        NRF_LOG_WARNING("GAP_EVT_SEC_REQUEST");
    }
    break;

    default:
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// static functions here //////////////////////////////////////////////////

/// @brief this function is used to handle the security parameter request
/// @param p_ble_evt
static void nrf_handle_security_param_request(ble_evt_t const *p_ble_evt)
{
    // show the ble gpa security param by central
    NRF_LOG_INFO("%x,%x,%x,%x,%x,%x|| %d,%d ||%x,%x,%x,%x ||%x,%x,%x,%x", p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.bond,
                 p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.mitm, p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.lesc,
                 p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.keypress, p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.io_caps,
                 p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.oob, p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.min_key_size,
                 p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.max_key_size, p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.kdist_own.enc,
                 p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.kdist_own.id, p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.kdist_own.sign,
                 p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.kdist_own.link, p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.kdist_peer.enc,
                 p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.kdist_peer.id, p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.kdist_peer.sign,
                 p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.kdist_peer.link);

    uint32_t err_code = 0;

    /// check for existing keys and delete it

    // //// now we can store the pairing key
    // uint32_t uid = (connected_peer_addr.addr[0] |
    //                 (connected_peer_addr.addr[1] << 1) | (connected_peer_addr.addr[2] << 2) |
    //                 (connected_peer_addr.addr[3] << 3));

    // err_code = nvs_delete_data(uid);
    // if (err_code != nrf_OK)
    // {
    //     NRF_LOG_ERROR("del %d", err_code);
    // }
    /// get the index from the connection handle
    uint8_t index = ble_gap_get_gap_index(p_ble_evt->evt.gap_evt.conn_handle);
    /// check for a valid index
    if (index < BLE_GAP_MAX_NO_OF_DEVICES)
    {
        err_code = sd_ble_gap_sec_params_reply(p_ble_evt->evt.gap_evt.conn_handle, BLE_GAP_SEC_STATUS_SUCCESS, &gap_sec_param[gap_inst[index].ble_gap_security_param_index], (ble_gap_sec_keyset_t *)&gap_inst[index].key_set);
        NRF_ASSERT(err_code);
    }
    else 
    {
        NRF_LOG_ERROR("max index");
    }
}

/// @brief this function is used to load the peripheral keys and reply to the phone
/// @param conn_handle
/// @return
static void nrf_handle_security_info_request(ble_evt_t const *p_ble_evt)
{

    /// log the security info
    NRF_LOG_INFO("add %d,t %d,ediv %d,rnad %d,if %x",
                 p_ble_evt->evt.gap_evt.params.sec_info_request.peer_addr.addr_id_peer,
                 p_ble_evt->evt.gap_evt.params.sec_info_request.peer_addr.addr_type,
                 p_ble_evt->evt.gap_evt.params.sec_info_request.master_id.ediv,
                 p_ble_evt->evt.gap_evt.params.sec_info_request.master_id.rand[2],
                 // the omfp required
                 ((p_ble_evt->evt.gap_evt.params.sec_info_request.enc_info << 2) |
                  (p_ble_evt->evt.gap_evt.params.sec_info_request.id_info << 1) |
                  (p_ble_evt->evt.gap_evt.params.sec_info_request.sign_info)));

    uint32_t err_code = 0;

    /// get the index
    uint8_t index = ble_gap_get_gap_index(p_ble_evt->evt.gap_evt.conn_handle);

    if (index < BLE_MAX_DEVICE_SUPPORTED)
    {
        ble_gap_enc_info_t peer_en_info =
            {
                .auth = gap_inst[index].key_set.keys_own.p_enc_key->enc_info.auth,
                .lesc = gap_inst[index].key_set.keys_own.p_enc_key->enc_info.lesc,
                .ltk_len = gap_inst[index].key_set.keys_own.p_enc_key->enc_info.ltk_len,
            };

        //// now we can store the pairing key
        // uint32_t uid = (connected_peer_addr.addr[0] |
        //                 (connected_peer_addr.addr[1] << 1) | (connected_peer_addr.addr[2] << 2) |
        //                 (connected_peer_addr.addr[3] << 3));
        // /// get the data from uid
        // err_code = nvs_read_data(uid, peer_en_info.ltk, peer_en_info.ltk_len);
        // if (err_code != nrf_OK)
        // {
        //     NRF_LOG_ERROR("keys read %d", err_code);
        //     return err_code;
        // }

        ///// reply the security peer
        err_code = sd_ble_gap_sec_info_reply(p_ble_evt->evt.gap_evt.conn_handle, &peer_en_info, NULL, NULL);
        NRF_ASSERT(err_code);
    }
}

/// @brief this function is handle the authentication status of the data
/// @param p_ble_evt
static void nrf_handle_authentication_status(ble_evt_t const *p_ble_evt)
{

    // NRF_LOG_INFO("au %d,e %d,b %d,l %d,s1 %x,s2%x,ko %x,kp %x",
    //              p_ble_evt->evt.gap_evt.params.auth_status.auth_status,
    //              p_ble_evt->evt.gap_evt.params.auth_status.error_src,
    //              p_ble_evt->evt.gap_evt.params.auth_status.bonded,
    //              p_ble_evt->evt.gap_evt.params.auth_status.lesc,

    //              //// the sm1 level
    //              ((p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv1 << 3) |
    //               (p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv2 << 2) |
    //               (p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv3 << 1) |
    //               (p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv4)),

    //              /// sm2 level
    //              ((p_ble_evt->evt.gap_evt.params.auth_status.sm2_levels.lv1 << 3) |
    //               (p_ble_evt->evt.gap_evt.params.auth_status.sm2_levels.lv2 << 2) |
    //               (p_ble_evt->evt.gap_evt.params.auth_status.sm2_levels.lv3 << 1) |
    //               (p_ble_evt->evt.gap_evt.params.auth_status.sm2_levels.lv4)),

    //              /// kdist owner
    //              ((p_ble_evt->evt.gap_evt.params.auth_status.kdist_own.enc << 3) |
    //               (p_ble_evt->evt.gap_evt.params.auth_status.kdist_own.id << 2) |
    //               (p_ble_evt->evt.gap_evt.params.auth_status.kdist_own.link << 1) |
    //               (p_ble_evt->evt.gap_evt.params.auth_status.kdist_own.sign)),

    //              /// kdist peer
    //              ((p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer.enc << 3) |
    //               (p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer.id << 2) |
    //               (p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer.link << 1) |
    //               (p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer.sign)));

    uint32_t err = 0;
    /// get the index from the conn handle
    uint8_t index = ble_gap_get_gap_index(conn_handle);

    if (index < BLE_MAX_DEVICE_SUPPORTED)
    {
        // //// now we can store the pairing key
        // uint32_t uid = (connected_peer_addr.addr[0] |
        //                 (connected_peer_addr.addr[1] << 1) | (connected_peer_addr.addr[2] << 2) |
        //                 (connected_peer_addr.addr[3] << 3));

        // err =nvs_add_data(uid, gap_inst[index].key_set.keys_own.p_enc_key->enc_info.ltk, gap_inst[index].key_set.keys_own.p_enc_key->enc_info.ltk_len);
        // if(err != nrf_OK)
        // {
        //     NRF_LOG_ERROR("addk %d",err);
        // }
    }
}

/// @brief this is to start the dhkey calculation
/// @param conn_handle
/// @return err code
static uint32_t nrf_start_dhkey_calculation(ble_evt_t const *p_ble_evt)
{
    /// get the index from the conn handle
    uint32_t err_code = 0;
    static ble_gap_lesc_dhkey_t peer_dh_key;

    /// get the index from the connection handle
    uint8_t index = ble_gap_get_gap_index(conn_handle);
    /// check for a valid index
    if (index >= BLE_GAP_MAX_NO_OF_DEVICES)
    {
        return ble_gap_err_device_index_invalid;
    }

    //// first check that is the public key of peer is valid or not
    // err_code = uECC_valid_public_key(gap_inst[index].key_set.keys_peer.p_pk->pk, uECC_secp256r1());
    // err_code = uECC_valid_public_key(p_ble_evt->evt.gap_evt.params.lesc_dhkey_request.p_pk_peer->pk, uECC_secp256r1());
    // if (err_code != 1)
    // {
    //     NRF_LOG_ERROR("inv peer pub key");
    //     return nrf_ERR_OPERATION_FAILED;
    // }
   
    /// compute here the shared secret from our private key and peer public key
    err_code = uECC_shared_secret(u8_ptr gap_inst[index].key_set.keys_peer.p_pk->pk, u8_ptr gap_inst[index].private_key_device,
                                  peer_dh_key.key, uECC_secp256r1());
    if (err_code != 1)
    {
        NRF_LOG_ERROR("inv dh key");
        return nrf_ERR_OPERATION_FAILED;
    }

    err_code = sd_ble_gap_lesc_dhkey_reply(conn_handle, &peer_dh_key);
    NRF_ASSERT(err_code);

    return err_code;
}