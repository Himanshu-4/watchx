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

// #include "FreeRTOS.h"
// #include "FreeRTOSConfig.h"
// #include "semphr.h"

/// we dont want to spend the time in initing the fifo memory space to 0 so custom allocate the buufer
#define FIFO_BUFFER_SECTION __section(".fifo_buffer")

///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
///////////////////////   create API for the Freertos so that it is easi;y modifieable

// wait only for 10 seconds
#define wait_for_uart (pdMS_TO_TICKS(NRF_UART_LOGS_WAIT_TIME))

#define UART_TX_BUFFER_SIZE NRF_CONFIG_UART_TX_BUFF_SIZE

#define UART_RX_BUFFER_SIZE NRF_CONFIG_UART_RX_BUFF_SIZE

#define UART_LOGGER_BAUD_RATE NRF_UART_LOG_BAUD_RATE

typedef struct __PACKED __UART_FIFO_STRUCT__
{

    uint16_t head;
    uint16_t tail;
    bool enable_flag;

    /// @brief to indicate that whether hardware start the transfer
    bool xfr_start;
    //// ring buffer to hold the  data
    uint8_t* pbuff;

} uart_trx_struct_t;

// typedef struct __PACKED __UART_RX_STRUCT__
// {
//     uint16_t head;
//     uint16_t tail;
//     bool enable_flag;

// // #if defined FREERTOS_ENV
// //     SemaphoreHandle_t mutex;
// //     StaticSemaphore_t mutex_semaphore_buffer;
// // #endif
//     /// ring buffer for holding the RX data
//     uint8_t buff[1];

// } uart_rx_stuct_t;

typedef struct __PACKED __UART_STRUCT__
{
    // ---------------------------------------------------------------------------
    /// +++++++++++ define the TX part here +++++++++++++++++++++++++++++++
    uart_trx_struct_t tx;
    // ---------------------------------------------------------------------------
    /// +++++++++++ define the RX part here +++++++++++++++++++++++++++++++
    uart_trx_struct_t rx;

} uart_log_struct_t;

static uart_log_struct_t FIFO_BUFFER_SECTION uart_log;

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////// Handle UArt TX here


/// @brief tha tx_complete callback
void uart_tx_cmplete_callback(void);

/// @brief callback for rx complete
void rx_data_ready_callback(void);

/// @brief logger inits to init the Logger module 
/// @param  
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

    memset(&uart_log, 0, sizeof(uart_log));

    uart_log.rx.pbuff = &rx_buff;
    uart_log.tx.pbuff = &tx_buff;

    // add the callback for tx complete
    uart_add_irq_handler(UART_INT_ENDTX, uart_tx_cmplete_callback);
    // enable the intr  // always specify the mask when enabling interrupt

    // add callback for rx complete
    uart_add_irq_handler(UART_INT_RXDRDY, rx_data_ready_callback);
    // enable the interrupt for the rx complete // always specify the mask when enbaling intr

    // start the rx transmission
    uart_set_rx_buff((uint8_t*) &uart_log.rx.buff, 1);
    
    /// enable the interupt for the transmission and reception
    uart_enable_int(UART_INT_ENDTX_Mask);
    uart_enable_int(UART_INT_RXDRDY_Mask);
}

/// @brief logger deinit to deinit the logger module 
/// @param  
void logger_deinit(void)
{
    /// disable the APIs
    uart_log.rx.enable_flag = 0;
    uart_log.tx.enable_flag = 0;

    // also reset the metadata
    uart_log.rx.head=0;
    uart_log.rx.tail =0;

    uart_log.tx.head=0;
    uart_log.tx.tail =0;
    
    // disable the isr
    uart_disable_isr();
    // deinit the uart
    uart_deinit();
}

/// @brief flush the tx buffer
/// @param  void
void logger_flush_tx_buffer(void)
{
    uart_stop_tx()
}

