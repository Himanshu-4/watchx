////////// include the gap security params
#include "ble_gap_func.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

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

/// @brief this is to get the task handle
extern volatile xTaskHandle ble_gap_taskhandle;

/// @brief this is used to store the pointer to a gap event
extern volatile ble_evt_t *ble_gap_Evt;
//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// static declarations here

static volatile uint16_t conn_handle = 0;

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

/// @brief this funtion is to handle the paskkey display event
/// @param p_ble_evt
static void nrf_handle_passkey_display_Evt(ble_evt_t const *p_ble_evt);

/// @brief this function is used to handle the connection security update process
/// @param p_ble_evt
static void nrf_handle_conn_security_update(ble_evt_t const *p_ble_evt);

/// @brief this function is used to handle the dh key reqquest
/// @param p_ble_evt
static void nrf_handle_lesc_dhkey_request(ble_evt_t const *p_ble_evt);

//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// macro functions

#define task_notify(x)                                              \
    if (ble_gap_taskhandle != NULL)                                 \
    {                                                               \
        xTaskNotify(ble_gap_taskhandle, x, eSetValueWithOverwrite); \
    }

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

        /// only call the callback for a valid conn hadnle
        if (GAP_Callbacks[ble_gap_evt_disconnected].callback != NULL)
        {
            GAP_Callbacks[ble_gap_evt_disconnected].callback(
                GAP_Callbacks[ble_gap_evt_disconnected].callback_param, &p_ble_evt->evt.gap_evt);
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
        NRF_LOG_INFO("connparm updatereq");
        const ble_gap_conn_params_t device_preferd_conn_params =
            {
                .min_conn_interval = MIN_CONN_INTERVAL,
                .max_conn_interval = MAX_CONN_INTERVAL,
                .slave_latency = SLAVE_LATENCY,
                .conn_sup_timeout = CONN_SUP_TIMEOUT

            };
        // init the connection parameter update request
        sd_ble_gap_conn_param_update(conn_handle,
                                     &device_preferd_conn_params);
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
        NRF_LOG_WARNING("SEC_PARAMS_REQUEST");
        nrf_handle_security_param_request(p_ble_evt);
    }
    break;

    case BLE_GAP_EVT_SEC_INFO_REQUEST:
    {
        NRF_LOG_WARNING("SEC_INFO_REQUEST");
        nrf_handle_security_info_request(p_ble_evt);
    }
    break;

    case BLE_GAP_EVT_PASSKEY_DISPLAY:
    {
        NRF_LOG_WARNING("PASSKEY_DISPLAY");
        nrf_handle_passkey_display_Evt(p_ble_evt);
    }
    break;

    case BLE_GAP_EVT_KEY_PRESSED:
    {
        NRF_LOG_WARNING("KEY_PRESSED");
    }
    break;

    case BLE_GAP_EVT_AUTH_KEY_REQUEST:
    {
        NRF_LOG_WARNING("AUTH_KEY_REQUEST");
    }
    break;

    case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
    {
        NRF_LOG_WARNING("LESC_DHKEY_REQUEST");
        nrf_handle_lesc_dhkey_request(p_ble_evt);
    }
    break;

    case BLE_GAP_EVT_CONN_SEC_UPDATE:
    {
        NRF_LOG_WARNING("CONN_SEC_UPDATE");
        nrf_handle_conn_security_update(p_ble_evt);

        /// call the callback
        // if (GAP_Callbacks[ble_gap_evt_sec_procedure_cmpt].callback != NULL)
        // {
        //     GAP_Callbacks[ble_gap_evt_sec_procedure_cmpt].callback(
        //         GAP_Callbacks[ble_gap_evt_disconnected].callback_param, &p_ble_evt->evt.gap_evt);
        // }
    }
    break;

    case BLE_GAP_EVT_AUTH_STATUS:
    {
        NRF_LOG_WARNING("AUTH_STATUS");

        nrf_handle_authentication_status(p_ble_evt);
    }
    break;

    case BLE_GAP_EVT_SEC_REQUEST:
    {
        NRF_LOG_WARNING("SEC_REQUEST");
    }
    break;

    default:
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// static functions definations  here //////////////////////////////////////////////////

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

    //// notify the task about that a new bond formation is started
    task_notify(BLE_SECEVT_NOTIF_SEC_PARAM_REQ);
}

