/****
 *
 * This is a  logger module based on UART & DMA implementations
 *
 * this logger trasmitter and reciever supports the buffer overiding feature
 * i.e. the CPU can overwrite the last data or we can say that the buffer
 * will overshift if full
 *
 * same for the RX part that means the reciever will override the data
 *
 *
 */
#include "logger.h"

#include "gpio.h"
#include "uart_drv.h"

/// we dont want to spend the time in initing the fifo memory space to 0 so custom allocate the buffer
#define FIFO_BUFFER_SECTION __section(".fifo_buffer")

#define UART_TX_BUFFER_SIZE NRF_CONFIG_UART_TX_BUFF_SIZE

#define UART_RX_BUFFER_SIZE NRF_CONFIG_UART_RX_BUFF_SIZE

#define UART_LOGGER_BAUD_RATE NRF_UART_LOG_BAUD_RATE

#define UART_DMA_MAX_SIZE 255

typedef struct __PACKED __UART_FIFO_STRUCT__
{

    uint16_t head; /// the UART_BUFF_SIZE is dead end , niether head and tail present there
    uint16_t tail;

    /// @brief  check whether buff is full or not
    bool buff_full;

    /// @brief  enable flag for the modules
    bool enable_flag;
    /// @brief to indicate that whether hardware start the transfer
    bool xfr_start;
    //// ring buffer to hold the  data
    uint8_t* pbuff;

} uart_trx_struct_t;

/// @brief srtuct for UART transmission
typedef struct __PACKED __UART_STRUCT__
{
    // ---------------------------------------------------------------------------
    /// +++++++++++ define the TX part here +++++++++++++++++++++++++++++++
    uart_trx_struct_t tx;
    // ---------------------------------------------------------------------------
    /// +++++++++++ define the RX part here +++++++++++++++++++++++++++++++
    uart_trx_struct_t rx;

} uart_log_struct_t;

static uart_log_struct_t uart_log;

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////// Handle UArt TX here

/// @brief tha tx_complete callback
void uart_tx_cmplete_callback(void);

/// @brief callback for rx complete
void rx_data_ready_callback(void);

/// @brief logger inits to init the Logger module
/// @param void
void logger_init(void)
{

    /// define those buffers to be only used by the
    static uint8_t FIFO_BUFFER_SECTION tx_buff[UART_TX_BUFFER_SIZE];
    static uint8_t FIFO_BUFFER_SECTION rx_buff[UART_RX_BUFFER_SIZE];

    /// it uses stack rather than flash,
    /// with static keyword it will then use flash
    const static uart_config_t my_uart_cfg = {.baud_rate = UART_LOGGER_BAUD_RATE,
                                              .hardware_flow.flow_control = HARDWARE_FLOW_DISABLE,
                                              .parity = PARITY_EXCLUDED,
                                              .rxd_pin = UART_RX_DEFAULT_PIN,
                                              .txd_pin = UART_TX_DEFAULT_PIN};

    // configure the uart
    uart_config(&my_uart_cfg);
    // init the uart
    uart_init();
    // enable the isr
    uart_enable_isr();

    uart_log.rx.pbuff = rx_buff;
    uart_log.tx.pbuff = tx_buff;

    // add the callback for tx complete
    uart_add_irq_handler(UART_INT_ENDTX, uart_tx_cmplete_callback);
    // enable the intr  // always specify the mask when enabling interrupt

    // add callback for rx complete
    uart_add_irq_handler(UART_INT_RXDRDY, rx_data_ready_callback);
    // enable the interrupt for the rx complete // always specify the mask when enbaling intr

    /// enable the interupt for the transmission and reception
    uart_enable_int(UART_INT_ENDTX_Mask);
    uart_enable_int(UART_INT_RXDRDY_Mask);
}

/// @brief logger deinit to deinit the logger module
/// @param void
void logger_deinit(void)
{
    /// disable the APIs
    logger_flush_buffer();

    /// store the pointer addr
    uint8_t* rx_ptr = uart_log.rx.pbuff;
    memset(&uart_log.rx, 0, sizeof(uart_log.rx));
    uart_log.rx.pbuff = rx_ptr;

    /// store the pointer addr
    uint8_t* tx_ptr = uart_log.tx.pbuff;
    memset(&uart_log.tx, 0, sizeof(uart_log.tx));
    uart_log.tx.pbuff = tx_ptr;

    // disable the isr
    uart_disable_isr();
    // deinit the uart
    uart_deinit();
}

/// @brief flush the tx buffer
/// @param  void
void logger_flush_tx_buffer(void)
{
    logger_stop_tx();
    /// reset the FIFO
    uart_log.tx.head = 0;
    uart_log.tx.tail = 0;
    uart_log.tx.buff_full = 0;
}

