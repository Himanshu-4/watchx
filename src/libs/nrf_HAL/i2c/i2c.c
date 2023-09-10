#include "i2c.h"
#include "gpio.h"

/// @brief defien the Irq handlers
static void (*i2c_0_irq_handlers[I2C_TOTAL_INTERRUPT])(void) = {NULL};

/// @brief defien the Irq handlers
static void (*i2c_1_irq_handlers[I2C_TOTAL_INTERRUPT])(void) = {NULL};


void SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void)
{
    if (NRF_TWIM0->EVENTS_STOPPED)
    {
        // clear the interurpt
        NRF_TWIM0->EVENTS_STOPPED = 0;

        if (i2c_0_irq_handlers[I2C_INT_STOPPED_Func] != NULL)
        {
            i2c_0_irq_handlers[I2C_INT_STOPPED_Func]();
        }
    }
    else if (NRF_TWIM0->EVENTS_ERROR)
    {
        // clear the interurpt
        NRF_TWIM0->EVENTS_ERROR = 0;

        if (i2c_0_irq_handlers[I2C_INT_ERROR_Func] != NULL)
        {
            i2c_0_irq_handlers[I2C_INT_ERROR_Func]();
        }
    }
    else if (NRF_TWIM0->EVENTS_SUSPENDED)
    {
        // clear the interurpt
        NRF_TWIM0->EVENTS_SUSPENDED = 0;

        if (i2c_0_irq_handlers[I2C_INT_SUSPENDED_Func] != NULL)
        {
            i2c_0_irq_handlers[I2C_INT_SUSPENDED_Func]();
        }
    }

    else if (NRF_TWIM0->EVENTS_RXSTARTED)
    {
        // clear the interurpt
        NRF_TWIM0->EVENTS_RXSTARTED = 0;

        if (i2c_0_irq_handlers[I2C_INT_RXSTARTED_Func] != NULL)
        {
            i2c_0_irq_handlers[I2C_INT_RXSTARTED_Func]();
        }
    }
    else if (NRF_TWIM0->EVENTS_TXSTARTED)
    {
        // clear the interurpt
        NRF_TWIM0->EVENTS_TXSTARTED = 0;

        if (i2c_0_irq_handlers[I2C_INT_TXSTARTED_Func] != NULL)
        {
            i2c_0_irq_handlers[I2C_INT_TXSTARTED_Func]();
        }
    }

    else if (NRF_TWIM0->EVENTS_LASTRX)
    {
        // clear the interurpt
        NRF_TWIM0->EVENTS_LASTRX = 0;

        if (i2c_0_irq_handlers[I2C_INT_LASTRX_Func] != NULL)
        {
            i2c_0_irq_handlers[I2C_INT_LASTRX_Func]();
        }
    }
    else if (NRF_TWIM0->EVENTS_LASTTX)
    {
        // clear the interurpt
        NRF_TWIM0->EVENTS_LASTTX = 0;

        if (i2c_0_irq_handlers[I2C_INT_LASTTX_Func] != NULL)
        {
            i2c_0_irq_handlers[I2C_INT_LASTTX_Func]();
        }
    }
}

void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void)
{
    if (NRF_TWIM1->EVENTS_STOPPED)
    {
        // clear the interurpt
        NRF_TWIM1->EVENTS_STOPPED = 0;

        if (i2c_1_irq_handlers[I2C_INT_STOPPED_Func] != NULL)
        {
            i2c_1_irq_handlers[I2C_INT_STOPPED_Func]();
        }
    }
    else if (NRF_TWIM1->EVENTS_ERROR)
    {
        // clear the interurpt
        NRF_TWIM1->EVENTS_ERROR = 0;

        if (i2c_1_irq_handlers[I2C_INT_ERROR_Func] != NULL)
        {
            i2c_1_irq_handlers[I2C_INT_ERROR_Func]();
        }
    }
    else if (NRF_TWIM1->EVENTS_SUSPENDED)
    {
        // clear the interurpt
        NRF_TWIM1->EVENTS_SUSPENDED = 0;

        if (i2c_1_irq_handlers[I2C_INT_SUSPENDED_Func] != NULL)
        {
            i2c_1_irq_handlers[I2C_INT_SUSPENDED_Func]();
        }
    }

    else if (NRF_TWIM1->EVENTS_RXSTARTED)
    {
        // clear the interurpt
        NRF_TWIM1->EVENTS_RXSTARTED = 0;

        if (i2c_1_irq_handlers[I2C_INT_RXSTARTED_Func] != NULL)
        {
            i2c_1_irq_handlers[I2C_INT_RXSTARTED_Func]();
        }
    }
    else if (NRF_TWIM1->EVENTS_TXSTARTED)
    {
        // clear the interurpt
        NRF_TWIM1->EVENTS_TXSTARTED = 0;

        if (i2c_1_irq_handlers[I2C_INT_TXSTARTED_Func] != NULL)
        {
            i2c_1_irq_handlers[I2C_INT_TXSTARTED_Func]();
        }
    }

    else if (NRF_TWIM1->EVENTS_LASTRX)
    {
        // clear the interurpt
        NRF_TWIM1->EVENTS_LASTRX = 0;

        if (i2c_1_irq_handlers[I2C_INT_LASTRX_Func] != NULL)
        {
            i2c_1_irq_handlers[I2C_INT_LASTRX_Func]();
        }
    }
    else if (NRF_TWIM1->EVENTS_LASTTX)
    {
        // clear the interurpt
        NRF_TWIM1->EVENTS_LASTTX = 0;

        if (i2c_1_irq_handlers[I2C_INT_LASTTX_Func] != NULL)
        {
            i2c_1_irq_handlers[I2C_INT_LASTTX_Func]();
        }
    }
}




