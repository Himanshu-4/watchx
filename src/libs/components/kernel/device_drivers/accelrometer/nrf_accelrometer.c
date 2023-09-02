#include "nrf_accelromter.h"

#include "adxl345_lib.h"

/// include the gpiote lib
#include "gpiote.h"

/// include the freertos implemenatation
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "nrf_custom_log.h"

///===================================================================
///====================================================================
///============================ define the threshold value used for accel

#define ACCEL_CONFIG_OUTPUT_RANGE adxl_range_16g
#define ACCEL_CONFIG_DATA_RATE adxl_datarate_25

#define ACCEL_TAP_AXES (tap_axis_en_x | tap_axis_en_y | tap_axis_en_z)
#define ACCEL_TAP_TYPE (tap_en_singletap | tap_en_double_tap)
#define ACCEL_TAP_THRESH 20   /// 20 * 62.5mg/lsb
#define ACCEL_TAP_DURATION 10 //// 625usec/lsb

// double_Tap_configurations
#define ACCEL_DOUBLE_TAP_LATENCY 30 /// 1.25ms/lsb
#define ACCEL_DOUBLE_TAP_WINDOW 20  /// 1.25ms/lsb

// activity configurations
#define ACCEL_ACTIVITY_THRESH 10 // 10* 62.5mg/lsb
#define ACCEL_ACTIVITY_AXES (act_x_en | act_y_en | act_z_en)
#define ACCEL_ACTIVITY_OP_TYPE act_dc_oper

// inactivity configuration
#define ACCEL_INACTIVITY_THRESH 20 /// 20 * 62.5mg/lsb
#define ACCEL_INACTIVITY_TIME 5    // 5* 1sec/lsb
#define ACCEL_INACTIVITY_AXES (inact_x_en | inact_y_en | inact_z_en)
#define ACCEL_INACTIVITY_OP_TYPE act_dc_oper

/// freefall configuration
#define ACCEL_FREEFALL_THRESH 30 /// 30 * 62.5mg/lsb
#define ACCEL_FREEFALL_TIME 100  // 5 * 100 ms/lsb

/// fifo mode and sample configuration
#define ACCEL_FIFO_MODE adxl_fifomode_bypass
#define ACCEL_FIFO_SAMPLES 0

///

///===================================================================
///====================================================================
///============================ event Q related

#define ACCEL_EVENT_Q_LENGTH NRF_ACCEL_EVENT_Q_LEN
#define ACCEL_EVENT_Q_ITEM_SIZE NRF_ACCEL_EVENT_Q_ITEM_SIZE

static StaticQueue_t xstatic_q_for_accel_events;
static uint8_t ucQueueStorageArea_for_acceleventq[ACCEL_EVENT_Q_LENGTH * ACCEL_EVENT_Q_ITEM_SIZE];

static volatile xQueueHandle nrf_accel_evtqhandle = NULL;


static volatile uint8_t int_axis = ACCEL_INT_AXIS_NONE;
///=====================================================================
///=====================================================================
/// ============ function handlers

static void gpio_int_handler_for_accel_int1(void);