/// @brief flush the recieve buffer
/// @param  void
void logger_flush_rx_buffer(void)
{
    /// reset the buffer  and also resetart the transmission
    // stop the transmiison first
    logger_stop_rx();
    /// reset the FIFO
    uart_log.rx.head = 0;
    uart_log.rx.tail = 0;
    uart_log.rx.buff_full = 0;
}

/// @brief flush the buffers of tx and rx
/// @param void
void logger_flush_buffer(void)
{
    logger_flush_tx_buffer();
    logger_flush_rx_buffer();
}

/// @brief start the reception process
/// @param  void
/// @note this Api assumes that you updated the DMA pointers correctly
/// and will only start the RX process accordingly
void logger_start_rx(void)
{
    /// start the reception
    if (uart_log.rx.xfr_start)
    {
        return;
    }

    *uart_get_event_addr(UART_EVENT_RXSTARTED) = 0;

    /// change the flag
    uart_log.rx.xfr_start = 1;
    uart_log.rx.enable_flag = 1;

    /// start the continous reception mode
    uart_continous_reception();
    uart_start_reception();
    /// wait for the it to complete
    while (*uart_get_event_addr(UART_EVENT_RXSTARTED) != 1)
    {
        __NOP();
    } /// waste some CPU cycles to get the UART stop
    *uart_get_event_addr(UART_EVENT_RXSTARTED) = 0;
}

/// @brief stop the reception process
/// @param  void
void logger_stop_rx(void)
{
    /// disable the transfer and stop the hardware
    if (!uart_log.rx.xfr_start)
    {
        return;
    }
    *uart_get_event_addr(UART_EVENT_ENDRX) = 0;
    /// change the flag
    uart_log.rx.xfr_start = 0;
    uart_log.rx.enable_flag = 0;

    uart_one_shot_reception();
    //// first stop the transfers
    uart_stop_reception();

    /// wait for the it to complete
    while (*uart_get_event_addr(UART_EVENT_ENDRX) != 1)
    {
        __NOP();
    } /// waste some CPU cycles to get the UART stop
    *uart_get_event_addr(UART_EVENT_ENDRX) = 0;
}

/// @brief start the tx process again
/// @param void
/// @note this API will automatically set up the DMA pointers and also it only works when
/// there is data available in the buffer otherwise it won't start
void logger_start_tx(void)
{
    /// start the transmiison
    if (uart_log.tx.xfr_start)
    {
        return;
    }
    /// check if data is available in the buffer or not
    if ((uart_log.tx.head == uart_log.tx.tail) && (!(uart_log.tx.buff_full)))
    {
        return;
    }

    uint8_t dma_size = 0;
    /// calculate how much size we have to transfers
    if (uart_log.tx.head > uart_log.tx.tail)
    {
        dma_size = MIN_OF(UART_DMA_MAX_SIZE, (uart_log.tx.head - uart_log.tx.tail));
    } else
    {
        dma_size = MIN_OF(UART_DMA_MAX_SIZE, (UART_TX_BUFFER_SIZE - uart_log.tx.tail));
    }
    /// set up the DMA pointers correctly
    uart_set_tx_buff(&uart_log.tx.pbuff[uart_log.tx.tail], dma_size);

    *uart_get_event_addr(UART_EVENT_TXSTARTED) = 0;

    /// update the flag
    uart_log.tx.xfr_start = 1;
    uart_log.tx.enable_flag = 1;

    uart_start_transmit();
    /// wait for the it to complete
    while (*uart_get_event_addr(UART_EVENT_TXSTARTED) != 1)
    {
        __NOP();
    } /// waste some CPU cycles to get the UART stop
    *uart_get_event_addr(UART_EVENT_TXSTARTED) = 0;
}

/// @brief stop the tx process
/// @param  void
void logger_stop_tx(void)
{
    /// stop the transfer process
    uart_log.tx.xfr_start = 0;
    uart_log.tx.enable_flag = 0;

    *uart_get_event_addr(UART_EVENT_TXSTOPPED) = 0;
    //// first stop the transfers
    uart_stop_tranmsit();

    /// wait for the it to complete
    while (*uart_get_event_addr(UART_EVENT_TXSTOPPED) != 1)
    {
        __NOP();
    } /// waste some CPU cycles to get the UART stop
    *uart_get_event_addr(UART_EVENT_TXSTOPPED) = 0;
}

