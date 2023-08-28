#ifndef I2c_THREAD_SAFE_H
#define I2c_THREAD_SAFE_H


#include "i2c.h"

#define TOTAL_I2C_HARDWARE 2

typedef struct PACKED
{
    const uint8_t *tx_buff;
    uint8_t tx_size;
    uint8_t *rx_buff;
    uint8_t rx_size;
}i2c_xfr_config ;


enum 
{
    I2C_XFR_OK,
    I2C_ERROR_XFR_TIMEOUT =5,
    I2C_ERR_DATA_NACK = 4,
    I2C_ERR_ADDR_NACK = 2,
    I2C_ERR_DATA_OVRUN = 1
};




////// define the thread safe API here 
////////
/**
 * @name i2c_thread_safe_init
 * @param i2c_hardware to use 
 * @param my_i2c_config structure 
 * @param the timeout of the tranmsi=son 
 * @brief it is used to init the thread safe API 
*/
void i2c_thread_safe_init(uint8_t i2c_hardware ,const my_i2c_config * , uint32_t );

/***
 * @name i2c_thread_safe_deinit
 * @param i2c_hardware_number
 * @note used to deinit the i2c hardware 
*/
void i2c_thread_safe_deinit(uint8_t i2c_hardware);

/**
 * @name i2c_write_data_arr
 * @param i2c_hardware
 * @param deviceaddress 
 * @param tx_buffer
 * @param size of the tx buffer 
 * @return the error codes
 * @brief used to write data on the device hanging on the i2c line 
*/
 uint32_t  i2c_write_data_arr(uint8_t i2c_hardware ,uint8_t dev_addr, uint8_t * buff, uint8_t size);

/***
 * @name i2c_read_reg_data
 * @param i2c_hardware
 * @param device_address
 * @param ptr to rx buff 
 * @param rx_Size
 * @return error code 
 * @brief used to read the data from the device address 

*/
 uint32_t i2c_read_data_arr(uint8_t i2c_hardware, uint8_t dev_addr , uint8_t *reg_data , uint8_t size);

/**
 * @name  i2c_write_and_read_data_arr
 * @param i2c_hardware
 * @param device_Address
 * @param i2c_xfr_config_structure
 * @return error coede 
 * @brief used to get the data from a i2c register 
*/
uint32_t i2c_write_and_read_data_arr(uint8_t i2c_hardware, uint8_t dev_addr, i2c_xfr_config * );



#endif