/// @brief this is the accelrometer event lib
/// @param  void
void nrf_accel_evt_lib_init(void)
{
    nrf_accel_evtqhandle = xQueueCreateStatic(ACCEL_EVENT_Q_LENGTH,
                                              ACCEL_EVENT_Q_ITEM_SIZE, ucQueueStorageArea_for_acceleventq,
                                              &xstatic_q_for_accel_events);
 
    const my_gpio_cfg nrf_buttons_gpio_type =
        {
            .gpio_dir = GPIO_PIN_DIR_INPUT,
            .gpiopull_mode = GPIO_PIN_PULLUP,
            .gpio_sense_type = GPIO_PIN_SENSE_LOW,
            .gpio_input_buff = GPIO_PIN_INPUT_BUFF_CONNECT};

    // config the gpio hardware
    gpio_config(NRF_ADXL_INT1_PIN, &nrf_buttons_gpio_type);

    //// up pin gpiote
    const my_gpiote_cfg accel_pin_int_cfg =
        {
            .mode = GPIO_EVENT_MODE,
            .pinsel = NRF_ADXL_INT1_PIN,
            .outlevel = TASK_OUT_INIT_LEVEL_HIGH,
            .polarity = NRF_ACCEL_INT1_INTERRUPT_POLARITY,
        };
    //// config the gpio int channel
    gpio_config_channel(NRF_ACCEL_INT1_GPIO_CHANNEL, &accel_pin_int_cfg);
    //// config the gpio irq handler
    gpio_add_irq_handler(NRF_ACCEL_INT1_GPIO_CHANNEL, gpio_int_handler_for_accel_int1);
    
    //configure the accelrometer
    const adxl_cfg nrf_accel_cfg =
        {
            .link_autosleep = 1, .low_pwr = 0, .rate = ACCEL_CONFIG_DATA_RATE, .output_range = ACCEL_CONFIG_OUTPUT_RANGE};
    uint32_t ret = adxl_configure(&nrf_accel_cfg);
    NRF_ASSERT(ret);
    // configure the single ,double tap
    const taps_configurations nrf_accel_taps_config =
        {
            .tap_axes = ACCEL_TAP_AXES,
            .tap_type = ACCEL_TAP_TYPE,
            .tap_thresh = ACCEL_TAP_THRESH,
            .tap_durat = ACCEL_TAP_DURATION,
            // double_Tap_configurations
            .double_tap_lattency = ACCEL_DOUBLE_TAP_LATENCY,
            .double_tap_window = ACCEL_DOUBLE_TAP_WINDOW,

        };
    adxl_cfg_taps(&nrf_accel_taps_config);

    //// configure the activity ,inactivity
    const activity_inact_config nrf_Act_inact_config =
        {
            .act.axes = ACCEL_ACTIVITY_AXES,
            .act.thresh_act = ACCEL_ACTIVITY_THRESH,
            .act._ac_dc = ACCEL_ACTIVITY_OP_TYPE,

            .inact.axes = ACCEL_INACTIVITY_AXES,
            .inact.thresh_inact = ACCEL_INACTIVITY_THRESH,
            .inact.time_inact = ACCEL_INACTIVITY_TIME,
            .inact._ac_dc = ACCEL_INACTIVITY_OP_TYPE};
    adxl_cfg_act_inact(&nrf_Act_inact_config);

    /// configure the freefall
    adxl_cfg_freefall(ACCEL_FREEFALL_THRESH, ACCEL_FREEFALL_TIME);

    //// configure the fifo in bypass mode and its interrupt
    adxl_cfg_fifo(ACCEL_FIFO_MODE, ACCEL_FIFO_SAMPLES);

    /// first disable all interupts
    adxl_disable_all_ints();
    // /// configure the interrupts and reading type
    adxl_int_en_dis(accel_int_freefall, ACCEL_INT_ENABLE);
    adxl_int_en_dis(accel_int_inactivity, ACCEL_INT_ENABLE);
    adxl_int_en_dis(accel_int_activity, ACCEL_INT_ENABLE);
    adxl_int_en_dis(accel_int_double_tap, ACCEL_INT_ENABLE);
    adxl_int_en_dis(accel_int_single_tap, ACCEL_INT_ENABLE);

    // adxl_int_en_dis(accel_int_data_ready, ACCEL_INT_ENABLE);
    // //////// enable the int for the gpio
    gpio_int_enable(NRF_ACCEL_INT1_GPIO_CHANNEL);
}

/// @brief this is the accelrometer lib deinit
/// @param  void
void nrf_accel_evt_lib_deinit(void)
{
    adxl_disable_all_ints();

    vQueueDelete(nrf_accel_evtqhandle);
    /// disable the interrupt from gpio channel
    gpio_int_disable(NRF_ACCEL_INT1_GPIO_CHANNEL);
}

/// @brief this is to get event from accelrometer evtq
/// @param void
/// @return event
uint32_t nrf_accel_get_evtq(void)
{
    uint8_t evt = NRF_ACCEL_EVT_NONE;
    if(xQueueReceive(nrf_accel_evtqhandle , &evt , 0) != pdPASS)
    {
        evt =NRF_ACCEL_EVT_NONE;
    }
    return evt;
}

/// @brief this is the get event from accelrometer evtq in wait state
/// @param time
/// @return event
uint32_t nrf_accel_get_evtq_wait(uint32_t wait_time)
{
    uint8_t evt = NRF_ACCEL_EVT_NONE;
    if(xQueueReceive(nrf_accel_evtqhandle , &evt , pdMS_TO_TICKS(wait_time)) != pdPASS)
    {
        evt =NRF_ACCEL_EVT_NONE;
    }
    return evt;
}

/// @brief to get the axis of the event like if x axis triggers the event in activity or taps 
/// @param  void 
/// @return or combination axis of the event (x | Y | Z)
uint32_t nrf_accel_get_event_axis_type(void)
{
    return int_axis;
}

