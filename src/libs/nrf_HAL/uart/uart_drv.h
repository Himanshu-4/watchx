#ifndef _UART_H
#define _UART_H

#include "system.h"

// this uart is configured with easyDMA and we have only one uart peripheral
// we can use this 

// there are total 11 interrupts in the uart 
#define TOTAL_INTERUPTS 11

// uncomment it if you want to use all the interrupts of the uart peripheral
// #define FULL_IRQ_SUPPORTED 1

#define UARTE_PSEL_DISCONNECTED 0xFFFFFFFFUL

#define UART_ENABLE 8UL
#define UART_DISABLE 0UL







/// @brief uart task indexing for the get addr API
enum  _UART_TASK_TYPES_
{
    UART_TASK_START_RX,
    UART_TASK_STOP_RX,
    UART_TASK_START_TX,
    UART_TASK_STOP_TX,
    UART_TASK_FLUSH_RX

};

/// @brief uart events for the get addr API
enum _UART_EVENT_TYPES_
{
    UART_EVENT_CTS,
    UART_EVENT_NCTS,
    UART_EVENT_RXDRDY,
    UART_EVENT_ENDRX,
    UART_EVENT_TXDRDY,
    UART_EVENT_ENDTX,
    UART_EVENT_ERROR,
    UART_EVENT_RXTO,
    UART_EVENT_RXSTARTED,
    UART_EVENT_TXSTARTED,
    UART_EVENT_TXSTOPPED
};
/// @brief the interrupts no to add the irq handlers and enable disable it 
enum _UART_INTERRUPT_TYPES_
{
    UART_INT_CTS,        
    UART_INT_NCTS,       
    UART_INT_RXDRDY,     
    UART_INT_ENDRX,      
    UART_INT_TXDRDY,     
    UART_INT_ENDTX,      
    UART_INT_ERROR,      
    UART_INT_RXTO,       
    UART_INT_RXSTARTED,  
    UART_INT_TXSTARTED,  
    UART_INT_TXSTOPPED,  
};

/// @brief interrupt that can be set in interrupt set register 
enum _UART_INTERRUPT_MASK_TYPES_
{
    UART_INT_CTS_Mask       = 0,
    UART_INT_NCTS_Mask      = 1,
    UART_INT_RXDRDY_Mask    = 2,
    UART_INT_ENDRX_Mask     = 4,
    UART_INT_TXDRDY_Mask    = 7,
    UART_INT_ENDTX_Mask     = 8,
    UART_INT_ERROR_Mask     = 9,
    UART_INT_RXTO_Mask      = 17,
    UART_INT_RXSTARTED_Mask = 19,
    UART_INT_TXSTARTED_Mask = 20,
    UART_INT_TXSTOPPED_Mask = 22
};



/// @brief errors in the uart through error register
enum _UART_ERROR_TYPES_
{
    UART_ERR_OVERRUN = _BV(0),
    UART_ERR_PARITY  = _BV(1),
    UART_ERR_FRAMING = _BV(2),
    UART_ERR_BREAK   = _BV(3)
};
/// @brief the uart baud rate 
enum _UART_BAUD_RATE_TYPES_
{
    UART_Baudrate_1200     = 0x0004F000UL, //  1200 baud (actual rate: 1205)
    UART_Baudrate_2400     = 0x0009D000UL, //  2400 baud (actual rate: 2396)
    UART_Baudrate_4800     = 0x0013B000UL, //  4800 baud (actual rate: 4808)
    UART_Baudrate_9600     = 0x00275000UL, //  9600 baud (actual rate: 9598)
    UART_Baudrate_14400    = 0x003AF000UL, //  14400 baud (actual rate: 14401)
    UART_Baudrate_19200    = 0x004EA000UL, //  19200 baud (actual rate: 19208)
    UART_Baudrate_28800    = 0x0075C000UL, //  28800 baud (actual rate: 28777)
    UART_Baudrate_38400    = 0x009D0000UL, //  38400 baud (actual rate: 38369)
    UART_Baudrate_57600    = 0x00EB0000UL, //  57600 baud (actual rate: 57554)
    UART_Baudrate_76800    = 0x013A9000UL, //  76800 baud (actual rate: 76923)
    UART_Baudrate_115200   = 0x01D60000UL, //  115200 baud (actual rate: 115108)
    UART_Baudrate_230400   = 0x03B00000UL, //  230400 baud (actual rate: 231884)
    UART_Baudrate_250000   = 0x04000000UL, //  250000 baud
    UART_Baudrate_460800   = 0x07400000UL, //  460800 baud (actual rate: 457143)
    UART_Baudrate_921600   = 0x0F000000UL, //  921600 baud (actual rate: 941176)
    UART_Baudrate_1M       = 0x10000000UL //  1Mega baud

};


