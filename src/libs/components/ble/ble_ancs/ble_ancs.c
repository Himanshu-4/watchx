#include "ble_ancs.h"

#include "ble_gatt_client.h"

//// include the kernel memory manager 
#include "memory_manager/kernel_mem_manager.h"

//// include the link list 
#include "memory_manager/kernel_link_list.h"

// #include "stream_buffer.h"



/////////////////////////////// 16 bit uuid ////////////////////////////
#define BLE_BATT_SRVC_UUID 0x180F

BLE_UUID_16(batt_srvc_uuid, BLE_BATT_SRVC_UUID);

// static const uint8_t apple_ancs_service_uuid[]
// =
// {0x79, 0x05 , 0xF4 , 0x31 , 0xB5 , 0xCE , 0x4E ,
//  0x99 ,0xA4 ,0x0F ,0x4B ,0x1E ,0x12 ,0x2D ,0x00 ,0xD0};

// static const ble_uuid_t ancs_uuid_t =
// {
//     .type = BLE_UUID_TYPE_VENDOR_BEGIN,
//     .uuid = 0x31F4
// };

/**@brief 128-bit service UUID for the Apple Notification Center Service. */
static ble_uuid128_t const ble_ancs_base_service_uuid128 =
    {
        .uuid128 =
            {
                // 7905F431-B5CE-4E99-A40F-4B1E122D00D0
                0xd0, 0x00, 0x2d, 0x12, 0x1e, 0x4b, 0x0f, 0xa4,
                0x99, 0x4e, 0xce, 0xb5, 0x31, 0xf4, 0x05, 0x79}};

/**@brief 128-bit control point characteristic UUID. */
static ble_uuid128_t const ble_ancs_control_point_char_uuid128 =
    {
        .uuid128 =
            {
                // 69d1d8f3-45e1-49a8-9821-9BBDFDAAD9D9
                0xd9, 0xd9, 0xaa, 0xfd, 0xbd, 0x9b, 0x21, 0x98,
                0xa8, 0x49, 0xe1, 0x45, 0xf3, 0xd8, 0xd1, 0x69}};

/**@brief 128-bit notification source characteristics  UUID. */
static ble_uuid128_t const ble_ancs_notificatoin_source_char_uuid128 =
    {
        {// 9FBF120D-6301-42D9-8C58-25E699A21DBD
         0xbd, 0x1d, 0xa2, 0x99, 0xe6, 0x25, 0x58, 0x8c,
         0xd9, 0x42, 0x01, 0x63, 0x0d, 0x12, 0xbf, 0x9f

        }};

/**@brief 128-bit data source characteristics  UUID. */
static ble_uuid128_t const ble_ancs_data_source_char_uuid128 =
    {
        {// 22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB
         0xfb, 0x7b, 0x7c, 0xce, 0x6a, 0xb3, 0x44, 0xbe,
         0xb5, 0x4b, 0xd6, 0x24, 0xe9, 0xc6, 0xea, 0x22}};

/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

/// @brief global handler for the ble ancs profile
static ble_ancs_struct_t ble_ancs_handler = {0};

////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

static void ble_ancs_services_int()
{
    // Make sure that the instance of service is clear. GATT handles inside the service and characteristics are set to @ref BLE_GATT_HANDLE_INVALID.
    memset(&ble_ancs_handler.ancs_srvcs, 0, sizeof(ble_ancs_services_struct_t));

    uint32_t err_code = 0;
    // Assign UUID types.
    err_code = sd_ble_uuid_vs_add(&ble_ancs_base_service_uuid128, &ble_ancs_handler.ancs_srvcs.ancs_service.uuid.type);
    NRF_ASSERT(err_code);

    err_code = sd_ble_uuid_vs_add(&ble_ancs_control_point_char_uuid128, &ble_ancs_handler.ancs_srvcs.ancs_control_point_char.uuid.type);
    NRF_ASSERT(err_code);

    err_code = sd_ble_uuid_vs_add(&ble_ancs_notificatoin_source_char_uuid128, &ble_ancs_handler.ancs_srvcs.ancs_notif_src_char.uuid.type);
    NRF_ASSERT(err_code);

    err_code = sd_ble_uuid_vs_add(&ble_ancs_data_source_char_uuid128, &ble_ancs_handler.ancs_srvcs.ancs_data_source_char.uuid.type);
    NRF_ASSERT(err_code);

    // NRF_LOG_INFO("type %d, %d, %d, %d",ble_ancs_handler.ancs_srvc_char.ancs_service.uuid.type,
    // ble_ancs_handler.ancs_srvc_char.ancs_control_point_char.uuid.type, ble_ancs_handler.ancs_srvc_char.ancs_notif_src_char.uuid.type,
    //  ble_ancs_handler.ancs_srvc_char.ancs_data_source_char.uuid.type);
}

