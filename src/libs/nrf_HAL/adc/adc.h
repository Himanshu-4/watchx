#ifndef ADC_H
#define ADC_H

/// include the system header file
#include "system.h"


#define GET_REF_VOLTAGE(val) ((val)?(0.825f):(0.6f))

#define ADC_BUFF_SIZE_UNIT 4 

#define ADC_MIN_SAMPLING_RATE 85


extern void SAADC_IRQHandler(void);

enum
{
    EVENT_STARTED_FUNC = 0,
    EVENT_END_FUNC,
    EVENT_DONE_FUNC,
    EVENT_RESULTDONE_FUNC,
    EVENT_CALIBRATIONDONE_FUNC,
    EVENT_STOPPED_FUNC
};

/**
 * @name adc_init
 * @brief init the adc module, clear the channel enable register
 */
void adc_init(void);

/****
 * @name adc_deinit
 * @brief deinitalize the adc module
 */
void adc_deinit(void);

/// @brief specify the ADC resolution
enum _ADC_RESOLUTION_
{
    ADC_RESOLUTION_8bit = 0,
    ADC_RESOLUTION_10bit,
    ADC_RESOLUTION_12bit,
    ADC_RESOLUTION_14bit,
};



#define GET_ADC_RESOLUTION(x)  ( (x==ADC_RESOLUTION_8bit)?(8UL):((x==ADC_RESOLUTION_10bit)?\
                                (10UL):((x==ADC_RESOLUTION_12bit)?(12UL):(14UL))))


enum _ADC_OVERSAMPLING_
{
    ADC_NO_OVERSAMPLING= 0,
    ADC_OVER_2x,
    ADC_OVER_4x,
    ADC_OVER_8x,
    ADC_OVER_16x,
    ADC_OVER_32x,
    ADC_OVER_64x,
    ADC_OVER_128x,
    ADC_OVER_256x,
};

/// @brief if you turn on the continous conversion then the ADC internal timer is used for continous mode 
enum
{
    ADC_CONTINOUS_CONVERSION_DISABLED = 0,
    ADC_CONTINOUS_CONVERSION_ENABLED
};

typedef struct __PACKED
{
    uint8_t adc_trigger_mode;
    uint16_t adc_sampling_rate;

} adc_sample_cfg_struct_t;

typedef struct __PACKED
{
    uint8_t adc_resolution;
    uint8_t adc_oversamplnig;
    adc_sample_cfg_struct_t sample_cfg;
    /// @brief the ram buffer for easy DMA

    uint32_t *ram_buff_ptr; // the addr of the buffer 
    uint16_t ram_buff_size; // specify the amount to be transfered in words (4 byte) format

} adc_config_struct_t;

/***
 * @name adc_config
 * @param my_adc_config *
 * @return errorcodes if any
 * @brief configure the adc module accordingly , like the ram buffer allotment, oversampling and adc parameters
 */
uint8_t adc_config(const adc_config_struct_t *);

// define the adc channel number
enum
{
    ADC_CHANNEL_0,
    ADC_CHANNEL_1,
    ADC_CHANNEL_2,
    ADC_CHANNEL_3,
    ADC_CHANNEL_4,
    ADC_CHANNEL_5,
    ADC_CHANNEL_6,
    ADC_CHANNEL_7,
};

// select the analog input channel for the pin
/// @brief // define the analog input chaanels
enum
{
    PIN_NC, // Not connected
    PIN_2_AIN0,
    PIN_3_AIN1,
    PIN_4_AIN2,
    PIN_5_AIN3,
    PIN_28_AIN4,
    PIN_29_AIN5,
    PIN_30_AIN6,
    PIN_31_AIN7,
    VDD
};

/// @brief the ADC gain of the channel
enum
{
    Gain1_6, // 1/6
    Gain1_5, // 1/5
    Gain1_4, // 1/4
    Gain1_3, // 1/3
    Gain1_2, // 1/2
    Gain1,   // 1
    Gain2,   // 2
    Gain4    // 4
};

/// @brief the adc refernce voltage
enum
{
    ADC_REFERNCE_Internal, // Internal reference (0.6 V)
    ADC_REFERNCE_VDD1_4,   // VDD/4 as reference 0.825
};

