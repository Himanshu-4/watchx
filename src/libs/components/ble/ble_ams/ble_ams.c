#include "ble_ams.h"

/** @brief 128-bit service UUID for the Apple Media Service. */
static ble_uuid128_t const ble_apple_media_service_uuid128 =
{
    .uuid128 =
    {
        //  89 D3 50 2B 0F 36 43 3A 8E F4 C5 02 AD 55 F8 DC
        0xDC, 0xF8, 0x55, 0xAD, 0x02, 0xc5, 0xf4, 0x8e,
        0x3a, 0x43, 0x36, 0x0f, 0x2b, 0x50, 0xd3, 0x89
    }
};


/** @brief 128-bit AMS remote command  characteristic UUID. */
static ble_uuid128_t const  ble_ams_remote_command_char_uuid128 =
{
    .uuid128 = 
    {
        //  9B 3C 81 D8 57 B1 4A 8A B8 DF 0E 56 F7 CA 51 C2 (writeable, notifiable)
        0xc2, 0x51, 0xca, 0xf7, 0x56, 0x0e, 0xdf, 0xb8, 
        0x8a, 0x4a, 0xb1, 0x57, 0xd8, 0x81, 0x3c, 0x9b
    }
};

/**@brief 128-bit ams entity update char characteristics  UUID. */
static ble_uuid128_t const ble_ams_entity_update_char_uuid128 =
{
    {
        //2F 7C AB CE 80 8D 41 1F 9A 0C BB 92 BA 96 C1 02(writeable with response, notifiable)
        0x02, 0xc1, 0x96, 0xBA, 0x92, 0xBB, 0x0c, 0x9A, 
        0x1F, 0x41, 0x8D, 0x80, 0xce, 0xab, 0x7c, 0x2f
    }
};

/**@brief 128-bit ams entity attribute characteristics  UUID. */
static ble_uuid128_t const ble_ams_entity_attribute_char_uuid128 =
{
    {
        // C6 B2 F3 8C 23 AB 46 D8 A6 AB A3 A8 70 BB D5 D7 (readable, writeable)
        0xd7, 0xd5, 0xbb, 0x70, 0xa8, 0xa3, 0xab, 0xa6,
        0xd8, 0x46, 0xab , 0x23, 0x8c, 0xf3, 0xb2, 0xc6
    }
};




/////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////// defining the global struct 

static ble_ams_struct_t ble_ams_handler ={0};

////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



static void ble_ams_service_init()
{

    ///////// first make sure that all the data is 0 for the services 
    memset(&ble_ams_handler.ams_srvc_char ,0, sizeof(ble_ams_services_struct_t));

    uint32_t err_code =0;
    err_code = sd_ble_uuid_vs_add(&ble_apple_media_service_uuid128 , &ble_ams_handler.ams_srvc_char.ams_service.uuid.type);
    check_assrt(err_code, "uuid_addams");

    err_code = sd_ble_uuid_vs_add(&ble_ams_remote_command_char_uuid128, &ble_ams_handler.ams_srvc_char.ams_control_point_cahr.uuid.type);
    check_assrt(err_code, "uuid_addams");

    err_code = sd_ble_uuid_vs_add(&ble_ams_entity_update_char_uuid128 , &ble_ams_handler.ams_srvc_char.ams_entity_update_char.uuid.type);
    check_assrt(err_code, "uuid_addams");

    err_code = sd_ble_uuid_vs_add(&ble_ams_entity_attribute_char_uuid128 , &ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.uuid.type);
    check_assrt(err_code, "uuid_addams");

    NRF_LOG_INFO("ams_type %d,%d,%d,%d",ble_ams_handler.ams_srvc_char.ams_service.uuid.type, 
    ble_ams_handler.ams_srvc_char.ams_control_point_cahr.uuid.type , ble_ams_handler.ams_srvc_char.ams_entity_update_char.uuid.type , 
    ble_ams_handler.ams_srvc_char.ams_entity_attribute_char.uuid.type);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief this is the to be called when 
/// @param  
void ble_ams_pre_init(void)
{
    /// init the ble ams srvice 
    ble_ams_service_init();



}