/// @brief log the bytes to the UART ring buffer and make to ready it for transmission
/// @param buff
/// @param size
/// @param override
/// @return succ/err code
uint32_t logger_transmit_bytes(const uint8_t* pbuff, uint16_t size)
{
    if (!uart_log.tx.enable_flag)
    {
        return nrf_ERR_INVALID_STATE;
    }
    if (size >= UART_TX_BUFFER_SIZE)
    {
        return nrf_ERR_NO_MEMORY;
    }

    //////////// First check if we have to do segment copy or not
    if ((uart_log.tx.head + size) > UART_TX_BUFFER_SIZE)
    {
        uint16_t first_segment = UART_TX_BUFFER_SIZE - uart_log.tx.head;
        uint16_t second_segment = size - first_segment;

        /// check for shifting
        if ((uart_log.tx.head < uart_log.tx.tail) || (second_segment >= uart_log.tx.tail))
        {
            /// also stop the xfr
            logger_stop_tx();
            // i.e. buffer is also full
            uart_log.tx.buff_full = 1;
            uart_log.tx.tail = second_segment;
        }
        uart_log.tx.head = second_segment;
        /// copy the segments
        memcpy(&uart_log.tx.pbuff[uart_log.tx.head], pbuff, first_segment);
        //// copy the second segment
        memcpy(uart_log.tx.pbuff, pbuff + first_segment, second_segment);

    } else // no segment copy
    {
        // copy the data and update the index
        memcpy(&uart_log.tx.pbuff[uart_log.tx.head], pbuff, size);

        // /// check for shifting / overiding
        // if(uart_log.tx.head >= uart_log.tx.tail)
        // {
        //     /// this case there will be no shifting for sure

        // }
        if (uart_log.tx.head < uart_log.tx.tail)
        {
            /// check for shifting/overiding
            if ((uart_log.tx.head + size) >= uart_log.tx.tail)
            {
                /// stop the uart transfers as tail is shifted and update the pointers
                logger_stop_tx();
                // i.e. buffer is also full
                uart_log.tx.buff_full = 1;
                uart_log.tx.tail = uart_log.tx.head + size;

                if (uart_log.tx.tail >= UART_TX_BUFFER_SIZE)
                {
                    uart_log.tx.tail = 0;
                }
            }
        }

        /// update the head
        uart_log.tx.head += size;
        if (uart_log.tx.head >= UART_TX_BUFFER_SIZE) /// check if is at dead end
        {
            uart_log.tx.head = 0;
        }
    }

    /// start the logger to transmit
    logger_start_tx();

    return nrf_OK;
}

////////////////////////////////////////////////////////////////////////////
//////////////////////////// define the irqs handler of the uart

void uart_tx_cmplete_callback(void)
{
    /// check if transfer is stopped or not
    if (!uart_log.tx.enable_flag)
    {
        return;
    }
    /// increment the tail to that addr
    uart_log.tx.tail += uart_get_last_tx_len();

    /// now check if this make the tail exceeding the buffer length
    if (uart_log.tx.tail >= UART_TX_BUFFER_SIZE)
    {
        uart_log.tx.tail = 0;
    }
    /// check if there is any data left to transfers
    if ((uart_log.tx.head == uart_log.tx.tail) && (!uart_log.tx.buff_full))
    {
        uart_log.tx.xfr_start = 0;
        return;
    }
    uart_log.tx.buff_full = 0;

    uint8_t dma_size = 0;
    /// calculate how much size we have to transfers
    if (uart_log.tx.head > uart_log.tx.tail)
    {
        dma_size = MIN_OF(UART_DMA_MAX_SIZE, (uart_log.tx.head - uart_log.tx.tail));
    } else
    {
        dma_size = MIN_OF(UART_DMA_MAX_SIZE, (UART_TX_BUFFER_SIZE - uart_log.tx.tail));
    }
    /// set up the DMA pointers
    uart_set_tx_buff(&uart_log.tx.pbuff[uart_log.tx.tail], dma_size);
    uart_start_transmit();
}

// =================================================================================================
// /================================================================================================
// +++++++++++++++++++++++++++++++++++ Handle logger RX here +++++++++++++++++++++++++++++++++++++++
// ==================================================================================================

char logger_read_char(void)
{

    /// check if the no of bytes greater than 1
    /// find out where head and tail are located
    // int diff = rx_Head_index - rx_Tail_index;

    // if (diff == 0)
    //     return -1;
    char val = 0;

    return val;

    // else flush the buffer and then start transmit again
}

/// @brief get the pointer of the last element in the queue aka head
/// @param  offset(cannot larger than 255)
/// @return pointer
uint8_t* logger_get_rx_head_ptr(uint8_t offset)
{
    if (!uart_log.rx.enable_flag)
    {
        return NULL;
    }

    return NULL;
}

/// @brief read the no of bytes in the recive buffer
/// @param  void
/// @return no of bytes
uint8_t get_num_rx_bytes(void)
{

    return 0;
}

uint32_t logger_get_rx_data(uint8_t* rx_buff, uint8_t size)
{
    if (!uart_log.rx.enable_flag)
    {
        return nrf_ERR_INVALID_STATE;
    }
    //// get the mutex acquire

    // // check that is heads equals tail
    // if (rx_Tail_index == rx_Head_index)
    // {
    //     rx_Tail_equals_Head = 1;
    // }

    return nrf_OK;
}

/// @brief data ready callback for the logger module
/// @param
void rx_data_ready_callback(void)
{
    if (uart_log.rx.enable_flag)
        return;
}
