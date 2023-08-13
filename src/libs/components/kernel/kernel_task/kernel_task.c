#include "kernel_task.h"

/// include the softdevice
#include "ble_softdevice_init.h"

///////// incude the gap, gattt client and server
#include "ble_gap_func.h"
#include "ble_gatt_server.h"
#include "ble_gatt_client.h"

//// include the implmentation specific library
#include "ble_ams.h"
#include "ble_ancs.h"
#include "ble_peer_info.h"

/// freertos libraries
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/// include the driver common files
#include "device_drivers/drivers_common/drivers_common.h"

//================================================================================================
//=================================================================================================
//==================== Macros variables here ======================================================

#define KERNEL_TASK_NAME "Kernel_Task"

static StackType_t kernel_task_stack_buffer[NRF_KERNEL_TASK_STACK_DEPTH];

/* Structure that will hold the TCB of the task being created. */
static StaticTask_t kernel_Task_TCB;

static volatile uint8_t kernel_task_state;

/// @brief task handle for the kernel
static volatile xTaskHandle kernnel_Task_handle = NULL;

//// define a index so that we can have a instance of gap
#define BLE_GAP_DEVICE_INDEX 0
//================================================================================================
//=================================================================================================
//==================== extern variables here ======================================================

//================================================================================================
//=================================================================================================
//==================== Global variables here ======================================================

//================================================================================================
//=================================================================================================
//==================== function declarations here ======================================================
void kernel_task(void *param);

//================================================================================================
//=================================================================================================
//==================== functions definations here ======================================================

/// @brief the task preinit to initalise the task
/// @param  void
void Kernel_task_preinit(void)
{
    /// init the spi,i2c and gpio drivers
    Hardware_drivers_install();

    /// define and add  ble related callbacks here
    ///// add the callbacks for the gap
    ble_gap_add_callback(ble_gap_evt_connected, ble_device_connected_callback, NULL);
    ble_gap_add_callback(ble_gap_evt_disconnected, ble_device_disconnected_callback, NULL);

    //// assign the callback for that connection
    ble_gatt_client_add_callback()
    gatt_client_add_notif_callback( ble_task_handle_value_notification, NULL);
    gatt_client_add_indication_callback( ble_task_handle_value_indication, NULL);
    gatt_client_add_err_handler_callback( ble_client_error_handler, NULL);
    gatt_client_add_timeout_callback( ble_client_timeout_handler, NULL);
    

    /// init a gap instance
    ble_gap_instance_init(BLE_GAP_DEVICE_INDEX, PAIRING_TYPE_LESC);

    /// create a kernel task
    kernnel_Task_handle = xTaskCreateStatic(kernel_task, KERNEL_TASK_NAME, NRF_KERNEL_TASK_STACK_DEPTH, NULL, KERNEL_TASK_PRIORITY, kernel_task_stack_buffer, &kernel_Task_TCB);

    if (kernnel_Task_handle == NULL)
    {
        NRF_LOG_ERROR("kernel init falied");
    }
}

/// @brief to deinit the any pre initalise thinfs
/// @param  void
void kernel_task_deinit(void)
{
}