/// @brief flush the recieve buffer
/// @param  void
void logger_flush_rx_buffer(void)
{
    /// reset the buffer  and also resetart the transmission 
    // stop the transmiison first 
    uart_stop_rx();
    /// reset the FIFO
    uart_log.rx.head =0;
    uart_log.rx.tail =0;

}

/// @brief flush the buffers of tx and rx
/// @param
void logger_flush_buffer(void)
{
    uart_flush_tx_buffer();
    uart_flush_rx_buffer();
}

/// @brief start the reception process
/// @param  void
/// @note this Api assumes that you updated the DMA pointer;s correctly 
/// and will only start the RX process accordingly 
void logger_start_rx(void)
{
    /// start the reception
    if (uart_log.rx.xfr_start)
    {
        return;
    }

    *uart_get_event_addr(UART_EVENT_RXSTARTED) = 0;

    /// start the continous reception mode 
    uart_continous_reception();
    uart_start_reception();
    /// wait for the it to complete
    while (*uart_get_event_addr(UART_EVENT_RXSTARTED) != 1)
    {
        __NOP()
    } /// waste some CPU cycles to get the UART stop
    *uart_get_event_addr(UART_EVENT_RXSTARTED) = 0;
    
    /// change the flag
    uart_log.rx.xfr_start = 1;
    uart_log.rx.enable_flag = 1;
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
    uart_one_shot_reception();
    //// first stop the transfers
    uart_stop_reception();
    

    /// wait for the it to complete
    while (*uart_get_event_addr(UART_EVENT_ENDRX) != 1)
    {
        __NOP()
    } /// waste some CPU cycles to get the UART stop
    *uart_get_event_addr(UART_EVENT_ENDRX) = 0;
    /// change the flag
    uart_log.rx.xfr_start = 0;
    uart_log.rx.enable_flag = 0;
}


/// @brief start the tx process again
/// @param void
/// @note this API is assuming that you set up the DMA pointers correctly 
/// because this API won't set the DMA pointer 
void logger_start_tx(void)
{
    /// start the transmiison
    if (uart_log.tx.xfr_start)
    {
        return;
    }

    *uart_get_event_addr(UART_EVENT_TXSTARTED) = 0;

    uart_start_transmit();
    /// wait for the it to complete
    while (*uart_get_event_addr(UART_EVENT_TXSTARTED) != 1)
    {
        __NOP()
    } /// waste some CPU cycles to get the UART stop
    *uart_get_event_addr(UART_EVENT_TXSTARTED) = 0;
    
    /// change the flag
    uart_log.tx.xfr_start = 1;
    uart_log.tx.enable_flag = 1;
}

/// @brief stop the tx process
/// @param  void
void logger_stop_tx(void)
{
    /// disable the transfer and stop the hardware
    if (!uart_log.tx.xfr_start)
    {
        return;
    }
    *uart_get_event_addr(UART_EVENT_TXSTOPPED) = 0;
    //// first stop the transfers
    uart_stop_tranmsit();

    /// wait for the it to complete
    while (*uart_get_event_addr(UART_EVENT_TXSTOPPED) != 1)
    {
        __NOP()
    } /// waste some CPU cycles to get the UART stop
    *uart_get_event_addr(UART_EVENT_TXSTOPPED) = 0;
    /// change the flag
    uart_log.tx.xfr_start = 0;
    uart_log.tx.enable_flag = 0;
}

