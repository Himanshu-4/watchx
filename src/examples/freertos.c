
///////////////////////////////////////////////////////////////////////
//////////////////////// include the freertos librarys 
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"


#define ADC_BUFF_SIZE (4 * 4)

static uint32_t adc_out_buff[ADC_BUFF_SIZE];


////////// include user libs here 
#include "system.h"
#include "gpio.h"
#include "gpiote.h"
#include "timer.h"
#include "ppi.h"
#include "adc.h"
#include "time.h"

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// task 1 functions 
xTaskHandle task_1_handle = NULL;

void task_1_func(void *);

#define task_1_stack_size 1024 
#define task_1_name "task1"
#define task_1_param NULL
#define task_1_priority 4

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// task 2 functions 
xTaskHandle task_2_handle = NULL;

void task_2_func(void *);

#define task_2_stack_size 1024 
#define task_2_name "task2"
#define task_2_param NULL
#define task_2_priority 4


// #include "gpiote.h"

// #include <stdbool.h>
// #include <stdint.h>
// #include "nrf_delay.h"
// #include "boards.h"

int main()
{

    gpio_set_dir(LED_2, GPIO_PIN_DIR_OUTPUT);
    gpio_pin_set(LED_2);

     gpio_set_dir(LED_1, GPIO_PIN_DIR_OUTPUT);
    gpio_pin_set(LED_1);

    if(sys_init() != nrf_OK)
    {
    gpio_pin_reset(LED_2);
    }
    


    // adc channel_prop_config this can be used in variety of function 
    const adc_channel_prop_config adc_ch1_prop_conf=
    {
        .adc_diff_mode = ADC_MODE_SINGLE_ENDED,
        .adc_refernce = ADC_REFERNCE_Internal,
        .adc_resolution = ADC_RESOLUTION_12bit,
        .channel_gain = Gain1_6,
    };

    const my_adc_channel_config adc_ch1_config=
    {
        .adc_acq_time = ADC_ACQ_40us,
        .adc_burst_mode = ADC_BURST_MODE_DISABLED,
        .neg_sel_pin = PIN_NC,
        .neg_resistor_sel = ADC_RES_Pulldown,
        .pos_resistor_sel = PIN_2_AIN0,
        .pos_resistor_sel = ADC_RES_Bypass,
        .chnl_config = &adc_ch1_prop_conf
    };

    // adc configuration
    const my_adc_config adc_conf =
    {
        .adc_oversamplnig = ADC_NO_OVERSAMPLING,
        .adc_resolution = ADC_RESOLUTION_12bit,
        .ram_buff_ptr = adc_out_buff,
        .ram_buff_size = ADC_BUFF_SIZE,
        .sample_cfg.adc_trigger_mode = 0,
        .sample_cfg.adc_trigger_mode = ADC_SAMPLING_DISABLED
    };

    adc_config(&adc_conf);

    adc_channel_config(ADC_CHANNEL_0, &adc_ch1_config);

    adc_init();

    adc_start();

    adc_disable_isr();
    
    // take a sample from the adc 
    adc_take_asample();

    uint8_t res = pdFAIL;
    res = xTaskCreate(task_1_func, task_1_name, task_1_stack_size, task_1_param,task_1_priority, &task_1_handle);
    res = xTaskCreate(task_2_func, task_2_name, task_2_stack_size, task_2_param,task_2_priority, &task_2_handle);

    

    if(res == pdPASS)
    gpio_pin_reset(LED_2);

    vTaskStartScheduler();

    while(1)
    {
        __WFI();
    }
    return 0;
}


void task_1_func(void * param)
{
    UNUSED_VARIABLE(param);

    for(;;)
    {
        gpio_pin_toogle(LED_2);
        delay(100);

    }

    /// should never reach here 
    vTaskDelete(NULL);
}
void task_2_func(void * param)
{
    UNUSED_VARIABLE(param);

    for(;;)
    {
        gpio_pin_toogle(LED_1);
        delay(500);

    }

    /// should never reach here 
    vTaskDelete(NULL);
}



// #define FREERTOS
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
#include "uart_logs.h"
#include "nrf_time.h"

///// include the external sensor libraries
#include "st_accelrometer.h"
#include "fuel_gauge.h"
#include "eeg.h"
#include "tdcs.h"

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// task 1 functions
xTaskHandle task_1_handle = NULL;

void task_1_func(void *);

#define task_1_stack_size 256 // 1KB memory
#define task_1_name "task1"
#define task_1_param NULL
#define task_1_priority 4

// higher peiorirty number means lower priority

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// task 2 functions
xTaskHandle task_2_handle = NULL;

void task_2_func(void *);

#define task_2_stack_size 512
#define task_2_name "task2"
#define task_2_param NULL
#define task_2_priority 4 // lower priority than



int main()
{

    if (sys_init() != nrf_OK)
    {
        error_handler("clk init failed");
    }

    // install all the communication drivers
    driver_install();

    // ini the eeg module
    eeg_pins_init();
    // init the accelration module
    st_accel_init();

    //////////// intialise the tdcs pinoout
    tdcs_pins_init();

    uint8_t res = pdFAIL;
    res = xTaskCreate(task_1_func, task_1_name, task_1_stack_size, task_1_param, task_1_priority, &task_1_handle);
    if (res != pdPASS)
    {
        error_handler("task_1 not created");
    }

 res = xTaskCreate(task_2_func, task_2_name, task_2_stack_size, task_2_param, task_2_priority, &task_2_handle);
    if (res != pdPASS)
    {
        error_handler("task_1 not created");
    }
    // start the kernel
    vTaskStartScheduler();
    for (;;)
    {
        // printf("dev id is %x\r\n ", st_get_device_id());
        // printf("the vcell is %f V,soc %d %%, the remcap %dmAh,tte is %lds, current %ldmA, temperature %f*C status %X \r\n",
        // fgage_get_vcell(), fgage_get_soc(), fgage_get_remcap(), fgage_get_tte(), fgage_get_curent(), (float)fgage_get_temp(), fgage_get_status());

        __WFI();
        __NOP();
        // delay(500);
        // // no delay at al
        // uart_log_bytes((uint8_t *)"hye there\r\n",12 );

        nrf_delay_ms(1000);
    }

    return 0;
}


void task_1_func(void *param)
{

    UNUSED_VARIABLE(param);
    
    for(;;)
    {
        printf(GRN "tak 1 func and this is the tak 1\r\n");
        delay(20);
    }

    vTaskDelete(NULL);
}

void task_2_func(void *param)
{

    UNUSED_VARIABLE(param);
    
    for(;;)
    {

        printf(RED "tak 2 func and this is the tak 2\r\n");
        delay(10);
    }

    vTaskDelete(NULL);

}