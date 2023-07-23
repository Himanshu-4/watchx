////////// include the gap security params
#include "ble_gap_func.h"


extern const ble_gap_sec_params_t gap_sec_param[ble_gap_security_max_params_supported];

/// @brief callbacks for the BLE GAP events 
extern volatile ble_gap_procdeure_callbacks GAP_Callbacks[ble_gap_max_callback_supp];
 
/// @brief the advertisement state of the device  
extern volatile uint8_t  ble_advertisement_State;

/// @brief ble gap instance for multidevice 
extern volatile ble_gap_inst_Struct_t gap_inst[BLE_GAP_MAX_NO_OF_DEVICES];

// ble_gap_enc_key_t dev_enc_key1;

// ble_gap_enc_key_t dev_enc_key2;


// ble_gap_sec_keyset_t key_Set = {
//     /// owner key distribuition
//     .keys_own.p_enc_key = &dev_enc_key1,
//     .keys_own.p_id_key = NULL,
//     .keys_own.p_pk = &public_key_device,
//     .keys_own.p_sign_key = NULL,

//     ///// peer key distribution
//     .keys_peer.p_enc_key = NULL,
//     .keys_peer.p_id_key = NULL,
//     .keys_peer.p_pk = &public_key_peer,
//     .keys_peer.p_sign_key = NULL};



static volatile uint16_t conn_handle =0;

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

        uint8_t index = 10;
        // set the connection handle
        ble_gap_set_conn_handle(&index, conn_handle);

        /////// only call the callback when we have a valid index
        if (index <= BLE_GAP_MAX_NO_OF_DEVICES)
        {
            // call the callback
            if (GAP_Callbacks[ble_gap_evt_connected] != NULL)
            {
                ////// pass the index to the callback
                GAP_Callbacks[ble_gap_evt_connected](&index, &p_ble_evt->evt.gap_evt);
            }
        }
        else
        {
            ///////// disconnect the device
            ble_gap_disconnect(conn_handle);
        }
        ////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////

        ble_advertisement_State = false;
    }
    break;

    case BLE_GAP_EVT_DISCONNECTED:
    {
        NRF_LOG_ERROR("dct %d", p_ble_evt->evt.gap_evt.params.disconnected.reason);
        // after disconnecting

        /// remove the connection handle
        if (ble_gap_ok == ble_gap_remove_conn_handle(conn_handle))
        {
            /// only call the callback for a valid conn hadnle
            if (GAP_Callbacks[ble_gap_evt_disconnected] != NULL)
            {
                GAP_Callbacks[ble_gap_evt_disconnected](NULL, &p_ble_evt->evt.gap_evt);
            }
        }
    }
    break;

    case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
    {
        NRF_LOG_INFO("PHYurequest.");
        ble_gap_phys_t const phys =
            {
                // .rx_phys = BLE_GAP_PHY_AUTO,
                // .tx_phys = BLE_GAP_PHY_AUTO,
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
        // NRF_LOG_INFO("dl updated");
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
        // NRF_LOG_INFO("con prm udted");
    }
    break;

    case BLE_GAP_EVT_TIMEOUT:
    {
        /// disconnect the device and remove the conn handle
        ble_gap_disconnect(conn_handle);

        ////// call the callback

        if (GAP_Callbacks[ble_gap_evt_timeout] != NULL)
        {
            GAP_Callbacks[ble_gap_evt_timeout]((void *)&conn_handle, &p_ble_evt->evt.gap_evt);
        }
    }
    break;

        /////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////

        ////////////////// the gap security manager protocol

    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
    {
        NRF_LOG_WARNING("GAP_EVT_SEC_PARAMS_REQUEST");

        delay(100);
        /// show the ble gpa security param by central
        NRF_LOG_INFO("%x,%x,%x,%x,%x,%x|| %d,%d ||%x,%x,%x,%x ||%x,%x,%x,%x", p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.bond,
                     p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.mitm, p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.lesc,
                     p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.keypress, p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.io_caps,
                     p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.oob, p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.min_key_size,
                     p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.max_key_size, p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.kdist_own.enc,
                     p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.kdist_own.id, p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.kdist_own.sign,
                     p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.kdist_own.link, p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.kdist_peer.enc,
                     p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.kdist_peer.id, p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.kdist_peer.sign,
                     p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.kdist_peer.link);

        err_code = sd_ble_gap_sec_params_reply(conn_handle, BLE_GAP_SEC_STATUS_SUCCESS, &gap_sec_param[ble_gap_security_param_index], &key_Set);
        NRF_ASSERT(err_code);
    }
    break;

    case BLE_GAP_EVT_SEC_INFO_REQUEST:
    {
        NRF_LOG_WARNING("GAP_EVT_SEC_INFO_REQUEST");
    }
    break;

    case BLE_GAP_EVT_PASSKEY_DISPLAY:
    {
        NRF_LOG_WARNING("GAP_EVT_PASSKEY_DISPLAY");

        /// get the passkkey
        NRF_LOG_INFO("%d", p_ble_evt->evt.gap_evt.params.passkey_display.match_request);

        // memcpy(passkey, p_ble_evt->evt.gap_evt.params.passkey_display.passkey, sizeof(passkey));
        if (p_ble_evt->evt.gap_evt.params.passkey_display.match_request)
        {
            err_code = sd_ble_gap_auth_key_reply(conn_handle, BLE_GAP_AUTH_KEY_TYPE_PASSKEY, NULL);
            NRF_ASSERT(err_code);
        }
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
        NRF_LOG_INFO("%d, %d", p_ble_evt->evt.gap_evt.params.lesc_dhkey_request.oobd_req, p_ble_evt->evt.gap_evt.params.lesc_dhkey_request.p_pk_peer->pk[0]);

        err_code = sd_ble_gap_lesc_dhkey_reply(conn_handle, &my_dhkey);
    }
    break;

    case BLE_GAP_EVT_AUTH_STATUS:
    {
        NRF_LOG_WARNING("GAP_EVT_AUTH_STATUS");
    }
    break;

    case BLE_GAP_EVT_CONN_SEC_UPDATE:
    {
        NRF_LOG_WARNING("GAP_EVT_CONN_SEC_UPDATE");

        /// call the callback
        if (GAP_Callbacks[ble_gap_evt_sec_procedure_cmpt] != NULL)
        {
            GAP_Callbacks[ble_gap_evt_sec_procedure_cmpt](NULL, &p_ble_evt->evt.gap_evt);
        }
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
