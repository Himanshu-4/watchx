#ifndef _SPI_H
#define _SPI_H

#include "system.h"

// defination of external interrupts defined in the vector table 
extern void SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void);
extern void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void);
extern void SPIM2_SPIS2_SPI2_IRQHandler(void);



//// there are 3 spi instances defined in the module 
#define SPI0 0
#define SPI1 1
#define SPI2 2

#define TOATL_SPI_HARDWARE 3

// spi task enums to get the task index
enum _SPI_TASK_TYPES_
{
    SPI_TASKS_START,
    SPI_TASKS_STOP,
    SPI_TASKS_SUSPEND,
    SPI_TASKS_RESUME ,
};

/// @brief spi events address to get the events address 
enum _SPI_EVENTS_TYPES_
{
    SPI_EVENTS_STOPPED, //  0x104 SPI transaction has stopped
    SPI_EVENTS_ENDRX, //  0x110 End of RXD buffer reached
    SPI_EVENTS_END, //  0x118 End of RXD buffer and TXD buffer reached
    SPI_EVENTS_ENDTX, //  0x120 End of TXD buffer reached
    SPI_EVENTS_STARTED, //  
};

/// @brief define the SPI sck frequency only these are supported 
enum _SPI_CLOCK_FREQ_TYPES_
{
    SPI_SCK_125K  = 0x02000000UL, //  125 kbps
    SPI_SCK_250K  = 0x04000000UL, //  250 kbps
    SPI_SCK_500K  = 0x08000000UL, //  500 kbps
    SPI_SCK_1M    = 0x10000000UL, //  1 Mbps
    SPI_SCK_2M    = 0x20000000UL, //  2 Mbps
    SPI_SCK_4M    = 0x40000000UL, //  4 Mbps
    SPI_SCK_8M    = 0x80000000UL, //  8 Mbps

};

/// @brief  define the spi bit order 
enum _SPI_BIT_ORDER_TYPES_
{
    SPI_BIT_ORDER_MSB_FIRST, // bit order msb first 
    SPI_BIT_ORDER_LSB_FIRST  // bit order lsb first 
};

/// @brief spi mode  all 4 modes are supported 
enum _SPI_MODE_TYPES_
{
    SPI_MODE_0 =0,
    SPI_MODE_1, 
    SPI_MODE_2,
    SPI_MODE_3,
};

#define SPI_TOTAL_INTERRUPTS 5
/// @brief alll the events can be used as interrupts 
enum _SPI_INTERRUPT_TYPES_
{
    SPI_INT_STOPPED,  //  0x104 SPI transaction has stopped
    SPI_INT_ENDRX,    //  0x110 End of RXD buffer reached
    SPI_INT_END,      //  0x118 End of RXD buffer and TXD buffer reached
    SPI_INT_ENDTX,    //  0x120 End of TXD buffer reached
    SPI_INT_STARTED,

};

/// @brief th masking value for the interrupt
enum _SPI_INTERRUPT_MASK_TYPES_
{
    SPI_INT_STOPPED_Mask = 1,  //  0x104 SPI transaction has stopped
    SPI_INT_ENDRX_Mask = 4,    //  0x110 End of RXD buffer reached
    SPI_INT_END_Mask = 6,      //  0x118 End of RXD buffer and TXD buffer reached
    SPI_INT_ENDTX_Mask = 8,    //  0x120 End of TXD buffer reached
    SPI_INT_STARTED_Mask = 19,
};

/// @brief the function indexes of the static function array handlers 
enum _SPI_INTERRUPT_HANDLER_TYPES_
{
    SPI_INT_STOPPED_Func,  //  0x104 SPI transaction has stopped
    SPI_INT_ENDRX_Func,    //  0x110 End of RXD buffer reached
    SPI_INT_END_Func,      //  0x118 End of RXD buffer and TXD buffer reached
    SPI_INT_ENDTX_Func,    //  0x120 End of TXD buffer reached
    SPI_INT_STARTED_Func,
};


#define SPI_PIN_DSABLE U32_MAX

typedef struct __PACKED
{
    uint8_t sck_pin;
    uint8_t mosi_pin;
    uint8_t miso_pin;
    uint8_t spi_mode; 
    uint8_t spi_bit_order;
    uint32_t sck_freq;
    uint8_t dummy_tx_byte;
}my_spi_cfg;


#define SPI_MASTER_DISABLE 0
#define SPI_MASTER_ENABLE 7

/***
 * @name spi_config
 * @param[1] spi instance number
 * @param[2] config structure 
 * @brief used to configure the spi with the config structure 
*/
void spi_config(uint8_t, const my_spi_cfg *);


/**
 * @name spi_config_mode
 * @param spi hardware number
 * @param spi mode 
 * @brief used to change the sPI mode of the hardware 
*/
void spi_config_mode(uint8_t ,uint8_t);


/**
 * @name spi_Set_dummy_tx
 * @param spi_number
 * @param tx_byte
 * @brief used to set the dummy tx byte when size of rx> tx
*/
void spi_set_dummy_tx(uint8_t spi_num , uint8_t tx_byte);

