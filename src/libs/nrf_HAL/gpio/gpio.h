#ifndef _GPIO_H
#define _GPIO_H


// include the library file for the register deifnations 
#include "system.h"






//// GPIO BASE definations 

#define set 1
#define reset 0


// define the pin number 
enum  _GPIO_PIN_NUMBER_TYPES_
{
    PIN_0,
    PIN_1,
    PIN_2,
    PIN_3,    
    PIN_4,
    PIN_5,
    PIN_6,
    PIN_7,
    PIN_8,
    PIN_9,
    PIN_10,
    PIN_11,
    PIN_12,
    PIN_13,
    PIN_14,
    PIN_15,
    PIN_16,
    PIN_17,
    PIN_18,
    PIN_19,
    PIN_20,
    PIN_21,
    PIN_22,
    PIN_23,
    PIN_24,
    PIN_25,
    PIN_26,
    PIN_27,
    PIN_28,
    PIN_29,
    PIN_30,
    PIN_31,

};



#define LED_1 PIN_17
#define LED_2 PIN_18
#define LED_3 PIN_19
#define LED_4 PIN_20


#define BTN_1 PIN_13
#define BTN_2 PIN_14
#define BTN_3 PIN_15
#define BTN_4 PIN_16




/** @brief Pin direction definitions. */
enum _GPIO_PIN_DIR_TYPES_
{
    GPIO_PIN_DIR_INPUT  = 0, ///< Input.
    GPIO_PIN_DIR_OUTPUT = 1 ///< Output.
};

/** @brief Connection of input buffer. */
enum _GPIO_PIN_INPUT_BUFF_TYPES_
{
    GPIO_PIN_INPUT_BUFF_CONNECT    = 0,   ///< Connect input buffer.
    GPIO_PIN_INPUT_BUFF_DISCONNECT = 1 ///< Disconnect input buffer.
};

/**
 * @brief Enumerator used for selecting the pin to be pulled down or up at the time of pin
 * configuration.
 */
enum _GPIO_PIN_PULL_TYPES_
{
    GPIO_PIN_NOPULL   = 0, ///<  Pin pull-up resistor disabled.
    GPIO_PIN_PULLDOWN = 1, ///<  Pin pull-down resistor enabled.
    GPIO_PIN_PULLUP   = 3,   ///<  Pin pull-up resistor enabled.
};

/** @brief Enumerator used for selecting output drive mode. */
enum _GPIO_PIN_DRIVE_TYPES_
{
    GPIO_PIN_S0S1 = 0, ///< !< Standard '0', standard '1'.
    GPIO_PIN_H0S1 , ///< !< High-drive '0', standard '1'.
    GPIO_PIN_S0H1 , ///< !< Standard '0', high-drive '1'.
    GPIO_PIN_H0H1 , ///< !< High drive '0', high-drive '1'.
    GPIO_PIN_D0S1 , ///< !< Disconnect '0' standard '1'.
    GPIO_PIN_D0H1 , ///< !< Disconnect '0', high-drive '1'.
    GPIO_PIN_S0D1 , ///< !< Standard '0', disconnect '1'.
    GPIO_PIN_H0D1 , ///< !< High-drive '0', disconnect '1'.
};

/** @brief Enumerator used for selecting the pin to sense high or low level on the pin input. */
 enum _GPIO_PIN_SENSE_TYPES_
{
    GPIO_PIN_NOSENSE    =0, ///<  Pin sense level disabled.
    GPIO_PIN_SENSE_HIGH =2,     ///<  Pin sense high level.
    GPIO_PIN_SENSE_LOW  =3,      ///<  Pin sense low level.
};



typedef struct __PACKED  _MY_GPIO_CFG_
{
    uint8_t gpio_dir;
    uint8_t gpio_input_buff;
    uint8_t gpiopull_mode;
    uint8_t gpio_drive_type;
    uint8_t gpio_sense_type;

}my_gpio_cfg;


//// define the prototype of the iqr handler 
typedef void(*irq_handler)(void *);


/****
 * @name gpio_config
 * @param[1] gpio_number
 * @param[2] gpio_configuration
 * @return succ/failure of func
 * @brief set the gpio properties
*/
bool gpio_config(uint8_t gpio_num , const  my_gpio_cfg*  );

/***
 * @name gpio_Set_mode
 * @param[1] gpio_number 
 * @param[2] gpio direction
 * @return succ. failure
 * @brief set the gpio mode and in a thread safe mode 
*/
bool gpio_set_dir(uint8_t gpio_num, uint8_t direction);

/***
 * @name gpio_pin_Set
 * @param gpio_number
 * @return null
 * @brief set the level to 1 
*/
void gpio_pin_set(uint8_t );

/****
 * @name gpio_pin_reset
 * @param gpio_number
 * @return null
 * @brief reset the gpio pin
 * 
*/
void gpio_pin_reset(uint8_t );

/*****
 * @name gpio_pin_toggle
 * @param gpio_number 
 * @return null
 * @brief toggle the gpio pin level
*/
void gpio_pin_toogle(uint8_t );

/***
 * @name gpio_seet_level
 * @param gpio_number
 * @return NULL
 * @brief set the level of the gpio
*/
void gpio_set_level(uint8_t , bool);

/***
 * @name gpio_pin_read
 * @param gpio_number
 * @return bool level of the pin 
 * @brief read the data from that gpio number 
 * 
*/
bool gpio_pin_read(uint8_t );

/// @brief this is to reset the gpio pins 
/// @param  void 
void gpio_reset_all_pins(void);


#endif