#include "uart_drv.h"

static void (*uart_irq_handler[TOTAL_INTERUPTS])(void) = {NULL};

extern void UARTE0_UART0_IRQHandler(void);

void UARTE0_UART0_IRQHandler(void)
{

    // process major irqs here
    // all the rx related events
    if (NRF_UARTE0->EVENTS_ENDRX)
    {
        NRF_UARTE0->EVENTS_ENDRX = 0;

        if (uart_irq_handler[UART_EVENT_ENDRX] != NULL)
        {
            uart_irq_handler[UART_EVENT_ENDRX]();
        }
        // clear the event
    }
    else if (NRF_UARTE0->EVENTS_RXSTARTED)
    {
        // clear the event
        NRF_UARTE0->EVENTS_RXSTARTED = 0;
        if (uart_irq_handler[UART_EVENT_RXSTARTED] != NULL)
        {
            uart_irq_handler[UART_EVENT_RXSTARTED]();
        }
    }
    else if (NRF_UARTE0->EVENTS_RXTO)
    {
        // clear the event
        NRF_UARTE0->EVENTS_RXTO = 0;
        if (uart_irq_handler[UART_EVENT_RXTO] != NULL)
        {
            uart_irq_handler[UART_EVENT_RXTO]();
        }
    }

    //  all the tx related events
    if (NRF_UARTE0->EVENTS_ENDTX)
    {
        NRF_UARTE0->EVENTS_ENDTX = 0;
        if (uart_irq_handler[UART_EVENT_ENDTX] != NULL)
        {
            uart_irq_handler[UART_EVENT_ENDTX]();
        }
    }
    else if (NRF_UARTE0->EVENTS_TXSTARTED)
    {
        NRF_UARTE0->EVENTS_TXSTARTED = 0;
        if (uart_irq_handler[UART_EVENT_TXSTARTED] != NULL)
        {
            uart_irq_handler[UART_EVENT_TXSTARTED]();
        }
    }
    else if (NRF_UARTE0->EVENTS_TXSTOPPED)
    {
        NRF_UARTE0->EVENTS_TXSTOPPED = 0;
        if (uart_irq_handler[UART_EVENT_TXSTOPPED] != NULL)
        {
            uart_irq_handler[UART_EVENT_TXSTOPPED]();
        }
    }

#if defined(FULL_IRQ_SUPPORTED)
    // process minor irqs here
    if (NRF_UARTE0->EVENTS_CTS)
    {
        NRF_UARTE0->EVENTS_CTS = 0;
        if (uart_irq_handler[UART_INT_CTS] != NULL)
        {
            uart_irq_handler[UART_INT_CTS]();
        }
    }

    // you can implement if u want

#endif
}

/// @brief configure the uart hardware
/// @param cfg
void uart_config(const uart_config_t * cfg)
{
    NRF_UARTE0->BAUDRATE = cfg->baud_rate;
    NRF_UARTE0->CONFIG = (cfg->parity << UARTE_CONFIG_PARITY_Pos) | (cfg->hardware_flow.flow_control);
    NRF_UARTE0->PSEL.TXD = cfg->txd_pin;
    NRF_UARTE0->PSEL.RXD = cfg->rxd_pin;

    if (cfg->hardware_flow.flow_control)
    {
        // configure the pins
        NRF_UARTE0->PSEL.CTS = cfg->hardware_flow.cts_pin;
        NRF_UARTE0->PSEL.RTS = cfg->hardware_flow.rts_pin;
    }
}

/// @brief init the uart and also enable the uart engine
void uart_init(void)
{
    // clear the intset register
    NRF_UARTE0->INTENCLR = U32_MAX;

    
    // clear the short register 
    NRF_UARTE0->SHORTS =0;
    
    // clear the error src register
    NRF_UARTE0->ERRORSRC = 0x0FUL;

    // clear all the interrupts handlers
    for (uint8_t i = 0; i < TOTAL_INTERUPTS; i++)
    {
        uart_irq_handler[i] = NULL;
    }
    // enable the uart
    NRF_UARTE0->ENABLE = UART_ENABLE;
}

