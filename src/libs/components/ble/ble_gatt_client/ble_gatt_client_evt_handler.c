#include "ble_gatt_client.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
/////////// extern variables

extern volatile uint8_t client_buff[BLE_CLIENT_MESSAGE_BUFFER_SIZE];

extern volatile xTaskHandle client_taskhandle;

extern volatile gatt_client_callback_handler gatt_client_callbacks[ble_gatt_client_max_callbacks_supp] = {NULL};

/////////// @ref define a refernce functions that are using tas notification

#define task_notify(x)                                             \
    if (client_taskhandle != NULL)                                 \
    {                                                              \
        xTaskNotify(client_taskhandle, x, eSetValueWithOverwrite); \
    }

void ble_gatt_client_handler(ble_evt_t const *p_ble_evt)
{

    ////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////   BLE GATT client   events //////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    ///////////////       there are    events ////////////////////////////////////////////

    switch (p_ble_evt->header.evt_id)
    {

        ////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////
        ////////// discover database items
        /**< Primary Service Discovery Response event.          \n See @ref ble_gattc_evt_prim_srvc_disc_rsp_t.          */
    case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
    {
        ///////// check about the gatt status
        uint16_t gatt_status = p_ble_evt->evt.gattc_evt.gatt_status;

        if (gatt_status == BLE_GATT_STATUS_SUCCESS)
        {
            // copy the content to the client buffer
            memcpy(u8(client_buff), &p_ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[0], sizeof(ble_service_struct_t));
            /// notify tabout the task

            // NRF_LOG_INFO("s%d,%x,%d,%d,%d", p_ble_evt->evt.gattc_evt.gatt_status, p_ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[0].uuid.uuid,
            //  p_ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[0].uuid.type ,p_ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[0].handle_range.start_handle,
            // p_ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[0].handle_range.end_handle);
        }

        else
        {
            NRF_LOG_ERROR("gatt eer");
        }
        task_notify(gatt_status);

        // for (int i = 0; i < p_ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp.count; i++)
        // {
        //     NRF_LOG_INFO("uuid is %x, type is %d", p_ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[i].uuid.uuid,
        //                  p_ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[i].uuid.type);
        // }
    }
    break;

    /**< Relationship Discovery Response event.             \n See @ref ble_gattc_evt_rel_disc_rsp_t.                */
    case BLE_GATTC_EVT_REL_DISC_RSP:
    {
    }
    break;

    /**< Characteristic Discovery Response event.           \n See @ref ble_gattc_evt_char_disc_rsp_t.               */
    case BLE_GATTC_EVT_CHAR_DISC_RSP:
    {
        /////// the message buffer have the content of serach charcteristic
        ble_char_struct_t serch_char;

        /// copy the serach char from the client buff
        memcpy(&serch_char, (uint8_t *)client_buff, sizeof(ble_char_struct_t));
        uint16_t gatt_status = p_ble_evt->evt.gattc_evt.gatt_status;

        if (gatt_status == BLE_GATT_STATUS_SUCCESS)
        {
            gatt_status = ble_client_err_char_not_found;

            /// copy the content to the buffer
            /// serach the char with the uuid
            for (int i = 0; i < p_ble_evt->evt.gattc_evt.params.char_disc_rsp.count; i++)
            {
                //// match the characteristic type first
                if (p_ble_evt->evt.gattc_evt.params.char_disc_rsp.chars[i].uuid.type != serch_char.characterstic.uuid.type)
                {
                    continue;
                }
                ///// serach for the uuid
                if (p_ble_evt->evt.gattc_evt.params.char_disc_rsp.chars[i].uuid.uuid == serch_char.characterstic.uuid.uuid)
                {
                    gatt_status = ble_client_ok;
                    /// found the uuid, change the gatt status
                    /// copy the content to buffer
                    memcpy((uint8_t *)client_buff, (uint8_t *)&p_ble_evt->evt.gattc_evt.params.char_disc_rsp.chars[i], sizeof(ble_char_struct_t));
                    /// exit from the loop
                    break;
                }
            }
        }
        task_notify(gatt_status);

        //// send the task notification
    }
    break;

    /**< Descriptor Discovery Response event.               \n See @ref ble_gattc_evt_desc_disc_rsp_t.               */
    case BLE_GATTC_EVT_DESC_DISC_RSP:
    {
        /////// the message buffer have the content of serach charcteristic
        ble_char_desc_struct_t search_desc;

        uint16_t gatt_status = p_ble_evt->evt.gattc_evt.gatt_status;

        memcpy((uint8_t *)&search_desc, (uint8_t *)client_buff, sizeof(ble_char_desc_struct_t));

        if (gatt_status == BLE_GATT_STATUS_SUCCESS)
        {
            gatt_status = ble_client_err_desc_not_found;

            /// copy the content to the buffer
            /// serach the char with the uuid
            for (int i = 0; i < p_ble_evt->evt.gattc_evt.params.desc_disc_rsp.count; i++)
            {
                ///// serach for the uuid
                if (p_ble_evt->evt.gattc_evt.params.desc_disc_rsp.descs[i].uuid.uuid == search_desc.descriptor.uuid.uuid)
                {
                    gatt_status = ble_client_ok;
                    /// found the uuid, change the gatt status
                    /// copy the content to buffer
                    memcpy(u8_ptr client_buff, u8_ptr & p_ble_evt->evt.gattc_evt.params.desc_disc_rsp.descs[i], sizeof(ble_char_desc_struct_t));
                    /// exit from the loop
                    break;
                }
            }
        }
        task_notify(gatt_status);
    }
    break;

    /**< Attribute Information Response event.              \n See @ref ble_gattc_evt_attr_info_disc_rsp_t. */
    case BLE_GATTC_EVT_ATTR_INFO_DISC_RSP:
    {
    }
    break;

        ////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////
        ////// server operations items

    //////// read events

    /**< Read Response event.                               \n See @ref ble_gattc_evt_read_rsp_t.                    */
    case BLE_GATTC_EVT_READ_RSP:
    {
        /// check the status of the gatt callback
        uint16_t gatt_status = p_ble_evt->evt.gattc_evt.gatt_status;

        if (gatt_status == BLE_GATT_STATUS_SUCCESS)
        {
            /// copy the content
            memcpy(u8_ptr client_buff, u8_ptr & p_ble_evt->evt.gattc_evt.params.read_rsp.data[0], MIN_OF(sizeof(client_buff), p_ble_evt->evt.gattc_evt.params.read_rsp.len));
        }

        task_notify(gatt_status);
    }
    break;

    /**< Read By UUID Response event.                       \n See @ref ble_gattc_evt_char_val_by_uuid_read_rsp_t.   */
    case BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP:
    {
    }
    break;

        /**< Read multiple Response event.                      \n See @ref ble_gattc_evt_char_vals_read_rsp_t.          */
    case BLE_GATTC_EVT_CHAR_VALS_READ_RSP:
    {
    }
    break;

    ////// write event
    /**< Write Response event.                              \n See @ref ble_gattc_evt_write_rsp_t.                   */
    case BLE_GATTC_EVT_WRITE_RSP:
    {
        //// based on the gatt status we have to send then gatt notificatin
        uint16_t gatt_status = p_ble_evt->evt.gattc_evt.gatt_status;
        task_notify(gatt_status);
    }
    break;

        //// handle the write command event
    case BLE_GATTC_EVT_WRITE_CMD_TX_COMPLETE:
    {
        //// based on the gatt status we have to send then gatt notificatin
        uint16_t gatt_status = p_ble_evt->evt.gattc_evt.gatt_status;
        task_notify(gatt_status);
    }
    break;

        ////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////
        /////// handle async server items
    /**< Handle Value Notification or Indication event.     \n Confirm indication with @ref sd_ble_gattc_hv_confirm.  \n See @ref ble_gattc_evt_hvx_t. */
    case BLE_GATTC_EVT_HVX:
    {
        // check that if it is a notification or indication
        //// execute the function only when gatt operation is sucees
        if (p_ble_evt->evt.gattc_evt.gatt_status != BLE_GATT_STATUS_SUCCESS)
        {
            ///// call the error handler
            if (gatt_client_callbacks[ble_gatt_client_error_callback] != NULL)
            {
                gatt_client_callbacks[ble_gatt_client_error_callback](NULL);
            }
            break;
        }

        if (p_ble_evt->evt.gattc_evt.params.hvx.type == BLE_GATT_HVX_INDICATION)
        {
            // check handler is not null
            if (gatt_client_callbacks[ble_gatt_client_indic_callback] != NULL)
            {
                // call the handler funtion
                gatt_client_callbacks[ble_gatt_client_timeout_callback](p_ble_evt);
            }

            /// send the indication confirmation
            sd_ble_gattc_hv_confirm(p_ble_evt->evt.gattc_evt.conn_handle, p_ble_evt->evt.gattc_evt.params.hvx.handle);
        }

        //////////////// handle the gatt notification
        else
        {

            // check handler is not null
            if (gatt_client_callbacks[ble_gatt_client_notif_callback] != NULL)
            {
                // call the handler funtions
                gatt_client_callbacks[ble_gatt_client_notif_callback](p_ble_evt);
            }
        }

        break;

    /**< Exchange MTU Response event.                       \n See @ref ble_gattc_evt_exchange_mtu_rsp_t.            */
    case BLE_GATTC_EVT_EXCHANGE_MTU_RSP:
    {
        uint16_t status = p_ble_evt->evt.gattc_evt.gatt_status;
        /// give the task notification
        task_notify(status);
        //// print the mtu set by server
        NRF_LOG_WARNING("RX MTU %d", p_ble_evt->evt.gattc_evt.params.exchange_mtu_rsp.server_rx_mtu);
    }
    break;

    /**< Timeout event.                                     \n See @ref ble_gattc_evt_timeout_t.                     */
    case BLE_GATTC_EVT_TIMEOUT:
    { // Disconnect on GATT Client timeout event.
        NRF_LOG_DEBUG("GATT Client Timeout.");
        sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                              BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        // APP_ERROR_CHECK(err_code);
        if (gatt_client_callbacks[ble_gatt_client_timeout_callback] != NULL)
        {
            gatt_client_callbacks[ble_gatt_client_timeout_callback](NULL);
        }
    }
    break;

    default:
        break;
    }
    }
