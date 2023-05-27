#include "ble_ancs.h"



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
        0x99, 0x4e, 0xce, 0xb5, 0x31, 0xf4, 0x05, 0x79
    }
};


/**@brief 128-bit control point characteristic UUID. */
static ble_uuid128_t const ble_ancs_control_point_char_uuid128 =
{
    .uuid128 = 
    {
        // 69d1d8f3-45e1-49a8-9821-9BBDFDAAD9D9
        0xd9, 0xd9, 0xaa, 0xfd, 0xbd, 0x9b, 0x21, 0x98,
        0xa8, 0x49, 0xe1, 0x45, 0xf3, 0xd8, 0xd1, 0x69
    }
};

/**@brief 128-bit notification source characteristics  UUID. */
static ble_uuid128_t const ble_ancs_notificatoin_source_char_uuid128 =
{
    {
        // 9FBF120D-6301-42D9-8C58-25E699A21DBD
        0xbd, 0x1d, 0xa2, 0x99, 0xe6, 0x25, 0x58, 0x8c,
        0xd9, 0x42, 0x01, 0x63, 0x0d, 0x12, 0xbf, 0x9f

    }
};

/**@brief 128-bit data source characteristics  UUID. */
static ble_uuid128_t const ble_ancs_data_source_char_uuid128 =
{
    {
        // 22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB
        0xfb, 0x7b, 0x7c, 0xce, 0x6a, 0xb3, 0x44, 0xbe,
        0xb5, 0x4b, 0xd6, 0x24, 0xe9, 0xc6, 0xea, 0x22
    }
};

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
    memset(&ble_ancs_handler.ancs_srvc_char , 0, sizeof(ble_ancs_services_struct_t));


    uint32_t err_code =0;
     // Assign UUID types.
    err_code = sd_ble_uuid_vs_add(&ble_ancs_base_service_uuid128, &ble_ancs_handler.ancs_srvc_char.ancs_service.uuid.type);
    check_assrt( err_code, "uuid_add");

    err_code = sd_ble_uuid_vs_add(&ble_ancs_control_point_char_uuid128, &ble_ancs_handler.ancs_srvc_char.ancs_control_point_char.uuid.type);
    check_assrt( err_code, "uuid_add");

    err_code = sd_ble_uuid_vs_add(&ble_ancs_notificatoin_source_char_uuid128, &ble_ancs_handler.ancs_srvc_char.ancs_notif_src_char.uuid.type);
    check_assrt( err_code, "uuid_add");

    err_code = sd_ble_uuid_vs_add(&ble_ancs_data_source_char_uuid128, &ble_ancs_handler.ancs_srvc_char.ancs_data_source_char.uuid.type);
    check_assrt( err_code, "uuid_add");

    NRF_LOG_INFO("type %d, %d, %d, %d",ble_ancs_handler.ancs_srvc_char.ancs_service.uuid.type,
    ble_ancs_handler.ancs_srvc_char.ancs_control_point_char.uuid.type, ble_ancs_handler.ancs_srvc_char.ancs_notif_src_char.uuid.type,
     ble_ancs_handler.ancs_srvc_char.ancs_data_source_char.uuid.type);
}



/// @brief this is to init the ble ancs service at start of the program 
/// @param  
void ble_ancs_pre_init(void)
{
    ble_ancs_services_int();

}

/// @brief this is to init the ancs profile from iphone 
/// @param conn_handle 
/// @return 
uint32_t  ble_ancs_init(uint16_t conn_handle)
{
    uint32_t ret_code = nrf_OK;

    ///// assign the handler and callback function 
    

    ///// search for services and charcteristics 


    return ret_code;
}