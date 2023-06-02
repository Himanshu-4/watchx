
#include "ble_gap_func.h"

////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////   GAP related functions  implimented here 

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  global variables here 

volatile ble_gap_procdeure_callbacks GAP_Callbacks[ble_gap_max_callback_supp] = {NULL};

///////////// this is to store the connection handle 
static volatile uint16_t ble_gap_conn_handles[BLE_GAP_MAX_NO_OF_DEVICES];



/// @brief this is to add the callback to the particular callback 
/// @param callback_type 
/// @param conn_handle 
/// @param callbacks 
void ble_gap_add_callback(uint8_t callback_type, ble_gap_procdeure_callbacks callbacks)
{
    //// find the index by the connection handle 

    if(callback_type > ble_gap_max_callback_supp) return ;
    ///// this is to set the callback 
    if(GAP_Callbacks[callback_type] == NULL)
    {
        //// add the callback 
        GAP_Callbacks[callback_type] = callbacks;
    }
}

/// @brief this is to remove the gap callbacks 
/// @param callback_type
void ble_gap_remove_callback(uint8_t callback_type)
{
    
    if(callback_type > ble_gap_max_callback_supp) return ;
    ///// this is to set the callback 
        //// add the callback 
        GAP_Callbacks[callback_type] = NULL;
    
}


/// @brief this is to set a connection handle to a particular index 
/// @param index 
/// @return the connection handle of particular index 
uint32_t ble_gap_set_conn_handle(uint8_t * index , uint16_t conn_handle)
{
    ///// search for the 0 connection handle 
    //// serach for the connection handle 
    for(uint8_t i = 0; i<BLE_GAP_MAX_NO_OF_DEVICES; i++)
    {
        if(ble_gap_conn_handles[i] == 0)
        {
            ble_gap_conn_handles[i] = conn_handle;
            *index = i;
            return nrf_OK;
        }
    }

    *index = BLE_GAP_MAX_NO_OF_DEVICES +1;
    return nrf_ERR_OUT_OF_MEM;
    
}



/// @brief this is to get the connection handle of the connected device 
/// @param  index of the device 
/// @return the conection handle , 
uint16_t ble_gap_get_conn_handle(uint8_t index )
{
    if(index > BLE_GAP_MAX_NO_OF_DEVICES)
    {
        return BLE_CONN_HANDLE_INVALID;
    }
    return ble_gap_conn_handles[index];
}


/// @brief this function is used to remove the connection handle from the global array list 
/// @param connection handle 
/// @return succ/failure of function 
uint8_t ble_gap_remove_conn_handle(uint16_t conn_handle)
{
    uint8_t ret = ble_gap_err_conn_handle_invalid;
    //// serach for the connection handle 
    for(uint8_t i = 0; i<BLE_GAP_MAX_NO_OF_DEVICES; i++)
    {
        if(ble_gap_conn_handles[i] == conn_handle)
        {
            ble_gap_conn_handles[i] = BLE_CONN_HANDLE_INVALID;
            ret = ble_gap_ok;
            break;
        }
    }

    return ret;
}

/// @brief this is to disconnect the device and also remove the connection handle from the connected device array 
/// @param conn_handle
void ble_gap_disconnect(uint16_t conn_handle)
{
    /////////// just disconnect the device from the master 

    sd_ble_gap_disconnect(conn_handle ,BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION );
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////// GLobal variables for storing the connectin hadles

static uint8_t adv_conn_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;



static void gap_service_init(void)
{
    uint32_t err_code;

    // BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    /// no level of security and no encryption at all
    static const ble_gap_conn_sec_mode_t sec_mode =
        {
            .lv = 1,
            .sm = 1};

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));

    check_assrt(err_code, "devnameset");

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT);

    check_assrt(err_code, "gap_appr_set");

    static const ble_gap_conn_params_t gap_conn_params =
        {
            .min_conn_interval = MIN_CONN_INTERVAL,
            .max_conn_interval = MAX_CONN_INTERVAL,
            .slave_latency = SLAVE_LATENCY,
            .conn_sup_timeout = CONN_SUP_TIMEOUT

        };
    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);

    check_assrt(err_code, "ble_gap_ppcp_set");
}

