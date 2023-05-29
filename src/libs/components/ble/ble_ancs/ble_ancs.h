#ifndef BLE_ANCS_H
#define BLE_ANCS_H

#include "ble_softdevice_init.h"


#define BLE_ANCS_ATTR_DATA_MAX              32  //!< Maximum data length of an iOS notification attribute.
#define BLE_ANCS_NB_OF_CATEGORY_ID          12  //!< Number of iOS notification categories: Other, Incoming Call, Missed Call, Voice Mail, Social, Schedule, Email, News, Health and Fitness, Business and Finance, Location, Entertainment.
#define BLE_ANCS_NB_OF_NOTIF_ATTR           8   //!< Number of iOS notification attributes: AppIdentifier, Title, Subtitle, Message, MessageSize, Date, PositiveActionLabel, NegativeActionLabel.
#define BLE_ANCS_NB_OF_APP_ATTR             1   //!< Number of iOS application attributes: DisplayName.
#define BLE_ANCS_NB_OF_EVT_ID               3   //!< Number of iOS notification events: Added, Modified, Removed.

/** @brief Length of the iOS notification data.
 *
 * @details 8 bytes:
 * Event ID |Event flags |Category ID |Category count|Notification UID
 * ---------|------------|------------|--------------|----------------
 * 1 byte   | 1 byte     | 1 byte     | 1 byte       | 4 bytes
 */
#define BLE_ANCS_NOTIFICATION_DATA_LENGTH   8

#define ANCS_UUID_SERVICE                   0xF431  //!< 16-bit service UUID for the Apple Notification Center Service.
#define ANCS_UUID_CHAR_CONTROL_POINT        0xD8F3  //!< 16-bit control point UUID.
#define ANCS_UUID_CHAR_DATA_SOURCE          0xC6E9  //!< 16-bit data source UUID.
#define ANCS_UUID_CHAR_NOTIFICATION_SOURCE  0x120D  //!< 16-bit notification source UUID.


#define BLE_ANCS_EVENT_FLAG_SILENT          0       //!< 0b.......1 Silent: First (LSB) bit is set. All flags can be active at the same time.
#define BLE_ANCS_EVENT_FLAG_IMPORTANT       1       //!< 0b......1. Important: Second (LSB) bit is set. All flags can be active at the same time.
#define BLE_ANCS_EVENT_FLAG_PREEXISTING     2       //!< 0b.....1.. Pre-existing: Third (LSB) bit is set. All flags can be active at the same time.
#define BLE_ANCS_EVENT_FLAG_POSITIVE_ACTION 3       //!< 0b....1... Positive action: Fourth (LSB) bit is set. All flags can be active at the same time.
#define BLE_ANCS_EVENT_FLAG_NEGATIVE_ACTION 4       //!< 0b...1.... Negative action: Fifth (LSB) bit is set. All flags can be active at the same time.

/** @defgroup BLE_ANCS_NP_ERROR_CODES Notification Provider (iOS) Error Codes
 * @{ */
#define BLE_ANCS_NP_UNKNOWN_COMMAND         0x01A0  //!< The command ID is unknown to the NP.
#define BLE_ANCS_NP_INVALID_COMMAND         0x01A1  //!< The command format is invalid.
#define BLE_ANCS_NP_INVALID_PARAMETER       0x01A2  //!< One or more parameters do not exist in the NP.
#define BLE_ANCS_NP_ACTION_FAILED           0x01A3  //!< The action failed to be performed by the NP.
/** @} */


/**@brief Event types that are passed from client to application on an event. */
typedef enum
{
    BLE_ANCS_C_EVT_DISCOVERY_COMPLETE,         /**< A successful connection has been established and the service was found on the connected peer. */
    BLE_ANCS_C_EVT_DISCOVERY_FAILED,           /**< It was not possible to discover the service or characteristics of the connected peer. */
    BLE_ANCS_C_EVT_NOTIF,                      /**< An iOS notification was received on the notification source control point. */
    BLE_ANCS_C_EVT_INVALID_NOTIF,              /**< An iOS notification was received on the notification source control point, but the format is invalid. */
    BLE_ANCS_C_EVT_NOTIF_ATTRIBUTE,            /**< A received iOS notification attribute has been parsed. */
    BLE_ANCS_C_EVT_APP_ATTRIBUTE,              /**< An iOS app attribute has been parsed. */
    BLE_ANCS_C_EVT_NP_ERROR,                   /**< An error has been sent on the ANCS Control Point from the iOS Notification Provider. */
} ble_ancs_c_evt_type_t;

