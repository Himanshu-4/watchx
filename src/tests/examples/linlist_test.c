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

KERNEL_LINKLIST_INSTANTISE(ll_inst, ll_mem,40,ll_mut);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////// genral task /////////////////////////////////////////////
void general_task_function(void *param)
{
    UNUSED_VARIABLE(param);
    uint32_t ret = 0;

    //// first to check that 
    uint32_t value =0;

    ret = kernel_ll_init(&ll_inst, ll_mem, 40, &ll_mut, 500,  4 + KERNEL_LINK_LIST_META_DATA_SIZE);
    NRF_ASSERT(ret);
    NRF_LOG_INFO("memadr %x",ll_mem);
    ///// check for the button events and print it
    for (;;)
    {
        uint8_t evt = nrf_get_btn_evtq();
        if (evt != 0)
        {
            // NRF_LOG_WARNING("%d", evt);
            if (evt == NRF_BUTTON_UP_EVT)
            {
                value++;
                ret = kernel_ll_add_data(&ll_inst, u8_ptr &value,4 );
                NRF_ASSERT(ret);
                // NRF_LOG_INFO("%d", nvs_add_data(uid, data_buff , min_Size + uid ));
                // NRF_LOG_WARNING("pointer %x",nvs_get_data_pointer(uid));
                //// start the advertise
                // NRF_LOG_INFO("adv%d", ble_gap_start_advertise(BLE_ADVERTISE_WITH_FAST_PAIR));
            }
            else if (evt == NRF_BUTTON_DOWN_EVT)
            {
                
                uint8_t find = value--;
                ret = kernel_ll_remove_data(&ll_inst, 0, &find, 1);
                NRF_ASSERT(ret);
                // get the data pointer
                // NRF_LOG_INFO("adv%d", ble_gap_stop_advertise());

            }
            else if (evt == NRF_BUTTON_MIDD_EVT)
            {
                // NRF_LOG_INFO("delete %d", ble_gap_delete_bonds());
                // ble_ams_print_media_info();
                //   ble_gap_print_keys(0);
                uint8_t find = value;
                uint32_t new_val = 0xff;
                uint32_t  *val = (uint32_t *) kernel_ll_get_data_ptr(&ll_inst, 0, &find ,1);
                NRF_LOG_INFO("value %d, ad %x ,vlaue %d ret %d",value,val,*val,kernel_ll_modify_data(&ll_inst, 0, &find, 1, u8_ptr &new_val));

                    uint32_t temp =0;
                kernel_ll_get_data(&ll_inst, 0, u8_ptr &new_val,1, u8_ptr &temp, 4 );
                NRF_LOG_INFO("ttn %d ,used size %d modfi %x",kernel_ll_get_total_nodes(&ll_inst),kernel_ll_get_size_used(&ll_inst),
                    temp);
            }
            else if(evt == NRF_BUTTON_HOME_EVT)
            {
                // NRF_LOG_WARNING("%d", ble_gap_delete_bonds());
                /// show memory content 
                NRF_LOG_INFO("mem cnt ");
                for (size_t i = 0; i < 40; i++)
                {
                    printf("%x ",ll_mem[i]);
                }
                printf("\r\n");
            }
        }
        delay(100);

        // NRF_LOG_INFO("main task 1");
    }

    vTaskDelete(NULL);
}





