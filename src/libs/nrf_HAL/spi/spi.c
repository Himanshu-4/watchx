#include "spi.h"
#include "gpio.h"

/// @brief spi 0 interrupts function
static void (*spi_intr_func[TOATL_SPI_HARDWARE][SPI_TOTAL_INTERRUPTS])(void) = {NULL};
/// @brief spi 1 interrupts function
// static void (*spi_1_intr_func[SPI_TOTAL_INTERRUPTS])(void) = {NULL};
// /// @brief spi 2 interrupts function
// static void (*spi_2_intr_func[SPI_TOTAL_INTERRUPTS])(void) = {NULL};

#ifndef FREERTOS_ENV

/// @brief weak definations of the functions
void __WEAK SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void)
{
    if (NRF_SPIM0->EVENTS_STARTED)
    {
        NRF_SPIM0->EVENTS_STARTED = 0;
        if (spi_intr_func[0][SPI_INT_STARTED_Func] != NULL)
        {
            spi_intr_func[0][SPI_INT_STARTED_Func]();
        }
    }
    else if (NRF_SPIM0->EVENTS_ENDRX)
    {
        NRF_SPIM0->EVENTS_ENDRX = 0;
        if (spi_intr_func[0][SPI_INT_ENDRX_Func] != NULL)
        {
            spi_intr_func[0][SPI_INT_ENDRX_Func]();
        }
    }
    else if (NRF_SPIM0->EVENTS_END)
    {
        NRF_SPIM0->EVENTS_END = 0;
        if (spi_intr_func[0][SPI_INT_END_Func] != NULL)
        {
            spi_intr_func[0][SPI_INT_END_Func]();
        }
    }
    else if (NRF_SPIM0->EVENTS_ENDTX)
    {
        NRF_SPIM0->EVENTS_ENDTX = 0;
        if (spi_intr_func[0][SPI_INT_ENDTX_Func] != NULL)
        {
            spi_intr_func[0][SPI_INT_ENDTX_Func]();
        }
    }
    else if (NRF_SPIM0->EVENTS_STARTED)
    {
        NRF_SPIM0->EVENTS_STARTED = 0;
        if (spi_intr_func[0][SPI_INT_STARTED_Func] != NULL)
        {
            spi_intr_func[0][SPI_INT_STARTED_Func]();
        }
    }
}

/// @brief  weak defination of the functions
void __WEAK SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void)
{
    if (NRF_SPIM1->EVENTS_STARTED)
    {
        NRF_SPIM1->EVENTS_STARTED = 0;
        if (spi_intr_func[1][SPI_INT_STARTED_Func] != NULL)
        {
            spi_intr_func[1][SPI_INT_STARTED_Func]();
        }
    }
    else if (NRF_SPIM1->EVENTS_ENDRX)
    {
        NRF_SPIM1->EVENTS_ENDRX = 0;
        if (spi_intr_func[1][SPI_INT_ENDRX_Func] != NULL)
        {
            spi_intr_func[1][SPI_INT_ENDRX_Func]();
        }
    }
    else if (NRF_SPIM1->EVENTS_END)
    {
        NRF_SPIM1->EVENTS_END = 0;
        if (spi_intr_func[1][SPI_INT_END_Func] != NULL)
        {
            spi_intr_func[1][SPI_INT_END_Func]();
        }
    }
    else if (NRF_SPIM1->EVENTS_ENDTX)
    {
        NRF_SPIM1->EVENTS_ENDTX = 0;
        if (spi_intr_func[1][SPI_INT_ENDTX_Func] != NULL)
        {
            spi_intr_func[1][SPI_INT_ENDTX_Func]();
        }
    }
    else if (NRF_SPIM1->EVENTS_STARTED)
    {
        NRF_SPIM1->EVENTS_STARTED = 0;
        if (spi_intr_func[1][SPI_INT_STARTED_Func] != NULL)
        {
            spi_intr_func[1][SPI_INT_STARTED_Func]();
        }
    }
}