//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//////////////// static function to get the address of the i2c

static __FORCE_INLINE uint32_t get_i2c_instance(uint8_t i2c_num)
{
    uint32_t addr = 0;
    switch (i2c_num)
    {
    case I2C0:
        addr = NRF_TWIM0_BASE;
        break;

    case I2C1:
        addr = NRF_TWIM1_BASE;
        break;
    }
    return addr;
}

/////////////////////////////////////////////////////////////////////////////
///////////// i2c configuration strucutre

 void i2c_config(uint8_t i2c_num, const my_i2c_config *cfg)
{

    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    // saftery check if peripheral is enable
    if (preg->ENABLE == I2C_MASTER_ENABLE)
    {
        return;
    }

    /// configure the GPIo pins
    my_gpio_cfg i2c_gpio_cfg =
        {
            .gpio_dir = GPIO_PIN_DIR_INPUT,
            .gpio_drive_type = GPIO_PIN_S0D1,
            .gpiopull_mode = GPIO_PIN_NOPULL,
            .gpio_input_buff =  GPIO_PIN_INPUT_BUFF_CONNECT,
            .gpio_sense_type =   GPIO_PIN_NOSENSE         
            };
    if (cfg->internal_pullup)
    {
        i2c_gpio_cfg.gpiopull_mode = GPIO_PIN_PULLUP;
    }

    gpio_config(cfg->scl_pin, &i2c_gpio_cfg);
    gpio_config(cfg->sda_pin, &i2c_gpio_cfg);

    // make the pin high 
    gpio_pin_set(cfg->sda_pin);
    gpio_pin_set(cfg->scl_pin);

    preg->ADDRESS = cfg->device_addr;
    preg->FREQUENCY = cfg->sck_freq;
    /// config the pins in i2c mux

    preg->PSEL.SDA = cfg->sda_pin;
    preg->PSEL.SCL = cfg->scl_pin;
}

 void i2c_module_init(uint8_t i2c_num)
{
    switch (i2c_num)
    {
    case I2C0:
        for (uint8_t i = 0; i < I2C_TOTAL_INTERRUPT; i++)
        {
            i2c_0_irq_handlers[i] = NULL;
        }
        break;

    case I2C1:
        for (uint8_t i = 0; i < I2C_TOTAL_INTERRUPT; i++)
        {
            i2c_1_irq_handlers[i] = NULL;
        }
        break;
    }
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    i2c_disable_isr(i2c_num);

    // clear the short register
    preg->SHORTS = 0;

    // clear the intr register
    preg->INTENCLR = U32_MAX;
    // enable the register
    preg->ENABLE = I2C_MASTER_ENABLE;
}

 void i2c_module_deinit(uint8_t i2c_num)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    i2c_disable_isr(i2c_num);

    // clear the short register
    preg->SHORTS = 0;
    // clear the intr register
    preg->INTENCLR = U32_MAX;
    // disable the register
    preg->ENABLE = I2C_MASTER_DISABLE;
}

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////  interrupt related APIs

__FORCE_INLINE void i2c_add_irq_handler(uint8_t i2c_num, uint8_t int_num, void (*func)(void))
{
    switch (i2c_num)
    {

    case I2C0:
        i2c_0_irq_handlers[int_num] = func;
        break;

    case I2C1:
        i2c_1_irq_handlers[int_num] = func;
        break;
    }
}