/// @brief it will deinit  and also disable the uart hardware , also disable the isr
void uart_deinit(void)
{
    uart_disable_isr();

    // make sure to not called stoptx and stoprx if you do then wait for txstopped and stoprx events
    NRF_UARTE0->ENABLE = UART_DISABLE;

    // clear the intset register
    NRF_UARTE0->INTENCLR = U32_MAX;
    // clear the short register 
    NRF_UARTE0->SHORTS =0;

    // deinits the pins
    NRF_UARTE0->PSEL.CTS = UARTE_PSEL_DISCONNECTED;
    NRF_UARTE0->PSEL.RTS = UARTE_PSEL_DISCONNECTED;
    NRF_UARTE0->PSEL.TXD = UARTE_PSEL_DISCONNECTED;
    NRF_UARTE0->PSEL.RXD = UARTE_PSEL_DISCONNECTED;
}

/// @brief flush the rx fifo data in the ram buffer
void uart_flush_rx_fifo(void)
{
    NRF_UARTE0->TASKS_FLUSHRX = 1UL;
}

/// @brief get the error source and also clear it
/// @return the error value
uint8_t uart_get_err_and_clear(void)
{
    uint32_t mask_val = 0;
    mask_val = NRF_UARTE0->ERRORSRC;
    NRF_UARTE0->ERRORSRC = mask_val;
    return mask_val;
}

////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////// UART interrupt related APIs