/**@brief Function for initializing the Advertising functionality. */
static void advertising_init(void)
{

    static const uint8_t ble_gap_adv_data_buff[BLE_GAP_ADV_SET_DATA_SIZE_MAX] =
        {0x03, BLE_GAP_AD_TYPE_APPEARANCE, LSB16(BLE_APPEARANCE_UNKNOWN), MSB16(BLE_APPEARANCE_UNKNOWN),
         0x02, BLE_GAP_AD_TYPE_FLAGS, BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE,
         0x11, BLE_GAP_AD_TYPE_SOLICITED_SERVICE_UUIDS_128BIT,  0xd0, 0x00, 0x2d, 0x12, 0x1e, 0x4b, 0x0f, 0xa4, 0x99, 0x4e, 0xce, 0xb5, 0x31, 0xf4, 0x05, 0x79,
         0x05, BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, 'W', 'A', 'T', 'X' };

    static uint8_t ble_gap_scan_rsp_adv_data_buff[BLE_GAP_ADV_SET_DATA_SIZE_MAX] =
        {0x03, 0x19, 0x41, 0x03,
         0x07, BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, 'W', 'A', 'T', 'C', 'H', 'X' };

    // static uint8_t ble_gap_scan_rsp_adv_data_buff[BLE_GAP_ADV_SET_DATA_SIZE_MAX];

    //// set the ble advertisement data
    static const ble_gap_adv_data_t ble_gap_adv_data =
        {
            .adv_data.p_data = (uint8_t *)ble_gap_adv_data_buff,
            .adv_data.len = sizeof(ble_gap_adv_data_buff),

            // .scan_rsp_data.p_data = NULL,
            // .scan_rsp_data.len = 0
            .scan_rsp_data.p_data = (uint8_t *)ble_gap_scan_rsp_adv_data_buff,
            .scan_rsp_data.len = sizeof(ble_gap_scan_rsp_adv_data_buff)};

    /// set the ble advertisement parameters
    static const ble_gap_adv_params_t ble_gap_adv_params =
        {
            // set the ble advertise property
            // make the device as connectable , scannable (scan request)  and unidirected advertisement
            .properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED,
            .properties.anonymous = 0,        // do'nt omit the adress from all PDUs
            .properties.include_tx_power = 1, // this is not supported in SD

            // ble peer advertisement address
            .p_peer_addr = NULL,

            .interval = BLE_ADV_INTERVAL, // ble advertisement interval

            .duration = BLE_ADV_DURATION, // ble advertisement duration

            .max_adv_evts = BLE_MAX_ADV_EVENTS, // max advertisement events

            .channel_mask[0] = 0x00, // this is the channel mask used for advertisemnet PDU
            .channel_mask[1] = 0x00,
            .channel_mask[2] = 0x00,
            .channel_mask[3] = 0x00,
            .channel_mask[4] = 0x00,

            .filter_policy = BLE_GAP_ADV_FP_ANY, // allow scan req and connect req from any device

            .primary_phy = BLE_GAP_PHY_1MBPS,  // indicate the phy on which primary advertisement packets are sent must be 1 mbps
            .secondary_phy = BLE_GAP_PHY_AUTO, // indicate the phy on which secondry adv packets are transmitted

            .set_id = 0, // this will be ignored because ext advertisement is not used

            .scan_req_notification = 1 // enable scan request notification

        };

    ///////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////

    uint32_t ret_code = NRF_SUCCESS;

    // const uint8_t  ble_addr[6] = {0x20, 0x21, 0x10, 0x30, 0x90, 0x80 };
    // ble_gap_addr_t const my_ble_addr =
    //     {
    //         .addr_type = BLE_GAP_ADDR_TYPE_PUBLIC,
    //         .addr[0] = ble_addr[]
    //         };

    // ret_code = sd_ble_gap_addr_set(&my_ble_addr);
    // check_assrt(ret_code, "ble_gapaddr");

    /// configure the advertisement on the device
    ret_code = sd_ble_gap_adv_set_configure(&adv_conn_handle, &ble_gap_adv_data, &ble_gap_adv_params);
    check_assrt(ret_code, "ble_gap_adv_set");
    // ret_code = sd_ble_gap_adv_start(adv_conn_handle, APP_BLE_CONN_CFG_TAG);
    //     check_assrt(ret_code, "ble_gap_adv_start");
}


