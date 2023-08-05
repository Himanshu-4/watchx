#include "ble_common_task.h"

#include "ble_softdevice_init.h"

///////include the gap api
#include "ble_gap_func.h"

///////// incude the gattt client and server 
#include "ble_gatt_server.h"

#include "ble_gatt_client.h"

//// include the ble specific library 
#include "ble_ams.h"
#include "ble_ancs.h"
#include "ble_peer_info.h"

///////////////////////////////////////////////////////////////////////
//// this will resume the callback 
#define resume 0x10
///// this will suspend the callback 
#define suspend 0x11

static volatile uint8_t client_task  = suspend; 
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////// global variables here 

#define BLE_GAP_DEVICE_INDEX 0


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
static void ble_task_handle_value_notification(void *param, ble_gattc_evt_t const *evt );

/// @brief handle value indication 
/// @param param 
/// @param evt 
static void ble_task_handle_value_indication(void *param, ble_gattc_evt_t const *evt );

/// @brief this function is called when there is an error triggered 
/// @param param 
static void ble_client_error_handler(void *param, uint16_t gatt_status );

/// @brief this function got called when the timeout occured 
/// @param param 
static void ble_client_timeout_handler(void *param);
//////////////////////////////////// this is the ble common task that is handling all the ble stuff 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void ble_common_task(void *param);

/// @brief the task handle for the ble common events task handler 
static xTaskHandle ble_common_Task_handle = NULL;
// /////////////////////////////////////  
/* Structure that will hold the TCB of the task being created. */
// static StaticTask_t ble_common_Task_buffer;

// static StackType_t ble_common_Task_Stack_buffer[BLE_COMMON_TASK_STACK_DEPTH];


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ble_task_pre_init(void *param)
{
    UNUSED_PARAMETER(param);
    // ble_common_Task_handle = xTaskCreateStatic(ble_common_task, BLE_COMMON_TASK_NAME, BLE_COMMON_TASK_STACK_DEPTH, param, BLE_COMMON_TASK_PRIORITY, ble_common_Task_Stack_buffer , &ble_common_Task_buffer );
    
    // NRF_ASSERT_HANDLE(ble_common_Task_handle);
    // vTaskSuspend(ble_common_Task_handle);

    ///// add the callbacks for the gap  
    ble_gap_add_callback(ble_gap_evt_connected,ble_device_connected_callback , NULL);
    ble_gap_add_callback(ble_gap_evt_disconnected ,ble_device_disconnected_callback, NULL );  

    /// init a gap instance 
    ble_gap_instance_init(BLE_GAP_DEVICE_INDEX, PAIRING_TYPE_LESC);

    ///@todo add the callback for the security procedure complete 
}




/////////////////////////////////////////////////////////////////////////////////////=============
//////////////////////////////////////////////////////////////////////////////////////////////////
///////======================================= static functions ==================================

/// @brief this is the function to init the gatt client modules for apple ancs abd ams task 
/// @param param 
static void ble_client_task_init_process(void *param)
{
    uint32_t err =0;
    uint16_t conn_handle = ble_gap_get_conn_handle(BLE_GAP_DEVICE_INDEX);

    NRF_LOG_INFO("chdl %x", conn_handle);
    /// init the ble client 
    NRF_LOG_INFO("cinit %d", gatt_client_init(conn_handle));
    NRF_ASSERT(err);

    //// assign the callback for that connection 
    err= gatt_client_add_notif_callback(conn_handle, ble_task_handle_value_notification, NULL);
    NRF_ASSERT(err);

    err = gatt_client_add_indication_callback(conn_handle ,ble_task_handle_value_indication , NULL);
    NRF_ASSERT(err);

    err = gatt_client_add_err_handler_callback(conn_handle , ble_client_error_handler , NULL );
    NRF_ASSERT(err);

    err = gatt_client_add_timeout_callback(conn_handle, ble_client_timeout_handler, NULL);
    NRF_ASSERT(err);

    /// @todo find why we have to give a delay here 
    // delay(2000);
       //// set the gatt server mtu 
    // err = gatt_client_set_server_mtu(conn_handle, BLE_GATT_SERVER_RX_MTU );
    // NRF_ASSERT(err);
    
    /// init the peer device 
    // err = ble_peer_device_init(conn_handle);
    // NRF_ASSERT(err);

    delay(100);
    /// start the pairing process 
    err = ble_gap_security_init(BLE_GAP_DEVICE_INDEX);
    NRF_LOG_INFO("bg,%d",err);
    // NRF_ASSERT(err);
    //// init the ancs , ams ,device info and other device functionality 
    /// init the apple ancs, ams task , current time task  
    // err = ble_ams_init(conn_handle);
    // NRF_ASSERT(err);

    // err = ble_ancs_init(conn_handle);
    // NRF_ASSERT(err);

}



