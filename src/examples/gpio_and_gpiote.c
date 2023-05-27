

#include "FreeRTOS.h"
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

// #include <stdbool.h>
// #include <stdint.h>
// #include "nrf_delay.h"
// #include "boards.h"

#define pin_18_gpt CHANNEL_1

#define pin_13_int CHANNEL_2

#define pin_14_int CHANNEL_5


void gpio_btn1_irqhand(void);

void gpio_btn2_irqhand(void);

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    
    /* Configure board. */
    // bsp_board_init(BSP_INIT_LEDS);
    gpio_set_mode(PIN_17, GPIO_PIN_DIR_OUTPUT);
    gpio_pin_set(PIN_17);

    if(sys_init() != nrf_OK)
    {
    gpio_pin_reset(PIN_17);
    }
    

    my_gpio_cfg gp_cfg = 
    {
        .gpio_dir = GPIO_PIN_DIR_INPUT,
        .gpiopull_mode = GPIO_PIN_PULLUP,
        .gpio_sense_type = GPIO_PIN_SENSE_LOW,
        .gpio_input_buff = GPIO_PIN_INPUT_BUFF_CONNECT
    };

    gpio_config(PIN_13, &gp_cfg);
    gpio_config(PIN_14, &gp_cfg);


    
        // installl the service 
     gpio_install_isr_servc();

    my_gpiote_cfg gpte_cfg =
    {
        .mode = GPIO_TASK_MODE,
        .outlevel = TASK_OUT_INIT_LEVEL_HIGH,
        .pinsel = PIN_18,
        .polarity = TASK_OUT_TOGGLE,
    };

    gpio_config_channel(pin_18_gpt, &gpte_cfg);

    // change the gpte structure 
    gpte_cfg.mode = GPIO_EVENT_MODE;
    gpte_cfg.pinsel = PIN_13;
    gpte_cfg.polarity = EVENT_FALLING_EDGE;
    
    gpio_config_channel(pin_13_int, &gpte_cfg);
    gpio_add_irq_handler(pin_13_int, gpio_btn1_irqhand );
    gpio_int_enable(pin_13_int);

    gpte_cfg.pinsel = PIN_14;
    gpio_config_channel(pin_14_int, &gpte_cfg);
    gpio_add_irq_handler(pin_14_int, gpio_btn2_irqhand );
    gpio_int_enable(pin_14_int);



    task_explicit_clr(pin_18_gpt);

    /* Toggle LEDs. */
    while (true)
    {
      
        // for (int i = 0; i < LEDS_NUMBER; i++)
        // {
        //     bsp_board_led_invert(i);
        //     nrf_delay_ms(500);
        // }
    }
}



void gpio_btn1_irqhand(void)
{
       gpio_pin_toogle(PIN_17);
        task_explicit_out(pin_18_gpt);
}

void gpio_btn2_irqhand(void)
{
        task_explicit_out(pin_18_gpt);
}

 for(uint8_t i=2; i<32; i++)
    {
        if(i == 21)continue;
        gpio_set_dir(i, GPIO_PIN_DIR_OUTPUT);

    }
    // gpio_set_dir(LED_4, GPIO_PIN_DIR_OUTPUT);
    // gpio_pin_set(LED_4);

    if (sys_init() != nrf_OK)
    {
        gpio_pin_reset(LED_4);
        return 0;

    }

    while (1)
    {
        for(uint8_t i=2; i<32; i++)
        {
            if(i ==21 )continue;
            gpio_pin_toogle(i);
        }
        nrf_delay_ms(1000);

    }
    

    
int main()
{
    for(int i=2; i<32; i++)
    {
        if(i==21)continue;
        gpio_set_dir(i, GPIO_PIN_DIR_OUTPUT);
    }
    // if(sys_init() != nrf_OK)
    // {
    //     error_handler("clk init failed");
    // }
    time_init();

    // install all the communication drivers 
    // driver_install();
    
    // // ini the eeg module
    // eeg_pins_init();
    // // init the accelration module 
    // st_accel_init();

    // //////////// intialise the tdcs pinoout
    // tdcs_pins_init();


    // uint8_t res = pdFAIL;
    // res = xTaskCreate(task_1_func, task_1_name, task_1_stack_size, task_1_param,task_1_priority, &task_1_handle);
    //     if(res != pdPASS)
    //     {
    //         error_handler("task_1 not created");
    //     }

    // res = xTaskCreate(task_2_func, task_2_name, task_2_stack_size, task_2_param,task_2_priority, &task_2_handle);
    // if(res == pdPASS)
    // gpio_pin_reset(LED_1);

   

    // vTaskStartScheduler();
    for (;;)
    {
        // printf("dev id is %x\r\n ", st_get_device_id());
    // printf("the vcell is %f V,soc %d %%, the remcap %dmAh,tte is %lds, current %ldmA, temperature %f*C status %X \r\n",          
    // fgage_get_vcell(), fgage_get_soc(), fgage_get_remcap(), fgage_get_tte(), fgage_get_curent(), (float)fgage_get_temp(), fgage_get_status());
       
    for(int i=2; i<32; i++)
    {
        if(i==21)continue;
        gpio_pin_toogle(i);
    }

        // __WFI();
        // __NOP();
        // delay(500);
        // // no delay at al
        // uart_log_bytes((uint8_t *)"hye there\r\n",12 );

    nrf_delay_ms(1000);
    }



    
    return 0;
}