/// @brief log the bytes to the UART ring buffer and make to ready it for transmission
/// @param buff
/// @param size
/// @param override
/// @return succ/err code
uint32_t logger_transmit_bytes(const uint8_t* buff, uint16_t size, bool override)
{

    uint32_t err = nrf_OK;
    //////////// there are 2 possible cases
    if (tx_Head_index >= tx_Tail_index)
    {
        // check if input data is bigger than remainng size
        if ((tx_ring_buff_size - (tx_Head_index - tx_Tail_index)) <= size)
        {
            err = nrf_ERR_NO_MEMORY;
            goto return_mech;
        }

        // check if head index equals tail
        if (tx_Head_index == tx_Tail_index)
        {
            tx_Tail_equals_Head = 1;
        }

        /////// there are 2 casess
        /// 256-head index can have all the data
        if ((tx_ring_buff_size - tx_Head_index) >= size)
        {
            // than start copying the data from tx_head_index
            memcpy((uint8_t*) &tx_ring_buff[tx_Head_index], (uint8_t*) buff, size);
            tx_Head_index += size;
        } else // split the data in multiple parts
        {
            // need to split the data in 2 xfrs
            uint8_t first_xfr = (tx_ring_buff_size - tx_Head_index);
            uint8_t rest_xfr = (size - first_xfr);

            // start copyting the data
            memcpy((uint8_t*) &tx_ring_buff[tx_Head_index], (uint8_t*) buff, first_xfr);
            memcpy((uint8_t*) &tx_ring_buff[0], (uint8_t*) (buff + first_xfr), rest_xfr);

            // update the tx_head index
            tx_Head_index = rest_xfr;
        }
    } else
    {
        if ((tx_Tail_index - tx_Head_index) <= size)
        {
            err = nrf_ERR_NO_MEMORY;
            goto return_mech;
        }

        /// transfer the content to the buffer
        memcpy((uint8_t*) &tx_ring_buff[tx_Head_index], (uint8_t*) buff, size);
        // update the index
        tx_Head_index += size;
    }

    // if tails equal head then start logging by triggering the start task explicitly
    if (tx_Tail_equals_Head)
    {
        // there are 2 cases
        if (tx_Head_index > tx_Tail_index)
        {
            last_tx_size = (tx_Head_index - tx_Tail_index);
        } else
        {
            last_tx_size = tx_ring_buff_size - tx_Tail_index;
        }
        uart_set_tx_buff((uint8_t*) &tx_ring_buff[tx_Tail_index], last_tx_size);
        // update the head pointer and then trigger the start task
        uart_start_transmit();

        tx_Tail_equals_Head = 0;
    }

return_mech:
    return err;
}

////////////////////////////////////////////////////////////////////////////
//////////////////////////// define the irqs handler of the uart

void uart_tx_cmplete_callback(void)
{

    // first update the no of bytes
    tx_Tail_index += last_tx_size;

    // set the equal flag // means no more data to trnasfer
    if (tx_Head_index == tx_Tail_index)
    {
        tx_Tail_equals_Head = 1;
        return;
    }

    if (tx_Tail_index >= tx_ring_buff_size)
    {
        tx_Tail_index = 0;
    }

    int no_of_bytes_remains = (tx_Head_index - tx_Tail_index);
    /// check now how much bytes left to send
    // if the head pointer is leading the tail pointer
    if (no_of_bytes_remains > 0)
    {
        last_tx_size = no_of_bytes_remains;
        uart_set_tx_buff((uint8_t*) &tx_ring_buff[tx_Tail_index], no_of_bytes_remains);
    } else // if the head pointer lags the tail pointer
    {
        uint8_t size = (tx_ring_buff_size - tx_Tail_index);

        last_tx_size = size;

        uart_set_tx_buff((uint8_t*) &tx_ring_buff[tx_Tail_index], size);

        /* code */
    }

    uart_start_transmit();
}

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////// Handle UArt RX here

char logger_read_char(void)
{
#if defined(FREERTOS_ENV)
    // wait for mutex
    if (xSemaphoreTake(mutex_for_rx_log, wait_for_uart) != pdTRUE)
    {
        return nrf_ERR_TIMEOUT;
    }
#endif

    /// check if the no of bytes greater than 1
    /// find out where head and tail are located
    int diff = rx_Head_index - rx_Tail_index;

    if (diff == 0)
        return -1;
    char val = rx_ring_buff[rx_Tail_index];

    rx_Tail_index++;
    if (rx_Tail_index >= rx_ring_buff_size)
    {
        rx_Tail_index = 0;
    }

#if defined(FREERTOS_ENV)
    // give back the mutex
    xSemaphoreGive(mutex_for_rx_log);
#endif

    return val;

    // else flush the buffer and then start transmit again
}

