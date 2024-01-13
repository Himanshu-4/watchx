#include "uart_logs.h"
#include "gpio.h"

#include "system.h"

#if defined USE_NRF_LOGS

///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
///////////////////////   create API for the Freertos so that it is easi;y modifieable

  #if defined FREERTOS_ENV

    #include "FreeRTOS.h"
    #include "FreeRTOSConfig.h"
    #include "semphr.h"

static SemaphoreHandle_t mutex_for_tx_log = NULL;

// these are used to create a
static StaticSemaphore_t TX_MutexBuffer;

    // wait only for 10 seconds
#define wait_for_uart (pdMS_TO_TICKS(NRF_UART_LOGS_WAIT_TIME))

static SemaphoreHandle_t mutex_for_rx_log = NULL;

// these are used to create a
static StaticSemaphore_t RX_MutexBuffer;

  #endif

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////// Handle UArt TX here

static volatile uint8_t tx_ring_buff[tx_ring_buff_size + 1];

///// define the global variables to use in our case
static volatile uint16_t tx_Head_index = 0;

static volatile uint16_t tx_Tail_index = 0;

static volatile bool tx_Tail_equals_Head = 1;

static volatile uint16_t last_tx_size = 0;

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////// Handle UArt RX here

static volatile uint8_t rx_ring_buff[rx_ring_buff_size + 1];

///// define the global variables to use in our case
static volatile uint16_t rx_Head_index = 0;

static volatile uint16_t rx_Tail_index = 0;

// static volatile bool rx_Tail_equals_Head = 1;

// static volatile uint16_t last_rx_size =0;

static volatile uint8_t rx_enable_flag = 0;

///////////////////////////////////////////////////////////////////////////
//////////////////////////// callbacks

/// @brief tha tx_complete callback
void uart_tx_cmplete_callback(void);

/// @brief callback for rx complete
void rx_complete_callback(void);

