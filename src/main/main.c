///////////////////////////////////////////////////////////////////
//////////////////// include the freertos librarys
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

////////// include user libs here
#include "system.h"
#include "gpio.h"
#include "gpiote.h"
#include "nrf_time.h"
#include "nvs.h"

////// include the helper library to help in  managing the device status and error code
#include "main_helper.h"

/// //////////////// include the ble init routines for init the BLE stack
#include "ble_softdevice_init.h"
/////////// include the gap function lib
#include "ble_gap_func.h"
////////// include the gatt client module
#include "ble_gatt_client.h"
/////// include the gatt server module
#include "ble_gatt_server.h"
////// include the ble ancs ///////////////////////////////
#include "ble_ancs.h"
////////// include the ams to init the services
#include "ble_ams.h"

#include "ble_common_task.h"

/////// include the device
#include "nrf_button.h"

//// include the kernel mem manager
#include "memory_manager/kernel_mem_manager.h"


#include "message_buffer.h"

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////// general Task Function decleartions /////////////////////////////////////

void general_task_function(void *);

#define genral_task_stack_size 256 // 1KB memory
#define genral_task_name "gen_task"
#define genral_task_param NULL
#define genral_task_priority TASK_PRIORITY_5

/* Structure that will hold the TCB of the task being created. */
static StaticTask_t gen_task_buffer;

/* Buffer that the task being created will use as its stack.  Note this is
   an array of StackType_t variables.  The size of StackType_t is dependent on
   the RTOS port. */
//// this will reserve the 1KB for the freertos task
static StackType_t gen_task_stack[genral_task_stack_size];

xTaskHandle genral_task_handle = NULL; //!< Reference to SoftDevice FreeRTOS task.

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////// defination of main task

static void ble_functionalities_init(void)
{
    // init the ble
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

    ////// init the common task
    ble_task_pre_init(NULL);
}

int main()
{

    if (sys_init() != nrf_OK)
    {
        APP_ERROR_HANDLER(nrf_ERR_OPERATION_FAILED);
    }

    // // install all the communication drivers
    driver_install();

    ////////////////////////////////////
    NRF_LOG_INFO("starting aPP");

    ble_gap_genreate_keypair();

    ble_functionalities_init();
    /// init nvs
    nvs_flash_init(NVS_FLASH_OPERATION_TIMEOUT);

    ////// init the devices here
    nrf_button_evt_lib_init();

    /// @todo have to implement the kernel init file
    // watchx_kernel_init();
    /// init the accelrometer task
    /// init the led , buzzer libs

    // hardfault is diabled . please enable it

    genral_task_handle = xTaskCreateStatic(general_task_function, genral_task_name, genral_task_stack_size,
                                           genral_task_param, genral_task_priority, gen_task_stack, &gen_task_buffer);
    if (genral_task_handle == NULL)
    {
        APP_ERROR_HANDLER(nrf_ERR_OUT_OF_MEM);
    }

    vTaskStartScheduler();
    for (;;)
    {
        // printf("dev id is %x\r\n ", st_get_device_id());
        // printf("the vcell is %f V,soc %d %%, the remcap %dmAh,tte is %lds, current %ldmA, temperature %f*C status %X \r\n",
        // fgage_get_vcell(), fgage_get_soc(), fgage_get_remcap(), fgage_get_tte(), fgage_get_curent(), (float)fgage_get_temp(), fgage_get_status());

        __WFI();
        __NOP();
        // delay(500);
        //     // // no delay at al
        // uart_log_bytes((uint8_t *)msg, sizeof(msg) );
        // gpio_pin_toogle(LED_1);

        nrf_delay_ms(100);
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////// ///////////// /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////// static functions define here /////////////////////////////////////////////

/// cereate a memory instance here

KERNEL_MEM_INSTANTISE(test_inst, my_pool, 100, test_mutex);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////// genral task /////////////////////////////////////////////
void general_task_function(void *param)
{
    UNUSED_VARIABLE(param);

    uint8_t ret = 0;
    for (;;)
    {
    
        ///// check for the button events and print it
        uint8_t evt = nrf_get_btn_evtq();
        if (evt != 0)
        {
            NRF_LOG_WARNING("evt %d", evt);
            if (evt == NRF_BUTTON_UP_EVT)
            {
                //     //// start the advertise
                   NRF_LOG_INFO("adv%d", ble_gap_start_advertise());
            }
            else if (evt == NRF_BUTTON_DOWN_EVT)
            {
                NRF_LOG_INFO("adv%d", ble_gap_stop_advertise());

            }
            else if (evt == NRF_BUTTON_MIDD_EVT)
            {

            }
        }
        delay(100);

        // NRF_LOG_INFO("main task 1");
    }

    vTaskDelete(NULL);
}