/// @brief this is to init the ble ancs service at start of the program
/// @param
void ble_ancs_pre_init(void)
{
    ble_ancs_services_int();
}

//// create a kernel memory instance to hold the data from the notification handler into this memory
/// this is useful because it can give us compile time memory consumption , which in our case is useful because now run time
///// consumption can be minimized

KERNEL_MEM_INSTANTISE(ble_ancs_mem_inst, ble_ancs_mem_pool, BLE_ANCS_MEM_SIZE, ble_ancs_memory_mutex);

/// @brief this is to init the ancs profile from iphone
/// @param conn_handle
/// @return succ/failure
uint32_t ble_ancs_init(uint16_t conn_handle)
{
    uint32_t ret_code = nrf_OK;

    ///// assign the handler and callback function
    ble_ancs_handler.conn_handle = BLE_ANCS_INSTANCE_INITED;
    ble_ancs_handler.conn_handle = conn_handle;
    ble_ancs_handler.total_notif_added = 0;


    /// init the kernel memory pool 
    kernel_mem_init(&ble_ancs_mem_inst, ble_ancs_mem_pool, BLE_ANCS_MEM_SIZE, &ble_ancs_memory_mutex, BLE_ANCS_MUTEX_TIMEOUT);
    ///// search for services and charcteristics

    return ret_code;
}