// please use this SPI if you only use 1 SPI in your project
void SPIM2_SPIS2_SPI2_IRQHandler(void)
{
    if (NRF_SPIM2->EVENTS_STARTED)
    {
        NRF_SPIM2->EVENTS_STARTED = 0;
        if (spi_intr_func[2][SPI_INT_STARTED_Func] != NULL)
        {
            spi_intr_func[2][SPI_INT_STARTED_Func]();
        }
    }
    else if (NRF_SPIM2->EVENTS_ENDRX)
    {
        NRF_SPIM2->EVENTS_ENDRX = 0;
        if (spi_intr_func[2][SPI_INT_ENDRX_Func] != NULL)
        {
            spi_intr_func[2][SPI_INT_ENDRX_Func]();
        }
    }
    else if (NRF_SPIM2->EVENTS_END)
    {
        NRF_SPIM2->EVENTS_END = 0;
        if (spi_intr_func[2][SPI_INT_END_Func] != NULL)
        {
            spi_intr_func[2][SPI_INT_END_Func]();
        }
    }
    else if (NRF_SPIM2->EVENTS_ENDTX)
    {
        NRF_SPIM2->EVENTS_ENDTX = 0;
        if (spi_intr_func[2][SPI_INT_ENDTX_Func] != NULL)
        {
            spi_intr_func[2][SPI_INT_ENDTX_Func]();
        }
    }
    else if (NRF_SPIM2->EVENTS_STARTED)
    {
        NRF_SPIM2->EVENTS_STARTED =  0;
        if (spi_intr_func[2][SPI_INT_STARTED_Func] != NULL)
        {
            spi_intr_func[2][SPI_INT_STARTED_Func]();
        }
    }
}

#endif

/// @brief this function is used to get the spi instance from number
/// @param spi_num
/// @return the spi address
static uint32_t get_spi_instance(uint8_t spi_num)
{
    uint32_t addr = 0;
    switch (spi_num)
    {
    case SPI0:
        addr = NRF_SPIM0_BASE;
        break;
    case SPI1:
        addr = NRF_SPIM1_BASE;
        break;
    case SPI2:
        addr = NRF_SPIM2_BASE;
        break;
    }
    return addr;
}

//// config the spi
__FORCE_INLINE void spi_config(uint8_t spi_num, const my_spi_cfg *cfg)
{
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);

    /// first check if SPI was enable or not
    if (preg->ENABLE == SPI_MASTER_ENABLE)
        return;

    // config the sck freq
    preg->FREQUENCY = cfg->sck_freq;
    // disable the interrutps
    preg->INTENCLR = U32_MAX;
    // ocr value
    preg->ORC = cfg->dummy_tx_byte;
    // config
    preg->CONFIG = (uint32_t)((cfg->spi_mode << 1) | (cfg->spi_bit_order));

    // // configure the GPIO ,, first we have to do it in GPIO peripheral
    // gpio_set_dir(cfg->sck_pin, GPIO_PIN_DIR_OUTPUT);
    // gpio_set_dir(cfg->mosi_pin, GPIO_PIN_DIR_OUTPUT);

    const my_gpio_cfg my_spi_pins_cfg =
        {
            .gpio_dir = GPIO_PIN_DIR_OUTPUT,
            .gpio_drive_type = GPIO_PIN_H0H1,
            .gpio_input_buff = GPIO_PIN_INPUT_BUFF_DISCONNECT,
            .gpiopull_mode = GPIO_PIN_NOPULL,
            .gpio_sense_type = GPIO_PIN_NOSENSE};

    gpio_config(cfg->sck_pin, &my_spi_pins_cfg);
    gpio_config(cfg->mosi_pin, &my_spi_pins_cfg);

    gpio_set_level(cfg->sck_pin, (cfg->spi_mode >> 1) & 1UL);
    gpio_pin_reset(cfg->mosi_pin);

    // cfg miso as input
    gpio_set_dir(cfg->miso_pin, GPIO_PIN_DIR_INPUT);

    // config the pins
    preg->PSEL.SCK = cfg->sck_pin;
    preg->PSEL.MISO = cfg->miso_pin;
    preg->PSEL.MOSI = cfg->mosi_pin;
}

__FORCE_INLINE void spi_config_mode(uint8_t spi_num, uint8_t spi_mode)
{
    if ((spi_mode > 3) || (spi_num > 2))
        return;

    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);
    // change the config mode
    // config
    preg->CONFIG = (spi_mode << 1) | (preg->CONFIG & 0x01);
}

/// @brief set the dummy tx byte according to the sensor
/// @param spi_num
/// @param tx_byte
__FORCE_INLINE void spi_set_dummy_tx(uint8_t spi_num, uint8_t tx_byte)
{
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);
    // ocr value
    preg->ORC = tx_byte;
}

