#ifndef _LOGGER_MODULE_H
#define _LOGGER_MODULE_H

/// include the uart buffer 

#include "ansii_color_codes.h"
#include "system.h"

///////////////////////////////////////////////////////
////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//////////////// basic uart Logger controlling functions ////////////////////

/// @brief logger inits to init the Logger module
/// @param void
void logger_init(void);

/// @brief logger deinit to deinit the logger module
/// @param void
void logger_deinit(void);

/// @brief flush the tx buffer
/// @param  void
void logger_flush_tx_buffer(void);

/// @brief flush the recieve buffer
/// @param  void
void logger_flush_rx_buffer(void);

/// @brief flush the buffers of tx and rx
/// @param void
void logger_flush_buffer(void);

/// @brief start the reception process
/// @param  void
/// @note this Api assumes that you updated the DMA pointers correctly
/// and will only start the RX process accordingly
void logger_start_rx(void);

/// @brief stop the reception process
/// @param  void
void logger_stop_rx(void);

/// @brief start the tx process again
/// @param void
/// @note this API will automatically set up the DMA pointers and also it only works when
/// there is data available in the buffer otherwise it won't start
void logger_start_tx(void);

/// @brief stop the tx process
/// @param  void
void logger_stop_tx(void);

/// @brief log the bytes to the UART ring buffer and make to ready it for transmission
/// @param buff
/// @param size
/// @param override
/// @return succ/err code
uint32_t logger_transmit_bytes(const uint8_t* pbuff, uint16_t size);


////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//////////////////////// this here is the RX part //////////////////////////


/// @brief get the pointer of the last element in the queue aka head 
/// @param  offset(cannot larger than 255)
/// @return pointer 
uint8_t * uart_get_head_ptr(uint8_t offset);

/**
 * @name read_char
 * @param void
 * @brief used to get one byte from the rx ringbuffer 
*/
char read_char(void);

/**
 * @name get_num_rx_bytes
 * @return return the no of bytes in the ring buffer 
 * @brief used to get the remaining bytes in the ring buffer 
*/
uint8_t get_num_rx_bytes(void);

/***
 * @name get_rx_data
 * @param buffer pointer 
 * @param size of the buffer 
 * @return status of the function , if everything is ok then succ . if not then error
 * @brief used to get the data from the buffer , must call in the order 
 * size = get_num_rx_bytes
 * arr[size]
 * get_rx_data(arr, size)
*/
uint32_t get_rx_data(uint8_t *rx_buff, uint8_t size);


#endif