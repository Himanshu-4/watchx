#ifndef BLE_ANCS_H
#define BLE_ANCS_H

#include "ble_softdevice_init.h"
#include "system.h"

/// include the kernel time manager 
#include "time_manager/kernel_time.h"


/// @defgroup BLE_ANCS_NP_ERROR_CODES Notification Provider (iOS) Error Codes

#define BLE_ANCS_NP_UNKNOWN_COMMAND         0x01A0  //!< The command ID is unknown to the NP.
#define BLE_ANCS_NP_INVALID_COMMAND         0x01A1  //!< The command format is invalid.
#define BLE_ANCS_NP_INVALID_PARAMETER       0x01A2  //!< One or more parameters do not exist in the NP.
#define BLE_ANCS_NP_ACTION_FAILED           0x01A3  //!< The action failed to be performed by the NP.


enum _BLE_ANCS_ERR_CODES_
{
    ble_ancs_ok = 0x00 ,
    ble_ancs_err_not_intited = NRF_BLE_ANCS_ERR_BASE,
    ble_ancs_err_unknown_cmd_id,

    /// @brief uid specific erros 
    ble_ancs_err_uid_fetch_Data_first,
    ble_ancs_err_uid_action_failed,
    ble_ancs_err_uid_Absent,
};

/// @brief define the notification event id for the notif structure 
enum _NOTIFICATION_EVENT_ID_
{
    BLE_ANCS_EVT_NOTIF_ADDED,
    BLE_ANCS_EVT_NOTIF_MODIFIED,
    BLE_ANCS_EVT_NOTIF_REMOVED,
};

/// @brief define the notification event flag of the ios notif 
enum _NOTIF_EVENT_FLAG_
{
    BLE_ANCS_NOTIF_EVENT_FLAG_SILENT = (1<<0),
    BLE_ANCS_NOTIF_EVENT_FLAG_IMP = (1<<1),
    BLE_ANCS_NOTIF_EVENT_FLAG_PRE_EXISTING = (1<<2),
    BLE_ANCS_NOTIF_EVENT_FLAG_POSITIVE_ACTION = (1<<3),
    BLE_ANCS_NOTIF_EVENT_FLAG_NEGATIVE_ACTION = (1<<4),
    BLE_ANCS_NOTIF_EVENT_FLAG_RFU,

};


/// @brief Category IDs for iOS notifications. */
typedef enum
{
    BLE_ANCS_CATEGORY_ID_OTHER,                /**< The iOS notification belongs to the "Other" category.  */
    BLE_ANCS_CATEGORY_ID_INCOMING_CALL,        /**< The iOS notification belongs to the "Incoming Call" category. */
    BLE_ANCS_CATEGORY_ID_MISSED_CALL,          /**< The iOS notification belongs to the "Missed Call" category. */
    BLE_ANCS_CATEGORY_ID_VOICE_MAIL,           /**< The iOS notification belongs to the "Voice Mail" category. */
    BLE_ANCS_CATEGORY_ID_SOCIAL,               /**< The iOS notification belongs to the "Social" category. */
    BLE_ANCS_CATEGORY_ID_SCHEDULE,             /**< The iOS notification belongs to the "Schedule" category. */
    BLE_ANCS_CATEGORY_ID_EMAIL,                /**< The iOS notification belongs to the "Email" category. */
    BLE_ANCS_CATEGORY_ID_NEWS,                 /**< The iOS notification belongs to the "News" category. */
    BLE_ANCS_CATEGORY_ID_HEALTH_AND_FITNESS,   /**< The iOS notification belongs to the "Health and Fitness" category. */
    BLE_ANCS_CATEGORY_ID_BUSINESS_AND_FINANCE, /**< The iOS notification belongs to the "Business and Finance" category. */
    BLE_ANCS_CATEGORY_ID_LOCATION,             /**< The iOS notification belongs to the "Location" category. */
    BLE_ANCS_CATEGORY_ID_ENTERTAINMENT,         /**< The iOS notification belongs to the "Entertainment" category. */
    BLE_ANCS_CATEGORY_ID_TOTAL
} ble_ancs_c_category_id_val_t;



/// @brief Control point command IDs that the Notification Consumer can send to the Notification Provider. */

#define BLE_ANCS_COMMAND_ID_GET_NOTIF_ATTRIBUTES 0      /**< Requests attributes to be sent from the NP to the NC for a given notification. */
#define BLE_ANCS_COMMAND_ID_GET_APP_ATTRIBUTES  1        /**< Requests attributes to be sent from the NP to the NC for a given iOS app. */
#define BLE_ANCS_COMMAND_ID_GET_PERFORM_NOTIF_ACTION 2  /**< Requests an action to be performed on a given notification. For example, dismiss an alarm. */