//// init the SPI module
__FORCE_INLINE void spi_module_init(uint8_t spi_num)
{
    // nullify the IRQ handlers
    for (uint8_t i = 0; i < SPI_TOTAL_INTERRUPTS; i++)
    {
        spi_intr_func[spi_num][i] = NULL;
    }

    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);

    // disable the SPI isr
    spi_disable_isr(spi_num);

    // clear the short register
    preg->SHORTS = 0;
    /// disable all the interrupts
    preg->INTENCLR = U32_MAX;
    // enable the hardware
    preg->ENABLE = SPI_MASTER_ENABLE;
}

__FORCE_INLINE void spi_module_deinit(uint8_t spi_num)
{
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);

    // disable the SPI isr
    spi_disable_isr(spi_num);

    /// disable all the interrupts
    preg->INTENCLR = U32_MAX;
    // enable the hardware
    preg->ENABLE = SPI_MASTER_DISABLE;
}

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////  interrupt related APIs

__FORCE_INLINE void spi_add_irq_handler(uint8_t spi_num, uint8_t int_num, void (*func)(void))
{
    spi_intr_func[spi_num][int_num] = func;
}

__FORCE_INLINE void spi_remove_irq_handler(uint8_t spi_num, uint8_t int_num)
{
    spi_intr_func[spi_num][int_num] = NULL;
}

__FORCE_INLINE void spi_enable_intr(uint8_t spi_num, uint8_t int_num)
{

    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);

    switch (int_num)
    {
    case SPI_INT_STOPPED:
        preg->INTENSET = _BV(SPI_INT_STOPPED_Mask);
        break;
    case SPI_INT_ENDRX:
        preg->INTENSET = _BV(SPI_INT_ENDRX_Mask);
        break;

    case SPI_INT_END:
        preg->INTENSET = _BV(SPI_INT_END_Mask);
        break;

    case SPI_INT_ENDTX:
        preg->INTENSET = _BV(SPI_INT_ENDTX_Mask);
        break;

    case SPI_INT_STARTED:
        preg->INTENSET = _BV(SPI_INT_STARTED_Mask);
        break;
    }
}

__FORCE_INLINE void spi_disable_intr(uint8_t spi_num, uint8_t int_num)
{
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);

    switch (int_num)
    {
    case SPI_INT_STOPPED:
        preg->INTENCLR = _BV(SPI_INT_STOPPED_Mask);
        break;
    case SPI_INT_ENDRX:
        preg->INTENCLR = _BV(SPI_INT_ENDRX_Mask);
        break;

    case SPI_INT_END:
        preg->INTENCLR = _BV(SPI_INT_END_Mask);
        break;

    case SPI_INT_ENDTX:
        preg->INTENCLR = _BV(SPI_INT_ENDTX_Mask);
        break;

    case SPI_INT_STARTED:
        preg->INTENCLR = _BV(SPI_INT_STARTED_Mask);
        break;
    }
}

__FORCE_INLINE void spi_enable_isr(uint8_t spi_num)
{
    switch (spi_num)
    {
    case SPI0:
        NVIC_SetPriority(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn, SPI_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn);
        break;
    case SPI1:
        NVIC_SetPriority(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn, SPI_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn);
        break;
    case SPI2:
        NVIC_SetPriority(SPIM2_SPIS2_SPI2_IRQn, SPI_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(SPIM2_SPIS2_SPI2_IRQn);
        break;
    }
}

__FORCE_INLINE void spi_disable_isr(uint8_t spi_num)
{
    switch (spi_num)
    {
    case SPI0:
        NVIC_DisableIRQ(SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn);
        break;
    case SPI1:
        NVIC_DisableIRQ(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn);
        break;

    case SPI2:
        NVIC_DisableIRQ(SPIM2_SPIS2_SPI2_IRQn);
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////   spi_transfer APIs

__FORCE_INLINE void spi_continous_xfr(uint8_t spi_num)
{
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);
    preg->SHORTS = _BV(17);
}

__FORCE_INLINE void spi_one_shot_xfr(uint8_t spi_num)
{
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);
    preg->SHORTS = 0;
}

/***
 * @name spi_polling_xfr
 * @param[1] spi instance
 * @param[2] tx buff ptr
 * @param[3] tx size
 * @param[4] rx buff ptr
 * @param[5] rx size
 * @param[6] timeout
 */
__FORCE_INLINE uint8_t spi_polling_xfr(uint8_t spi_num, const uint8_t *tx_buff, uint8_t tx_size, uint8_t *rx_buff, uint8_t rx_size)
{

    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);

    preg->TXD.PTR = (uint32_t)tx_buff;
    preg->TXD.MAXCNT = tx_size;

    preg->RXD.PTR = (uint32_t)rx_buff;
    preg->RXD.MAXCNT = rx_size;

    // start the transfer
    preg->TASKS_START = 1UL;

    // if the event is 1 then break from the loop
    while (preg->EVENTS_END == 0)
    {
        // nrf_delay_us(1);
        // timeout--;
        // if(timeout <=2)
        // {
        //     return nrf_ERR_TIMEOUT;
        // }
    }
    // reset the event
    preg->EVENTS_END = 0;
    return nrf_OK;
}

