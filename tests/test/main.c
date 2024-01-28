/******
 * Parallelism: The ability to perform multiple operations simultaneously.
 *  Multiprocessing is an example that spreads tasks over multiple CPU cores.
Concurrency: The ability to execute more than one program or task simultaneously.
Tasks can run in an overlapping manner and need not be parallel.

I/O bound task: A task dominated by waiting for input/output to complete.
Coroutine (coro): A specialised function (co-operative routine) that is intended to run
concurrently with other coros. Concurrency is achieved by periodically yielding to the
scheduler, enabling other coros to run.

Event loop: A loop that monitors tasks to run. For microcontrollers, we have this
 running on a single CPU core and single thread.
Scheduler: An event loop that facilitates asynchronous programming.

Pre-emptive scheduling: A scheduling technique that temporarily interrupts an
running task without cooperation from the task, for it to be resumed at a later time.

Cooperative scheduling: A scheduling technique that never initiates a context switch
between tasks. It has lower overhead compared to pre-emptive scheduling and requires tasks
to periodically yield to the scheduler.
*/

///////////////////////////////////////////////////////////////////
//////////////////// include the freertos librarys
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

////////// include user libs here
#include "system.h"
#include "nrf_custom_log.h"

/// include the kernel task
#include "kernel_task.h"

#include "nrf_button.h"

#include "nrf_gfx.h"

#include "gpiote.h"

#include "logger.h"

#include "nrf_time.h"

#include "device_drivers/drivers_common/drivers_common.h"
//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////// general Task Function decleartions /////////////////////////////////////


void general_task_function(void*);

#define genral_task_stack_size 256 // 1KB memory
#define genral_task_name       "gen_task"
#define genral_task_param      NULL
#define genral_task_priority   5

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
void gpio_irq_han(void);
int main()
{

    if (sys_init() != nrf_OK)
    {
        NRF_LOG_ERROR("sys init failed"
                      " cant tell why its failed "
                      " but the string can be represented as this");
        // APP_ERROR_HANDLER(nrf_ERR_OPERATION_FAILED);
    }



    genral_task_handle = xTaskCreateStatic(general_task_function,
                                           genral_task_name,
                                           genral_task_stack_size,
                                           genral_task_param,
                                           genral_task_priority,
                                           gen_task_stack,
                                           &gen_task_buffer);
    if (genral_task_handle == NULL)
    {
        APP_ERROR_HANDLER(nrf_ERR_NO_MEMORY);
    }

    // Hardware_drivers_install();
    time_init();
    logger_init();
    logger_enable_tx();
    gpio_install_isr_servc();

    /// init the kernel task preinit
    // Kernel_task_preinit();


    const my_gpio_cfg nrf_buttons_gpio_type =
    {
        .gpio_dir = GPIO_PIN_DIR_INPUT,
        .gpiopull_mode = GPIO_PIN_PULLUP,
        .gpio_sense_type = GPIO_PIN_SENSE_LOW,
        .gpio_input_buff = GPIO_PIN_INPUT_BUFF_CONNECT
    };

    const my_gpiote_cfg home_pin_int_cfg =
    {
        .mode = GPIO_EVENT_MODE,
        .pinsel = PIN_16,
        .outlevel = TASK_OUT_INIT_LEVEL_HIGH,
        .polarity = EVENT_RISING_EDGE
    };
  

    // config the gpio hardware 
    gpio_config(PIN_16 , &nrf_buttons_gpio_type);
    
    gpio_config_channel(GPIOTE_CHANNEL_4, &home_pin_int_cfg);

    
    gpio_add_irq_handler(GPIOTE_CHANNEL_4, gpio_irq_han);

    // gpio_int_disable(NRF_CONFIG_MIDDLE_BUTTON_GPIOTE_CHANNEL);
    
    gpio_int_enable(GPIOTE_CHANNEL_4);

    

    printf("initing  rtos\r\n");
    printf("ptr %x \r\n",logger_get_tx_buff_addr());
    nrf_delay_ms(100);
    logger_flush_buffer();
    // watchdog_init(1);
    // watchdog_start();
    /// @todo have to implement the kernel init file
    // watchx_kernel_init();
    /// init the accelrometer task
    /// init the led , buzzer libs

    // hardfault is diabled . please enable it

    // app_start_scheduler();

    // vTaskStartScheduler();
    for (;;)
    {
        // printf("dev id is %x\r\n ", st_get_device_id());
        // printf("the vcell is %f V,soc %d %%, the remcap %dmAh,tte is %lds, current %ldmA, temperature %f*C status %X \r\n",
        // fgage_get_vcell(), fgage_get_soc(), fgage_get_remcap(), fgage_get_tte(), fgage_get_curent(), (float)fgage_get_temp(),
        // fgage_get_status());


        __WFI();
        __NOP();
        // delay(500);
        //     // // no delay at al
        // uart_log_bytes((uint8_t *)msg, sizeof(msg) );
        // gpio_pin_toogle(LED_1);
        // printf("hi from board\r\n");
        nrf_delay_ms(1000);
    }
    system_soft_reset();

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

void general_task_function(void* param)
{
    UNUSED_VARIABLE(param);
    uint32_t ret = 0;

    NRF_LOG_INFO("ptr %x",logger_get_tx_buff_addr());
    NRF_LOG_INFO(" testing UART example");
    delay(100);
    ///// check for the button events and print it
    int i = 0;


    logger_flush_buffer();

    for (;;)
    {
        uint8_t evt = nrf_btn_get_evtq();
        if (evt != 0)
        {
            // NRF_LOG_WARNING("%d", evt);
            if (evt == NRF_BUTTON_UP_EVT)
            {
             
            } else if (evt == NRF_BUTTON_DOWN_EVT)
            {
                nrf_gfx_lib_clear_display();

            } else if (evt == NRF_BUTTON_MIDD_EVT)
            {

            }
        }

        // /// handle the accelrometer here
        // uint8_t evttype =  nrf_accel_get_evtq();

        // if(evttype != NRF_ACCEL_EVT_NONE)
        // {
        //     NRF_LOG_INFO("evt is %d",evttype) ;
        // }

        // nrf_accel_read_raw();
        // NRF_LOG_INFO("main task 1");
    }

    vTaskDelete(NULL);
}

/// @brief this is the IRQ handler for the GPIO interrupt
/// @param void
/// @note 
/// @todo d  
void gpio_irq_han(void)
{
    logger_transmit_bytes("0123456789\r\n",12);
    logger_transmit_bytes("ABCDEF++GHIJ\r\n",14);
    logger_transmit_bytes("abcdef--ghij\r\n",14);
    // printf("interrupt trger from pin 16\r\n");
    // printf("interrupt trger from pin 16\r\n");
    
}