/**@brief IDs for actions that can be performed for iOS notifications. */

#define  ACTION_ID_POSITIVE  0  /**< Positive action. */
#define  ACTION_ID_NEGATIVE  1    /**< Negative action. */


/**@brief App attribute ID values.
 * @details Currently, only one value is defined. However, the number of app
 * attributes might increase. For this reason, they are stored in an enumeration.
 */
typedef enum
{
    BLE_ANCS_APP_ATTR_ID_DISPLAY_NAME = 0          /**< Command used to get the display name for an app identifier. */
} ble_ancs_c_app_attr_id_val_t;

/**@brief IDs for iOS notification attributes. */
typedef enum
{
    BLE_ANCS_NOTIF_ATTR_ID_APP_IDENTIFIER = 0,     // Identifies that the attribute data is of an "App Identifier" type. */
    BLE_ANCS_NOTIF_ATTR_ID_TITLE,                  // Identifies that the attribute data is a "Title". need to followed by 2 byte max len 
    BLE_ANCS_NOTIF_ATTR_ID_SUBTITLE,               // Identifies that the attribute data is a "Subtitle". */need to followed by 2 byte max len
    BLE_ANCS_NOTIF_ATTR_ID_MESSAGE,                // Identifies that the attribute data is a "Message". */need to followed by 2 byte max len
    BLE_ANCS_NOTIF_ATTR_ID_MESSAGE_SIZE,           // Identifies that the attribute data is a "Message Size". */
    BLE_ANCS_NOTIF_ATTR_ID_DATE,                   // Identifies that the attribute data is a "Date". */
    BLE_ANCS_NOTIF_ATTR_ID_POSITIVE_ACTION_LABEL,  // The notification has a "Positive action" that can be executed associated with it. */
    BLE_ANCS_NOTIF_ATTR_ID_NEGATIVE_ACTION_LABEL,  // The notification has a "Negative action" that can be executed associated with it. */
} ble_ancs_c_notif_attr_id_val_t;


// /**@brief Parsing states for received iOS notification and app attributes. */
// typedef enum
// {
//     COMMAND_ID,    /**< Parsing the command ID. */
//     NOTIF_UID,     /**< Parsing the notification UID. */
//     APP_ID,        /**< Parsing app ID. */
//     ATTR_ID,       /**< Parsing attribute ID. */
//     ATTR_LEN1,     /**< Parsing the LSB of the attribute length. */
//     ATTR_LEN2,     /**< Parsing the MSB of the attribute length. */
//     ATTR_DATA,     /**< Parsing the attribute data. */
//     ATTR_SKIP,     /**< Parsing is skipped for the rest of an attribute (or entire attribute). */
//     DONE,          /**< Parsing for one attribute is done. */
// } ble_ancs_c_parse_state_t;


//// ancs time format yyyyMMdd'T'HHmmSS.



/// @brief this is the ios notification structure 
typedef PACKED_STRUCT _IOS_NOTIFICATION_STRUCTURE_
{
    uint8_t event_id;
    uint8_t event_Flag;
    uint8_t category_id;
    uint8_t category_count;
    uint32_t notif_uid;
}ble_ancs_notif_struct_t;


#define BLE_ANCS_INSTANCE_INITED 0x11
#define BLE_ANCS_INSTANCE_DEINITED 0x22



typedef PACKED_STRUCT __ALIGNED(4) _BLE_ANCS_SERVICES_STRUCT_
{
    /// @brief this is the ble ancs service uuid 
    ble_gattc_service_t ancs_service;
    
    /// @brief  this is the ancs control point uuid and its desc 
    ble_gattc_char_t ancs_control_point_char;
    ble_gattc_desc_t ancs_control_point_desc;

    /// @brief this is the ancs notification src char and its desc 
    ble_gattc_char_t ancs_notif_src_char;
    ble_gattc_desc_t ancs_notif_src_desc;

    /// @brief this is the ancs data source char and its desc 
    ble_gattc_char_t ancs_data_source_char;
    ble_gattc_desc_t ancs_data_source_desc;


}ble_ancs_services_struct_t;


typedef struct _BLE_ANCS_STRUCT_
{
    ble_ancs_services_struct_t ancs_srvcs;

    uint16_t total_notif_added;

    uint16_t conn_handle;
    uint8_t ble_ancs_instance_inited;


}ble_ancs_struct_t;

