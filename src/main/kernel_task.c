#include "kernel_task.h"

//// include the nvs library
#include "nvs.h"

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
/// implement the driver for the kernel 
#include "device_drivers/drivers_common/drivers_common.h"

#include "device_drivers/accelrometer/nrf_accelromter.h"
#include "device_drivers/button/nrf_button.h"
#include "device_drivers/display/nrf_oled_driver.h"
#include "device_drivers/display/nrf_gfx.h"
//================================================================================================
//=================================================================================================
//==================== extern variables here ======================================================

//================================================================================================
//=================================================================================================
//==================== Global variables here ======================================================

#define KERNEL_TASK_NAME "Kernel_Task"

static StackType_t kernel_task_stack_buffer[NRF_KERNEL_TASK_STACK_DEPTH];

/* Structure that will hold the TCB of the task being created. */
static StaticTask_t kernel_Task_TCB;

static volatile uint8_t kernel_task_state;

/// @brief task handle for the kernel
static xTaskHandle kernnel_Task_handle = NULL;

//================================================================================================
//=================================================================================================
//==================== Macros  here ======================================================

/// @brief this is used to run the kernel task , this is run to completion the kernel_Taskhandle cant be null
#define kernel_task_run(x) \
    kernel_task_state = x; \
    vTaskResume(kernnel_Task_handle);

//// define a index so that we can have a instance of gap
#define BLE_GAP_DEVICE_INDEX 0

//================================================================================================
//=================================================================================================
//==================== function declarations here ======================================================

/// this is the kernel task
void kernel_task(void *param);

/// @brief handle the ble notification
/// @param param
/// @param evt
static void ble_client_handle_value_notification(ble_gattc_evt_t const *p_ble_evt);

/// @brief handle the ble indication
/// @param param
/// @param evt
static void ble_client_handle_value_indication(ble_gattc_evt_t const *p_ble_evt);

/// @brief this function got called when the timeout occured
/// @param param
static void ble_client_timeout_handler(ble_gattc_evt_t const *p_ble_evt);

/// @brief this function is called when there is an error triggered
/// @param param
static void ble_client_error_handler(ble_gattc_evt_t const *p_ble_evt);

/// @brief   device connected callback
/// @param param
/// @param gap_evt
static void ble_device_connected_callback(void *param, ble_gap_evt_t const *gap_evt);

/// @brief / device disconnected callback
/// @param param
/// @param gap_evt
static void ble_device_disconnected_callback(void *param, ble_gap_evt_t const *gap_evt);

/// @brief this is the timeout callback for the gap events
/// @param param
/// @param gap_evt
static void ble_gap_timeout_callback(void *param, ble_gap_evt_t const *gap_evt);

//================================================================================================
//=================================================================================================
//==================== functions definations here ======================================================