/**@brief Category IDs for iOS notifications. */
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
    BLE_ANCS_CATEGORY_ID_ENTERTAINMENT         /**< The iOS notification belongs to the "Entertainment" category. */
} ble_ancs_c_category_id_val_t;

/**@brief Event IDs for iOS notifications. */
typedef enum
{
    BLE_ANCS_EVENT_ID_NOTIFICATION_ADDED,     /**< The iOS notification was added. */
    BLE_ANCS_EVENT_ID_NOTIFICATION_MODIFIED,  /**< The iOS notification was modified. */
    BLE_ANCS_EVENT_ID_NOTIFICATION_REMOVED    /**< The iOS notification was removed. */
} ble_ancs_c_evt_id_values_t;

/**@brief Control point command IDs that the Notification Consumer can send to the Notification Provider. */
typedef enum
{
    BLE_ANCS_COMMAND_ID_GET_NOTIF_ATTRIBUTES,      /**< Requests attributes to be sent from the NP to the NC for a given notification. */
    BLE_ANCS_COMMAND_ID_GET_APP_ATTRIBUTES,        /**< Requests attributes to be sent from the NP to the NC for a given iOS app. */
    BLE_ANCS_COMMAND_ID_GET_PERFORM_NOTIF_ACTION,  /**< Requests an action to be performed on a given notification. For example, dismiss an alarm. */
} ble_ancs_c_cmd_id_val_t;

/**@brief IDs for actions that can be performed for iOS notifications. */
typedef enum
{
    ACTION_ID_POSITIVE = 0,  /**< Positive action. */
    ACTION_ID_NEGATIVE       /**< Negative action. */
} ble_ancs_c_action_id_values_t;

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
    BLE_ANCS_NOTIF_ATTR_ID_APP_IDENTIFIER = 0,     /**< Identifies that the attribute data is of an "App Identifier" type. */
    BLE_ANCS_NOTIF_ATTR_ID_TITLE,                  /**< Identifies that the attribute data is a "Title". */
    BLE_ANCS_NOTIF_ATTR_ID_SUBTITLE,               /**< Identifies that the attribute data is a "Subtitle". */
    BLE_ANCS_NOTIF_ATTR_ID_MESSAGE,                /**< Identifies that the attribute data is a "Message". */
    BLE_ANCS_NOTIF_ATTR_ID_MESSAGE_SIZE,           /**< Identifies that the attribute data is a "Message Size". */
    BLE_ANCS_NOTIF_ATTR_ID_DATE,                   /**< Identifies that the attribute data is a "Date". */
    BLE_ANCS_NOTIF_ATTR_ID_POSITIVE_ACTION_LABEL,  /**< The notification has a "Positive action" that can be executed associated with it. */
    BLE_ANCS_NOTIF_ATTR_ID_NEGATIVE_ACTION_LABEL,  /**< The notification has a "Negative action" that can be executed associated with it. */
} ble_ancs_c_notif_attr_id_val_t;

/**@brief Flags for iOS notifications. */
typedef struct
{
    uint8_t silent          : 1;  //!< If this flag is set, the notification has a low priority.
    uint8_t important       : 1;  //!< If this flag is set, the notification has a high priority.
    uint8_t pre_existing    : 1;  //!< If this flag is set, the notification is pre-existing.
    uint8_t positive_action : 1;  //!< If this flag is set, the notification has a positive action that can be taken.
    uint8_t negative_action : 1;  //!< If this flag is set, the notification has a negative action that can be taken.
} ble_ancs_c_notif_flags_t;

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










/**@brief iOS notification structure. */
// typedef struct
// {
//     uint32_t                        notif_uid;       //!< Notification UID.
//     ble_ancs_c_evt_id_values_t      evt_id;          //!< Whether the notification was added, removed, or modified.
//     ble_ancs_c_notif_flags_t        evt_flags;       //!< Bitmask to signal whether a special condition applies to the notification. For example, "Silent" or "Important".
//     ble_ancs_c_category_id_val_t    category_id;     //!< Classification of the notification type. For example, email or location.
//     uint8_t                         category_count;  //!< Current number of active notifications for this category ID.
// } ble_ancs_c_evt_notif_t;

/**@brief iOS attribute structure. This type is used for both notification attributes and app attributes. */
// typedef struct
// {
//     uint16_t                          attr_len;     //!< Length of the received attribute data.
//     uint32_t                          attr_id;      //!< Classification of the attribute type. For example, "Title" or "Date".
//     uint8_t                         * p_attr_data;  //!< Pointer to where the memory is allocated for storing incoming attributes.
// } ble_ancs_c_attr_t;