__FORCE_INLINE void i2c_remove_irq_handler(uint8_t i2c_num, uint8_t int_num)
{
    switch (i2c_num)
    {

    case I2C0:
        i2c_0_irq_handlers[int_num] = NULL;
        break;

    case I2C1:
        i2c_1_irq_handlers[int_num] = NULL;
        break;
    }
}

__FORCE_INLINE void i2c_enable_intr(uint8_t i2c_num, uint8_t int_num)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    switch (int_num)
    {
    case I2C_INT_STOPPED:
        preg->INTENSET = _BV(I2C_INT_STOPPED_Mask);
        break;

    case I2C_INT_ERROR:
        preg->INTENSET = _BV(I2C_INT_ERROR_Mask);
        break;

    case I2C_INT_SUSPENDED:
        preg->INTENSET = _BV(I2C_INT_SUSPENDED_Mask);
        break;

    case I2C_INT_RXSTARTED:
        preg->INTENSET = _BV(I2C_INT_RXSTARTED_Mask);
        break;

    case I2C_INT_TXSTARTED:
        preg->INTENSET = _BV(I2C_INT_TXSTARTED_Mask);
        break;

    case I2C_INT_LASTRX:
        preg->INTENSET = _BV(I2C_INT_LASTRX_Mask);
        break;

    case I2C_INT_LASTTX:
        preg->INTENSET = _BV(I2C_INT_LASTTX_Mask);
        break;
    }
}

__FORCE_INLINE void i2c_disable_intr(uint8_t i2c_num, uint8_t int_num)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    switch (int_num)
    {
    case I2C_INT_STOPPED:
        preg->INTENCLR = _BV(I2C_INT_STOPPED_Mask);
        break;

    case I2C_INT_ERROR:
        preg->INTENCLR = _BV(I2C_INT_ERROR_Mask);
        break;

    case I2C_INT_SUSPENDED:
        preg->INTENCLR = _BV(I2C_INT_SUSPENDED_Mask);
        break;

    case I2C_INT_RXSTARTED:
        preg->INTENCLR = _BV(I2C_INT_RXSTARTED_Mask);
        break;

    case I2C_INT_TXSTARTED:
        preg->INTENCLR = _BV(I2C_INT_TXSTARTED_Mask);
        break;

    case I2C_INT_LASTRX:
        preg->INTENCLR = _BV(I2C_INT_LASTRX_Mask);
        break;

    case I2C_INT_LASTTX:
        preg->INTENCLR = _BV(I2C_INT_LASTTX_Mask);
        break;
    }
}

__FORCE_INLINE void i2c_enable_isr(uint8_t i2c_num)
{
    switch (i2c_num)
    {
    case I2C0:
        NVIC_SetPriority(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn, I2C_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn);
        break;

    case I2C1:
        NVIC_SetPriority(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn, I2C_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn);
        break;
    }
}

__FORCE_INLINE void i2c_disable_isr(uint8_t i2c_num)
{
    switch (i2c_num)
    {
    case I2C0:
        NVIC_DisableIRQ(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn);
        break;
    case I2C1:
        NVIC_DisableIRQ(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn);
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   i2c _transfer APIs

__FORCE_INLINE void i2c_start_tx(uint8_t i2c_num)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);
    // trigger the task
    preg->TASKS_STARTTX = 1UL;
}

__FORCE_INLINE void i2c_start_rx(uint8_t i2c_num)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);
    // trigger the task
    preg->TASKS_STARTRX = 1UL;
}

__FORCE_INLINE void i2c_stop_xfr(uint8_t i2c_num)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    // trigger the task
    preg->TASKS_STOP = 1UL;
}

__FORCE_INLINE void i2c_suspend_xfr(uint8_t i2c_num)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    preg->TASKS_SUSPEND = 1UL;
}