void kernel_task(void *param)
{
    UNUSED_PARAMETER(param);

    /// direct gor to main loop
    goto main_loop;

/// @brief this is to init the ble functionality and then go to main loop
ble_funcs_init:
{
    uint32_t err = 0;
    uint16_t conn_handle = ble_gap_get_conn_handle(BLE_GAP_DEVICE_INDEX);

    /// init the ble client
    err = gatt_client_init(conn_handle);
    NRF_ASSERT(err);

    /// @note this function will wait for the security procedure to finish and then proceed .
    /// start the pairing process
    err = ble_gap_security_init(BLE_GAP_DEVICE_INDEX);
    NRF_ASSERT(err);

    /// @todo find why we have to give a delay here
    // delay(2000);
    //// set the gatt server mtu
    err = gatt_client_set_server_mtu(conn_handle, BLE_GATT_SERVER_RX_MTU);
    NRF_ASSERT(err);

    /// init the peer device
    // err = ble_peer_device_init(conn_handle);
    // NRF_ASSERT(err);

    //// init the ancs , ams ,device info and other device functionality
    /// init the apple ancs, ams task , current time task
    // err = ble_ams_init(conn_handle);
    // NRF_ASSERT(err);
    // err = ble_ancs_init(conn_handle);
    // NRF_ASSERT(err);

    goto main_loop;
}

/// @brief this is to deinit the ble functionality and go to main loop
ble_funcs_deinit:
{
    gatt_client_deinit(gap_evt->conn_handle);

    ////////// call the deinit process of the above init process

    //// deinit the ancs , ams ,device info and other device functionality

    /// deinit the apple ancs, ams task
    // err = ble_ams_deinit();
    // NRF_ASSERT(err);

    // err = ble_ancs_deinit();
    // NRF_ASSERT(err);

    // err = ble_peer_device_info_deinit();
    // NRF_ASSERT(err);
    goto main_loop;
}

main_loop:
    for (;;)
    {
        // check if a switch is requierd
        if (kernel_task_state != state_idle)
        {
            switch (kernel_task_state)
            {
            case kernel_state_ble_disconnected:
                goto ble_funcs_deinit;

            default:
                break;
            }
        }

        //// now here it would be only be in a while loop
        delay(100);
    }

    //// never reach here
    vTaskDelete(NULL);
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
+*
/// @brief   device connected callback
/// @param param
/// @param gap_evt
static void ble_device_connected_callback(void *param, ble_gap_evt_t const *gap_evt)
{
    uint32_t err = 0;
    //////// set the connection handle
    err = ble_gap_set_conn_handle(BLE_GAP_DEVICE_INDEX, gap_evt->conn_handle);
    NRF_ASSERT(err);

    kernel_task_state = kernel_state_ble_connected;
    // NRF_LOG_INFO("i %dconha %x",device_index,conn_handle);
}

/// @brief / device disconnected callback
/// @param param
/// @param gap_evt
static void ble_device_disconnected_callback(void *param, ble_gap_evt_t const *gap_evt)
{
    ////// suspend the client task , no need to call here
    /// call the instance clear functions
    ble_gap_instance_clear(BLE_GAP_DEVICE_INDEX);

    kernel_task_state = kernel_state_ble_disconnected;
    // NRF_LOG_WARNING("deinit %x",gap_evt->conn_handle);
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////// ble handle task notification

/// @brief handle the ble notification
/// @param param
/// @param evt
static void ble_task_handle_value_notification(void *param, ble_gattc_evt_t const *evt)
{
    ///////////// these callbacks are gonna run in the softdevice event handler
    /// the param is null for now , can be updated in future
    UNUSED_PARAMETER(param);

    /// handle the peer device notification
    ble_peer_Device_notification_handler(evt);

    /// ble ams client handler
    ble_ams_client_event_handler(param, evt);

    /// handle the ancs client
    ble_ancs_client_event_handler(param, evt);

    //// switch the kernel task state
    kernel_task_state = kernel_state_ble_recv_notif;
}

/// @brief handle the ble indication
/// @param param
/// @param evt
static void ble_task_handle_value_indication(void *param, ble_gattc_evt_t const *evt)
{
    UNUSED_PARAMETER(param);

    ble_peer_Device_indication_handler(evt);

    kernel_task_state = kernel_state_ble_recv_indic;
}

/// @brief this function is called when there is an error triggered
/// @param param
static void ble_client_error_handler(void *param, uint16_t gatt_status)
{
    UNUSED_PARAMETER(param);

    NRF_LOG_ERROR("err  %x", gatt_status);
}

/// @brief this function got called when the timeout occured
/// @param param
static void ble_client_timeout_handler(void *param)
{
    UNUSED_PARAMETER(param);

    NRF_LOG_ERROR("timeout");
}