//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/// @brief this is the enum for storing the meta data of the notification 
enum _BLE_ANCS_NOTIF_STORED_METAS_
{
    BLE_ANCS_NOTIF_META_EVT_FLAG=0,
    BLE_ANCS_NOTIF_META_CATG_ID,
    BLE_ANCS_NOTIF_META_CATG_COUNT,
    BLE_ANCS_NOTIF_META_CATG_MSG_SIZE_4,
    BLE_ANCS_NOTIF_META_DATA_
};


/// @brief this is to fetch the notification  
enum _BLE_ANCS_NOTIF_FETCH_LEVEL_
{
    BLE_ANCS_NOTIF_FETCH_PENDING,
    BLE_ANCS_NOTIF_FETCH_APP_ATT,
    BLE_ANCS_NOTIF_FETCH_TITLE_ONLY,
    BLE_ANCS_NOTIF_FETCH_WITH_SUBTITLE,
    BLE_ANCS_NOTIF_FETCH_FULL_MSG,
};

/// @brief this is the index of the stored meta data  
enum _BLE_ANCS_NOTIF_UID_META_DATA_INDEX_
{
    BLE_ANCS_META_INDEX_UID = 0x00,
    BLE_ANCS_META_INDEX_EVT_FLAG =  BLE_ANCS_META_INDEX_UID + 4,
    BLE_ANCS_META_INDEX_CATG_ID,
    BLE_ANCS_META_INDEX_CATG_COUNT,
    BLE_ANCS_META_INDEX_NOTIF_FETCHED_LEVEL,
    BLE_ANCS_META_INDEX_APP_ID ,
    BLE_ANCS_META_INDEX_APP_DISPLAY_NAME = BLE_ANCS_META_INDEX_APP_ID+4,
    BLE_ANCS_META_INDEX_NOTIF_TITLE = BLE_ANCS_META_INDEX_APP_DISPLAY_NAME +4,
    BLE_ANCS_META_INDEX_NOTIF_SUBTITLE = BLE_ANCS_META_INDEX_NOTIF_TITLE +4,
    BLE_ANCS_META_INDEX_NOTIF_MSG =  BLE_ANCS_META_INDEX_NOTIF_SUBTITLE +4,
    BLE_ANCS_META_INDEX_NOTIF_MSG_SIZE = BLE_ANCS_META_INDEX_NOTIF_MSG +4,
    BLE_ANCS_META_INDEX_POS_ACTION = BLE_ANCS_META_INDEX_NOTIF_MSG_SIZE +2,
    BLE_ANCS_META_INDEX_NEG_ACTION,
  
};


//// the notification uid format 
typedef PACKED_STRUCT _BLE_ANCS_NOTIF_UID_
{
    /// the notification uid of the particular notification 
    uint32_t nuid;
    
    /// @brief notification properties  
    uint8_t event_Flag;
    uint8_t category_id;
    uint8_t category_count;
    
    uint8_t notif_fetched_level;

    /// @brief this is the notif data that are fetched from iphone , these store the pointer 
    //// where that strings are present 
    char *app_identifier; /// this contain the application name 
    char *app_attr_display_name; /// this contain the name of the msg 

    char *notif_title; /// this contains the title of notif 
    char *notif_subtitle; /// this contains the subtitle of notif 
    char *notif_msg; /// this contains the actual msg 
    
    uint16_t notif_msg_size;

    /// @brief keep track of the actions that can be performed 
    uint8_t positive_action;
    uint8_t negative_action;

    /// @brief keep track of at what time the notif rcvd 
    kernel_time_struct_t time_rcvd;
    kernel_date_struct_t date_rcvd;


}ble_ancs_notif_metadata_struct_t;



////////////////////////////////////////////////////////////////////////////////////////////////
///////////// function declartions //////////////////////////////////////////////////////////


/// @brief this is to init the services of the ble ancs at the start of program  
/// @param void 
void ble_ancs_pre_init(void);

/// @brief this is to init the ancs profile from iphone 
/// @param conn_handle 
/// @return succ/failure 
uint32_t  ble_ancs_init(uint16_t conn_handle);

/// @brief this is to deinit the ancs protocol 
/// @param  void 
/// @return succ/failure 
uint32_t ble_ancs_deinit(void);


//////========================================================================================
////////=======================================================================================