/// @brief enable the nvic side interrupt and config priority
void uart_enable_isr(void)
{
    NVIC_SetPriority(UARTE0_UART0_IRQn, UART_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(UARTE0_UART0_IRQn);
}

/// @brief disable the nvic side interrupt
/// @param  void
void uart_disable_isr(void)
{
    NVIC_DisableIRQ(UARTE0_UART0_IRQn);
}

/// @brief uart add irq handler for the interrupt
void uart_add_irq_handler(uint8_t int_type, void (*func)(void))
{
    uart_irq_handler[int_type] = func;
}

/// @brief remove the irq handler of the interrupt
void uart_remove_irq_handler(uint8_t int_type)
{
    uart_irq_handler[int_type] = NULL;
}

/// @brief enable the interrupt for the event
//// make sure to call this fun in parameter int_Type_mask
void uart_enable_int(uint8_t int_type_mask)
{
    NRF_UARTE0->INTENSET = _BV(int_type_mask);
}

//// @brief disable the interrupt for the event
void uart_disable_int(uint8_t int_type_mask)
{
    NRF_UARTE0->INTENCLR = _BV(int_type_mask);
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////// uart transmission related APIs

//// @brief start the transmission process
void uart_start_transmit(void)
{
    NRF_UARTE0->TASKS_STARTTX = 1UL;
}

/// @brief stop the transmission process
void uart_stop_tranmsit(void)
{
    NRF_UARTE0->TASKS_STOPTX = 1UL;

}

/// @brief start the reception
void uart_start_reception(void)
{
    NRF_UARTE0->TASKS_STARTRX = 1UL;
}

/// @brief stop the reception
void uart_stop_reception(void)
{
    NRF_UARTE0->TASKS_STOPRX = 1UL;
}

/// @brief one shot reception this will short the endrx and stoprx
void uart_one_shot_reception(void)
{
    NRF_UARTE0->SHORTS = 0;
    NRF_UARTE0->SHORTS = _BV(UARTE_SHORTS_ENDRX_STOPRX_Pos);
}

/// @brief continous reception this will short the endrx event  and startrx task
void uart_continous_reception(void)
{
    NRF_UARTE0->SHORTS = 0;
    NRF_UARTE0->SHORTS = _BV(UARTE_SHORTS_ENDRX_STARTRX_Pos);
}

//////////////////////////////////////////////////////////////////////
/////////////////////////////////////// DMA buffers related APIs

//// @brief set the dma buffer ptr and size
void uart_set_tx_buff(const uint8_t * ptr , uint8_t size )
{
    NRF_UARTE0->TXD.PTR = (uint32_t)ptr;
    NRF_UARTE0->TXD.MAXCNT = size;
}

//// @brief set the dma buffer ptr and size
void uart_set_rx_buff(uint8_t * ptr, uint8_t size)
{
    NRF_UARTE0->RXD.PTR = (uint32_t)ptr;
    NRF_UARTE0->RXD.MAXCNT = size;
}

/// @brief get the dma tx buffer address
uint32_t *uart_get_dma_tx_buff(void)
{
    return (uint32_t *)NRF_UARTE0->TXD.PTR;
}

/// @brief get the dma rx buffer address
uint32_t *uart_get_dma_rx_buff(void)
{
    return (uint32_t *)NRF_UARTE0->RXD.PTR;
}

/// @brief get the last datalen xfr by the dma
/// @param void
/// @return  lenght of data transfer by DMA 
uint8_t uart_get_last_rx_len(void)
{
    return NRF_UARTE0->RXD.AMOUNT;
}

/// @brief get the last transfer by the DMA 
/// @param void
/// @return lenght of data transfer by DMA 
uint8_t uart_get_last_tx_len(void)
{
    return NRF_UARTE0->TXD.AMOUNT;
}

////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//////////////////////////uart TASk and event  address

/// @brief get the task addr
uint32_t *uart_get_task_addr(uint8_t task_type)
{
    uint32_t *ptr = NULL;
    switch (task_type)
    {
    case UART_TASK_START_RX:
        /* code */
        ptr = (uint32_t *)&NRF_UARTE0->TASKS_STARTRX;
        break;

    case UART_TASK_STOP_RX:
        ptr = (uint32_t *)&NRF_UARTE0->TASKS_STOPRX;
        break;
    case UART_TASK_START_TX:
        ptr = (uint32_t *)&NRF_UARTE0->TASKS_STARTTX;
        break;

    case UART_TASK_STOP_TX:
        ptr = (uint32_t *)&NRF_UARTE0->TASKS_STOPTX;
        break;

    case UART_TASK_FLUSH_RX:
        ptr = (uint32_t *)&NRF_UARTE0->TASKS_FLUSHRX;
        break;
    }
    return ptr;
}

/// @brief get the event addr
uint32_t *uart_get_event_addr(uint8_t event_type)
{
    uint32_t *ptr = NULL;
    switch (event_type)
    {
    case UART_EVENT_CTS:
        /* code */
        ptr = (uint32_t *)&NRF_UARTE0->EVENTS_CTS;
        break;
    case UART_EVENT_NCTS:
        ptr = (uint32_t *)&NRF_UARTE0->EVENTS_NCTS;
        /* code */
        break;
    case UART_EVENT_RXDRDY:
        ptr = (uint32_t *)&NRF_UARTE0->EVENTS_RXDRDY;
        /* code */
        break;
    case UART_EVENT_ENDRX:
        ptr = (uint32_t *)&NRF_UARTE0->EVENTS_ENDRX;
        /* code */
        break;
    case UART_EVENT_TXDRDY:
        ptr = (uint32_t *)&NRF_UARTE0->EVENTS_TXDRDY;
        /* code */
        break;
    case UART_EVENT_ENDTX:
        ptr = (uint32_t *)&NRF_UARTE0->EVENTS_ENDTX;
        /* code */
        break;
    case UART_EVENT_ERROR:
        ptr = (uint32_t *)&NRF_UARTE0->EVENTS_ERROR;
        /* code */
        break;
    case UART_EVENT_RXTO:
        ptr = (uint32_t *)&NRF_UARTE0->EVENTS_RXTO;
        /* code */
        break;
    case UART_EVENT_RXSTARTED:
        ptr = (uint32_t *)&NRF_UARTE0->EVENTS_RXSTARTED;
        /* code */
        break;

    case UART_EVENT_TXSTARTED:
        ptr = (uint32_t *)&NRF_UARTE0->EVENTS_TXSTARTED;
        /* code */
        break;
    case UART_EVENT_TXSTOPPED:
        ptr = (uint32_t *)&NRF_UARTE0->EVENTS_TXSTOPPED;
        /* code */
        break;
    }

    return ptr;
}