/// @brief preinit the gap so that it can the BLE GAP properly
/// @param  void
/// @return succ/failure 
void ble_gap_pre_init(void)
{
    gap_service_init();
    advertising_init();
}



/// @brief this saves the advertisement state 
volatile uint8_t advertisement_state = false;


uint32_t ble_gap_start_advertise(void)
{
    NRF_LOG_WARNING("starting adv");

    if(advertisement_state == true)
    {
        return nrf_ERR_INVALID_STATE;
    }
    if (adv_conn_handle != BLE_GAP_ADV_SET_HANDLE_NOT_SET)
    {
        uint32_t err_code = NRF_SUCCESS;
        err_code = sd_ble_gap_adv_start(adv_conn_handle, NRF_SOFTDEVICE_DEFAULT_CONFIG_TAG);
        // check_assrt(err_code, "ble_gap_adv_start");

        if(err_code == nrf_OK)
        {
            advertisement_state = true;
        }        
        return err_code;
    }
    return nrf_ERR_INVALID_STATE;
}



uint32_t  ble_gap_stop_advertise(void)
{
    if(advertisement_state == false)
    {
        return nrf_ERR_INVALID_STATE;
    }
    if (adv_conn_handle != BLE_GAP_ADV_SET_HANDLE_NOT_SET)
    {
        uint32_t ret_code = NRF_SUCCESS;
        ret_code = sd_ble_gap_adv_stop(adv_conn_handle);

        if(ret_code == nrf_OK)
        {
            advertisement_state = false;
        }
        return ret_code;
    }

    return nrf_ERR_INVALID_STATE;
}



void ble_gap_security_init(uint16_t conn_handle , ble_gap_sec_params * sec_param )
{
    uint32_t ret_code = 0;

    //////////////////////////////////////// //////////////
    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    //// init the authentication procedure
    static const ble_gap_sec_params_t gap_sec_params =
        {
            .bond = BLE_GAP_SEC_PARAM_BOND,
            .io_caps = BLE_GAP_SEC_PARAM_IO_CAPS,
            .keypress = BLE_GAP_SEC_PARAM_KEYPRESS,
            .lesc = BLE_GAP_SEC_PARAM_LESC_SUPPORT,
            .max_key_size = BLE_GAP_SEC_PARAM_MAX_KEY_SIZE,
            .min_key_size = BLE_GAP_SEC_PARAM_MIN_KEY_SIZE,
            .mitm = BLE_GAP_SEC_PARAM_MITM,
            .oob = BLE_GAP_SEC_PARAM_OOB_SUPPORT,
            
            //// @brief what the device will send the keys 
            .kdist_own.enc = BLE_GAP_SEC_PARAM_LTK,
            .kdist_own.id = BLE_GAP_SEC_PARAM_IRK,
            .kdist_own.sign = BLE_GAP_SEC_PARAM_CONN_SIGNATURE_RESOLVING_KEY,
            .kdist_own.link = BLE_GAP_SEC_PARAM_DERV_LINK_FROM_LTK,
            
            //// @brief what the peer device will have to send the keys 
            .kdist_peer.enc = BLE_GAP_SEC_PARAM_LTK,
            .kdist_peer.id = BLE_GAP_SEC_PARAM_IRK,
            .kdist_peer.sign = BLE_GAP_SEC_PARAM_CONN_SIGNATURE_RESOLVING_KEY,
            .kdist_peer.link = BLE_GAP_SEC_PARAM_DERV_LINK_FROM_LTK,

        };
    ret_code = sd_ble_gap_authenticate(conn_handle, &gap_sec_params );
    check_assrt(ret_code, "gap sec param");


}


