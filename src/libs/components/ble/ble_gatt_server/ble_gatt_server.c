
#include "ble_softdevice_init.h"
#include "ble_gatt_server.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// ATTributes Definations ///////////////////////////////////////////////////////////////////////////////////////////////

static uint16_t BLE_conn_hnadle =BLE_CONN_HANDLE_INVALID; 
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
///////////////// UUIDS number ////////////////////////////////////////////////////////


//////////////////////////////////////////////////
///////////////////////////////////////////////
/////////////// battery service uuid
#define BLE_UUID_BATTERY_SERVICE 0x180F /**< Battery service UUID. */
/// battery level uuid
#define BLE_UUID_BATT_LEVEL_CHAR 0x2A19
//////// battery custom data
#define BLE_UUID_BATT_CUSTOM_DATA_CHAR 0xff00




///////// descriptors
#define BLE_UUID_CHAR_USER_DESCRIPTOR 0x2901
///////client char char description
#define BLE_UUID_CHAR_CLIENT_CHAR_DESCRIPTOR 0x2902

///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////// UUID definations ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

/////////// declare the service uuids
BLE_UUID_16(gatt_service_uuid, BLE_UUID_GATT);

BLE_UUID_16(gatt_char_srvice_changed_uuid, BLE_UUID_GATT_CHARACTERISTIC_SERVICE_CHANGED);

//////////////////////////////////////////////////////////////
////////// define the battery service uuid////////////
//////////////////////////////////////////////////////
BLE_UUID_16(battery_info_service_uuid, BLE_UUID_BATTERY_SERVICE);

BLE_UUID_16(battery_level_percent_uuid, BLE_UUID_BATT_LEVEL_CHAR);


//////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//////////////// EASE info uuids


///////////////////////////////////////////////////////////////////////////////
/////////////////// char property definations ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//////////////////////////////  attribute mmetadata

/// @brief no security at all and the value is handled by the application layer (authorize through application)
static const ble_gatts_attr_md_t char_prop_val_handle_by_app_no_security =
    {
        //// no security is needed for read operations
        .read_perm.lv = 1,
        .read_perm.sm = 1,

        // no security is needed for write operations
        .write_perm.lv = 1,
        .write_perm.sm = 1,

        // must use  variable length array
        ////// variable length arrya attribute data sharing means the device will only send
        /////// data size specified in the reply value not to the max len  
        .vlen = 1,

        // softdevice will copy the att data to its own stack
        .vloc = BLE_GATTS_VLOC_STACK,

        /////// no authorization is required from the application
        .rd_auth = 1,
        .wr_auth = 1,

};

/// @brief no security the value is handled by the ble stack (do not authorize it through application)
static const ble_gatts_attr_md_t char_prop_val_handle_by_stack_no_security =
    {
        //// no security is needed for read operations
        .read_perm.lv = 1,
        .read_perm.sm = 1,

        // no security is needed for write operations
        .write_perm.lv = 1,
        .write_perm.sm = 1,

        // no variable length array
        .vlen = 1,

        // softdevice will copy the att data to its own stack
        // .vloc = BLE_GATTS_VLOC_USER,
        .vloc = BLE_GATTS_VLOC_STACK,

        /////// no authorization is required from the application
        .rd_auth = 0,
        .wr_auth = 0,

};

////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////// char properties definations //////////////////

/// @brief  read only data no descriptor
static const ble_gatts_char_md_t char_prop_read_only_no_desc =
    {
        /// describe the characteristic properties
        .char_props.read = true,
        .char_props.auth_signed_wr = false,
        .char_props.broadcast = false,
        .char_props.indicate = false,
        .char_props.notify = false,
        .char_props.write = false,
        .char_props.write_wo_resp = false,

        // these are ultimately set to 0
        // .char_ext_props.reliable_wr =false,
        // .char_ext_props.wr_aux = false,

        .p_char_user_desc = NULL,
        .char_user_desc_max_size = 0,
        .char_user_desc_size = 0,

        .p_char_pf = NULL,
        .p_user_desc_md = NULL,
        .p_cccd_md = NULL,
        .p_sccd_md = NULL,

};

/// @brief  write only property with cccd
static const ble_gatts_char_md_t char_prop_write_with_cccd =
    {
        /// describe the characteristic properties
        .char_props.read = false,
        .char_props.auth_signed_wr = false,
        .char_props.broadcast = false,
        .char_props.indicate = false,
        .char_props.notify = false,
        .char_props.write = true,
        .char_props.write_wo_resp = false,

        // these are ultimately set to 0
        // .char_ext_props.reliable_wr =false,
        // .char_ext_props.wr_aux = false,

        .p_char_user_desc = NULL,
        .char_user_desc_max_size = 0,
        .char_user_desc_size = 0,

        .p_char_pf = NULL,
        .p_user_desc_md = NULL,
        ////// in thw wrirte only there is no cccd 
        .p_cccd_md = NULL,
        .p_sccd_md = NULL,

};

