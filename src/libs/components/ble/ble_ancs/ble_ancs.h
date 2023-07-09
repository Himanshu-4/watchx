#ifndef BLE_ANCS_H
#define BLE_ANCS_H

#include "ble_softdevice_init.h"
#include "system.h"



#define ANCS_UUID_SERVICE                   0xF431  //!< 16-bit service UUID for the Apple Notification Center Service.
#define ANCS_UUID_CHAR_CONTROL_POINT        0xD8F3  //!< 16-bit control point UUID.
#define ANCS_UUID_CHAR_DATA_SOURCE          0xC6E9  //!< 16-bit data source UUID.
#define ANCS_UUID_CHAR_NOTIFICATION_SOURCE  0x120D  //!< 16-bit notification source UUID.



/// @defgroup BLE_ANCS_NP_ERROR_CODES Notification Provider (iOS) Error Codes

#define BLE_ANCS_NP_UNKNOWN_COMMAND         0x01A0  //!< The command ID is unknown to the NP.
#define BLE_ANCS_NP_INVALID_COMMAND         0x01A1  //!< The command format is invalid.
#define BLE_ANCS_NP_INVALID_PARAMETER       0x01A2  //!< One or more parameters do not exist in the NP.
#define BLE_ANCS_NP_ACTION_FAILED           0x01A3  //!< The action failed to be performed by the NP.


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


/**@brief Parsing states for received iOS notification and app attributes. */
typedef enum
{
    COMMAND_ID,    /**< Parsing the command ID. */
    NOTIF_UID,     /**< Parsing the notification UID. */
    APP_ID,        /**< Parsing app ID. */
    ATTR_ID,       /**< Parsing attribute ID. */
    ATTR_LEN1,     /**< Parsing the LSB of the attribute length. */
    ATTR_LEN2,     /**< Parsing the MSB of the attribute length. */
    ATTR_DATA,     /**< Parsing the attribute data. */
    ATTR_SKIP,     /**< Parsing is skipped for the rest of an attribute (or entire attribute). */
    DONE,          /**< Parsing for one attribute is done. */
} ble_ancs_c_parse_state_t;





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



typedef PACKED_STRUCT _BLE_ANCS_SERVICES_STRUCT_
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


typedef PACKED_STRUCT _BLE_ANCS_STRUCT_
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
    BLE_ANCS_NOTIF_META_CATG_COUNT_ID,
    BLE_ANCS_NOTIF_META_DATA_
};

//// the notification uid format 
typedef PACKED_STRUCT _BLE_ANCS_NOTIF_UID_
{
    uint8_t event_Flag;
    uint8_t category_id;
    uint8_t category_count;
    
}ble_ancs_notif_stored_struct_t;



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



uint32_t ble_ancs_get_total_nuid(void);


uint32_t ble_ancs_remove_nuid();

/// @brief this function is to read the notification uid characteristic 
/// @return succ/err code 
uint32_t ble_ancs_read_ancs_nuid_char(uint16_t index , );


///// this function can remove the notif from the notif 
/// @brief this function is to perform the notification action on the nuid 
/// @param nuid 
/// @param action 
/// @return succ/err code 
uint32_t ble_ancs_perform_notif_Action(uint32_t nuid, uint8_t action);

/// @brief this func is used to get the string from the category id 
/// @param cat_id 
/// @return return the string frim the category id 
char * ble_ancs_get_string(uint8_t cat_id);

////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////


/// @brief this the callback handler for the apple ancs events 
/// @param param 
/// @param evt 
void ble_ancs_client_event_handler(void *param ,ble_gattc_evt_t *evt);

#endif