/// @brief this is to deinit the ancs protocol
/// @param  void
/// @return succ/failure
uint32_t ble_ancs_deinit(void)
{
    NRF_LOG_WARNING("ancs deinit");

    ble_ancs_handler.conn_handle = BLE_ANCS_INSTANCE_DEINITED;
    ble_ancs_handler.conn_handle = BLE_CONN_HANDLE_INVALID;
    ble_ancs_handler.total_notif_added = 0;

    kernel_mem_deinit(&ble_ancs_mem_inst);

    return nrf_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
static const char *category_strings[BLE_ANCS_CATEGORY_ID_TOTAL] =
{
    "OTHER",
    "INCOMING_CALL",
    "MISSED_CALL",
    "VOICE_MAIL",
    "SOCIAL",
    "SCHEDULE",
    "EMAIL",
    "NEWS",
    "HEALTH_AND_FITNESS",
    "BUSINESS_AND_FINANCE",
    "LOCATION",
    "ENTERTAINMENT",
};


/// @brief this func is used to get the string from the category id 
/// @param cat_id 
/// @return return the string frim the category id 
const char * ble_ancs_get_catg_string(uint8_t cat_id)
{
    if(cat_id >= BLE_ANCS_CATEGORY_ID_TOTAL)
    {
        return NULL;
    }
    return category_strings[cat_id];
} 


//////========================================================================================
////////=======================================================================================

/// @brief to get the total no of uids rcvd till now 
/// @param  void 
/// @return the total no of uids 
uint32_t ble_ancs_get_total_nuid(void)
{
    if(ble_ancs_handler.ble_ancs_instance_inited != BLE_ANCS_INSTANCE_INITED)
    {
        return ble_ancs_err_not_intited;
    }


    return ble_ancs_ok;
}

/// @brief to get the notification meta data 
/// @param uid 
/// @param notif_meta 
/// @return 
uint32_t ble_ancs_get_notif_meta_data(uint32_t uid, ble_ancs_notif_metadata_struct_t * notif_meta)
{
   if(ble_ancs_handler.ble_ancs_instance_inited != BLE_ANCS_INSTANCE_INITED)
    {
        return ble_ancs_err_not_intited;
    }


    return ble_ancs_ok; 
}

/// @brief this function is to read the notification uid characteristic
/// @param nuid pointer  
/// @return succ/err code 
uint32_t ble_ancs_read_ancs_nuid_char(uint16_t index , uint32_t *nuid )
{
   if(ble_ancs_handler.ble_ancs_instance_inited != BLE_ANCS_INSTANCE_INITED)
    {
        return ble_ancs_err_not_intited;
    }


    return ble_ancs_ok; 
}

/// @brief to remove that particular uid from cache and iphone 
/// @param nuid 
/// @return succ/failure 
uint32_t ble_ancs_remove_nuid(uint32_t nuid)
{
   if(ble_ancs_handler.ble_ancs_instance_inited != BLE_ANCS_INSTANCE_INITED)
    {
        return ble_ancs_err_not_intited;
    }


    return ble_ancs_ok; 
}

/// @brief to clear the data recvd in the nuid like title, msg , msg size ,etc 
/// @param nuid 
/// @return succ/failure 
uint32_t ble_ancs_clear_nuid(uint32_t nuid)
{
   if(ble_ancs_handler.ble_ancs_instance_inited != BLE_ANCS_INSTANCE_INITED)
    {
        return ble_ancs_err_not_intited;
    }


    return ble_ancs_ok; 
}

/// @brief this func must be called before reading the notification attributes 
/// @param uid 
/// @param fetch_level
/// @return succ/failure
uint32_t ble_ancs_fetch_notif_data(uint32_t uid , uint8_t fetch_level)
{
   if(ble_ancs_handler.ble_ancs_instance_inited != BLE_ANCS_INSTANCE_INITED)
    {
        return ble_ancs_err_not_intited;
    }


    return ble_ancs_ok; 
}

//// below functions are only operation when you called above function firsst
//////=======================================================================================
////////======================================================================================

/// @brief to get the current time that when the notif rcvd 
/// @param nuid 
/// @param ancs_time structure for the time deifne in kernel time manager  
/// @return succ/failure 
uint32_t ble_ancs_get_notif_time(uint32_t nuid, kernel_time_struct_t *ancs_time)
{
   if(ble_ancs_handler.ble_ancs_instance_inited != BLE_ANCS_INSTANCE_INITED)
    {
        return ble_ancs_err_not_intited;
    }


    return ble_ancs_ok; 
}

/// @brief to get the notification date from the notification uid 
/// @param nuid 
/// @param ancs_date 
/// @return succ/failure 
uint32_t ble_ancs_get_notif_date(uint32_t nuid, kernel_date_struct_t *ancs_date)
{
   if(ble_ancs_handler.ble_ancs_instance_inited != BLE_ANCS_INSTANCE_INITED)
    {
        return ble_ancs_err_not_intited;
    }


    return ble_ancs_ok; 
}

/// @brief get the time differnce between the current time and when the notification recieved 
/// @param nuid 
/// @param notif_time 
/// @return succ/failure 
uint32_t ble_ancs_get_notif_rcvd_time(uint32_t nuid, kernel_time_struct_t * notif_time )
{
   if(ble_ancs_handler.ble_ancs_instance_inited != BLE_ANCS_INSTANCE_INITED)
    {
        return ble_ancs_err_not_intited;
    }


    return ble_ancs_ok; 
}

/// @brief get the title string  of the notification 
/// @param uid 
/// @param title 
/// @return succ/failure 
uint32_t ble_ancs_get_notif_title(uint32_t uid, const char * title)
{
   if(ble_ancs_handler.ble_ancs_instance_inited != BLE_ANCS_INSTANCE_INITED)
    {
        return ble_ancs_err_not_intited;
    }


    return ble_ancs_ok; 
}

/// @brief get the subtitle of the notif string 
/// @param uid 
/// @param subtitle 
/// @return succ/failure 
uint32_t ble_ancs_get_notif_subtitle(uint32_t uid, const char *subtitle)
{
   if(ble_ancs_handler.ble_ancs_instance_inited != BLE_ANCS_INSTANCE_INITED)
    {
        return ble_ancs_err_not_intited;
    }


    return ble_ancs_ok; 
}

/// @brief get the msg string of the notif 
/// @param uid 
/// @param notif_msg 
/// @return succ/failure 
uint32_t ble_ancs_get_notif_msg(uint32_t uid, const char * notif_msg)
{
   if(ble_ancs_handler.ble_ancs_instance_inited != BLE_ANCS_INSTANCE_INITED)
    {
        return ble_ancs_err_not_intited;
    }


    return ble_ancs_ok; 
}

/// @brief get the msg size of the notification 
/// @param uid 
/// @param size 
/// @return succ/failure 
uint32_t ble_ancs_get_notif_msg_size(uint32_t uid  , uint32_t *size)
{
   if(ble_ancs_handler.ble_ancs_instance_inited != BLE_ANCS_INSTANCE_INITED)
    {
        return ble_ancs_err_not_intited;
    }


    return ble_ancs_ok; 
}

/// @brief get the action that can be oerformed on that particular notif
/// @param nuid 
/// @param actions 
/// @return succ/failure 
uint32_t ble_ancs_get_notif_aciton(uint32_t nuid, uint32_t *actions)
{
   if(ble_ancs_handler.ble_ancs_instance_inited != BLE_ANCS_INSTANCE_INITED)
    {
        return ble_ancs_err_not_intited;
    }


    return ble_ancs_ok; 
}



///// this function can remove the notif from the notif 
/// @brief this function is to perform the notification action on the nuid 
/// @param nuid 
/// @param action 
/// @return succ/err code 
uint32_t ble_ancs_perform_notif_Action(uint32_t nuid, uint8_t action)
{
   if(ble_ancs_handler.ble_ancs_instance_inited != BLE_ANCS_INSTANCE_INITED)
    {
        return ble_ancs_err_not_intited;
    }


    return ble_ancs_ok; 
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////


/// @brief this the callback handler for the apple ancs events
/// @param param
/// @param evt
bool ble_ancs_client_event_handler( ble_gattc_evt_t const *evt)
{
    /// check for a valid conn handle
    if (evt->conn_handle != ble_ancs_handler.conn_handle)
    {
        return;
    }

    //// search for the char handles

    /// handle the notif source char events
    if (evt->params.hvx.handle == ble_ancs_handler.ancs_srvcs.ancs_notif_src_char.handle_value)
    {
        /// the data you recived here is in the structural format of @ble_ancs_notif_struct_t

        //// first verify that the length is proper
        if (evt->params.hvx.len != sizeof(ble_ancs_notif_struct_t))
        {
            return;
        }

        ble_ancs_notif_struct_t *my_notif_struct = (ble_ancs_notif_struct_t *)evt->params.hvx.data;

        //// show the content of the notif data // for debugginf purpose 
        NRF_LOG_INFO("ev-%d f-%d c-%d cc-%d u-%d",my_notif_struct->event_id,
        my_notif_struct->event_Flag, my_notif_struct->category_id,my_notif_struct->category_count,
        my_notif_struct->notif_uid);
        

        switch (my_notif_struct->event_id)
        {
        case BLE_ANCS_EVT_NOTIF_ADDED:
        {
            // sizeof(ble_ancs_notif_metadata_struct_t);

            // ble_ancs_notif_metadata_struct_t notif_meta;
            // notif_meta.event_Flag = my_notif_struct->event_id;
            // notif_meta.category_id = my_notif_struct->category_id;
            // // notif_meta.notif_fetched = BLE_ANCS_NOTIF_DATA_PENING;

            

            /// add the notification id to the ancs mem pool 
            // kernel_mem_add_data(&ble_ancs_mem_inst, my_notif_struct->event_id, )
        }
        break;

        case BLE_ANCS_EVT_NOTIF_MODIFIED:
        {
            /// modify the notification id to the ancs mem pool
        }
        break;

        case BLE_ANCS_EVT_NOTIF_REMOVED:
        {
            /// remove the notification id from the ancs mem pool
        }
        break;

        default:
            break;
        }
 
    }

    /// handle the data source  char
    else if (evt->params.hvx.handle == ble_ancs_handler.ancs_srvcs.ancs_data_source_char.handle_value)
    {

        /// do the respective action based on the uid 
    }
}