/// @brief  read notify read notify with cccd
static const ble_gatts_char_md_t char_prop_read_notify_with_cccd =
    {
        /// describe the characteristic properties
        .char_props.read = true,
        .char_props.auth_signed_wr = false,
        .char_props.broadcast = false,
        .char_props.indicate = false,
        .char_props.notify = true,
        .char_props.write = false,
        .char_props.write_wo_resp = false,

        // these are ultimately set to 0
        // .char_ext_props.reliable_wr =false,
        // .char_ext_props.wr_aux = false,

        .p_char_user_desc = NULL,
        .char_user_desc_max_size = 0,
        .char_user_desc_size = 0,

        .p_char_pf = NULL,
        .p_user_desc_md = NULL,
        .p_cccd_md = &char_prop_val_handle_by_stack_no_security,
        .p_sccd_md = NULL,

};

/// @brief read write eith cccd
static const ble_gatts_char_md_t char_prop_read_write_with_cccd =
    {
        /// describe the characteristic properties
        .char_props.read = true,
        .char_props.auth_signed_wr = false,
        .char_props.broadcast = false,
        .char_props.indicate = false,
        .char_props.notify = false,
        .char_props.write = true,
        .char_props.write_wo_resp = false,

        // these are ultimately set to 0
        // .char_ext_props.reliable_wr =false,
        // .char_ext_props.wr_aux = false,

        .p_char_user_desc = NULL,
        .char_user_desc_max_size = 0,
        .char_user_desc_size = 0,

        .p_char_pf = NULL,
        .p_user_desc_md = NULL,
        .p_cccd_md = NULL,
        .p_sccd_md = NULL,

};

#define BLE_GATT_UNIT_PERCENTAGE 0x27AD
#define BLE_GATT_SIG_DESCP_NORMAL 0x1100

static const ble_gatts_char_pf_t ble_batt_lvl_char_pf =
    {
        .format = BLE_GATT_CPF_FORMAT_UINT8,
        .exponent = 0,
        .name_space = BLE_GATT_CPF_NAMESPACE_BTSIG,
        .unit = BLE_GATT_UNIT_PERCENTAGE,
        .desc = BLE_GATT_SIG_DESCP_NORMAL

};

static const ble_gatts_char_md_t char_prop_read_notify_with_cccd_pres_user_desc =
    {
        /// describe the characteristic properties
        .char_props.read = true,
        .char_props.auth_signed_wr = false,
        .char_props.broadcast = false,
        .char_props.indicate = false,
        .char_props.notify = true,
        .char_props.write = false,
        .char_props.write_wo_resp = false,

        // these are ultimately set to 0
        // .char_ext_props.reliable_wr =false,
        // .char_ext_props.wr_aux = false,

        .p_char_user_desc = (uint8_t *)BATT_LEVEL_PERCENTAGE_USER_DESCRIPTOR,
        .char_user_desc_max_size = sizeof(BATT_LEVEL_PERCENTAGE_USER_DESCRIPTOR),
        .char_user_desc_size = sizeof(BATT_LEVEL_PERCENTAGE_USER_DESCRIPTOR),

        .p_char_pf = &ble_batt_lvl_char_pf,
        // .p_user_desc_md = &char_prop_val_handle_by_stack_no_security,
        .p_user_desc_md = NULL,
        .p_cccd_md = &char_prop_val_handle_by_stack_no_security,
        .p_sccd_md = NULL,

};

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////// char definations ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   battery level chars

static uint8_t batt_level = 20;

/// @brief  battery percentage char description
static const ble_gatts_attr_t battery_lvl_percent_char_data =
    {
        .p_uuid = &battery_level_percent_uuid,

        .init_len = sizeof(batt_level),
        .max_len = BATT_SVC_BATT_LVL_CHAR_MAX_LEN,
        .init_offs = 0,

        .p_value = &batt_level,

        // .p_attr_md = &char_prop_val_handle_by_app_no_security
        .p_attr_md = &char_prop_val_handle_by_app_no_security};



/////////////////////////  DEfines the HAndles /////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//////////////// services handles/////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/// gatt and gap service is handle by softdevice and no handle is required
// static uint16_t gatt_srvc_handle = BLE_GATT_HANDLE_INVALID;

///////// battery level service //////////////////////////
static uint16_t batt_srvc_handle = BLE_GATT_HANDLE_INVALID;


///////////////////////////////////////////////////////////////////
//////////////// characteristcs handle ///////////////////////////
/////////////////////////////////////////////////////////////////

/////////////////// Battery service characteristics ///////////

ble_gatts_char_handles_t batt_svc_battery_level_char_handle;



//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/////////////////// initialize the gatt server and database //////////////////

void ble_gatt_server_pre_init(void)
{   

    ///////////////// battery information att table defination

    // battery service init
    sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &battery_info_service_uuid, &batt_srvc_handle);

    // battery level percentage char init
    sd_ble_gatts_characteristic_add(batt_srvc_handle, &char_prop_read_notify_with_cccd_pres_user_desc,
                                    &battery_lvl_percent_char_data, &batt_svc_battery_level_char_handle);

 

}

void ble_gatt_server_init(uint16_t conn_handle)
{
    BLE_conn_hnadle = conn_handle;
}


void ble_gatt_server_send_batt_notif(uint8_t *data, uint16_t len)
{
    uint16_t xfrlen = len;
    ble_gatts_hvx_params_t batt_param =
    {
        .type = BLE_GATT_HVX_NOTIFICATION,
        .offset =0,
        .p_data = data,
        .p_len = &xfrlen,
        .handle = batt_svc_battery_level_char_handle.value_handle
    };
    sd_ble_gatts_hvx(BLE_conn_hnadle, &batt_param);
}
