#include "spi_thread_safe.h"

//// include the freertos libs
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "task.h"

#include "nrf_custom_log.h"

/// define the timeout here
static uint32_t spi_timeout[TOATL_SPI_HARDWARE];

// use semaphore spi handle
static SemaphoreHandle_t spi_semphr_handle[TOATL_SPI_HARDWARE] = {NULL};

// these are used to create a semaphore buffer
static StaticSemaphore_t spi_semphr_buffer[TOATL_SPI_HARDWARE];

//////////////////////////////////////////////////////
//////////////////// the task notification handle to wait for the transfer complete from the isr
static volatile xTaskHandle spi_task_handle[TOATL_SPI_HARDWARE] = {NULL};

static volatile uint8_t current_csn_pin[TOATL_SPI_HARDWARE] = {0};

/// @brief spi int handler
void spi_0_int_handler(void);
void spi_1_int_handler(void);
void spi_2_int_handler(void);

/// @brief define the irq handler for the spi
// void (*spi_interrupt_handler[TOATL_SPI_HARDWARE])(void) = {spi_0_int_handler, spi_1_int_handler, spi_2_int_handler};

void spi_thread_safe_init(uint8_t spi_num, const my_spi_cfg *spi_cfg, uint32_t timeout)
{

    spi_timeout[spi_num] = timeout;
    /// create the interrupt and assign the irq handler
    // config the spi
    spi_config(spi_num, spi_cfg);
    spi_module_init(spi_num);

    spi_one_shot_xfr(spi_num);
    // enable the global isr
    spi_enable_isr(spi_num);
    // assign the irq handlers
    // spi_add_irq_handler(spi_num, SPI_INT_END, spi_interrupt_handler[spi_num]);
    // enable the interrupt
    spi_enable_intr(spi_num, SPI_INT_END);

    // create the mutex
    spi_semphr_handle[spi_num] = xSemaphoreCreateMutexStatic(&spi_semphr_buffer[spi_num]);

    // always give the  semaphore after initaling
    xSemaphoreGive(spi_semphr_handle[spi_num]);
}

void spi_thread_safe_deinit(uint8_t spi_num)
{
    // nullify the task handle
    spi_task_handle[spi_num] = NULL;

    // deinit the spi hardware
    spi_module_deinit(spi_num);
    // disable the global isr
    spi_disable_isr(spi_num);
    // assign the irq handlers
    // spi_remove_irq_handler(spi_num, SPI_INT_END);
    // enable the interrupt
    spi_disable_intr(spi_num, SPI_INT_END);

    // delete the semaphore
    vSemaphoreDelete(spi_semphr_handle[spi_num]);
    spi_semphr_handle[spi_num] = NULL;
}

