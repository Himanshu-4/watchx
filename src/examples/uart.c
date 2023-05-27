
///////////////////////////////////////////////////////////////////////
//////////////////////// include the freertos librarys 
// #include "FreeRTOS.h"
// #include "FreeRTOSConfig.h"
// #include "task.h"
// not using right now 



////////// include user libs here 
#include "system.h"
#include "gpio.h"
#include "gpiote.h"
#include "timer.h"
#include "ppi.h"
#include "adc.h"
#include "time.h"
#include "uart_drv.h"


// #define ADC_BUFF_SIZE (4 * 4)

// static uint32_t adc_out_buff[ADC_BUFF_SIZE];


volatile uint8_t rx_buff[8] ={0};

void rx_hand(void);

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
    

    const uart_config_t my_uart_cfg =
    {
        .rxd_pin = UART_RX_DEFAULT_PIN,
        .txd_pin = UART_TX_DEFAULT_PIN,
        .baud_rate = UART_BAUDRATE_BAUDRATE_Baud115200,
        .parity = PARITY_EXCLUDED,
        .hardware_flow.flow_control =0,

    };

    uart_config(&my_uart_cfg);

    uart_init();

    char buff[] = "my name is himanshu\r\n";

    uart_set_tx_buff((uint8_t *)buff, sizeof(buff));

    uart_set_rx_buff((uint8_t * )rx_buff, sizeof(rx_buff));

    uart_enable_isr();

    uart_add_irq_handler(UART_INT_ENDRX,rx_hand );

    uart_enable_int(UART_INT_ENDRX_Mask);
    
    uart_continous_reception();

    uart_start_reception();

    uart_start_transmit();

    gpio_pin_reset(LED_1);

    // // adc channel_prop_config this can be used in variety of function 
    // const adc_channel_prop_config adc_ch1_prop_conf=
    // {
    //     .adc_diff_mode = ADC_MODE_SINGLE_ENDED,
    //     .adc_refernce = ADC_REFERNCE_Internal, 
    //     .adc_resolution = ADC_RESOLUTION_12bit,
    //     .channel_gain = Gain1_6,
    // };

    // const my_adc_channel_config adc_ch1_config=
    // {
    //     .adc_acq_time = ADC_ACQ_40us,
    //     .adc_burst_mode = ADC_BURST_MODE_DISABLED,
    //     .neg_sel_pin = PIN_NC,
    //     .neg_resistor_sel = ADC_RES_Pulldown,
    //     .pos_resistor_sel = PIN_2_AIN0,
    //     .pos_resistor_sel = ADC_RES_Bypass,
    //     .chnl_config = &adc_ch1_prop_conf
    // };

    // // adc configuration
    // const my_adc_config adc_conf =
    // {
    //     .adc_oversamplnig = ADC_NO_OVERSAMPLING,
    //     .adc_resolution = ADC_RESOLUTION_12bit,
    //     .ram_buff_ptr = adc_out_buff,
    //     .ram_buff_size = ADC_BUFF_SIZE,
    //     .sample_cfg.adc_sampling_rate = 0,
    //     .sample_cfg.adc_trigger_mode = ADC_SAMPLING_DISABLED
    // };

    // adc_config(&adc_conf);

    // adc_channel_config(ADC_CHANNEL_0, &adc_ch1_config);

    // adc_init();

    // adc_start();

    // adc_disable_isr();
    
    // // take a sample from the adc 
    // adc_take_asample();

    while(1)
    {
        __WFI();
        __NOP();
    }
    return 0;
}

void rx_hand(void)
{
    uart_set_tx_buff((uint8_t * )rx_buff, sizeof(rx_buff));
    uart_start_transmit();
}