/// @brief to get the total no of uids rcvd till now 
/// @param  void 
/// @return the total no of uids 
uint32_t ble_ancs_get_total_nuid(void);

/// @brief this function is to read the notification uid value based on index 
/// @param nuid pointer  
/// @return succ/err code 
uint32_t ble_ancs_read_ancs_nuid_value(uint16_t index , uint32_t *nuid );

/// @brief to get the notification meta data with the help of index  
/// @param uid 
/// @param notif_meta 
/// @return succ/failure 
uint32_t ble_ancs_get_notif_meta_info(uint16_t index, ble_ancs_notif_metadata_struct_t * notif_meta);

/// @brief this is to modify the meta data info of that particular uid  
/// @param uid 
/// @param meta_Data_index  @ref _BLE_ANCS_NOTIF_UID_META_DATA_INDEX_
/// @param data_pointer 
/// @param size 
/// @return succ/failure 
uint32_t ble_ancs_modify_meta_info(uint32_t uid, uint16_t meta_Data_index, void *data_pointer , uint8_t size);

/// @brief to remove that particular uid from cache and iphone 
/// @param nuid 
/// @return succ/failure 
uint32_t ble_ancs_remove_nuid(uint32_t nuid);

/// @brief to clear the data recvd in the nuid like title, msg , all string data  
/// @param nuid 
/// @return succ/failure 
uint32_t ble_ancs_clear_nuid(uint32_t nuid);

/// @brief this func must be called before reading the notification attributes 
/// @param uid 
/// @param notif_fetch_level @ref _BLE_ANCS_NOTIF_FETCH_LEVEL_
/// @return succ/failure
uint32_t ble_ancs_fetch_notif_data(uint32_t uid ,uint8_t fetch_level);

//// below functions are only operation when you called above function firsst
//////=======================================================================================
////////======================================================================================

/// @brief to get the notification meta data 
/// @param uid 
/// @param notif_meta 
/// @return 
uint32_t ble_ancs_get_notif_meta_data(uint32_t uid, ble_ancs_notif_metadata_struct_t * notif_meta);

/// @brief to get the current time that when the notif rcvd 
/// @param nuid 
/// @param ancs_time structure for the time deifne in kernel time manager  
/// @return succ/failure 
uint32_t ble_ancs_get_notif_time(uint32_t nuid, kernel_time_struct_t *ancs_time);

/// @brief to get the notification date from the notification uid 
/// @param nuid 
/// @param ancs_date 
/// @return succ/failure 
uint32_t ble_ancs_get_notif_date(uint32_t nuid, kernel_date_struct_t *ancs_date);

/// @brief get the time differnce between the current time and when the notification recieved 
/// @param nuid 
/// @param notif_time 
/// @return succ/failure 
uint32_t ble_ancs_get_notif_rcvd_time(uint32_t nuid, kernel_time_struct_t * notif_time );

/// @brief get the title string  of the notification 
/// @param uid 
/// @param title 
/// @return succ/failure 
uint32_t ble_ancs_get_notif_title(uint32_t uid, const char * title);

/// @brief get the subtitle of the notif string 
/// @param uid 
/// @param subtitle 
/// @return succ/failure 
uint32_t ble_ancs_get_notif_subtitle(uint32_t uid, const char *subtitle);

/// @brief get the msg string of the notif 
/// @param uid 
/// @param notif_msg 
/// @return succ/failure 
uint32_t ble_ancs_get_notif_msg(uint32_t uid, const char * notif_msg);

/// @brief get the msg size of the notification 
/// @param uid 
/// @param size 
/// @return succ/failure 
uint32_t ble_ancs_get_notif_msg_size(uint32_t uid  , uint32_t *size);

/// @brief get the action that can be oerformed on that particular notif
/// @param nuid 
/// @param actions 
/// @return succ/failure 
uint32_t ble_ancs_get_notif_aciton(uint32_t nuid, uint32_t *actions);



///// this function can remove the notif from the notif 
/// @brief this function is to perform the notification action on the nuid 
/// @param nuid 
/// @param action 
/// @return succ/err code 
uint32_t ble_ancs_perform_notif_Action(uint32_t nuid, uint8_t action);

/// @brief this func is used to get the string from the category id 
/// @param cat_id 
/// @return return the string frim the category id 
const char * ble_ancs_get_catg_string(uint8_t cat_id);

////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////


/// @brief this the callback handler for the apple ancs events 
/// @param param 
/// @param evt 
bool ble_ancs_client_event_handler(ble_gattc_evt_t const *evt);

#endif