__FORCE_INLINE void i2c_resume_xfr(uint8_t i2c_num)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    preg->TASKS_RESUME = 1UL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief ////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
__FORCE_INLINE uint8_t i2c_polling_write_data(uint8_t i2c_num, const uint8_t *tx_buff, uint8_t tx_size, uint32_t timeout)
{
    uint8_t err_code = nrf_OK;

    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    // configure the dma xfr
    preg->TXD.PTR = (uint32_t)tx_buff;
    preg->TXD.MAXCNT = tx_size;

    // short the last tx and stop
    i2c_short_event(i2c_num, I2C_SHORT_LASTTX_STOP);
    // trigger the tx task
    preg->TASKS_STARTTX = 1UL;

    // if the event is 1 hen break from the loop
    while (preg->EVENTS_LASTTX == 0)
    {
        nrf_delay_ms(1);
        timeout--;

        if(i2c_get_err_and_clear(i2c_num) != I2C_NO_ERR)
        {
       
            i2c_stop_xfr(i2c_num);
            err_code = nrf_ERR_OCCURED;
            // go to return mechanisms
            goto return_mech;
        }

        if (timeout <= 2)
        {
            // stop the transmission if timeout occur 
            i2c_stop_xfr(i2c_num);
            err_code = nrf_ERR_TIMEOUT;
            goto return_mech;
        }

    }

    /// this is the scope of the return mechanism
    return_mech:
    // clear the event
    preg->EVENTS_LASTTX = 0;
    // short the last tx and stop
    i2c_unshort_event(i2c_num, I2C_SHORT_LASTTX_STOP);
    return err_code;
}

__FORCE_INLINE uint8_t i2c_polling_read_data(uint8_t i2c_num, uint8_t *rx_buff, uint8_t rx_size, uint32_t timeout)
{

    uint8_t err_code = nrf_OK;
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    // configure the dma xfr
    preg->RXD.PTR = (uint32_t)rx_buff;
    preg->RXD.MAXCNT = rx_size;

    // short the last tx and stop
    i2c_short_event(i2c_num, I2C_SHORT_LASTRX_STOP);
    // trigger the tx task
    preg->TASKS_STARTRX = 1UL;

    // if the event is 1 hen break from the loop
    while (preg->EVENTS_LASTRX == 0)
    {
        nrf_delay_ms(1);
        timeout--;

        if(i2c_get_err_and_clear(i2c_num) != I2C_NO_ERR)
        {
 
            i2c_stop_xfr(i2c_num);
            err_code = nrf_ERR_OCCURED;
            goto return_mech;
        }
        if (timeout <= 2)
        {
            i2c_stop_xfr(i2c_num);
            err_code = nrf_ERR_TIMEOUT;
            goto return_mech;
        }
    }

    // clear the event
    preg->EVENTS_LASTRX = 0;

    return_mech:
    // short the last tx and stop
    i2c_unshort_event(i2c_num, I2C_SHORT_LASTRX_STOP);
    return err_code;
}

__FORCE_INLINE uint8_t i2c_polling_write_and_read(uint8_t i2c_num, const uint8_t *tx_buff, uint8_t tx_size, uint8_t *rx_buff, uint8_t rx_size, uint32_t timeout)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    uint8_t err_code =nrf_OK;

    // configure the dma xfr
    preg->RXD.PTR = (uint32_t)rx_buff;
    preg->RXD.MAXCNT = rx_size;

    // configure the dma xfr
    preg->TXD.PTR = (uint32_t)tx_buff;
    preg->TXD.MAXCNT = tx_size;

    // short the last tx and start rx
    i2c_short_event(i2c_num, I2C_SHORT_LASTTX_STARTRX);
    i2c_short_event(i2c_num, I2C_SHORT_LASTRX_STOP);

    // start the task 
    preg->TASKS_STARTTX = 1UL;

    // if the event is 1 hen break from the loop
    while (preg->EVENTS_LASTTX == 0)
    {
        
        nrf_delay_ms(1);
        timeout--;

        
        if(i2c_get_err_and_clear(i2c_num) != I2C_NO_ERR)
        {
            
            i2c_stop_xfr(i2c_num);
            err_code = nrf_ERR_OCCURED;
            goto return_mech;
            //  return nrf_ERR_INVALID;
        }


        if (timeout <= 2)
        {
            
            i2c_stop_xfr(i2c_num);
            err_code =nrf_ERR_TIMEOUT;
            goto return_mech;
            //  return nrf_ERR_INVALID;
        }

    }

    // clear the event
    preg->EVENTS_LASTTX = 0;

    // now wait for last rx

    // if the event is 1 hen break from the loop
    while (preg->EVENTS_LASTRX == 0)
    {
        nrf_delay_ms(1);
        timeout--;

        
        if(i2c_get_err_and_clear(i2c_num) != I2C_NO_ERR)
        {
            
            i2c_stop_xfr(i2c_num);
            err_code = nrf_ERR_OCCURED;
            goto return_mech;
            //  return nrf_ERR_INVALID;
        }

        if (timeout <= 2)
        {
            
            i2c_stop_xfr(i2c_num);
            err_code =  nrf_ERR_TIMEOUT;
            goto return_mech;
            // return nrf_ERR_INVALID;
        }
    }

    // clear the intr
    preg->EVENTS_LASTRX = 0;


    // now unshort the reg

    return_mech:
    // unshort the last tx and stop
    i2c_unshort_event(i2c_num, I2C_SHORT_LASTTX_STARTRX);
    i2c_unshort_event(i2c_num , I2C_SHORT_LASTRX_STOP);

    return err_code;
}