/// @brief the task preinit to initalise the task
/// @param  void
void Kernel_task_preinit(void)
{
    /// init the spi,i2c and gpio drivers
    Hardware_drivers_install();
    
    /// init the ble functionalities of the device
    //////////////////////////////////////////////////////////
    // init the ble stack
    nrf_softdevice_init_and_start();
    //// init the gatt server
    ble_gatt_server_pre_init();
    /// init the gatt client module
    gatt_client_pre_init();
    /// pre init the gap
    ble_gap_pre_init();
    // init the ble ancs
    ble_ancs_pre_init();
    // init the ams service
    ble_ams_pre_init();


    //// init the kernel time module here 
    kernel_time_pre_init(NULL,NULL);

    /// init nvs flash library 
    nvs_flash_init(NRF_CONFIG_NVS_FLASH_OPERATION_TIMEOUT);

    /// define and add  ble related callbacks here
    ///// add the callbacks for the gap
    ble_gap_add_callback(ble_gap_evt_connected, ble_device_connected_callback, NULL);
    ble_gap_add_callback(ble_gap_evt_disconnected, ble_device_disconnected_callback, NULL);
    ble_gap_add_callback(ble_gap_evt_timeout, ble_gap_timeout_callback, NULL);

    //// add the callback for the gatt client
    ble_gatt_client_add_callback(ble_gatt_client_notif_callback, ble_client_handle_value_notification);
    ble_gatt_client_add_callback(ble_gatt_client_indic_callback, ble_client_handle_value_indication);
    ble_gatt_client_add_callback(ble_gatt_client_error_callback, ble_client_error_handler);
    ble_gatt_client_add_callback(ble_gatt_client_timeout_callback, ble_client_timeout_handler);

    /// init a gap instance and define the pairing type 
    ble_gap_instance_init(BLE_GAP_DEVICE_INDEX, PAIRING_TYPE_LESC);

    /// create a kernel task
    kernnel_Task_handle = xTaskCreateStatic(kernel_task, KERNEL_TASK_NAME, NRF_KERNEL_TASK_STACK_DEPTH, NULL, KERNEL_TASK_PRIORITY, kernel_task_stack_buffer, &kernel_Task_TCB);

    if (kernnel_Task_handle == NULL)
    {
        NRF_LOG_ERROR("kernel init falied");
    }
}


/// @todo have to implement 
/// @brief to deinit the any pre initalise thinfs
/// @param  void
void kernel_task_deinit(void)
{
}

