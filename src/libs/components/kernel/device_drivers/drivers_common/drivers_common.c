#include "drivers_common.h"


#if defined(FREERTOS_ENV)

#include "i2c_thread_safe.h"
#include "spi_thread_safe.h"

#else

#include "spi.h"
#include "i2c.h"

#endif

// uart log library
#include "logger.h"
#include "gpiote.h"
#include "nrf_time.h"
#include "ansii_color_codes.h"



////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/// @brief install the hardware driver 
/// @param void 
void Hardware_drivers_install(void)
{

#if defined(USE_TIMER_DELAY)

    time_init();
#endif

#if defined(NRF_CONFIG_INSTALL_GPIO_ISR_SERVICE)

    gpio_install_isr_servc();
#endif

#if defined(NRF_CONFIG_I2C_DRIVER_INSTALL)

    const my_i2c_config i2c_cfg =
        {
            .device_addr = 0x01, // dummy device address
            .internal_pullup = I2C_internal_pullup_enable,
            .sda_pin = NRF_CONFIG_I2C_SDA_LINE,
            .scl_pin = NRF_CONFIG_I2C_SCL_LINE,
            .sck_freq = NRF_CONFIG_I2C_CLK_FREQ};

    //// if freertos is defined then use the thread safe i2c API
#if defined(FREERTOS_ENV)

    i2c_thread_safe_init(NRF_CONFIG_I2C_HARDWARE_USE, &i2c_cfg, NRF_CONFIG_I2C_WAIT_FOR_DATA_RESPONSE);
#else

    i2c_config(NRF_CONFIG_I2C_HARDWARE_USE, &i2c_cfg);
    i2c_module_init(NRF_CONFIG_I2C_HARDWARE_USE);

#endif
#endif

#if defined(NRF_CONFIG_SPI_DRIVER_INSTALL)

    const my_spi_cfg spi_cfg =
        {
            .dummy_tx_byte = NRF_CONFIG_SPI_DUMMY_TX_BYTE,
            .miso_pin = NRF_CONFIG_SPI_MISO_LINE,
            .mosi_pin = NRF_CONFIG_SPI_MOSI_LINE,
            .sck_pin = NRF_CONFIG_SPI_SCK_LINE,
            .spi_mode = NRF_CONFIG_SPI_MODE_USED,
            .spi_bit_order = NRF_CONFIG_SPI_BIT_ORDER,
            .sck_freq = NRF_CONFIG_SPI_SCK_FREQUENCY};

    //// if freertos is defined then use the thread safe i2c API
#if defined(FREERTOS_ENV)
    spi_thread_safe_init(NRF_CONFIG_SPI_HARDWARE_USE, &spi_cfg, NRF_CONFIG_SPI_DATA_RESPONSE_TIME);

#else
    spi_config(NRF_CONFIG_SPI_HARDWARE_USE, &spi_cfg);
    spi_module_init(NRF_CONFIG_SPI_HARDWARE_USE);

#endif
#endif

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#if defined(UART_DRIVER_INSTALL)

    logger_init();

#endif
}

/// @brief the hardware driver uninstall function 
/// @param  void 
void Hardware_drivers_uninstall(void)
{

#if defined(USE_TIMER_DELAY)

    time_deinit();
#endif

#if defined(INSTALL_GPIO_ISR_SERVICE)

    gpio_uninstall_isr_servc();
#endif

    //// if freertos is defined then use the thread safe i2c API
#if defined(FREERTOS_ENV)

#if defined(I2C_DRIVER_INSTALL)
    i2c_thread_safe_deinit(I2C_HARDWARE_USE);
#endif

#if defined(SPI_DRIVER_INSTALL)
    spi_thread_safe_deinit(SPI_HARDWARE_USE);
#endif

#if defined(UART_DRIVER_INSTALL)
    logger_deinit();
#endif

//// else use the polling API used to transfer data
#else

#if defined(I2C_DRIVER_INSTALL)

    i2c_module_deinit(I2C_HARDWARE_USE);

#endif

#if defined(SPI_DRIVER_INSTALL)

    spi_module_deinit(SPI_HARDWARE_USE);
#endif

#if defined(UART_DRIVER_INSTALL)

    logger_deinit();

#endif

#endif
}