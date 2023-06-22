#include "ble_common_task.h"

#include "ble_softdevice_init.h"

///////include the gap api
#include "ble_gap_func.h"

///////// incude the gattt client and server 
#include "ble_gatt_server.h"

#include "ble_gatt_client.h"

#include "FreeRTOS.h"

#include "ble_current_time.h"


///////////////////////////////////////////////////////////////////////
//// this will resume the callback 
#define resume 0x10
///// this will suspend the callback 
#define suspend 0x11

static volatile uint8_t client_task  = suspend; 
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////// global variables here 

static uint8_t device_index = 0;


/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//////////////////////////// function prototypes 

/// @brief   device connected callback 
/// @param param 
/// @param gap_evt
static void ble_device_connected_callback(void *param , ble_gap_evt_t const  * gap_evt);

/// @brief / device disconnected callback 
/// @param param 
/// @param gap_evt 
static void ble_device_disconnected_callback(void *param , ble_gap_evt_t const  * gap_evt);

/// @brief handle value notification handle 
/// @param param 
/// @param evt 
static void ble_task_handle_value_notification(void *param, ble_gattc_evt_t *evt );

//////////////////////////////////// this is the ble common task that is handling all the ble stuff 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void ble_common_task(void *param);

/// @brief the task handle for the ble common events task handler 
static xTaskHandle ble_common_Task_handle = NULL;
// /////////////////////////////////////  
/* Structure that will hold the TCB of the task being created. */
static StaticTask_t ble_common_Task_buffer;

static StackType_t ble_common_Task_Stack_buffer[BLE_COMMON_TASK_STACK_DEPTH];


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ble_common_task_pre_init(void *param)
{
    ble_common_Task_handle = xTaskCreateStatic(ble_common_task, BLE_COMMON_TASK_NAME, BLE_COMMON_TASK_STACK_DEPTH, param, BLE_COMMON_TASK_PRIORITY, ble_common_Task_Stack_buffer , &ble_common_Task_buffer );
    
    NRF_ASSERT_HANDLE(ble_common_Task_handle);
    vTaskSuspend(ble_common_Task_handle);

    ///// add the callbacks
    ble_gap_add_callback(ble_gap_evt_connected,ble_device_connected_callback );
    ble_gap_add_callback(ble_gap_evt_disconnected ,ble_device_disconnected_callback );

}




/////////////////////////////////////////////////////////////////////////////////////=============
//////////////////////////////////////////////////////////////////////////////////////////////////
///////======================================= static functions ==================================

/// @brief this is the function to init the gatt client modules for apple ancs abd ams task 
/// @param param 
static void ble_client_task_init_process(void *param)
{
    uint32_t err =0;
    uint16_t conn_handle = ble_gap_get_conn_handle(device_index);
    //// assign the callback for the 
    err= gatt_client_add_notif_callback(conn_handle, ble_task_handle_value_notification, NULL);
    NRF_ASSERT(err);

    //// init the ancs , ams ,device info and other device functionality 
    /// init the apple ancs, ams task , current time task  
    err = ble_ams_init(conn_handle);
    NRF_ASSERT(err);

    err = ble_ancs_init(conn_handle);
    NRF_ASSERT(err);

    // err = ble_time_init(conn_handle);
    // NRF_ASSERT(err);

    // err = ble_peer_device_info_init(conn_handle);
    // NRF_ASSERT(err);
}


static void ble_client_task_deinit_process(void *param)
{
    ////////// call the deinit process of the above init process     
    uint32_t err =0;
    //// deinit the ancs , ams ,device info and other device functionality 
    
    /// deinit the apple ancs, ams task 
    err = ble_ams_deinit();
    NRF_ASSERT(err);

    err = ble_ancs_deinit();
    NRF_ASSERT(err);
    
    // err = ble_time_deinit();
    // NRF_ASSERT(err);

    // err = ble_peer_device_info_deinit();
    // NRF_ASSERT(err);
}


/// @brief this is the function defination for the ble task 
/// @param param 
static void ble_common_task(void *param)
{

    // UNUSED_PARAMETER(param);

    uint8_t *callback_resume = (uint8_t *) param;

    function_start:
    NRF_LOG_WARNING("cb rsume");

    ble_client_task_init_process(NULL);

    //////// spend the task in a while loop 
    for(;;)
    {

        if(*callback_resume  == resume )
        {
            goto function_suspend;
        }


        delay(10000);

    }

    function_suspend:

    ///// call the deinit function , the param may be diffenrt 
    ble_client_task_deinit_process(NULL);

    vTaskSuspend(NULL);

    goto function_start;
    
    ///// should never reach here 
    system_soft_reset();
    // vTaskDelete(NULL);
}
// uint32_t ble_apple_task_start(void *param)
// {
//     /// check that the task is not created and then create the task and assign the handle 

// }




// uint32_t ble_apple_task_close(void *param)
// {

// }



static void ble_device_connected_callback(void *param , ble_gap_evt_t const  * gap_evt)
{
    /// get the device index from the callback 
    device_index = *(uint8_t *)param;

    if(device_index == 0)
    {
        NRF_LOG_ERROR("invlid index");
        return ;
    }

    NRF_LOG_INFO("i %d",device_index);
    //////// start the encrytption pro

    ///////////// resume the task 
    vTaskResume(ble_common_Task_handle);

    /// start the function from over 
    client_task = resume;
    
    /// init the securoty procedure s
    // ble_gap_security_init(ble_gap_get_conn_handle(device_index),ble_gap_security_param1 );
}


static void ble_device_disconnected_callback(void *param , ble_gap_evt_t const  * gap_evt)
{
    ////// suspend the client task , no need to call here 
    // vTaskSuspend(ble_common_Task_handle);

    client_task = suspend;

}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////// ble handle task notification 
static void ble_task_handle_value_notification(void *param, ble_gattc_evt_t *evt )
{
    /// the param is null for now , can be updated in future 
    UNUSED_PARAMETER(param);

    ///////////// these callbacks are gonna run in the softdevice event handler 
    ble_ams_client_event_handler(param, evt);
    ble_ancs_client_event_handler(param , evt);
    
}