// /**@brief iOS notification attribute structure for incoming attributes. */
// typedef struct
// {
//     uint32_t                          notif_uid;    //!< UID of the notification that the attribute belongs to.
//     ble_ancs_c_attr_t                 attrs;        //!< A received attribute.
// } ble_ancs_c_evt_attr_t;

// typedef struct
// {
//     uint16_t                          attr_len;     //!< Length of the received attribute data.
//     uint32_t                          attr_id;      //!< Classification of the attribute type. For example, "Title" or "Date".
//     uint8_t                         * p_attr_data;  //!< Pointer to where the memory is allocated for storing incoming attributes.
// } ble_ancs_c_evt_app_attr_t;

// /**@brief iOS notification attribute content requested by the application. */
// typedef struct
// {
//     bool                              get;          //!< Boolean to determine whether this attribute will be requested from the Notification Provider.
//     uint32_t                          attr_id;      //!< Attribute ID: AppIdentifier(0), Title(1), Subtitle(2), Message(3), MessageSize(4), Date(5), PositiveActionLabel(6), NegativeActionLabel(7).
//     uint16_t                          attr_len;     //!< Length of the attribute. If more data is received from the Notification Provider, all the data beyond this length is discarded.
//     uint8_t                         * p_attr_data;  //!< Pointer to where the memory is allocated for storing incoming attributes.
// } ble_ancs_c_attr_list_t;

// /**@brief Structure used for holding the Apple Notification Center Service found during the
//           discovery process.
//  */
// typedef struct
// {
//     ble_gattc_service_t service;            //!< The GATT Service holding the discovered Apple Notification Center Service. (0xF431).
//     ble_gattc_char_t    control_point_char; //!< ANCS Control Point Characteristic. Allows interaction with the peer (0xD8F3).
//     ble_gattc_char_t    notif_source_char;  //!< ANCS Notification Source Characteristic. Keeps track of arrival, modification, and removal of notifications (0x120D).
//     ble_gattc_desc_t    notif_source_cccd;  //!< ANCS Notification Source Characteristic Descriptor. Enables or disables GATT notifications.
//     ble_gattc_char_t    data_source_char;   //!< ANCS Data Source Characteristic, where attribute data for the notifications is received from peer (0xC6E9).
//     ble_gattc_desc_t    data_source_cccd;   //!< ANCS Data Source Characteristic Descriptor. Enables or disables GATT notifications.
// } ble_ancs_c_service_t;

// /**@brief ANCS client module event structure.
//  *
//  * @details The structure contains the event that is to be handled by the main application.
//  */
// typedef struct
// {
//     ble_ancs_c_evt_type_t  evt_type;                       //!< Type of event.
//     uint16_t               conn_handle;                    //!< Connection handle on which the ANCS service was discovered on the peer device. This is filled if the @p evt_type is @ref BLE_ANCS_C_EVT_DISCOVERY_COMPLETE.
//     ble_ancs_c_evt_notif_t notif;                          //!< iOS notification. This is filled if @p evt_type is @ref BLE_ANCS_C_EVT_NOTIF.
//     uint16_t               err_code_np;                    //!< An error coming from the Notification Provider. This is filled with @ref BLE_ANCS_NP_ERROR_CODES if @p evt_type is @ref BLE_ANCS_C_EVT_NP_ERROR.
//     ble_ancs_c_attr_t      attr;                           //!< iOS notification attribute or app attribute, depending on the event type.
//     uint32_t               notif_uid;                      //!< Notification UID.
//     uint8_t                app_id[BLE_ANCS_ATTR_DATA_MAX]; //!< App identifier.
//     ble_ancs_c_service_t   service;                        //!< Information on the discovered Alert Notification Service. This is filled if the @p evt_type is @ref BLE_ANCS_C_EVT_DISCOVERY_COMPLETE.
// } ble_ancs_c_evt_t;

// /**@brief iOS notification event handler type. */
// // typedef void (*ble_ancs_c_evt_handler_t) (ble_ancs_c_evt_t * p_evt);