uint8_t spi_poll_xfr_thread_safe(uint8_t spi_num, uint8_t csn_pin, const spi_xfr_buff *spi_buff)
{
    // take the semaphore
    if (xSemaphoreTake(spi_semphr_handle[spi_num], spi_timeout[spi_num]) != pdPASS)
    {
        return SPI_ERROR_XFR_TIMEOUT;
    }
    uint32_t err = SPI_XFR_OK;

    // get the current task handle
    spi_task_handle[spi_num] = xTaskGetCurrentTaskHandle();

    // assign the csn pin
    current_csn_pin[spi_num] = csn_pin;

    spi_set_tx_buff(spi_num, spi_buff->tx_buff, spi_buff->tx_size);
    spi_set_rx_buff(spi_num, spi_buff->rx_buff, spi_buff->rx_size);
    // pull the spi csn pin low
    gpio_pin_reset(csn_pin);
    // start the xfr
    spi_start_xfr(spi_num);

 
    // wait for the notification to be recievde from the isr
    if (xTaskNotifyWait(0, U32_MAX, &err, spi_timeout[spi_num]) != pdPASS)
    {
        NRF_LOG_ERROR("spi notif err");
        // pull the pin to high
        gpio_pin_set(csn_pin);
        err = SPI_ERROR_XFR_TIMEOUT;
    }

    // nullify the handle
    spi_task_handle[spi_num] = NULL;
    // give back the semaphore after xfr is complete so that other task can avail the function
    xSemaphoreGive(spi_semphr_handle[spi_num]);

    return err;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t spi_poll_xfr_thread_safe_differnt(uint8_t spi_num, uint8_t csn_pin, const spi_xfr_buff *spi_buff)
{
    // take the semaphore
    if (xSemaphoreTake(spi_semphr_handle[spi_num], spi_timeout[spi_num]) != pdPASS)
    {
        return SPI_ERROR_XFR_TIMEOUT;
    }
    uint32_t err = SPI_XFR_OK;

    // get the current task handle
    spi_task_handle[spi_num] = xTaskGetCurrentTaskHandle();

    // assign the csn pin
    current_csn_pin[spi_num] = csn_pin;

    NRF_LOG_INFO("buff %x, s %d",spi_buff->tx_buff, spi_buff->tx_size);
    spi_set_tx_buff(spi_num, spi_buff->tx_buff, spi_buff->tx_size);
    spi_set_rx_buff(spi_num, spi_buff->rx_buff, spi_buff->rx_size);
    // start the xfr
    spi_start_xfr(spi_num);

    // wait for the notification to be recievde from the isr
    if (xTaskNotifyWait(0x00, U32_MAX, &err, spi_timeout[spi_num]) != pdPASS)
    {
        // pull the pin to high
        gpio_pin_set(csn_pin);
        err = SPI_ERROR_XFR_TIMEOUT;
        goto return_mech;
    }

    // pull the spi csn pin low
    gpio_pin_reset(csn_pin);
    delay(1);
    gpio_pin_set(csn_pin);

return_mech:
    // give back the semaphore after xfr is complete so that other task can avail the function
    xSemaphoreGive(spi_semphr_handle[spi_num]);
    // nullify the handle
    spi_task_handle[spi_num] = NULL;

    return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief ////////////////////////////////////////////////////////////////////////////////////////
/// @param  /
void spi_0_int_handler(void)
{
    gpio_pin_set(current_csn_pin[SPI0]);

    // check and clear the event
    if (NRF_SPIM0->EVENTS_END)
    {
        // clear the interrupt
        NRF_SPIM0->EVENTS_END = 0;
    }
    /////////// high task wokn = false so to avoid context switching
    BaseType_t high_task_awoken = pdFALSE;
    uint32_t res = nrf_OK;

    if (spi_task_handle[SPI0] != NULL)
    {
        xTaskNotifyFromISR(spi_task_handle[SPI0], res, eSetValueWithOverwrite, &high_task_awoken);
    }

    portYIELD_FROM_ISR(high_task_awoken);
}

void spi_1_int_handler(void)
{
    gpio_pin_set(current_csn_pin[SPI1]);

    // check and clear the event
    if (NRF_SPIM1->EVENTS_END)
    {
        // clear the interrupt
        NRF_SPIM1->EVENTS_END = 0;
    }
    /////////// high task wokn = false so to avoid context switching
    BaseType_t high_task_awoken = pdFALSE;
    uint32_t res = nrf_OK;

    if (spi_task_handle[SPI1] != NULL)
    {
        xTaskNotifyFromISR(spi_task_handle[SPI1], res, eSetValueWithOverwrite, &high_task_awoken);
    }

    portYIELD_FROM_ISR(high_task_awoken);
}

void spi_2_int_handler(void)
{
    gpio_pin_set(current_csn_pin[SPI2]);
    /////////// high task wokn = false so to avoid context switching
    BaseType_t high_task_awoken = pdFALSE;
    uint32_t res = nrf_ERR_OCCURED;

    // check and clear the event
    if (NRF_SPIM2->EVENTS_END)
    {
        // clear the interrupt
        NRF_SPIM2->EVENTS_END = 0;
        NRF_SPIM2->EVENTS_ENDRX = 0;
        NRF_SPIM2->EVENTS_ENDTX = 0;
        res = nrf_OK;
    }

    if (spi_task_handle[SPI2] != NULL)
    {
        xTaskNotifyFromISR(spi_task_handle[SPI2], res, eSetValueWithOverwrite, &high_task_awoken);
    }

    portYIELD_FROM_ISR(high_task_awoken);
}

#ifdef FREERTOS_ENV

// void __WEAK SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void)
// {
//     spi_0_int_handler();
// }
// /// @brief  weak defination of the functions
// void __WEAK SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void)
// {
//     spi_1_int_handler();
// }

// please use this SPI if you only use 1 SPI in your project
void SPIM2_SPIS2_SPI2_IRQHandler(void)
{
    spi_2_int_handler();
}

#endif