/// @brief this function is used to load the peripheral keys and reply to the phone
/// @param conn_handle
/// @return
static void nrf_handle_security_info_request(ble_evt_t const *p_ble_evt)
{
    ble_gap_Evt = (ble_evt_t *)p_ble_evt;
    /// log the security info
    // NRF_LOG_INFO("add %d,t %d,ediv %d,rnad %d,if %x",
    //              p_ble_evt->evt.gap_evt.params.sec_info_request.peer_addr.addr_id_peer,
    //              p_ble_evt->evt.gap_evt.params.sec_info_request.peer_addr.addr_type,
    //              p_ble_evt->evt.gap_evt.params.sec_info_request.master_id.ediv,
    //              p_ble_evt->evt.gap_evt.params.sec_info_request.master_id.rand[2],
    //              // the omfp required
    //              ((p_ble_evt->evt.gap_evt.params.sec_info_request.enc_info << 2) |
    //               (p_ble_evt->evt.gap_evt.params.sec_info_request.id_info << 1) |
    //               (p_ble_evt->evt.gap_evt.params.sec_info_request.sign_info)));

    delay(20);
    /// notify the task about that peer is already bonded and want the ltk
    task_notify(BLE_SECEVT_NOTIF_SEC_INFO_REQ);
}

/// @brief this funtion is to handle the paskkey display event
/// @param p_ble_evt
static void nrf_handle_passkey_display_Evt(ble_evt_t const *p_ble_evt)
{
    uint16_t conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

    /// check that match request is present or not
    if (p_ble_evt->evt.gap_evt.params.passkey_display.match_request == 1)
    {
        uint32_t err = sd_ble_gap_auth_key_reply(conn_handle, BLE_GAP_AUTH_KEY_TYPE_PASSKEY, NULL);
    }
    else
    {
        NRF_LOG_ERROR("no match");
    }
}

/// @brief this function is used to handle the connection security update process
/// @param p_ble_evt
static void nrf_handle_conn_security_update(ble_evt_t const *p_ble_evt)
{
    /// after running the function notify the task
    /// log about the security status
    // NRF_LOG_INFO("%d,%d,%d", p_ble_evt->evt.gap_evt.params.conn_sec_update.conn_sec.sec_mode.sm,
    //              p_ble_evt->evt.gap_evt.params.conn_sec_update.conn_sec.sec_mode.lv, p_ble_evt->evt.gap_evt.params.conn_sec_update.conn_sec.encr_key_size);

    task_notify(BLE_SECEVT_NOTIF_CONN_SEC_UPDATE);
}

/// @brief this function is used to handle the dh key reqquest
/// @param p_ble_evt
static void nrf_handle_lesc_dhkey_request(ble_evt_t const *p_ble_evt)
{
    // get the dhkey
    // NRF_LOG_INFO("%d, %d", p_ble_evt->evt.gap_evt.params.lesc_dhkey_request.oobd_req, p_ble_evt->evt.gap_evt.params.lesc_dhkey_request.p_pk_peer->pk[0]);

    // notify the task that the device is suceesfully bonded
    task_notify(BLE_SECEVT_NOTIF_LESC_DHKEY_REQ);
}

/**@brief GAP connection security modes.
 *
 * Security Mode 0 Level 0: No access permissions at all (this level is not defined by the Bluetooth Core specification).\n
 * Security Mode 1 Level 1: No security is needed (aka open link).\n
 * Security Mode 1 Level 2: Encrypted link required, MITM protection not necessary.\n
 * Security Mode 1 Level 3: MITM protected encrypted link required.\n
 * Security Mode 1 Level 4: LESC MITM protected encrypted link using a 128-bit strength encryption key required.\n
 * Security Mode 2 Level 1: Signing or encryption required, MITM protection not necessary.\n
 * Security Mode 2 Level 2: MITM protected signing required, unless link is MITM protected encrypted.\n
 */

/// @brief this function is handle the authentication status of the data
/// @param p_ble_evt
static void nrf_handle_authentication_status(ble_evt_t const *p_ble_evt)
{

    NRF_LOG_INFO("au %x,e %d,b %d,l %d,s1 %x,s2 %x,ko %x,kp %x",
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

    task_notify(BLE_SECEVT_NOTIF_AUTH_STATUS);
}
