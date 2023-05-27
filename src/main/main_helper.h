#ifndef MAIN_HELPER_H
#define MAIN_HELPER_H


#include "system.h"


/////////////////////////////////////////////////
///////////////////////////////////////////////
/////////////////// enums for storing the error code and status code 

enum
{
    error_none = 0x01,
    error_timeout,
    error_eeg_queue_err,
    error_eeg_hardware_err,
    error_tdcs_headband_not_in_correct_pos,
    error_tdcs_current_limit_exceeds_hard_detect,
    error_tdcs_current_limit_exceeds_soft_detect,
    error_eeg_xyz,
    error_batt_low
    
};

////////// enum to store status of the system 
enum
{
    status_none = 0x01,
    status_idle,
    status_eeg_runs,
    status_tdcs_runs, 
    status_pwr_save,
    status_firmware_update_mode 
    
};



////////////////////////////////////////////////////////////////
/////////////////////// enum to store the color of the system led 
enum
{
    RED_COLOR =0x01,
    GREEN_COLOR,
    BLUE_COLOR,
    ORANGE_COLOR,
    YELLOW_COLOR,
    NO_COLOR,
    WHITE_COLOR
};



/////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
////////////////// define the priorityies of the task 
#define TASK_PRIORITY_1  1
#define TASK_PRIORITY_2  2 
#define TASK_PRIORITY_3  3
#define TASK_PRIORITY_4  4 
#define TASK_PRIORITY_5  5


//////// wait for the queue and  message 
#define wait_for_q_msg 10  
#define wait_for_msg_buffer 10

#define wait_for_ble_notifc U32_MAX


#define message_buffer_size 22
///////////
//////////////////////// init the device related functionality 
/////////// init the message buffer , status and err code queues

/***
 * @name device_mgmt_init 
 * @note used to init the device management system like q and message buffer 
*/
void device_mgmmt_init(void);


/// @brief //////////////////////////////
/////////////////////////////////
// typedef void * TaskHandle_t;

/// @brief ////// delete the task properly 
/// @param handle 
void taskdelete( xTaskHandle * handle);

/// @brief used to validate that task handle is not null 
/// @param handle 
void  validate_Task_handle(xTaskHandle handle);



/***
 * @name system_shutdown 
 * @param void
 * @note used to shutdown the system
*/
void system_shutdown(void);



#endif