__FORCE_INLINE void i2c_set_device_addr(uint8_t i2c_num, uint8_t dev_addr)
{

    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    preg->ADDRESS = dev_addr;
}


__FORCE_INLINE uint8_t i2c_get_err_and_clear(uint8_t i2c_num)
{
        // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    uint32_t err =0;
    err = preg->ERRORSRC;
    preg->ERRORSRC = 0X07;
    return err;

}
///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/////////////I2C  short register configuration

__FORCE_INLINE void i2c_short_event(uint8_t i2c_num, uint8_t short_mask)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    preg->SHORTS |= _BV(short_mask);
}

__FORCE_INLINE void i2c_unshort_event(uint8_t i2c_num, uint8_t short_mask)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    preg->SHORTS &= ~_BV(short_mask);
}

///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/////////////I2C DMA APIS

__FORCE_INLINE void i2c_set_tx_buff(uint8_t i2c_num, const uint8_t *tx_buff, uint8_t tx_size)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    preg->TXD.PTR = (uint32_t)tx_buff;
    preg->TXD.MAXCNT = tx_size;
}

__FORCE_INLINE void i2c_set_rx_buff(uint8_t i2c_num, uint8_t *rx_buff, uint8_t rx_size)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    preg->RXD.PTR = (uint32_t)rx_buff;
    preg->RXD.MAXCNT = rx_size;
}

uint32_t *i2c_get_tx_buff(uint8_t i2c_num)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    return (uint32_t *)preg->TXD.PTR;
}

uint32_t *i2c_get_rx_buff(uint8_t i2c_num)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    return (uint32_t *)preg->RXD.PTR;
}

uint8_t i2c_get_tx_amount(uint8_t i2c_num)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    return preg->TXD.AMOUNT;
}

uint8_t i2c_get_rx_amount(uint8_t i2c_num)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    return preg->RXD.AMOUNT;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////  i2c task and event address API

uint32_t *i2c_get_task_addr(uint8_t i2c_num, uint8_t task_num)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    uint32_t *addr = NULL;

    switch (task_num)
    {
    case I2C_TASKS_STARTRX:
        addr = (uint32_t *)preg->TASKS_STARTRX;
        break;

    case I2C_TASKS_STARTTX:
        addr = (uint32_t *)preg->TASKS_STARTTX;
        break;

    case I2C_TASKS_STOP:
        addr = (uint32_t *)preg->TASKS_STOP;
        break;

    case I2C_TASKS_SUSPEND:
        addr = (uint32_t *)preg->TASKS_SUSPEND;
        break;

    case I2C_TASKS_RESUME:
        addr = (uint32_t *)preg->TASKS_RESUME;
        break;
    }

    return addr;
}

uint32_t *i2c_get_event_addr(uint8_t i2c_num, uint8_t event_num)
{
    // get the I2c instances and typecast it
    NRF_TWIM_Type *preg = (NRF_TWIM_Type *)get_i2c_instance(i2c_num);

    uint32_t *addr = NULL;

    switch (event_num)
    {
    case I2C_EVENTS_STOPPED:
        addr = (uint32_t *)preg->EVENTS_STOPPED;
        break;

    case I2C_EVENTS_ERROR:
        addr = (uint32_t *)preg->EVENTS_ERROR;
        break;

    case I2C_EVENTS_SUSPENDED:
        addr = (uint32_t *)preg->EVENTS_SUSPENDED;
        break;

    case I2C_EVENTS_RXSTARTED:
        addr = (uint32_t *)preg->EVENTS_RXSTARTED;
        break;

    case I2C_EVENTS_TXSTARTED:
        addr = (uint32_t *)preg->EVENTS_TXSTARTED;
        break;

    case I2C_EVENTS_LASTRX:
        addr = (uint32_t *)preg->EVENTS_LASTRX;
        break;

    case I2C_EVENTS_LASTTX:
        addr = (uint32_t *)preg->EVENTS_LASTTX;
        break;
    }

    return addr;
}
