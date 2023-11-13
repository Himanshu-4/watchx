#ifndef SPI_THREAD_SAFE_H
#define SPI_THREAD_SAFE_H



#include "gpio.h"

#include "spi.h"


#define TOATL_SPI_HARDWARE 3

typedef __PACKED_STRUCT PACKED
{
  const uint8_t * tx_buff ;
 uint8_t tx_size;
  uint8_t *rx_buff ;
  uint8_t rx_size ;
}spi_xfr_buff;

// create the semaphore 

enum 
{
    SPI_XFR_OK,
    SPI_ERROR_XFR_TIMEOUT,
    SPI_ERROR_BUSY
};


/**
 * @name  spi_thread_safe_init
 * @param spi hardware number 
 * @param spi configuration structure 
 * @param the timeout of the transmision 
 * @brief used to init the spin hardware in thread safe mode 
*/
void spi_thread_safe_init(uint8_t spi_num, const my_spi_cfg *spi_cfg , uint32_t);

/**
 * @name spi_thread_safe_deinit
 * @param spi_number
 * @brief used to deinit the spi hardware in thread safe mode 
 * 
*/
void spi_thread_safe_deinit(uint8_t spi_num);


/***
 * @name spi_poll_xfr_thread_safe
 * @param spi hardware number 
 * @param chip selsect pin 
 * @param spi transfer buffer strucutre 
 * @brief used for transfer of the content over spi in thread safe mode  
*/
uint8_t spi_poll_xfr_thread_safe(uint8_t spi_num, uint8_t csn_pin, const spi_xfr_buff  * spi_buff);


/***
 * @name spi_poll_xfr_thread_safe_duffernt 
 * @param spi hardware number 
 * @param chip select pin
 * @param spi transfer buffer 
 * @brief . 
*/
uint8_t spi_poll_xfr_thread_safe_differnt(uint8_t spi_num, uint8_t csn_pin,const  spi_xfr_buff *spi_buff);

#endif