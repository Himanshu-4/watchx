#include "i2c_thread_safe.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"

void i2c_0_stopped_event_callback_func(void);
void i2c_1_stopped_event_callback_func(void);

void i2c_0_err_callback_func(void);
void i2c_1_err_callback_func(void);

// use semaphore spi handle
static SemaphoreHandle_t i2c_semphr_handle[TOTAL_I2C_HARDWARE] = {NULL};

// these are used to create a semaphore buffer
static StaticSemaphore_t i2c_mutex_buffer[TOTAL_I2C_HARDWARE];

/// @brief / define the timeout of the transmiison
static uint32_t i2c_timeout[TOTAL_I2C_HARDWARE] = {0};

/// @brief  the callback that would be assigned to the stopped event callback function
static void (*i2c_stopped_event_callback[TOTAL_I2C_HARDWARE])(void) = {i2c_0_stopped_event_callback_func,
                                                                       i2c_1_stopped_event_callback_func};

/// @brief  the callback that would be assigned to the error events like nak for address and data
static void (*i2c_err_event_callback[TOTAL_I2C_HARDWARE])(void) = {i2c_0_err_callback_func,
                                                                   i2c_1_err_callback_func};

// defination of external interrupts defined in the vector table
extern void SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void);
extern void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void);

static volatile xTaskHandle i2c_task_handle[TOTAL_I2C_HARDWARE] = {NULL};

// create the  kernel object here  create the static semaphore here

////// define the thread safe API here

void i2c_thread_safe_init(uint8_t i2c_hardware, const my_i2c_config *i2c_cfg, uint32_t timeout)
{

    // get the task handle
    // nullify the task handle
    i2c_task_handle[i2c_hardware] = NULL;

    // equiate the timeout
    i2c_timeout[i2c_hardware] = timeout;

    // config and init the i2c hardware and also the isr unit
    i2c_config(i2c_hardware, i2c_cfg);
    i2c_module_init(i2c_hardware);
    i2c_enable_isr(i2c_hardware);
    // enable intr for error cases
    i2c_get_err_and_clear(i2c_hardware);

    // add the irq handler for stop event
    i2c_add_irq_handler(i2c_hardware, I2C_INT_STOPPED_Func, i2c_stopped_event_callback[i2c_hardware]);

    /// add the irq handler for the error events of the i2c
    i2c_add_irq_handler(i2c_hardware, I2C_INT_ERROR_Func, i2c_err_event_callback[i2c_hardware]);

    /// enable interrupt for stopped events
    i2c_enable_intr(i2c_hardware, I2C_INT_ERROR);

    // also enable the I2c interrupt for stopped event
    i2c_enable_intr(i2c_hardware, I2C_INT_STOPPED);

    // create the mutex
    i2c_semphr_handle[i2c_hardware] = xSemaphoreCreateMutexStatic(&i2c_mutex_buffer[i2c_hardware]);
    
    if (i2c_semphr_handle[i2c_hardware] == NULL)
    {
        APP_ERROR_HANDLER(nrf_ERR_OUT_OF_MEM);
    }

    // always give the  semaphore after initaling
    xSemaphoreGive(i2c_semphr_handle[i2c_hardware]);
}


void i2c_thread_safe_deinit(uint8_t i2c_hardware)
{
    /// delete the semaphore
    vSemaphoreDelete(i2c_semphr_handle[i2c_hardware]);
    // disable the int 
        /// enable interrupt for stopped events
    i2c_disable_intr(i2c_hardware, I2C_INT_ERROR);

    // also enable the I2c interrupt for stopped event
    i2c_disable_intr(i2c_hardware, I2C_INT_STOPPED);

    i2c_remove_irq_handler(i2c_hardware, I2C_INT_STOPPED_Func );
    i2c_remove_irq_handler(i2c_hardware, I2C_INT_ERROR_Func);

    i2c_module_deinit(i2c_hardware);
    i2c_disable_isr(i2c_hardware);

}


/**
 * @name i2c_write_data_arr
 * @param i2c_hardware
 * @param deviceaddress 
 * @param tx_buffer
 * @param size of the tx buffer 
 * @return the error codes
 * @brief used to write data on the device hanging on the i2c line 
*/
uint32_t i2c_write_data_arr(uint8_t i2c_hardware, uint8_t dev_addr, uint8_t *buff, uint8_t size)
{
    if (xSemaphoreTake(i2c_semphr_handle[i2c_hardware], i2c_timeout[i2c_hardware]) != pdPASS)
    {
        return I2C_ERROR_XFR_TIMEOUT;
    }

    uint32_t err = nrf_OK;

    i2c_task_handle[i2c_hardware] = xTaskGetCurrentTaskHandle();

    // set up the transfer
    // set the dev addr
    i2c_set_device_addr(i2c_hardware, dev_addr);
    // set the tx buffer
    i2c_set_tx_buff(i2c_hardware, buff, size);

    /// short the lasttx and stopped
    i2c_short_event(i2c_hardware, I2C_SHORT_LASTTX_STOP);
    // start the transfer
    i2c_start_tx(i2c_hardware);

    // now wait for the notification from the isr
    if (xTaskNotifyWait(0, U32_MAX, &err, i2c_timeout[i2c_hardware]) != pdPASS)
    {
        err = I2C_ERROR_XFR_TIMEOUT;
    }

    /// unshort the lasttx and stopped
    i2c_unshort_event(i2c_hardware, I2C_SHORT_LASTTX_STOP);

    // give the semaphore
    xSemaphoreGive(i2c_semphr_handle[i2c_hardware]);

    return err;
}

