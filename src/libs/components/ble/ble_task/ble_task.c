#include "ble_common_task.h"

#include "ble_softdevice_init.h"

///////include the gap api
#include "ble_gap_func.h"

///////// incude the gattt client and server 
#include "ble_gatt_server.h"

#include "ble_gatt_client.h"

#include "FreeRTOS.h"


///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////////////////// global variables here 

static uint8_t device_index = 0;

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//////////////////////////// function prototypes 
///////// add the callback to the ble client  
static void ble_device_connected_callback(void *param , ble_gap_evt_t const  * gap_evt);

static void ble_device_disconnected_callback(void *param , ble_gap_evt_t const  * gap_evt);


//////////////////////////////////// this is the ble common task that is handling all the ble stuff 
static void ble_common_task(void *param);


static xTaskHandle ble_common_Task_handle = NULL;
// //////////////////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////  
/* Structure that will hold the TCB of the task being created. */
static StaticTask_t ble_common_Task_buffer;

static StackType_t ble_common_Task_Stack_buffer[BLE_COMMON_TASK_STACK_DEPTH];


void ble_common_task_pre_init(void *param)
{


    ble_common_Task_handle = xTaskCreateStatic(ble_common_task, BLE_COMMON_TASK_NAME, BLE_COMMON_TASK_STACK_DEPTH, param, BLE_COMMON_TASK_PRIORITY, ble_common_Task_Stack_buffer , &ble_common_Task_buffer );
    vTaskSuspend(ble_common_Task_handle);

    ///// add the callbacks
    ble_gap_add_callback(ble_gap_evt_connected,ble_device_connected_callback );
    ble_gap_add_callback(ble_gap_evt_disconnected ,ble_device_disconnected_callback );

}



static void ble_common_task(void *param)
{

    UNUSED_PARAMETER(param);


    for(;;)
    {


    }

    ///// never reach here 
    vTaskDelete(NULL);
}
// uint32_t ble_apple_task_start(void *param)
// {
//     /// check that the task is not created and then create the task and assign the handle 

// }




// uint32_t ble_apple_task_close(void *param)
// {

// }





// void ble_apple_Task(void *param)
// {
//     uint16_t * conn_handle = param;

//     ble_ams_init(conn_handle);
//     ble_ancs_init(conn_handle);


//     for(;;)
//     {



//     }



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

    //////// start the encrytption process 



}


static void ble_device_disconnected_callback(void *param , ble_gap_evt_t const  * gap_evt)
{
    


}