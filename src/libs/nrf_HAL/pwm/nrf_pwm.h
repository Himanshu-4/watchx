#ifndef _NRF_PWM_H
#define _NRF_PWM_H

#include "system.h"


/////////// define the enum heres 

/// @brief define the pwm hardware , there are 3 pwm hardware present in the nrf52 make a
#define PWM_0   0
#define PWM_1   1
#define PWM_2   2


/// @brief define the operating mode of the pwm 
enum _PWM_OPERATION_MODE_TYPES
{
    PWM_MODE_UP, 
    PWM_MODE_UP_AND_DOWN
};

/// @brief define the pwm clock frequencies that are supported 
enum _PWM_CLK_FREQUENCY_TYPES_
{
    PWM_CLK_FREQ_16MHZ = 0,  // Divide by 1 (16MHz)
    PWM_CLK_FREQ_8MHZ =  1,  //Divide by 2 ( 8MHz)
    PWM_CLK_FREQ_4MHZ =  2,  //Divide by 4 ( 4MHz)
    PWM_CLK_FREQ_2MHZ =  3,  //Divide by 8 ( 2MHz)
    PWM_CLK_FREQ_1MHZ =  4,  // Divide by 16 ( 1MHz)
    PWM_CLK_FREQ_500KHZ = 5, // Divide by 32 ( 500kHz)
    PWM_CLK_FREQ_250KHZ = 6, // Divide by 64 ( 250kHz)
    PWM_CLK_FREQ_125KHZ = 7, // Divide by 128 ( 125kHz)

};

enum _PWM_CHANNEL_TYPES_
{
    PWM_CHANEEL_1 = 0x00,
    PWM_CHANNEL_2,
    PWM_CHANEEL_3,
    PWM_CHANEEL_4
};

/// @brief define the error code that can be return by the pwm hardware 
enum _PWM_ERROR_CODES_TYPES_
{
    pwm_ok = 0x00,
    pwm_err_hardware_not_exist,
    pwm_invalid_param,  
};

/// @brief define the configuration structure for the pwm 
typedef  struct _PWM_MODE_CONFIG_
{
    uint8_t mode;
    uint8_t freq;
    uint8_t pwm_mode;
    uint8_t pwm_xtz;

}pwm_config_t;

/// @brief this is to configure the pwm hardware according to the config struture 
/// @param pwm_hardware 
/// @param  configuration structure 
/// @return succ/failure of the function 
uint8_t pwm_module_init(uint8_t pwm_hardware, pwm_config_t *);

/// @brief this is to deinit the pwm hardware 
/// @param pwm_hardware 
/// @return 
uint8_t pwm_module_deinit(uint8_t pwm_hardware);

uint8_t pwm_interrupts_enable(uint8_t pwm_harware);

uint8_t pwm_interrupt_disable(uint8_t pwm_hardware);

/// @brief  this is the interrupt handler functions that contains the array of the interrutps 
typedef void (*pwm_int_handler[]) (void );


uint8_t pwm_add_irq_handler(uint8_t , pwm_int_handler);

#endif