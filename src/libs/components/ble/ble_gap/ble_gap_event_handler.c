////////// include the gap security params 
#include "ble_gap_func.h"

/////////// include the gatt client module ///////////

//////////// the connection handle 
uint16_t ble_peer_conn_handle = BLE_CONN_HANDLE_INVALID;



extern volatile uint8_t advertisement_state;

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
        NRF_LOG_INFO("connected");
        ble_peer_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        // err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);

        static const ble_gap_conn_params_t device_preferd_conn_params =
            {
                .min_conn_interval = MIN_CONN_INTERVAL,
                .max_conn_interval = MAX_CONN_INTERVAL,
                .slave_latency = SLAVE_LATENCY,
                .conn_sup_timeout = CONN_SUP_TIMEOUT

            };
        // init the connection parameter update request
        sd_ble_gap_conn_param_update(p_ble_evt->evt.gap_evt.conn_handle,
                                     &device_preferd_conn_params);


    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////

    advertisement_state = false;

    }
    break;

    case BLE_GAP_EVT_DISCONNECTED:
    {
        NRF_LOG_ERROR("disconnected");
        // after disconnecting
        ble_peer_conn_handle = BLE_CONN_HANDLE_INVALID;

    }
    break;

    case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
    {
        NRF_LOG_INFO("PHY update request.");
        ble_gap_phys_t const phys =
            {
                // .rx_phys = BLE_GAP_PHY_AUTO,
                // .tx_phys = BLE_GAP_PHY_AUTO,
                .rx_phys = BLE_GAP_PHY_2MBPS,
                .tx_phys = BLE_GAP_PHY_2MBPS};
        err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
        APP_ERROR_CHECK(err_code);
    }
    break;

    case BLE_GAP_EVT_PHY_UPDATE:
    {
        // the phy update rwquest is complete
        NRF_LOG_INFO("updated PHY");
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
        err_code = sd_ble_gap_data_length_update(p_ble_evt->evt.gap_evt.conn_handle, &gap_dl_update, NULL);
    }
    break;

    case BLE_GAP_EVT_DATA_LENGTH_UPDATE:
    {
        NRF_LOG_INFO("dl updated");
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
    }
    break;

    case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
    {
        NRF_LOG_INFO("conn parm update req");
    }
    break;

    case BLE_GAP_EVT_CONN_PARAM_UPDATE:
    {
        NRF_LOG_INFO("con prm udted");
    }
    break;

    case BLE_GAP_EVT_TIMEOUT:
    {
        NRF_LOG_ERROR("gap evt timeout ");
    }
    break;

    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////

        ////////////////// the gap security manager protocol

    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
    {
        NRF_LOG_WARNING("BLE_GAP_EVT_SEC_PARAMS_REQUEST");
    }
    break;

    case BLE_GAP_EVT_SEC_INFO_REQUEST:
    {
        NRF_LOG_WARNING("BLE_GAP_EVT_SEC_INFO_REQUEST");
    }
    break;

    case BLE_GAP_EVT_PASSKEY_DISPLAY:
    {
        NRF_LOG_WARNING("BLE_GAP_EVT_PASSKEY_DISPLAY");
    }
    break;

    case BLE_GAP_EVT_KEY_PRESSED:
    {
        NRF_LOG_WARNING("BLE_GAP_EVT_KEY_PRESSED");
    }
    break;

    case BLE_GAP_EVT_AUTH_KEY_REQUEST:
    {
        NRF_LOG_WARNING("BLE_GAP_EVT_AUTH_KEY_REQUEST");
    }
    break;

    case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
    {
        NRF_LOG_WARNING("BLE_GAP_EVT_LESC_DHKEY_REQUEST");
    }
    break;

    case BLE_GAP_EVT_AUTH_STATUS:
    {
        NRF_LOG_WARNING("BLE_GAP_EVT_AUTH_STATUS");
    }
    break;

    case BLE_GAP_EVT_CONN_SEC_UPDATE:
    {
        NRF_LOG_WARNING("BLE_GAP_EVT_CONN_SEC_UPDATE");
    }
    break;



    case BLE_GAP_EVT_SEC_REQUEST:
    {
        NRF_LOG_WARNING("BLE_GAP_EVT_SEC_REQUEST");
    }
    break;

    default:
        break;
    }
}