/// @brief put the event in the event q forcefully and wait for the time if the q is full
/// @param evt 
/// @param time 
/// @return succ/failure 
uint32_t nrf_accel_put_in_evtq_wait(uint8_t evt, uint32_t time)
{
    if (evt >= NRF_ACCEL_EVT_MAX_LIMIT)
    {
        return nrf_ERR_INVALID_PARAM;
    }
    if (xQueueSend(nrf_accel_evtqhandle, &evt, pdMS_TO_TICKS(time)) != pdPASS)
    {
        return nrf_ERR_TIMEOUT;
    }
    return nrf_OK;
}

/// @brief put the event in the evtq forcefully aka overwriting if the q is full 
/// @param evt 
/// @return succ/errcode
uint32_t nrf_accel_put_in_evtq(uint8_t evt)
{
    if (evt >= NRF_ACCEL_EVT_MAX_LIMIT)
    {
        return nrf_ERR_INVALID_PARAM;
    }
    xQueueOverwrite(nrf_accel_evtqhandle, &evt);
  
    return nrf_OK;
}

/// @brief this is to reset the event q from the accelrometer
/// @param void
void nrf_accel_reset_evtq(void)
{
    xQueueReset(nrf_accel_evtqhandle);
}

/// @brief this will pause the events 
/// @param  void 
void nrf_accel_pasue_events(void)
{
    //////// enable the int for the gpio
    gpio_int_enable(NRF_ACCEL_INT1_GPIO_CHANNEL);
    xQueueReset(nrf_accel_evtqhandle);
}

/// @brief this will resume the events 
/// @param  void 
void nrf_accel_resume_events(void)
{
    //////// enable the int for the gpio
    gpio_int_enable(NRF_ACCEL_INT1_GPIO_CHANNEL);
    xQueueReset(nrf_accel_evtqhandle);
}

/// @brief read the raw accelration of the accelrometer 
/// @param  void 
void nrf_accel_read_raw(void)
{
    uint8_t databuff[6]  =  {0};
    int16_t data[3] = {0};

    adxl_read_data(databuff,6);
    read_accelration(data, databuff, 6 );

    printf("%f,%f,%f\r\n",data[0] *accel_gain_in_g, 
    data[1]*accel_gain_in_g, data[2]*accel_gain_in_g );

    // adxl_Read_reg();
}


static void gpio_int_handler_for_accel_int1(void)
{
    BaseType_t high_task_woken = pdFALSE;
    uint8_t evt_To_append = NRF_ACCEL_EVT_NONE;

    /// get the interrupt type
    uint8_t int_type =0;
    int_type =  adxl_read_int_type();
   
    int_axis = ACCEL_INT_AXIS_NONE;
    switch (int_type)
    {
    case ADXL_INT_MASK_FREEFALL:
        evt_To_append = NRF_ACCEL_EVT_FREEFALL;
        break;
    case ADXL_INT_MASK_INACTIVITY:
        evt_To_append = NRF_ACCEL_EVT_INACTIVITY;
        break;
    case ADXL_INT_MASK_ACTIVITY :
        evt_To_append = NRF_ACCEL_EVT_ACTIVITY;
        int_axis = adxl_read_int_axis(ACCEL_GETAXIS_FOR_ACTIVITY);
        break;
    case ADXL_INT_MASK_DOUBLE_TAP:
        evt_To_append = NRF_ACCEL_EVT_DOUBLE_TAP;
        int_axis = adxl_read_int_axis(ACCEL_GETAXIS_FOR_TAPS);
        break;

    case ADXL_INT_MASK_SINGLE_TAP:
        evt_To_append = NRF_ACCEL_EVT_SINGLE_TAP;
        int_axis = adxl_read_int_axis(ACCEL_GETAXIS_FOR_TAPS);
        break;

    /// @brief fifo related events 
    case ADXL_INT_MASK_DATA_READY:
        evt_To_append = NRF_ACCEL_EVT_FIFO_DATARDY;
        break;

    case ADXL_INT_MASK_WATERMARK:
        evt_To_append = NRF_ACCEL_EVT_FIFO_WATERMARK;
        break;

    case ADXL_INT_MASK_OVERRUN:
        evt_To_append = NRF_ACCEL_EVT_FIFO_OVERRUN;
        break;

    default:
        evt_To_append = NRF_ACCEL_EVT_FIFO_DATARDY;
        break;
    }
    // if activity or tap then get the axis
    xQueueOverwriteFromISR(nrf_accel_evtqhandle, &evt_To_append, &high_task_woken);

    portYIELD_FROM_ISR(high_task_woken);
}