/// @brief the acquistion time for the ADC to gather 1 sample
enum
{
    ADC_ACQ_3us,  // 3 us
    ADC_ACQ_5us,  // 5 us
    ADC_ACQ_10us, //  10 us
    ADC_ACQ_15us, //  15 us
    ADC_ACQ_20us, //  20 us
    ADC_ACQ_40us, //  40 us

};

/// @brief // select the resistor value of the pin ladder
enum
{
    ADC_RES_Bypass,   // Bypass resistor ladder
    ADC_RES_Pulldown, // Pull-down to GND
    ADC_RES_Pullup,   // pull up resistor connected to pin
    ADC_RES_VDD1_2,   // connect the pin to VDD/2
};


// Enable differential mode
// SE 0 Single ended, PSELN will be ignored, negative input to ADC
// shorted to GND
// Diff 1 Differential
// G RW BURST Enable burst mode
// Disabled 0 Burst mode is disabled (normal operation)
// Enabled 1 Burst mode is enabled. SAADC takes 2^OVERSAMPLE number of
// samples as fast as it can, and sends the average to Data RAM

/// @brief  adc mode whether single ended or differnetial
enum
{
    ADC_MODE_SINGLE_ENDED,
    ADC_MODE_DIFFERENTIAL
};

/// @brief the burst mode
enum
{
    ADC_BURST_MODE_DISABLED,
    ADC_BURST_MODE_ENABLED // this is the oversampling for the channel 2^oversample times and avreage
};

/// @brief  this is the channel configuaration
typedef struct __PACKED
{
    uint8_t channel_gain;
    uint8_t adc_refernce;
    uint8_t adc_diff_mode;
    uint8_t adc_resolution;

} adc_channel_prop_config_struct_t;

typedef struct __PACKED
{
    uint8_t pos_resistor_sel;
    uint8_t neg_resistor_sel;
    uint8_t pos_sel_pin;
    uint8_t neg_sel_pin;
    uint8_t adc_acq_time;
    uint8_t adc_burst_mode; // this is the oversampling control
    const adc_channel_prop_config_struct_t *chnl_config;

} adc_channel_config_struct_t;

/*****
 * @name adc_channel_config
 * @param[1] channel number
 * @param[2] my_adc_channel_config
 * @brief configure a adc channel with single ended
 */
void adc_channel_config(uint8_t, const adc_channel_config_struct_t *);

enum
{
    ADC_TASK_START,      // tart the ADC and prepare the result buffer in RAM
    ADC_TASK_SAMPLE,     // Take one ADC sample, if scan is enabled all channels are sampled
    ADC_TASK_STOP,       // Stop the ADC and terminate any on-going conversion
    ADC_TASK_CALIBRATION // Starts offset auto-calibration
};

enum
{
    ADC_EVENT_STARTED,       // The ADC has started
    ADC_EVENT_END,           // The ADC has filled up the Result buffer
    ADC_EVENT_DONE,          // a conversion is completed for the task_Sample
    ADC_EVENT_RESULTDONE,    // result is ready to xfr to ram
    ADC_EVENT_CALIBRATEDONE, //  the clibraion is completed
    ADC_EVENT_STOPPED,       // the adc is stoped
};

/*****
 * @name adc_int_enable_event
 * @param the event number
 * @brief the event on which you want to be interrupted
 */
void adc_int_enable_event(uint8_t);

/*****
 * @name adc_int_disable_event
 * @param the event number
 * @brief the event on which you want to be interrupted
 */
void adc_int_disable_event(uint8_t);

/*****
 * @name adc_get_int_src
 * @param void
 * @brief get the interrupt source like the
 */
void adc_get_int_src(void);


/****
 * @name adc_add_irq_handler
 * @param[1] the event type
 * @param[2] the callback function
 * @brief add the irq handler for the callback function
 */
void adc_add_irq_handler(uint8_t, void (*)(void));

/****
 * @name adc_remove_irq_handler
 * @param event_type
 * @brief remove the callback function from the static
 *
 */
void adc_remove_irq_handler(uint8_t);

enum
{
    CHANNEL_HIGH_LIMIT = 1,
    CHANNEL_LOW_LIMIT
};

