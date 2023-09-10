#include "gpio.h"




bool gpio_config(uint8_t gpio_num , const my_gpio_cfg* gpio_cfg )
{
    NRF_P0->PIN_CNF[gpio_num] = (gpio_cfg->gpio_dir        <<  GPIO_PIN_CNF_DIR_Pos) |
                                (gpio_cfg->gpio_input_buff << GPIO_PIN_CNF_INPUT_Pos)|
                                (gpio_cfg->gpiopull_mode   << GPIO_PIN_CNF_PULL_Pos) |
                                (gpio_cfg->gpio_drive_type << GPIO_PIN_CNF_DRIVE_Pos)|
                                (gpio_cfg->gpio_sense_type << GPIO_PIN_CNF_SENSE_Pos);

    return nrf_OK;
        
}


bool gpio_set_dir(uint8_t gpio_num, uint8_t gpio_mode)
{
     if(gpio_mode == GPIO_PIN_DIR_INPUT)
    {
    // set the dirclr reg
    NRF_P0->DIRCLR = _BV(gpio_num);
    }
    else 
    {
    // set the dirset reg 
    NRF_P0->DIRSET = _BV(gpio_num);
    }

    return nrf_OK;
}

__FORCE_INLINE void gpio_pin_set(uint8_t gpio_num)
{
    // set the level to 1
    NRF_P0->OUTSET = _BV(gpio_num);
}

__FORCE_INLINE void gpio_pin_reset(uint8_t gpio_num)
{   
    // set the level to 0
    NRF_P0->OUTCLR = _BV(gpio_num);
}

__FORCE_INLINE void gpio_pin_toogle(uint8_t gpio_num)
{
    // read the level from out reg 
    uint8_t val =  1U & ( NRF_P0->OUT >> gpio_num );

    if(val) // if val == high then set to low 
    {
        NRF_P0->OUTCLR = _BV(gpio_num);
    }
    else 
    {
        NRF_P0->OUTSET = _BV(gpio_num);
    }
}

__FORCE_INLINE void gpio_set_level(uint8_t gpio_num, bool val)
{
    if(val)
    {
        NRF_P0->OUTSET = _BV(gpio_num);
    }
    else 
    {
         NRF_P0->OUTCLR = _BV(gpio_num);
    }
}


__FORCE_INLINE bool gpio_pin_read(uint8_t gpio_num)
{
    return (1U & ( NRF_P0->IN >> gpio_num));
}



void gpio_reset_all_pins(void)
{
    for(uint8_t i= PIN_0; i<PIN_31; i++)
    {
        NRF_P0->PIN_CNF[i] =0;
    }
}