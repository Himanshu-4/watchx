#ifndef I2C_H
#define I2C_H

#include "system.h"

// defination of external interrupts defined in the vector table 
extern void SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void);
extern void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void);

#define I2C0 0
#define I2C1 1


/// @brief enum to store the task index 
enum
{
    I2C_TASKS_STARTRX,  //  Start TWI receive sequence
    I2C_TASKS_STARTTX,  //  Start TWI transmit sequence
    I2C_TASKS_STOP,  //  Stop TWI transaction. Must be issued while the TWI master is not suspended.
    I2C_TASKS_SUSPEND,  //  Suspend TWI transaction
    I2C_TASKS_RESUME,  //
};

/// @brief i2c event indexeses  
enum
{
    I2C_EVENTS_STOPPED, //   TWI stopped
    I2C_EVENTS_ERROR, //   TWI error
    I2C_EVENTS_SUSPENDED, //   Last byte has been sent out after the SUSPEND task has been issued, TWI traffic is now suspended.
    I2C_EVENTS_RXSTARTED, //   Receive sequence started
    I2C_EVENTS_TXSTARTED, //   Transmit sequence started
    I2C_EVENTS_LASTRX, //   Byte boundary, starting to receive the last byte
    I2C_EVENTS_LASTTX, //  
};

/// @brief the I2c frequecy 
enum
{
    I2C_FREQ_100KHZ = 0x01980000UL, // 100 kbps
    I2C_FREQ_250KHZ = 0x04000000UL, // 250 kbps
    I2C_FREQ_400KHZ = 0x06400000UL, // 400 kbps

};

//// define total no of interupts supported by I2c 
#define I2C_TOTAL_INTERRUPT 7


/// @brief I2c interupt indexces 
enum
{
    I2C_INT_STOPPED, //   TWI stopped
    I2C_INT_ERROR, //   TWI error
    I2C_INT_SUSPENDED, //   Last byte has been sent out after the SUSPEND task has been issued, TWI traffic is now suspended.
    I2C_INT_RXSTARTED, //   Receive sequence started
    I2C_INT_TXSTARTED, //   Transmit sequence started
    I2C_INT_LASTRX, //   Byte boundary, starting to receive the last byte
    I2C_INT_LASTTX, //  
};


/// @brief I2c interupt  function indexces
enum
{
    I2C_INT_STOPPED_Func, //   TWI stopped
    I2C_INT_ERROR_Func, //   TWI error
    I2C_INT_SUSPENDED_Func, //   Last byte has been sent out after the SUSPEND task has been issued, TWI traffic is now suspended.
    I2C_INT_RXSTARTED_Func, //   Receive sequence started
    I2C_INT_TXSTARTED_Func, //   Transmit sequence started
    I2C_INT_LASTRX_Func, //   Byte boundary, starting to receive the last byte
    I2C_INT_LASTTX_Func, //  
};

/// @brief I2c interupt indexces function
enum
{
    I2C_INT_STOPPED_Mask = 1, //   TWI stopped
    I2C_INT_ERROR_Mask = 9, //   TWI error
    I2C_INT_SUSPENDED_Mask = 18, //   Last byte has been sent out after the SUSPEND task has been issued, TWI traffic is now suspended.
    I2C_INT_RXSTARTED_Mask = 19, //   Receive sequence started
    I2C_INT_TXSTARTED_Mask = 20, //   Transmit sequence started
    I2C_INT_LASTRX_Mask = 23, //   Byte boundary, starting to receive the last byte
    I2C_INT_LASTTX_Mask = 24, //  
};


#define I2C_Pin_Disable U32_MAX



#define I2C_MASTER_ENABLE 0x06
#define I2C_MASTER_DISABLE 0



/// @brief i2c short register to directly interconnect the task and events 
enum
{
    I2C_SHORT_LASTTX_STARTRX =7,
    I2C_SHORT_LASTTX_SUSPEND ,
    I2C_SHORT_LASTTX_STOP ,
    I2C_SHORT_LASTRX_STARTTX ,
    I2C_SHORT_LASTRX_STOP =12 ,
};

