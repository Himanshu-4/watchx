
#ifndef _GPIOTE_H
#define _GPIOTE_H


#include "gpio.h"

// this symbol is defined in the starttup file 
extern void GPIOTE_IRQHandler(void);








/****
 * @name  gpio_install_isr_servc
 * @param void
 * @brief install the isr service means , nvic enable the iqr number service to the cortex processor
*/
bool gpio_install_isr_servc(void);

/****
 * @name  gpio_uninstall_isr_servc
 * @param void
 * @brief uninstall the isr service means , nvic enable the iqr number service to the cortex processor
*/
bool gpio_uninstall_isr_servc(void);


enum _GPIO_TASK_TYPES_
{
    GPIO_TASK_ADDR_OUT,
    GPIO_TASK_ADDR_SET,
    GPIO_TASK_ADDR_CLR
};

/****
 * @name task_addr_get_ch_no
 * @param[1] channel no
 * @param[2] task set, clr, out 
 * @return return the address of that task address
 * @brief can be used in the PPI scenarios
 * 
*/
uint32_t * task_addr_get_ch_no(uint8_t , uint8_t );

/*****
 * @name ecent_addr_get_ch_no
 * @param channel number 
 * @return the address of that event 
 * @brief can be used with PPI scenario
*/
uint32_t * event_addr_get_ch_no(uint8_t);

///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
// the mode of that GPIO channel
enum _GPIO_MODE_TYPES_
{
    GPIO_TE_DISABLED=0,
    GPIO_EVENT_MODE =1,
    GPIO_TASK_MODE = 3,
};

// define the chaanel number here 
enum _GPIO_CHANNEL_TYPES_
{
    GPIOTE_CHANNEL_1,
    GPIOTE_CHANNEL_2, 
    GPIOTE_CHANNEL_3,
    GPIOTE_CHANNEL_4,
    GPIOTE_CHANNEL_5,
    GPIOTE_CHANNEL_6,
    GPIOTE_CHANNEL_7,
};

// define the polarity here for the task mode and for the event mode 
enum _GPIO_POLARITY_TYPES_
{
    EVENT_NONE,
    EVENT_RISING_EDGE,
    EVENT_FALLING_EDGE,
    EVENT_PIN_CHANGE,

};

// task configurations enums of the polarity of the task
enum _GPIO_TASK_OUT_TYPES_
{
    GPIO_TASK_OUT_NONE,
    GPIO_TASK_OUT_SET,
    GPIO_TASK_OUT_CLR,
    GPIO_TASK_OUT_TOGGLE

};

enum _GPIO_TASK_OUT_LEVEL_TYPES_
{
    TASK_OUT_INIT_LEVEL_LOW,
    TASK_OUT_INIT_LEVEL_HIGH
};
typedef struct __PACKED 
{
    uint8_t mode;
    uint8_t pinsel;
    uint8_t polarity;
    uint8_t outlevel;
}my_gpiote_cfg;

/****
 * @name gpio_config_channel
 * @param[1] channel number 
 * @param[2] gpio_te_cfg structure 
 * @return succ/failure
 * 
*/
bool gpio_config_channel(uint8_t , const my_gpiote_cfg *);

/******
 * @name gpio_remove_channel
 * @param channel number
 * @brief  remove that gpio_pin from the GPIOTE module
 */ 
void gpio_remove_channel(uint8_t );

/***
 * @name gpio_add_irq_handler
 * @param channel number 
 * @param function pointer
 * 
*/
void gpio_add_irq_handler(uint8_t , void (*)(void));

/****
 * @name gpio_remove_irq_handler
 * @param channel_number
 * @brief make the function pointer to NULL
*/
void gpio_remove_irq_handler(uint8_t );

/***
 * @name gpio_int_enable
 * @param chaneel_number
 * @brief set the int reg
*/
void gpio_int_enable(uint8_t );


/***
 * @name gpio_int_disable
 * @param chaneel_number
 * @brief clear the int reg
*/
void gpio_int_disable(uint8_t);

/*****
 * @name task_explcit_set  
 * @param channel number 
 * @brief explicitly set the task set register to trigger the action on gpio pin
*/
void task_explicit_set(uint8_t);

/*****
 * @name task_explcit_clr  
 * @param channel number 
 * @brief explicitly set the task clr register to trigger the action on gpio pin
*/
void task_explicit_clr(uint8_t);


/*****
 * @name task_explcit_set  
 * @param channel number 
 * @brief explicitly set the task out  register to trigger the action on gpio pin based on polarity sel
*/
void task_explicit_out(uint8_t);

#endif 
