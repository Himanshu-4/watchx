#ifndef _UART_LOGS_H
#define _UART_LOGS_H

/// include the uart buffer 

#include "ansii_color_codes.h"
#include "system.h"

///////////////////////////////////////////////////////
////////////////////////////////////////////////////

/// define the functions here 

////////////// this is for the tX Part only 

/****
 * @name uart_log_init
 * @param void 
 * @brief used to init the log of the uart , set ups intr and isrs 
*/
void uart_log_init(void);

/****
 * @name uart_log_deinit
 * @param void 
 * @brief used to deinit the log of the uart , disable  intr and isrs 
*/
void uart_log_deinit(void);

/****
 * @name uart_log_bytes
 * @param buffer pointer 
 * @param the size of the buffer 
 * @brief it directly copies the data from the buffer into the ring buffer and 
 * then enables the transfer 
*/
uint32_t  uart_log_bytes(const uint8_t * buff , uint16_t size );


/***
 * @name uart_flush_buffer
 * @param  void
 * @brief used to flush the ring buffer and makes h == t
*/
void uart_flush_buffer(void);

/// @brief flush the recieve buffer  
/// @param  void
void uart_flush_rx_buffer(void);

/// @brief flush the tx buffer 
/// @param  void
void uart_flush_tx_buffer(void);

/***
 * @name uart_stop_logging
 * @param void
 * @brief used to temporarly stop logging of the uart 
*/
void uart_stop_logging(void);

/***
 * @name uart_start_logging 
 * @param void
 * @brief used to restart the logging or start it 
*/
void uart_start_logging();

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