// typedef struct
// {
//     ble_ancs_c_attr_list_t * p_attr_list;              //!< The current list of attributes that are being parsed. This will point to either @ref ble_ancs_c_t::ancs_notif_attr_list or @ref  ble_ancs_c_t::ancs_app_attr_list.
//     uint32_t                 nb_of_attr;               //!< Number of possible attributes. When parsing begins, it is set to either @ref BLE_ANCS_NB_OF_NOTIF_ATTR or @ref BLE_ANCS_NB_OF_APP_ATTR.
//     uint32_t                 expected_number_of_attrs; //!< The number of attributes expected upon receiving attributes. Keeps track of when to stop reading incoming attributes.
//     ble_ancs_c_parse_state_t parse_state;              //!< ANCS notification attribute parsing state.
//     ble_ancs_c_cmd_id_val_t  command_id;               //!< Variable to keep track of what command type is being parsed ( @ref BLE_ANCS_COMMAND_ID_GET_NOTIF_ATTRIBUTES or @ref BLE_ANCS_COMMAND_ID_GET_APP_ATTRIBUTES).
//     uint8_t                * p_data_dest;              //!< Attribute that the parsed data is copied into.
//     uint16_t                 current_attr_index;       //!< Variable to keep track of the parsing progress, for the given attribute.
//     uint32_t                 current_app_id_index;     //!< Variable to keep track of the parsing progress, for the given app identifier.
// } ble_ancs_parse_sm_t;

/**@brief iOS notification structure, which contains various status information for the client. */
// typedef struct
// {
//     ble_ancs_c_evt_handler_t         evt_handler;                                     //!< Event handler to be called for handling events in the Apple Notification client application.
//     ble_srv_error_handler_t          error_handler;                                   //!< Function to be called in case of an error.
//     nrf_ble_gq_req_error_cb_t        gatt_err_handler;                                //!< Error handler to be called in case of an error from SoftDevice.
//     uint16_t                         conn_handle;                                     //!< Handle of the current connection. Set with @ref nrf_ble_ancs_c_handles_assign when connected.
//     ble_ancs_c_service_t             service;                                         //!< Structure to store the different handles and UUIDs related to the service.
//     ble_ancs_c_attr_list_t           ancs_notif_attr_list[BLE_ANCS_NB_OF_NOTIF_ATTR]; //!< For all attributes: contains information about whether the attributes are to be requested upon attribute request, and the length and buffer of where to store attribute data.
//     ble_ancs_c_attr_list_t           ancs_app_attr_list[BLE_ANCS_NB_OF_APP_ATTR];     //!< For all app attributes: contains information about whether the attributes are to be requested upon attribute request, and the length and buffer of where to store attribute data.
//     uint32_t                         number_of_requested_attr;                        //!< The number of attributes that are to be requested when an iOS notification attribute request is made.
//     ble_ancs_parse_sm_t              parse_info;                                      //!< Structure containing different information used to parse incoming attributes correctly (from data_source characteristic).
//     ble_ancs_c_evt_t                 evt;                                             //!< Allocate memory for the event here. The event is filled with several iterations of the @ref ancs_parse_get_attrs_response function when requesting iOS notification attributes. 
//     nrf_ble_gq_t                   * p_gatt_queue;                                    //!< Pointer to the BLE GATT Queue instance.

// } ble_ancs_c_t;

/**@brief Apple Notification client init structure, which contains all options and data needed for
 *        initialization of the client. */
// typedef struct
// {
//     ble_ancs_c_evt_handler_t   evt_handler;    //!< Event handler to be called for handling events in the Battery Service.
//     ble_srv_error_handler_t    error_handler;  //!< Function to be called in case of an error.
//     nrf_ble_gq_t             * p_gatt_queue;   //!< Pointer to the BLE GATT Queue instance.
// } ble_ancs_c_init_t;



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
    ble_ancs_services_struct_t ancs_srvc_char;


}ble_ancs_struct_t;


/**@brief Apple Notification Center Service UUIDs. */
// extern const ble_uuid128_t ble_ancs_base_uuid128;     //!< Service UUID.
// extern const ble_uuid128_t ble_ancs_cp_base_uuid128;  //!< Control Point UUID.
// extern const ble_uuid128_t ble_ancs_ns_base_uuid128;  //!< Notification Source UUID.
// extern const ble_uuid128_t ble_ancs_ds_base_uuid128;  //!< Data Source UUID.







////////////////////////////////////////////////////////////////////////////////////////////////
///////////// function declartions //////////////////////////////////////////////////////////


/// @brief this is to init the services of the ble ancs at the start of program  
/// @param void 
void ble_ancs_pre_init(void);

/// @brief this is to init the ancs profile from iphone 
/// @param conn_handle 
/// @return 
uint32_t  ble_ancs_init(uint16_t conn_handle);

#endif