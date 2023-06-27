#include "ble_gatt_client.h"

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
/////////// extern variables

extern volatile ble_client_struct client_struct[BLE_NO_OF_CLIENT_SUPPORTED];

extern volatile xTaskHandle ble_client_task_handle;

extern volatile uint8_t msg_buff[BLE_CLIENT_MESSAGE_BUFFER_SIZE];

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

     
        /////// check that if task handle not null
        if (ble_client_task_handle != NULL)
        {
            /// copy the data into the msg buffer
            memcpy(u8(msg_buff), u8(p_ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[0]), s(ble_gattc_service_t));
            xTaskNotify(ble_client_task_handle, p_ble_evt->evt.gattc_evt.gatt_status, eSetValueWithOverwrite);
        }


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
        ble_char_struct_t serach_char;

        uint16_t gatt_status = p_ble_evt->evt.gattc_evt.gatt_status;

        memcpy(u8(serach_char), u8(msg_buff), sizeof(ble_char_struct_t));

        if (gatt_status == BLE_GATT_STATUS_SUCCESS)
        {
            gatt_status = ble_client_err_char_not_found;

            /// copy the content to the buffer
            /// serach the char with the uuid
            for (int i = 0; i < p_ble_evt->evt.gattc_evt.params.char_disc_rsp.count; i++)
            {
                //// match the characteristic type first 
                if(p_ble_evt->evt.gattc_evt.params.char_disc_rsp.chars[i].uuid.type !=  serach_char.characterstic.uuid.type)
                {
                    continue;
                }
                ///// serach for the uuid
                if (p_ble_evt->evt.gattc_evt.params.char_disc_rsp.chars[i].uuid.uuid == serach_char.characterstic.uuid.uuid)
                {
                    /// found the uuid, change the gatt status
                    gatt_status = ble_client_ok;

                    /// copy the content to buffer
                    memcpy(u8(p_ble_evt->evt.gattc_evt.params.char_disc_rsp.chars[i]), u8(msg_buff), sizeof(ble_char_struct_t));
                    /// exit from the loop
                    break;
                }
            }
        }
        else
        {
            memset(u8(msg_buff), 0, sizeof(msg_buff));
        }

        //// send the task notification
        xTaskNotify(ble_client_task_handle, gatt_status, eSetValueWithOverwrite);
    }
    break;

    /**< Descriptor Discovery Response event.               \n See @ref ble_gattc_evt_desc_disc_rsp_t.               */
    case BLE_GATTC_EVT_DESC_DISC_RSP:
    {
        /////// the message buffer have the content of serach charcteristic
        ble_char_desc_struct_t search_desc;

        uint16_t gatt_status = p_ble_evt->evt.gattc_evt.gatt_status;

        memcpy(u8(search_desc), u8(msg_buff), sizeof(ble_char_desc_struct_t));

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
                    /// found the uuid, change the gatt status
                    gatt_status = ble_client_ok;

                    /// copy the content to buffer
                    memcpy(u8(p_ble_evt->evt.gattc_evt.params.desc_disc_rsp.descs[i]), u8(msg_buff), sizeof(ble_char_desc_struct_t));
                    /// exit from the loop
                    break;
                }
            }
        }
        else
        {
            memset(u8(msg_buff), 0, sizeof(msg_buff));
        }

        //// send the task notification
        xTaskNotify(ble_client_task_handle, gatt_status, eSetValueWithOverwrite);
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
            memcpy(u8(msg_buff), u8(p_ble_evt->evt.gattc_evt.params.read_rsp.data[0]), MIN_OF(sizeof(msg_buff), p_ble_evt->evt.gattc_evt.params.read_rsp.len));
        }

        xTaskNotify(ble_client_task_handle, gatt_status, eSetValueWithOverwrite);
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

        if (gatt_status == BLE_GATT_STATUS_SUCCESS)
        {
            NRF_LOG_INFO("w succ");
        }

        xTaskNotify(ble_client_task_handle, gatt_status, eSetValueWithOverwrite);
    }
    break;

        //// handle the write command event
    case BLE_GATTC_EVT_WRITE_CMD_TX_COMPLETE:
    {
        //// based on the gatt status we have to send then gatt notificatin
        uint16_t gatt_status = p_ble_evt->evt.gattc_evt.gatt_status;

        if (gatt_status == BLE_GATT_STATUS_SUCCESS)
        {
            NRF_LOG_INFO("wcmd succ");
        }

        xTaskNotify(ble_client_task_handle, gatt_status, eSetValueWithOverwrite);
    }
    break;

        ////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////
        /////// handle async server items
    /**< Handle Value Notification or Indication event.     \n Confirm indication with @ref sd_ble_gattc_hv_confirm.  \n See @ref ble_gattc_evt_hvx_t. */
    case BLE_GATTC_EVT_HVX:
    {
        // check that if it is a notification or indication

        if (p_ble_evt->evt.gattc_evt.params.hvx.type == BLE_GATT_HVX_INDICATION)
        {
            // call all the timeout handler  code here
            for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
            {
                if (client_struct[i].conn_handle == p_ble_evt->evt.gattc_evt.conn_handle)
                {
                    //// execute the function only when gatt operation is sucees
                    if (p_ble_evt->evt.gattc_evt.gatt_status == BLE_GATT_STATUS_SUCCESS)
                    {
                        // check handler is not null
                        if (client_struct[i].client_ind_handler != NULL)
                        {
                            // call the handler funtions with data and len
                            client_struct[i].client_ind_handler(client_struct[i].client_indi_hand_param,(ble_gattc_evt_t *) &p_ble_evt->evt.gattc_evt);
                        }

                        /// send the indication confirmation
                        sd_ble_gattc_hv_confirm(client_struct[i].conn_handle, p_ble_evt->evt.gattc_evt.params.hvx.handle);
                    }
                    else
                    {
                        ///// call the error handler
                        if (client_struct[i].client_err_handler != NULL)
                        {
                            client_struct[i].client_err_handler(client_struct[i].client_err_hand_param, p_ble_evt->evt.gattc_evt.gatt_status);
                        }
                    }
                    break;
                }
            }
        }

        //////////////// handle the gatt notification
        else
        {
            // call all the timeout handler  code here
            for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
            {
                if (client_struct[i].conn_handle == p_ble_evt->evt.gattc_evt.conn_handle)
                {
                    //// execute the function only when gatt operation is sucees
                    if (p_ble_evt->evt.gattc_evt.gatt_status == BLE_GATT_STATUS_SUCCESS)
                    {
                        // check handler is not null
                        if (client_struct[i].client_notif_handler != NULL)
                        {
                            // call the handler funtions
                            client_struct[i].client_notif_handler(client_struct[i].client_notif_hand_param, (ble_gattc_evt_t *) &p_ble_evt->evt.gattc_evt);
                        }
                    }

                    else
                    {
                        ///// call the error handler
                        if (client_struct[i].client_err_handler != NULL)
                        {
                            client_struct[i].client_err_handler(client_struct[i].client_err_hand_param, p_ble_evt->evt.gattc_evt.gatt_status);
                        }
                    }
                    break;
                }
            }
        }
    }
    break;

    /**< Exchange MTU Response event.                       \n See @ref ble_gattc_evt_exchange_mtu_rsp_t.            */
    case BLE_GATTC_EVT_EXCHANGE_MTU_RSP:
    {
        //// print the mtu set by server
        NRF_LOG_WARNING("server RX MTU %d", p_ble_evt->evt.gattc_evt.params.exchange_mtu_rsp.server_rx_mtu);
    }
    break;

    /**< Timeout event.                                     \n See @ref ble_gattc_evt_timeout_t.                     */
    case BLE_GATTC_EVT_TIMEOUT:
    { // Disconnect on GATT Client timeout event.
        NRF_LOG_DEBUG("GATT Client Timeout.");
        sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                              BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        // APP_ERROR_CHECK(err_code);

        // call all the timeout handler  code here
        for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
        {
            if (client_struct[i].conn_handle == p_ble_evt->evt.gattc_evt.conn_handle)
            {
                // check handler is not null
                if (client_struct[i].client_timeout_handler != NULL)
                {
                    // call the handler funtions
                    client_struct[i].client_timeout_handler(client_struct[i].client_timeout_hand_param);
                }
                break;
            }
        }
    }
    break;

    default:
        break;
    }
}
