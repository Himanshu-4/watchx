
///////////////////////////////////////////////////////////////////////
//////////////////////// include the freertos librarys 
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"




// #include "timers.h"
// #include "bsp.h"
//#include "nordic_common.h"
// #include "nrf_drv_clock.h"
//#include "sdk_errors.h"
//#include "app_error.h"

////////// include user libs here 
#include "system.h"
#include "gpio.h"
#include "gpiote.h"
#include "timer.h"
#include "ppi.h"

// #include "gpiote.h"

// #include <stdbool.h>
// #include <stdint.h>
// #include "nrf_delay.h"
// #include "boards.h"

////// prototype of the function 
void timer_2_handler(void);

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    
    /* Configure board. */
    // bsp_board_init(BSP_INIT_LEDS);
    gpio_set_dir(LED_2, GPIO_PIN_DIR_OUTPUT);
    gpio_pin_set(LED_2);

    if(sys_init() != nrf_OK)
    {
    gpio_pin_reset(LED_2);
    }

    

    // my_gpio_cfg gp_cfg = 
    // {
    //     .gpio_dir = GPIO_PIN_DIR_INPUT,
    //     .gpiopull_mode = GPIO_PIN_PULLUP,
    //     .gpio_sense_type = GPIO_PIN_SENSE_LOW,
    //     .gpio_input_buff = GPIO_PIN_INPUT_BUFF_CONNECT
    // };

    // gpio_config(PIN_13, &gp_cfg);
    // gpio_config(PIN_14, &gp_cfg);

    const my_gpiote_cfg my_gpio_cfg
    =
    {
        .mode = GPIO_TASK_MODE,
        .outlevel = TASK_OUT_INIT_LEVEL_HIGH,
        .pinsel = LED_1,
        .polarity = TASK_OUT_TOGGLE,
    };

    gpio_config_channel(GPIOTE_CHANNEL_1, &my_gpio_cfg);


 

    // now init the timer 1 and use its event compare register
    timer_module_init();
    
    const my_timer_cfg timer_1_cfg =
    {
        .mode = TIMER_MODE,
        .timer_tick_rate = TIMER_FREQ_1MHz,
        .timer_width = TIMER_32_BIT_WIDTH
    };

    // configure the timer 

    timer_config(TIMER_1, &timer_1_cfg);

//'////// you can program the same capture compare to a interrupt and autorelaod value 
    timer_add_interupt_time(TIMER_1, CAPTURE_COMPARE_1, 1000000UL);

    timer_set_auto_reload_val(TIMER_1, CAPTURE_COMPARE_1,1000000UL);

    ppi_init();

    ppi_configure_channel(PPI_CHANNEL_0, timer_get_event_addr(TIMER_1, CAPTURE_COMPARE_1), task_addr_get_ch_no(GPIOTE_CHANNEL_1, TASK_ADDR_OUT));

    ppi_channel_enable(PPI_CHANNEL_0);

    timer_start(TIMER_1);

    const my_timer_cfg timer_2_cfg =
    {
        .mode = TIMER_MODE,
        .timer_tick_rate = TIMER_FREQ_1MHz,
        .timer_width = TIMER_32_BIT_WIDTH
    };

    // configure the timer 

    timer_config(TIMER_2, &timer_2_cfg);

//'////// you can program the same capture compare to a interrupt and autorelaod value 
    timer_add_interupt_time(TIMER_2, CAPTURE_COMPARE_1, 1000000UL);

    timer_set_auto_reload_val(TIMER_2, CAPTURE_COMPARE_1,1000000UL);

    timer_add_irq_handler(TIMER_2,timer_2_handler );

    timer_interrupt_enable(TIMER_2, CAPTURE_COMPARE_1);

    timer_start(TIMER_2);
    // configure the PPI channels 
    /* Toggle LEDs. */
    while (true)
    {
        __WFI();
      
        // for (int i = 0; i < LEDS_NUMBER; i++)
        // {
        //     bsp_board_led_invert(i);
        //     nrf_delay_ms(500);
        // }
    }
}


////// timer_2 handler 
void timer_2_handler(void)
{
    uint8_t int_src = timer_get_int_src(TIMER_2);

    if(int_src == CAPTURE_COMPARE_1)
    {
        gpio_pin_toogle(LED_2);
    }
}