/****
 * @name adc_add_channel_irq_handler
 * @param[1] channel_number
 * @param[2] the high or low limit
 * @param[3] the callback function
 * @brief add the irq handler for the callback function
 */
void adc_add_channel_irq_handler(uint8_t, uint8_t, void ( *)(void));

/****
 * @name adc_add_channel_irq_handler
 * @param[1] event_Type
 * @param[2] the high or low limit
 * @brief add the irq handler for the callback function
 */
void adc_remove_channel_irq_handler(uint8_t, uint8_t);

/*****
 * @name adc_config_channel_int
 * @param[1] channel number
 * @param[2] the upper limit
 * @param[3] the lower limit
 */
void adc_config_channel_int(uint8_t, int, int);

/***
 * @name adc_int_enable
 * @param[1] channel_number
 * @param[2] the high or low event
 * @brief enable the interrupt
 */
void adc_int_enable(uint8_t, uint8_t);

/****
 * @name adc_int_disable
 * @param[1] channel_number
 * @param[2] the high or low event
 * @brief  disable the interrupt on the channel number
 */
void adc_int_disable(uint8_t, uint8_t);

/***
 * @name adc_continous_conversion
 * @param channel number
 * @brief only make a continous conversion
 */
void adc_continous_conversion(uint8_t);

/***
 * @name adc_enable_isr
 * @brief enable the  NVIC interrupt service on the adc
 * nullify the irq handlers
 * */
void adc_enable_isr(void);

/***
 * @name adc_disable_isr
 * @brief disable the NVIC interurpt
 */
void adc_disable_isr(void);


/****
 * @name adc_start
 * @brief start the adc .
 */
void adc_start(void);

/***
 * @name adc_Stop
 * @brief stop the adc
 */
void adc_stop(void);

/****
 * @name adc_take_asample
 * @brief take one adc sample  , store it in ram and stop the conversion,
 * samples conversion trigger by software
 */
void adc_take_asample(void);

/*****
 * @name adc_start_calibration
 * @brief start the adc claibration process
 */
void adc_start_calibration(void);

/*****
 * @name is_adc_calibrated
 * @return true if the adc calibration is done, false if not , must call the function adc_start_calibration
 * @brief check that if the calibration process completed or not
 */
bool is_adc_calibrated(void);

enum
{
    ADC_MODULE_READY, // the Adc module is ready
    ADC_MODULE_BUSY   // the adc module is busy in some conversion
};
/****
 * @name adc_check_busy
 * @return true if Adc is busy or 0 if in idle state
 * @brief check for the ADC to busy in conversion or not
 */
bool adc_check_busy(void);

/****
 * @name adc_get_voltage
 * @param[1] the digital value from the ADC
 * @param[2] adc_channel_char_conig structure
 * @return the raw voltage from the digital value
 * @brief the adc characteristics are stored in static variable used by this function
 * and is init at the adc_config function
 */
float adc_get_voltage(uint16_t, const adc_channel_prop_config_struct_t *);
////////////``````````````````````````````````````````````````````````````````````````````````````
///````````````````````````````````````````````````````````````````````````````````````````````````````
// Oversampling configuration. OVERSAMPLE should not be combined with SCAN. The RESOLUTION is
// applied before averaging, thus for high OVERSAMPLE a higher RESOLUTION should be used.
//````````````````````````````````````````````````````````````````````````````````````````````````````
//`````````````````````````````````````````````````````````````````````````````````````````````````````
///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/*****
 * @name adc_get_task_addr
 * @param task_type defined in enum
 * @brief to get the task addresss , can be used with PPI
 */
uint32_t *adc_get_task_addr(uint8_t);

/****
 * @name adc_get_event_addr
 * @param event_type // defined in enum
 * @brief can be used to get the adc event address
 */
uint32_t *adc_get_event_addr(uint8_t);

/*****
 * @name adc_get_event_high_limit_addr
 * @param channel_number
 * @brief used to get the high limit address of the ADC
 */
uint32_t *adc_get_event_high_limit_addr(uint8_t);

/***
 * @name adc_get_event_low_limit_addr
 * @param channel_number
 * @brief to get the lower limit address of the ADC
 */
uint32_t *adc_get_event_low_limit_addr(uint8_t);

///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
#endif