/// @brief uart Hardware flow control
enum  _UART_FLOW_CONTROL_TYPES_
{
    HARDWARE_FLOW_DISABLE,
    HARDWARE_FLOW_ENABLE
};


// hardware flow control configurations 
typedef  struct __PACKED
{
    uint8_t flow_control;
    uint32_t cts_pin;
    uint32_t rts_pin;
}flow_control;

enum _UART_PARITY_TYPES_
{
    PARITY_EXCLUDED = 0x00,
    PARITY_INCLUDED = 0x07
};


/// @brief  uart configurations 
typedef  struct __PACKED 
{   
    uint32_t rxd_pin;
    uint32_t txd_pin;
    flow_control hardware_flow;
    uint32_t baud_rate;
    uint8_t parity;
}uart_config_t;

typedef uart_config_t * uart_cfg;

/****
 * @name uart init function
 * @param configuration sttucture 
 * @return succ.failure of func 
 * @brief configure the uart in easyDMA mode 
*/
void uart_config(const uart_config_t *  );

/// @brief init the uart and also enable the uart engine  
void uart_init(void);

/// @brief it will deinit  and also disable the uart hardware , also disable the isr 
void uart_deinit(void);

/// @brief flush the rx fifo data in the ram buffer 
void uart_flush_rx_fifo(void);

/// @brief get the error source and also clear it 
/// @return the error value  
uint8_t uart_get_err_and_clear(void);
////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////// UART interrupt related APIs 

/// @brief enable the nvic side interrupt and config priority
void uart_enable_isr(void);

/// @brief disable the nvic side interrupt
/// @param  void
void uart_disable_isr(void);

/// @brief uart add irq handler for the interrupt
void uart_add_irq_handler(uint8_t, void (*)(void));

/// @brief remove the irq handler of the interrupt 
void uart_remove_irq_handler(uint8_t);

/// @brief enable the interrupt for the event
void uart_enable_int(uint8_t);

//// @brief disable the interrupt for the event
void uart_disable_int(uint8_t);


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////// uart transmission related APIs

//// @brief start the transmission process 
void uart_start_transmit(void);

/// @brief stop the transmission process  
void uart_stop_tranmsit(void);

/// @brief start the reception 
void uart_start_reception(void);

/// @brief stop the reception 
void uart_stop_reception(void);

/// @brief one shot reception this will short the endrx and stoprx 
void uart_one_shot_reception(void);

/// @brief continous reception this will short the endrx event  and startrx task
void uart_continous_reception(void);

//////////////////////////////////////////////////////////////////////
/////////////////////////////////////// DMA buffers related APIs 

//// @brief set the dma buffer ptr and size 
void uart_set_tx_buff(const uint8_t *, uint8_t );


//// @brief set the dma buffer ptr and size 
void uart_set_rx_buff(uint8_t *, uint8_t);

/// @brief get the dma tx buffer address
uint32_t * uart_get_dma_tx_buff(void);

/// @brief get the dma rx buffer address 
uint32_t * uart_get_dma_rx_buff(void);

/// @brief get the last datalen xfr by the dma
/// @param void
/// @return  lenght of data transfer by DMA 
uint8_t uart_get_last_rx_len(void);

/// @brief get the last transfer by the DMA 
/// @param void
/// @return lenght of data transfer by DMA 
uint8_t uart_get_last_tx_len(void);

////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//////////////////////////uart TASk and event  address 

/// @brief get the task addr 
uint32_t * uart_get_task_addr(uint8_t);

/// @brief get the event addr
uint32_t * uart_get_event_addr(uint8_t);


#endif 