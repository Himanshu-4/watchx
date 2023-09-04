#include "nrf_oled_driver.h"

#include "spi_thread_safe.h"

#define OLED_SPI_USED NRF_CONFIG_OLED_SPI_HOST_USED

#define OLED_SPI_MAX_XFR_BYTES 256

//// put the oled in commnad mode 
#define NRF_OLED_CMD_MODE() 

#define NRF_OLED_DATA_MODE() 



static void oled_send(uint8_t *data, uint16_t size)
{
    /// split the transactions in multiple spi transfers into 200 bytes 
    // uint8_t split = ((size)/OLED_SPI_MAX_XFR_BYTES) + ((size)%256)?(0):(1); 
    // for (size_t i = 0; i < ; i++)
    // {
    //     /* code */
    // }
    
    spi_xfr_buff buff =
    {
        .rx_buff = NULL,
        .rx_size = 0,
        .tx_buff = data,
        .tx_size = size
    };
    // spi_poll_xfr_thread_safe(OLED_SPI_USED, NRF_CONFIG_OLED_CHIP_SELECT_PIN, );
}