static void ble_client_task_deinit_process(void *param)
{
    ////////// call the deinit process of the above init process     
    uint32_t err =0;

    NRF_LOG_ERROR("task deinit");
    //// deinit the ancs , ams ,device info and other device functionality 
    
    /// deinit the apple ancs, ams task 
    // err = ble_ams_deinit();
    // NRF_ASSERT(err);

    // err = ble_ancs_deinit();
    // NRF_ASSERT(err);
    
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

    ble_client_task_init_process(NULL);

    //////// spend the task in a while loop 
    for(;;)
    {

        if(*callback_resume  == suspend )
        {
            break;
        }


        delay(1000);


    }
        
    ///// call the deinit function , the param may be diffenrt 
    ble_client_task_deinit_process(NULL);
    ///// should never reach here 
    vTaskDelete(NULL);
}





///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


/// @brief   device connected callback 
/// @param param 
/// @param gap_evt
static void ble_device_connected_callback(void *param , ble_gap_evt_t const  * gap_evt)
{
    uint32_t err =0;

    //////// set the connection handle 
    err =  ble_gap_set_conn_handle(BLE_GAP_DEVICE_INDEX, gap_evt->conn_handle);
    NRF_ASSERT(err);

    // NRF_LOG_INFO("i %dconha %x",device_index,conn_handle);

 
    // ///////////// resume the task 
    // vTaskResume(ble_common_Task_handle);

    //create the taskm 
    
    xTaskCreate(ble_common_task, BLE_COMMON_TASK_NAME, BLE_COMMON_TASK_STACK_DEPTH, param, BLE_COMMON_TASK_PRIORITY, &ble_common_Task_handle);
    
    /// start the function from over 
    client_task = resume;
    
    /// init the securoty procedure s
    // ble_gap_security_init(ble_gap_get_conn_handle(device_index),ble_gap_security_param1 );
}

/// @brief / device disconnected callback 
/// @param param 
/// @param gap_evt 
static void ble_device_disconnected_callback(void *param , ble_gap_evt_t const  * gap_evt)
{
    ////// suspend the client task , no need to call here 
    // vTaskSuspend(ble_common_Task_handle);
    // uint16_t *conn_handle = param;
    /// call the deinit process 
    gatt_client_deinit( gap_evt->conn_handle);


    // NRF_LOG_WARNING("deinit %x",gap_evt->conn_handle);

    client_task = suspend;

}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////// ble handle task notification 

/// @brief handle the ble notification 
/// @param param 
/// @param evt 
static void ble_task_handle_value_notification(void *param, ble_gattc_evt_t const  *evt )
{
    ///////////// these callbacks are gonna run in the softdevice event handler 
    /// the param is null for now , can be updated in future 
    UNUSED_PARAMETER(param);

    /// handle the peer device notification 
    ble_peer_Device_notification_handler(evt);
    
    /// ble ams client handler 
    ble_ams_client_event_handler(param, evt);

   /// handle the ancs client  
    ble_ancs_client_event_handler(param , evt);
    
}

/// @brief handle the ble indication 
/// @param param 
/// @param evt 
static void ble_task_handle_value_indication(void *param,ble_gattc_evt_t const *evt )
{
    UNUSED_PARAMETER(param);

    ble_peer_Device_indication_handler(evt);
}

/// @brief this function is called when there is an error triggered 
/// @param param 
static void ble_client_error_handler(void *param, uint16_t gatt_status )
{
    UNUSED_PARAMETER(param);

    NRF_LOG_ERROR("err  %x",gatt_status);
}

/// @brief this function got called when the timeout occured 
/// @param param 
static void ble_client_timeout_handler(void *param)
{
    UNUSED_PARAMETER(param);
    
    NRF_LOG_ERROR("timeout");
   
}