/// @brief get the pointer of the last element in the queue aka head
/// @param  offset(cannot larger than 255)
/// @return pointer
uint8_t* logger_get_rx_head_ptr(uint8_t offset)
{
    if (!rx_enable_flag)
    {
        return NULL;
    }

    /// check if size is not 0
    if ((rx_Head_index - rx_Tail_index) == 0)
    {
        return NULL;
    }

    return &rx_ring_buff[rx_Tail_index];
}

/// @brief read the no of bytes in the recive buffer
/// @param  void
/// @return no of bytes
uint8_t get_num_rx_bytes(void)
{
    /// find out where head and tail are located
    uint8_t diff = 0;
    if (rx_Head_index >= rx_Tail_index)
    {
        diff = rx_Head_index - rx_Tail_index;
    } else
    {
        diff = rx_ring_buff_size - (rx_Tail_index - rx_Head_index);
    }
    return diff;
}

uint32_t logger_get_rx_data(uint8_t* rx_buff, uint8_t size)
{
    if (!rx_enable_flag)
        return nrf_ERR_INVALID_STATE;
        //// get the mutex acquire

#if defined(FREERTOS_ENV)
    // wait for mutex
    if (xSemaphoreTake(mutex_for_rx_log, wait_for_uart) != pdTRUE)
    {
        return nrf_ERR_TIMEOUT;
    }

#endif
    /// find out where head and tail are located
    int diff = rx_Head_index - rx_Tail_index;

    /// check the invalid paramters
    if (size > MOD(diff))
    {
        return nrf_ERR_INVALID_PARAM;
    }

    if (diff > 0) // head is leading the tail index
    {
        memcpy(rx_buff, (uint8_t*) &rx_ring_buff[rx_Tail_index], size);
        // update the tail index
        rx_Tail_index += size;
    } else // head is lagging the tail index
    {
        // change the negative sign
        diff = -diff;

        uint8_t first_byte = rx_ring_buff_size - rx_Tail_index;
        uint8_t second_byte = diff - first_byte;

        /// first send the first bytes
        memcpy(rx_buff, (uint8_t*) &rx_ring_buff[rx_Tail_index], first_byte);

        memcpy(rx_buff + first_byte, (uint8_t*) rx_ring_buff, second_byte);

        rx_Tail_index = second_byte;
    }

// // check that is heads equals tail
// if (rx_Tail_index == rx_Head_index)
// {
//     rx_Tail_equals_Head = 1;
// }
//// After transfer is complete give back the mutex
#if defined(FREERTOS_ENV)
    // give back the mutex
    xSemaphoreGive(mutex_for_rx_log);
#endif

    return nrf_OK;
}

/// @brief data ready callback for the logger module 
/// @param  
void rx_data_ready_callback(void)
{
    if (!rx_enable_flag)
        return;
    // /// get the amount send by dma
    // uint8_t xfr_byte = uart_get_rx_amount();

    // // no we knnow that how many bytes are transfered , increment head index

    // rx_Head_index += xfr_byte;
    rx_Head_index++;
    // now check if it fits in 8 byte size
    if (rx_Head_index >= rx_ring_buff_size)
    {
        // uint8_t remaing_size = ( rx_ring_buff_size - rx_Head_index);
        // // make the remainng bytes to 0
        // memset(&rx_ring_buff[rx_Head_index], -1, remaing_size);
        // // move the head index to the front of ring buffer
        rx_Head_index = 0;
    }

    // specify this to rx dma buffers
    uart_set_rx_buff((uint8_t*) &rx_ring_buff[rx_Head_index], 1);
    // start the reception
    uart_start_reception();
}