#define I2C_internal_pullup_enable 1
#define I2C_internal_pullup_disable 0

/// @brief I2c configuration structure 
typedef struct __PACKED
{
    uint8_t sda_pin;
    uint8_t scl_pin;
    uint32_t sck_freq;
    uint8_t device_addr;
    uint8_t internal_pullup;
}my_i2c_config;

/////////////////////////////////////////////////////////////////////////////
///////////// i2c configuration strucutre 
/***
 * @name i2c_config
 * @param[1] i2c_hadwareno
 * @param[2] configuration structure 
 * @brief used to configure the i2c hardware make sure that i2c is being disabled at that time 
*/
void i2c_config(uint8_t, const my_i2c_config * );


/***
 * @name i2c_module_init
 * @param i2c_hadwareno
 * @brief used to enable and init the i2c engine 
*/
void i2c_module_init(uint8_t);

/***
 * @name i2c_module_deinit
 * @param i2c_hardware_no
 * @brief used to disable and deinit the i2c engine 
*/
void i2c_module_deinit(uint8_t);

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////  interrupt related APIs

/***
 * @name i2c_add_irq_handler
 * @param[1] i2c hardware number 
 * @param[2] the interrupt number 
 * @param[3] the function addr 
 * @brief used to addd the irq handler to a particular handler 
*/
void i2c_add_irq_handler(uint8_t , uint8_t, void (*)(void));


/***
 * @name i2c_remove_irq_handler
 * @param[1] i2c hardware number 
 * @param[2] the interrupt number
 * @brief used to remove  the irq handler to a particular handler 
*/
void i2c_remove_irq_handler(uint8_t, uint8_t);

/***
 * @name i2c_enable_intr
 * @param[1] i2c haradware number 
 * @param[2] the intr number 
 * @brief used to enable the interurupt 
*/
void i2c_enable_intr(uint8_t, uint8_t);

/***
 * @name i2c_disable_intr
 * @param[1] i2c haradware number 
 * @param[2] the intr number 
 * @brief used to disable the interurupt 
*/
void i2c_disable_intr(uint8_t , uint8_t);

/**
 * @name i2c_enable_isr
 * @param i2c hardware number 
 * @brief used to globally enable the interruprt at NVIc 
*/
void i2c_enable_isr(uint8_t);


/**
 * @name i2c_enable_isr
 * @param i2c hardware number 
 * @brief used to globally disable the interruprt at NVIc 
*/
void i2c_disable_isr(uint8_t);


/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   i2c _transfer APIs

/***
 * @name i2c_start_tx
 * @param I2c instance
 * @brief used to start a data tx 
*/
void i2c_start_tx(uint8_t);

/***
 * @name i2c_start_rx
 * @param I2c hardwar number
 * @brief used to start data rx 
*/
void i2c_start_rx(uint8_t);

/***
 * @name i2c_stop_xfr
 * @param i2c hardware number 
 * @brief used to stop any ongoing transaction
*/
void i2c_stop_xfr(uint8_t);

/***
 * @name i2c_suspend_xfr
 * @param i2c hardware number 
 * @brief suspend any ongoing xfr . master will do a clock stretch until resume , please brief that you cannot
 * use a stop after suspend , you have to resume the transaction 
*/
void i2c_suspend_xfr(uint8_t);

/***
 * @name i2c_resume_xfr 
 * @param i2c hardware number 
 * @brief used to resume the i2c transfer . the clcok stretching is over and mmaster will resume from the 
 * pending DMA buffer and xfr all the data 
*/
void i2c_resume_xfr(uint8_t);

/***
 * @name i2c_polling_write_data
 * @param[1] i2c hardware number 
 * @param[2] tx buffer
 * @param[3] size of tx buffer
 * @param[4] timeout 
 * @return succ/failure of the function
 * @brief this is polling xfr the CPU will wait until the xfr is over 
*/
uint8_t i2c_polling_write_data(uint8_t, const uint8_t *, uint8_t , uint32_t );