void kernel_task(void *param)
{
    UNUSED_PARAMETER(param);

    /// int the oled screen and gfx lib 
    nrf_oled_screen_init();
    nrf_gfx_lib_init(2,3);

    /// init the device driver at the start of kernel task 
    nrf_button_evt_lib_init();
    nrf_accel_evt_lib_init();
       
    /// global err to store the err of retutrns
    uint32_t err = 0;
    /// direct gor to main loop
    goto main_loop;

/// @brief this is to init the ble functionality when connected and then go to main loop
ble_funcs_init:
{

    uint16_t conn_handle = ble_gap_get_conn_handle(BLE_GAP_DEVICE_INDEX);
    NRF_LOG_INFO("connhand %d", conn_handle);


    /// @note this function will wait for the security procedure to finish and then proceed .
    /// start the pairing process
    err = ble_gap_security_init(BLE_GAP_DEVICE_INDEX);
    NRF_ASSERT(err);
    if (err != nrf_OK)
    {
        goto main_loop;
    }
    
    /// init the gatt server 
    ble_gatt_server_init(conn_handle);

    //  init the ble client
    err = gatt_client_init(conn_handle);
    NRF_ASSERT(err);

    //// set the gatt server mtu
    err = gatt_client_set_server_mtu(conn_handle, BLE_GATT_SERVER_RX_MTU);
    NRF_ASSERT(err);

    /// init the peer device
    err = ble_peer_device_init(conn_handle);
    NRF_ASSERT(err);

    // gatt_client_explore_servsice(conn_handle, 10);
    /// init the apple ams
    err = ble_ams_init(conn_handle);
    NRF_ASSERT(err);

    err = ble_ancs_init(conn_handle);
    NRF_ASSERT(err);

    goto main_loop;
}

/// @brief this is to deinit the ble functionality when disconnected and go to main loop
ble_funcs_deinit:
{
    uint16_t conn_handle = ble_gap_get_conn_handle(BLE_GAP_DEVICE_INDEX);

    err = gatt_client_deinit(conn_handle);
    NRF_ASSERT(err);

    NRF_LOG_WARNING("disc %d", conn_handle);
    ////////// call the deinit process of the above init process

    // deinit the ancs , ams ,device info and other device functionality
    // deinit the apple ancs, ams task
    err = ble_ams_deinit();
    NRF_ASSERT(err);

    err = ble_ancs_deinit();
    NRF_ASSERT(err);

    err = ble_peer_device_info_deinit();
    NRF_ASSERT(err);

    ble_gap_instance_clear(BLE_GAP_DEVICE_INDEX);
    goto main_loop;
}

/// this will be invoked when a notification is recvd
ble_func_recv_notif:
{

    ble_ancs_process_notif();
    goto main_loop;
}

ble_func_recv_indic:
{
    NRF_LOG_INFO("ble indc recv");
    goto main_loop;
}

main_loop:
    kernel_task_state = kernel_state_idle;
    for (;;)
    {

        switch (kernel_task_state)
        {
        case kernel_state_ble_disconnected:
            goto ble_funcs_deinit;

        case kernel_state_ble_connected:
            goto ble_funcs_init;

        case kernel_state_ble_notif_recv:
            goto ble_func_recv_notif;
        case kernel_state_ble_indic_recv:
            goto ble_func_recv_indic;
        default:
            /// we can also have some code here for idle situations
            break;
        }
        //// now here it would be only be in a while loop
        vTaskSuspend(NULL);
    }

    //// never reach here
    vTaskDelete(NULL);
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
////////////////// ble gap callbacks ////////////////////////////////////////////////////

/// @brief   device connected callback
/// @param param
/// @param gap_evt
static void ble_device_connected_callback(void *param, ble_gap_evt_t const *gap_evt)
{
    UNUSED_PARAMETER(param);
    //////// set the connection handle
    uint32_t err = ble_gap_set_conn_handle(BLE_GAP_DEVICE_INDEX, gap_evt->conn_handle);
    NRF_ASSERT(err);

    kernel_task_run(kernel_state_ble_connected);
}

/// @brief / device disconnected callback
/// @param param
/// @param gap_evt
static void ble_device_disconnected_callback(void *param, ble_gap_evt_t const *gap_evt)
{
    UNUSED_PARAMETER(param);
    NRF_LOG_ERROR("dct %x", gap_evt->params.disconnected.reason);
    kernel_task_run(kernel_state_ble_disconnected);
}

/// @brief this is the timeout callback for the gap events
/// @param param
/// @param gap_evt
static void ble_gap_timeout_callback(void *param, ble_gap_evt_t const *gap_evt)
{
    UNUSED_PARAMETER(param);
    /// disconnect from the peer
    NRF_LOG_ERROR("gap timeout");
    ble_gap_disconnect(gap_evt->conn_handle);
}
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////// ble client callbacks

/// @brief handle the ble notification
/// @param param
/// @param evt
static void ble_client_handle_value_notification(ble_gattc_evt_t const *gattc_evt)
{
    bool kernel_Task_woken = 0;
    ///////////// these callbacks are gonna run in the softdevice event handler
    /// handle the peer device notification
    kernel_Task_woken = ble_peer_Device_notification_handler(gattc_evt);
    /// ble ams client handler
    kernel_Task_woken = ble_ams_client_event_handler(gattc_evt);
    /// handle the ancs client
    kernel_Task_woken = ble_ancs_client_event_handler(gattc_evt);

    /////// only handle the ble event when required like a uurgent notif rcvd like call,alarm
    if (kernel_Task_woken)
    { //// switch the kernel task state
        kernel_task_run(kernel_state_ble_notif_recv);
    }
}

/// @brief handle the ble indication
/// @param param
/// @param evt
static void ble_client_handle_value_indication(ble_gattc_evt_t const *gattc_evt)
{
    ble_peer_Device_indication_handler(gattc_evt);

    kernel_task_run(kernel_state_ble_indic_recv);
}

/// @brief this function is called when there is an error triggered
/// @param param
static void ble_client_error_handler(ble_gattc_evt_t const *gattc_evt)
{
    NRF_LOG_ERROR("err  %x", gattc_evt->error_handle);
}

/// @brief this function got called when the timeout occured
/// @param param
static void ble_client_timeout_handler(ble_gattc_evt_t const *gattc_evt)
{
    ble_gap_disconnect(gattc_evt->conn_handle);
    NRF_LOG_ERROR("gattc timeout %d", gattc_evt->params.timeout.src);
}