/***
 * @name i2c_read_reg_data
 * @param i2c_hardware
 * @param device_address
 * @param ptr to rx buff 
 * @param rx_Size
 * @return error code 
 * @brief used to read the data from the device address 

*/
uint32_t i2c_read_data_arr(uint8_t i2c_hardware, uint8_t dev_addr, uint8_t *buff, uint8_t size)
{
    if (xSemaphoreTake(i2c_semphr_handle[i2c_hardware], i2c_timeout[i2c_hardware]) != pdPASS)
    {
        return I2C_ERROR_XFR_TIMEOUT;
    }

    uint32_t err = nrf_OK;
    i2c_task_handle[i2c_hardware] = xTaskGetCurrentTaskHandle();

    // set up the transfer
    // set the dev addr
    i2c_set_device_addr(i2c_hardware, dev_addr);
    // set the tx buffer
    i2c_set_rx_buff(i2c_hardware, buff, size);

    // short the last rx and stop
    i2c_short_event(i2c_hardware, I2C_SHORT_LASTRX_STOP);

    // start the transfer
    i2c_start_rx(i2c_hardware);

    // now wait for the notification from the isr
    if (xTaskNotifyWait(0, U32_MAX, &err, i2c_timeout[i2c_hardware]) != pdPASS)
    {
        err = I2C_ERROR_XFR_TIMEOUT;
    }

    // unshort the last rx and stop
    i2c_unshort_event(i2c_hardware, I2C_SHORT_LASTRX_STOP);

    // give the semaphore
    xSemaphoreGive(i2c_semphr_handle[i2c_hardware]);

    return err;
}


/**
 * @name  i2c_write_and_read_data_arr
 * @param i2c_hardware
 * @param device_Address
 * @param i2c_xfr_config_structure
 * @return error coede 
 * @brief used to get the data from a i2c register 
*/
uint32_t i2c_write_and_read_data_arr(uint8_t i2c_hardware, uint8_t dev_addr, i2c_xfr_config *cfg)
{

    if (xSemaphoreTake(i2c_semphr_handle[i2c_hardware], i2c_timeout[i2c_hardware]) != pdPASS)
    {
        return I2C_ERROR_XFR_TIMEOUT;
    }

    uint32_t err = nrf_OK;
    i2c_task_handle[i2c_hardware] = xTaskGetCurrentTaskHandle();

    // set up the transfer
    // set the dev addr
    i2c_set_device_addr(i2c_hardware, dev_addr);
    // set the rx buffer
    i2c_set_rx_buff(i2c_hardware, cfg->rx_buff, cfg->rx_size);
    // set tx_buffer
    i2c_set_tx_buff(i2c_hardware, cfg->tx_buff, cfg->tx_size);

    // short the event of lastTX and start_rx
    i2c_short_event(i2c_hardware, I2C_SHORT_LASTTX_STARTRX);
    i2c_short_event(i2c_hardware, I2C_SHORT_LASTRX_STOP);

    // start the transfer
    i2c_start_tx(i2c_hardware);

    // now wait for the notification from the isr
    if (xTaskNotifyWait(0, U32_MAX, &err, i2c_timeout[i2c_hardware]) != pdPASS)
    {
        err = I2C_ERROR_XFR_TIMEOUT;
    }

    // unshort the event
    i2c_unshort_event(i2c_hardware, I2C_SHORT_LASTTX_STARTRX);
    i2c_unshort_event(i2c_hardware, I2C_SHORT_LASTRX_STOP);

    // give the semaphore
     // nullify the task handle after use 
    i2c_task_handle[i2c_hardware] = NULL;
    xSemaphoreGive(i2c_semphr_handle[i2c_hardware]);

    return err;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void i2c_0_stopped_event_callback_func(void)
{
     BaseType_t high_priorty_task_woken = pdFALSE;
    
    uint8_t status = nrf_OK;

    if (i2c_task_handle[I2C0] != NULL)
    {
        xTaskNotifyFromISR(i2c_task_handle[I2C0], status, eSetValueWithOverwrite, &high_priorty_task_woken);
    }

    /// do a contect switch if required
    portYIELD_FROM_ISR(high_priorty_task_woken);
}

void i2c_1_stopped_event_callback_func(void)
{
     BaseType_t high_priorty_task_woken = pdFALSE;
    
    uint8_t status = nrf_OK;

    if (i2c_task_handle[I2C1] != NULL)
    {
        xTaskNotifyFromISR(i2c_task_handle[I2C1], status, eSetValueWithOverwrite, &high_priorty_task_woken);
    }

    /// do a contect switch if required
    portYIELD_FROM_ISR(high_priorty_task_woken);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void i2c_0_err_callback_func(void)
{
    BaseType_t high_priorty_task_woken = pdFALSE;
    
    uint8_t status = nrf_OK;

    // clear the start events

    status = i2c_get_err_and_clear(I2C0);

    
    // also stop the xfr 
    i2c_stop_xfr(I2C0);

    if (i2c_task_handle[I2C0] != NULL)
    {
        xTaskNotifyFromISR(i2c_task_handle[I2C0], status, eSetValueWithOverwrite, &high_priorty_task_woken);
    }

    /// do a contect switch if required
    portYIELD_FROM_ISR(high_priorty_task_woken);
}

void i2c_1_err_callback_func(void)
{
      BaseType_t high_priorty_task_woken = pdFALSE;
    
    uint8_t status = nrf_OK;

    // clear the start events

    status = i2c_get_err_and_clear(I2C1);

    // also stop the xfr 
    i2c_stop_xfr(I2C1);

    if (i2c_task_handle[I2C1] != NULL)
    {
        xTaskNotifyFromISR(i2c_task_handle[I2C1], status, eSetValueWithOverwrite, &high_priorty_task_woken);
    }

    /// do a contect switch if required
    portYIELD_FROM_ISR(high_priorty_task_woken);
}