/***
 * @name i2c_polling_write_data
 * @param[1] i2c hardware number 
 * @param[2] rx buffer
 * @param[3] size of rx buffer
 * @param[4] timeout 
 * @return succ/failure of the function
 * @brief this is polling xfr the CPU will wait until the xfr is over 
*/
uint8_t i2c_polling_read_data(uint8_t, uint8_t *, uint8_t , uint32_t);

/***
 * @name i2c_polling_write_and_read
 * @param[1] i2c hardware number
 * @param[2] tx buffer pointer
 * @param[3] size of tx buff
 * @param[4] rx_buffer pointer
 * @param[5] rx buffer  size 
 * @param[6] timeout
 * @return succ/failure of the function
 * @brief this is polling xfr , the CPU can wait until the xfr is over 
*/
uint8_t i2c_polling_write_and_read(uint8_t, const uint8_t *, uint8_t ,uint8_t *, uint8_t , uint32_t);

/***
 * @name i2c_set_device_addr
 * @param[1] i2c hardware number 
 * @param[2] the peer address 
 * @brief you can change this address between transaction
 *  but make sure use same address for same xfrs to that particular device 
*/
void i2c_set_device_addr(uint8_t, uint8_t);


/// @brief the error cases of the I2C
enum 
{
    I2C_NO_ERR =0,
    I2C_ERROR_OVERRUN = 0x01 ,
    I2C_ERROR_ADDR_NACK = 0x02,
    I2C_ERROR_DATA_NACK = 0x04,
};

/***
 * @name i2c_get_err_and_clear
 * @param i2c_number
 * @return the error code and 
 * @brief used to get the error and also clear the error  
*/
uint8_t i2c_get_err_and_clear(uint8_t);



///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/////////////I2C  short register configuration 

/****
 * @name i2c_short_event
 * @param[1] i2c hardware
 * @param[2] event number 
 * @brief is used to short the event and task internally 
*/
void i2c_short_event(uint8_t, uint8_t);


/****
 * @name i2c_unshort_event
 * @param[1] i2c hardware
 * @param[2] event number 
 * @brief is used to  unshort the event and task internally 
*/
void i2c_unshort_event(uint8_t, uint8_t);


///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/////////////I2C DMA APIS

/****
 * @name i2c_set_tx_buff
 * @param[1] i2c hardware number 
 * @param[2] tx buffer address
 * @param[3] tx size
 * @brief used to set the DMA param , which is double buffer you can update it for next xfr after 
 * recieving txstrted event
*/
void i2c_set_tx_buff(uint8_t, const uint8_t *, uint8_t);

/****
 * @name i2c_set_rx_buff
 * @param[1] i2c hardware number 
 * @param[2] rx buffer address
 * @param[3] rx size
 * @brief used to set the DMA param , which is double buffer you can update it for next xfr after 
 * recieving rxstrted event
*/
void i2c_set_rx_buff(uint8_t, uint8_t *, uint8_t);

/***
 * @name i2c_get_tx_buff
 * @param i2c hardware number 
 * @brief used to get the current address stored in the dma pointer 
*/
uint32_t * i2c_get_tx_buff(uint8_t);

/***
 * @name i2c_get_rx_buff
 * @param i2c hardware number 
 * @brief used to get the current pointer stored in the dma pointer 
*/
uint32_t * i2c_get_rx_buff(uint8_t);

/***
 * @name i2c_get_tx_amount
 * @param i2c hardware number 
 * @brief used to get the amount that is being xfred till now after the start of the txstarted event 
*/
uint8_t i2c_get_tx_amount(uint8_t);
/***
 * @name i2c_get_rx_amount
 * @param i2c hardware number 
 * @brief used to get the amount that is being xfred till now after the start of the rxstarted event 
*/
uint8_t i2c_get_rx_amount(uint8_t);

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////  i2c task and event address API

/***
 * @name i2c_get_task_addr
 * @param[1] i2c hardware number 
 * @param[2] task index number
 * @brief used to get the 
*/
uint32_t * i2c_get_task_addr(uint8_t, uint8_t);
/***
 * @name i2c_get_event_addr
 * @param[1] i2c harddware number 
 * @param[2] event number 
 * @brief to get the event address 
*/
uint32_t * i2c_get_event_addr(uint8_t, uint8_t);









#endif