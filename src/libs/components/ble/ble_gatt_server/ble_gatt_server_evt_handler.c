
#include "ble_gatt_server.h"
#include "ble_softdevice_init.h"
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
////////////////// cccd value of the characteristics
#define NOTIFICATION_ENABLED 1
#define NOTIFICATION_AND_INDICATION_DISABLED 0
#define INDICATION_ENABLED 2

///////////////////////////////////////////////////////////////////
//////////////// characteristcs handle ///////////////////////////
/////////////////////////////////////////////////////////////////

/////////////////// Battery service characteristics ///////////

extern ble_gatts_char_handles_t batt_svc_battery_level_char_handle;

extern ble_gatts_char_handles_t batt_svc_battery_custom_info_char_handle;


//////////// the gatt server handler function 
void ble_gatt_server_handler(ble_evt_t const *p_ble_evt)
{

    uint32_t err_code = 0;

    switch (p_ble_evt->header.evt_id)
    {

    case BLE_GATTS_EVT_TIMEOUT:
    { // Disconnect on GATT Server timeout event.
        NRF_LOG_ERROR("GATT Server Timeout.");
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                         BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
    }
    break;

    case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
    {
        NRF_LOG_DEBUG("ex MTU");
        err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                   BLE_GATT_SERVER_RX_MTU);
        APP_ERROR_CHECK(err_code);
    }
    break;

    case BLE_GATTS_EVT_SYS_ATTR_MISSING:
    {
        NRF_LOG_ERROR("sys attr missing");
        // supply the null values
        err_code = sd_ble_gatts_sys_attr_set(p_ble_evt->evt.gatts_evt.conn_handle,
                                             NULL, 0, BLE_GATTS_SYS_ATTR_FLAG_SYS_SRVCS);

        APP_ERROR_CHECK(err_code);
    }
    break;

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////// Handle the Data transfer request and responses
        ///// handle the write request
    case BLE_GATTS_EVT_WRITE:
    {
        // NRF_LOG_INFO("evtwrite cmd");
        // the app will enable the notification and indication here
        // NRF_LOG_INFO("data len %d ,data %d", p_ble_evt->evt.gatts_evt.params.write.len, p_ble_evt->evt.gatts_evt.params.write.data[0]);
    }
    break;

    ///// handle read write authorize request
    case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
    {
        NRF_LOG_INFO("evt rw_req");

        /////////// this is the reply response type dat strucutre
        ble_gatts_rw_authorize_reply_params_t ble_rsp_data = {0};
        //////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        ///// handle the write request
        if (p_ble_evt->evt.gatts_evt.params.authorize_request.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
        {
            ////////////////////////////////////// handle the write requesrt
            //      printf("len %d off %d autre %d op %d hdle %d uuid %x  data %x\r\n", p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.len,
            //     p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.offset,p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.auth_required,
            //     p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.op, p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.handle,
            //   p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.uuid, p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.data[0]);

            ///// first reply the write request

            /////// response type write request
            ble_rsp_data.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE,
            /// a succesful write operation
                ble_rsp_data.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS,

            ble_rsp_data.params.write.offset = 0,
            ///////// this must be 1 for succesfully ACK write operations to the APP
                ble_rsp_data.params.write.update = 1,

            /// dont send any data in the response
                ble_rsp_data.params.write.len = 0;
            ble_rsp_data.params.write.p_data = NULL;
            // .params.write.len = p_ble_evt->evt.gatts_evt.params.write.len,
            // .params.write.p_data = p_ble_evt->evt.gatts_evt.params.write.data
        }
        else if (p_ble_evt->evt.gatts_evt.params.authorize_request.type == BLE_GATTS_AUTHORIZE_TYPE_READ)
        {
            /////// response type write request
            ble_rsp_data.type = BLE_GATTS_AUTHORIZE_TYPE_READ,
            /// a succesful write operation
                ble_rsp_data.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS,

            ble_rsp_data.params.write.offset = 0,
            ///////// this must be 1 for succesfully ACK write operations to the APP
                ble_rsp_data.params.write.update = 1,

            /// dont send any data in the response
                ble_rsp_data.params.read.len = 0;
            ble_rsp_data.params.read.p_data = NULL;
        }

        // send response  to the APP
        sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle, &ble_rsp_data);
    }
    break;

    /////// handle condifrmation from indication
    case BLE_GATTS_EVT_HVC:
    {
        NRF_LOG_INFO("ind conf");
    }
    break;

    /////// handle value
    case BLE_GATTS_EVT_HVN_TX_COMPLETE:
    {
        NRF_LOG_INFO("ntf send");
    }
    break;

    default:
        // No implementation needed.
        break;
    }
}