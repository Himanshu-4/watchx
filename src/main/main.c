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

/////// include the device
#include "nrf_button.h"

#include "ble_gap_func.h"

//// include the kernel mem manager
#include "memory_manager/kernel_mem_manager.h"
#include "memory_manager/kernel_link_list.h"

/// include the kernel task
#include "kernel_task.h"

#include "ble_ams.h"
#include "ble_peer_info.h"

#include "device_drivers/accelrometer/nrf_accelromter.h"
//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////// general Task Function decleartions /////////////////////////////////////

void general_task_function(void *);

#define genral_task_stack_size 256 // 1KB memory
#define genral_task_name "gen_task"
#define genral_task_param NULL
#define genral_task_priority 5

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



int main()
{

    if (sys_init() != nrf_OK)
    {
        APP_ERROR_HANDLER(nrf_ERR_OPERATION_FAILED);
    }

    /// init the kernel task preinit
    Kernel_task_preinit();
    

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////// genral task /////////////////////////////////////////////
void general_task_function(void *param)
{
    UNUSED_VARIABLE(param);
    uint32_t ret = 0;

    ///// check for the button events and print it
    for (;;)
    {
        uint8_t evt = nrf_btn_get_evtq();
        if (evt != 0)
        {
            // NRF_LOG_WARNING("%d", evt);
            if (evt == NRF_BUTTON_UP_EVT)
            {
           
                // NRF_LOG_INFO("%d", nvs_add_data(uid, data_buff , min_Size + uid ));
                // NRF_LOG_WARNING("pointer %x",nvs_get_data_pointer(uid));
                //// start the advertise
                NRF_LOG_INFO("adv%d", ble_gap_start_advertise(BLE_ADVERTISE_WITH_FAST_PAIR));
            }
            else if (evt == NRF_BUTTON_DOWN_EVT)
            {
                
                // get the data pointer
                NRF_LOG_INFO("adv%d", ble_gap_stop_advertise());

            }
            else if (evt == NRF_BUTTON_MIDD_EVT)
            {
                // NRF_LOG_INFO("delete %d", ble_gap_delete_bonds());
                ble_ams_print_media_info();
                //   ble_gap_print_keys(0);
    
            }
            else if(evt == NRF_BUTTON_HOME_EVT)
            {
                // NRF_LOG_WARNING("%d", ble_gap_delete_bonds());
                /// show memory content 
                NRF_LOG_INFO("batt is %d",ble_peer_get_battery_info());
              
            }
        }

        /// handle the accelrometer here 
        uint8_t evttype =  nrf_accel_get_evtq();
        
        if(evttype != NRF_ACCEL_EVT_NONE)
        {
            NRF_LOG_INFO("evt is %d",evttype) ;
        }

        // nrf_accel_read_raw();
        // NRF_LOG_INFO("main task 1");
    }

    vTaskDelete(NULL);
}





