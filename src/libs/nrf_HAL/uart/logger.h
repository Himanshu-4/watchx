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

/// @brief enable the logger transmission
/// @param  void
void logger_enable_tx(void);

/// @brief disable the logger transmission
/// @param  void
void logger_disable_tx(void);

/// @brief enable the logger reception
/// @param  void
void logger_enable_rx(void);

/// @brief disable the logger reception 
/// @param  void
void logger_disable_rx(void);

/// @brief return the pointer of the TX addr 
/// @param  void
/// @return buff addr 
uint8_t * logger_get_tx_buff_addr(void);

/// @brief get the pointer of the RX buffer 
/// @param  void
/// @return buff address 
uint8_t * logger_get_rx_buff_addr(void);


/////////////////////////////////////////////////////////////////////////////
//////////////////////// transmit API  //////////////////////////

/// @brief log the bytes to the UART ring buffer and make to ready it for transmission
/// @param buff
/// @param size
/// @param override
/// @return succ/err code
uint32_t logger_transmit_bytes(const char* pbuff, uint16_t size);



////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//////////////////////// this here is the RX part //////////////////////////

/// @brief read one element from the RX buffer 
/// @param  void
/// @return return char read from the -1 if buff =empty 
char logger_read_char(void);

/// @brief get the pointer of the last element in the queue aka head
/// @param  offset(cannot larger than 255)
/// @return pointer
char * logger_get_rx_head_ptr(uint8_t offset);

/// @brief read the no of bytes in the recive buffer
/// @param  void
/// @return no of bytes
uint16_t logger_get_num_rx_bytes(void);

/// @brief get the data copy into the rx_buffer 
/// @param rx_buff 
/// @param size 
/// @return succ/failure
uint32_t logger_get_rx_data(char * pbuff, uint16_t size);




#endif