__FORCE_INLINE void spi_start_xfr(uint8_t spi_num)
{

    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);

    preg->TASKS_START = 1UL;
}

__FORCE_INLINE void spi_stop_xfr(uint8_t spi_num)
{
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);
    preg->TASKS_STOP = 1UL;
}

__FORCE_INLINE void spi_suspend_xfr(uint8_t spi_num)
{
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);
    preg->TASKS_SUSPEND = 1UL;
}

__FORCE_INLINE void spi_resume_xfr(uint8_t spi_num)
{
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);
    preg->TASKS_RESUME = 1UL;
}

///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/////////////SPI DMA APIS

__FORCE_INLINE void spi_set_tx_buff(uint8_t spi_num, const uint8_t *tx_buff, uint8_t tx_size)
{
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);

    preg->TXD.PTR = (uint32_t)tx_buff;
    preg->TXD.MAXCNT = tx_size;
}

__FORCE_INLINE void spi_set_rx_buff(uint8_t spi_num, uint8_t *rx_buff, uint8_t rx_size)
{
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);

    preg->RXD.PTR = (uint32_t)rx_buff;
    preg->RXD.MAXCNT = rx_size;
}

///// All the Below API have parameter type common aka SPI instances

/// @brief get the current address stored in the dma pointer of tx buffer
uint32_t *spi_get_tx_buff(uint8_t spi_num)
{
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);

    return (uint32_t *)(preg->TXD.PTR);
}

/// @brief get the current address stored in dma pointer of rx buffer
uint32_t *spi_get_rx_buff(uint8_t spi_num)
{
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);

    return (uint32_t *)(preg->RXD.PTR);
}

/// @brief get the tx amount in bytes xfred by dma (it can be same or less than maxcnt )
uint8_t spi_get_tx_amount(uint8_t spi_num)
{
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);

    return preg->TXD.AMOUNT;
}

/// @brief get the rx amount xfred by dma (it can be same or less than maxcnt )
uint8_t spi_get_rx_amount(uint8_t spi_num)
{
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);

    return preg->RXD.AMOUNT;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////  spi task and event address API

uint32_t *spi_get_task_addr(uint8_t spi_num, uint8_t task_type)
{
    uint32_t *addr = NULL;
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);
    switch (task_type)
    {

    case SPI_TASKS_START:
        addr = (uint32_t *)preg->TASKS_START;
        break;
    case SPI_TASKS_STOP:
        addr = (uint32_t *)preg->TASKS_STOP;
        break;
    case SPI_TASKS_RESUME:
        addr = (uint32_t *)preg->TASKS_RESUME;
        break;
    case SPI_TASKS_SUSPEND:
        addr = (uint32_t *)preg->TASKS_SUSPEND;
        break;
    }

    return addr;
}

uint32_t *spi_get_event_addr(uint8_t spi_num, uint8_t event_type)
{
    uint32_t *addr = NULL;
    // first get the address of the SPI instances
    NRF_SPIM_Type *preg = (NRF_SPIM_Type *)get_spi_instance(spi_num);

    switch (event_type)
    {
    case SPI_EVENTS_STOPPED:
        addr = (uint32_t *)preg->EVENTS_STOPPED;
        break;
    case SPI_EVENTS_ENDRX:
        addr = (uint32_t *)preg->EVENTS_ENDRX;
        break;
    case SPI_EVENTS_END:
        addr = (uint32_t *)preg->EVENTS_END;
        break;
    case SPI_EVENTS_ENDTX:
        addr = (uint32_t *)preg->EVENTS_ENDTX;
        break;
    case SPI_EVENTS_STARTED:
        addr = (uint32_t *)preg->EVENTS_STARTED;
        break;
    }
    return addr;
}