/***
 * @name spi_module_init
 * @param spi instance number 
 * @brief used to init the spi module and init the basic structures and also enable the SPI
*/
void spi_module_init(uint8_t);

/***
 * @name spi_module_deinit
 * @param spi instance number 
 * @brief used to deinit the spi module and deinit the  irqs and all stuff and also disable the SPI
*/
void spi_module_deinit(uint8_t);

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////  interrupt related APIs

/****
 * @name spi_add_irq_handler
 * @param[1] spi instance number 
 * @param[2] the interrupt number 
 * @param[3] the function address 
 * @brief used to add the the SPI irq handlers on specific interupt
*/
void spi_add_irq_handler(uint8_t, uint8_t , void (*)(void));

/****
 * @name spi_remove_irq_handler
 * @param[1] spi instance number 
 * @param[2] the interrupt number 
 * @brief used to remove the SPI irq handlers on specific intr number 
*/
void spi_remove_irq_handler(uint8_t, uint8_t );

/****
 * @name spi_enable_intr
 * @param[1] spi number 
 * @param[2] the interrutp numebr
 * @brief used to enable the interrupt in the intset register 
*/
void spi_enable_intr(uint8_t, uint8_t);

/****
 * @name spi_disable_intr
 * @param[1] spi number 
 * @param[2] the interrutp numebr
 * @brief used to disable the interrupt in the intset register 
*/
void spi_disable_intr(uint8_t, uint8_t);

/***
 * @name spi_enable_isr
 * @param spi instances 
 * @brief used to enable the interrupt at the NVIC side  register 
*/
void spi_enable_isr(uint8_t);

/***
 * @name spi_disable_isr
 * @param spi instances 
 * @brief used to disable the interrupt at the NVIC side  register 
*/
void spi_disable_isr(uint8_t);

/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   spi_transfer APIs

/**
 * @name spi_continous_xfr
 * @param spi instances 
 * @brief used to short the SPI END event and START Task
*/
void spi_continous_xfr(uint8_t);

/**
 * @name spi_one_shot_xfr
 * @param spi instances 
 * @brief used to unshort the SPI END event and START Task
*/
void spi_one_shot_xfr(uint8_t);


/***
 * @name spi_polling_xfr
 * @param[1] spi instance 
 * @param[2] tx buff ptr
 * @param[3] tx size  
 * @param[4] rx buff ptr 
 * @param[5] rx size 
 * @return succ or failure of the function
 * @brief used for polling xfr CPU wait for end event and 
*/
uint8_t spi_polling_xfr(uint8_t , const uint8_t *tx, uint8_t tx_size, uint8_t * rx, uint8_t rx_size);

/****
 * @name spi_Start_xfr
 * @param spi_instances
 * @brief used to trigger start transaction task
*/
void spi_start_xfr(uint8_t);

/****
 * @name spi_stop_xfr
 * @param spi_instances
 * @brief used to trigger stop transaction task
*/
void spi_stop_xfr(uint8_t );

/****
 * @name spi_supend_xfr
 * @param spi_instances
 * @brief used to trigger suspend transaction task
*/
void spi_suspend_xfr(uint8_t);

/****
 * @name spi_resume_xfr
 * @param spi_instances
 * @brief used to trigger resume transaction task
*/
void spi_resume_xfr(uint8_t);

///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/////////////SPI DMA APIS

/// @brief set the tx buffer of the SPI it also clear the dma amount register 
/// @param  spi_instances
/// @param  pointer to tx buffer
/// @param  size of the tx buffer in bytes 
void spi_set_tx_buff(uint8_t , const uint8_t *, uint8_t);


/// @brief set the rx buffer of the SPI it also clear the dma amount register 
/// @param  spi_instances
/// @param  pointer to rx buffer
/// @param  size of the rx buffer in bytes 
void spi_set_rx_buff(uint8_t, uint8_t *, uint8_t );

///// All the Below API have parameter type common aka SPI instances 

/// @brief get the current address stored in the dma pointer of tx buffer 
uint32_t * spi_get_tx_buff(uint8_t);

/// @brief get the current address stored in dma pointer of rx buffer 
uint32_t * spi_get_rx_buff(uint8_t);

/// @brief get the tx amount in bytes xfred by dma (it can be same or less than maxcnt )
uint8_t spi_get_tx_amount(uint8_t);

/// @brief get the rx amount xfred by dma (it can be same or less than maxcnt )
uint8_t spi_get_rx_amount(uint8_t);


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////  spi task and event address API

/***
 * @name spi_get_task_addr
 * @param[1] spi instances
 * @param[2] task type number 
 * @return return the address of the task
*/
uint32_t * spi_get_task_addr(uint8_t, uint8_t);

/***
 * @name spi_get_event_addr
 * @param[1] spi instances
 * @param[2] task event number 
 * @return return the event of the task
*/
uint32_t * spi_get_event_addr(uint8_t, uint8_t);



#endif