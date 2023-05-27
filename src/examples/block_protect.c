
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
#include "bprot.h"


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
    
    protect_flash_block(0); // protect flash block 0 
    protect_flash_block(25);// protect flash block 25
    protect_flash_block(125);// protect flash block 125
    protect_flash_block(64);// protect flash block 64
    protect_flash_block(96);// protect flash block 96
    protect_flash_block(77);// protect flash block 77
for(;;)
{
    __WFI();
    __NOP();
}

return 0;
}