//// initialize the uart log , it enables the uart , set up the intr callbacks and
void uart_log_init(void)
{
  /// it uses stack rather than flash, 
  /// with static keyword it will then use flash  
  const static uart_config_t my_uart_cfg = {.baud_rate = UART_LOG_BAUD_RATE,
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

  // add the callback for tx complete
  uart_add_irq_handler(UART_INT_ENDTX, uart_tx_cmplete_callback);
  // enable the intr  // always specify the mask when enabling interrupt
  uart_enable_int(UART_INT_ENDTX_Mask);

  tx_Head_index = 0;
  tx_Tail_index = 0;
  tx_Tail_equals_Head = 1;

  rx_Head_index = 0;
  rx_Tail_index = 0;
  rx_enable_flag = 1;

  uart_one_shot_reception();
  // add callback for rx complete
  uart_add_irq_handler(UART_INT_ENDRX, rx_complete_callback);
  // enable the interrupt for the rx complete // always specify the mask when enbaling intr
  uart_enable_int(UART_INT_ENDRX_Mask);

  // start the rx transmission
  uart_set_rx_buff((uint8_t*) &rx_ring_buff, 1);
  uart_start_reception();

  /// create the freertos mutex
  #if defined(FREERTOS_ENV)

  mutex_for_tx_log = xSemaphoreCreateMutexStatic(&TX_MutexBuffer);

  configASSERT(mutex_for_tx_log);

  /// @brief create mutex for rx logs
  mutex_for_rx_log = xSemaphoreCreateMutexStatic(&RX_MutexBuffer);

  configASSERT(mutex_for_rx_log);

  #endif
}

void uart_log_deinit()
{
  #if defined FREERTOS_ENV
  // delete the semaphores
  vSemaphoreDelete(mutex_for_tx_log);
  vSemaphoreDelete(mutex_for_rx_log);

  #endif
  // enable the isr
  uart_disable_isr();
  // deinit the uart
  uart_deinit();
}

uint32_t uart_log_bytes(const uint8_t* buff, uint16_t size)
{

  #if defined(FREERTOS_ENV)

  if (xSemaphoreTake(mutex_for_tx_log, wait_for_uart ) != pdTRUE)
    return nrf_ERR_TIMEOUT;

  #endif

  uint32_t err = nrf_OK;
    //////////// there are 2 possible cases
    if (tx_Head_index >= tx_Tail_index) {
        // check if input data is bigger than remainng size
        if ((tx_ring_buff_size - (tx_Head_index - tx_Tail_index)) <= size) {
          err = nrf_ERR_NO_MEMORY;
          goto return_mech;
      }

        // check if head index equals tail
        if (tx_Head_index == tx_Tail_index) {
          tx_Tail_equals_Head = 1;
      }

        /////// there are 2 casess
        /// 256-head index can have all the data
        if ((tx_ring_buff_size - tx_Head_index) >= size) {
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
    } else {
        if ((tx_Tail_index - tx_Head_index) <= size) {
          err = nrf_ERR_NO_MEMORY;
          goto return_mech;
      }

      /// transfer the content to the buffer
      memcpy((uint8_t*) &tx_ring_buff[tx_Head_index], (uint8_t*) buff, size);
      // update the index
      tx_Head_index += size;
    }

    // if tails equal head then start logging by triggering the start task explicitly
    if (tx_Tail_equals_Head) {
        // there are 2 cases
        if (tx_Head_index > tx_Tail_index) {
          last_tx_size = (tx_Head_index - tx_Tail_index);
        } else {
          last_tx_size = tx_ring_buff_size - tx_Tail_index;
        }
      uart_set_tx_buff((uint8_t*) &tx_ring_buff[tx_Tail_index], last_tx_size);
      // update the head pointer and then trigger the start task
      uart_start_transmit();

      tx_Tail_equals_Head = 0;
  }

return_mech:

  #if defined(FREERTOS_ENV)
  // return back the mutex
  xSemaphoreGive(mutex_for_tx_log);
  #endif
  return err;
}

////////////////////////////////////////////////////////////////////////////
//////////////////////////// define the irqs handler of the uart

void uart_tx_cmplete_callback(void)
{

  // first update the no of bytes
  tx_Tail_index += last_tx_size;

    // set the equal flag // means no more data to trnasfer
    if (tx_Head_index == tx_Tail_index) {
      tx_Tail_equals_Head = 1;
      return;
  }

    if (tx_Tail_index >= tx_ring_buff_size) {
      tx_Tail_index = 0;
  }

  int no_of_bytes_remains = (tx_Head_index - tx_Tail_index);
    /// check now how much bytes left to send
    // if the head pointer is leading the tail pointer
    if (no_of_bytes_remains > 0) {
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

char read_char(void)
{
  #if defined(FREERTOS_ENV)
    // wait for mutex
    if (xSemaphoreTake(mutex_for_rx_log, wait_for_uart) != pdTRUE) {
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
    if (rx_Tail_index >= rx_ring_buff_size) {
      rx_Tail_index = 0;
  }

  #if defined(FREERTOS_ENV)
  // give back the mutex
  xSemaphoreGive(mutex_for_rx_log);
  #endif

  return val;

  // else flush the buffer and then start transmit again
}

uint8_t get_num_rx_bytes(void)
{
  /// find out where head and tail are located
  uint8_t diff = 0;
    if (rx_Head_index >= rx_Tail_index) {
      diff = rx_Head_index - rx_Tail_index;
    } else {
      diff = rx_ring_buff_size - (rx_Tail_index - rx_Head_index);
    }
  return diff;
}

uint32_t get_rx_data(uint8_t* rx_buff, uint8_t size)
{
  if (!rx_enable_flag)
    return nrf_ERR_INVALID_STATE;
    //// get the mutex acquire

  #if defined(FREERTOS_ENV)
    // wait for mutex
    if (xSemaphoreTake(mutex_for_rx_log, wait_for_uart) != pdTRUE) {
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

void rx_complete_callback(void)
{
  if (!rx_enable_flag)
    return;
  // /// get the amount send by dma
  // uint8_t xfr_byte = uart_get_rx_amount();

  // // no we knnow that how many bytes are transfered , increment head index

  // rx_Head_index += xfr_byte;
  rx_Head_index++;
    // now check if it fits in 8 byte size
    if (rx_Head_index >= rx_ring_buff_size) {
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

void uart_flush_buffer(void)
{

  tx_Head_index = 0;
  tx_Tail_index = 0;
  tx_Tail_equals_Head = 1;

  rx_Head_index = 0;
  rx_Tail_index = 0;
}

void uart_stop_logging(void)
{

  rx_enable_flag = 0;
}

void uart_start_logging(void)
{

  rx_enable_flag = 1;
}

#endif