#include "adc.h"

// there are 6 events functions for the ADC core
static void (*adc_event_function[6])(void) = {NULL};

/// @brief  there are 16 channels interrupts for the ADC_channel
// 8 channels interrupt for the HIGH limit event
static void (*adc_channel_high_limit_func[8])(void) = {NULL};

// 8 channels interrupt for the HIGH limit event
static void (*adc_channel_low_limit_func[8])(void) = {NULL};

/// making the force inline function 
/////////// a static function to give back the gain based on the enum value
__STATIC_FORCEINLINE float get_gain(uint8_t gain_sel)
{
    float val = 0;
    switch (gain_sel)
    {
    case Gain1_6:
        val = 0.16666667f;
        break;

    case Gain1_5:
        val = 0.2f;
        break;

    case Gain1_4:
        val = 0.25f;
        break;

    case Gain1_3:
        val = 0.333333333f;
        break;

    case Gain1_2:
        val = 0.5f;
        break;

    case Gain1:
        val = 1.0f;
        break;

    case Gain2:
        val = 2.0f;
        break;

    case Gain4:
        val = 4.0f;
        break;
    }
    return val;
}



__STATIC_FORCEINLINE uint32_t get_adc_max_counter(uint8_t adc_res, uint8_t diff_mode)
{
    uint32_t ret_val =1;
    switch (adc_res)
    {
        case ADC_RESOLUTION_8bit:
        ret_val = (diff_mode)?(128):(256);
        break;

        case ADC_RESOLUTION_10bit:
        ret_val = (diff_mode)?(512):(1024);
        break;

        case ADC_RESOLUTION_12bit:
        ret_val = (diff_mode)?(2048):(4096);
        break;

        case ADC_RESOLUTION_14bit:
        ret_val = (diff_mode)?(8192):(16384);
        break;
    }
    return ret_val;
}

/// ////////////////////////////////////////////////////////////////////////////////
////////////  we can also uses a dynamic queue to store the function address and execute the handler at runtime but takes a lot of time
////////////// for optimisation
///
void SAADC_IRQHandler(void)
{
    // execute the event handler function
    if (NRF_SAADC->EVENTS_STARTED)
    {
        // clear the interrrupts
        NRF_SAADC->EVENTS_STARTED = 0;

        if (adc_event_function[EVENT_STARTED_FUNC] != NULL)
        {
            adc_event_function[EVENT_STARTED_FUNC]();
        }
    }
    else if (NRF_SAADC->EVENTS_END)
    {
        // clear the interupt
        NRF_SAADC->EVENTS_END = 0;

        if (adc_event_function[EVENT_END_FUNC] != NULL)
        {
            adc_event_function[EVENT_END_FUNC]();
        }
    }
    else if (NRF_SAADC->EVENTS_DONE)
    {

        // clear the interrupt
        NRF_SAADC->EVENTS_DONE = 0;
        if (adc_event_function[EVENT_DONE_FUNC] != NULL)
        {
            adc_event_function[EVENT_DONE_FUNC]();
        }
    }
    else if (NRF_SAADC->EVENTS_RESULTDONE)
    {

        // clear the interurpt
        NRF_SAADC->EVENTS_RESULTDONE = 0;
        if (adc_event_function[EVENT_STARTED_FUNC] != NULL)
        {
            adc_event_function[EVENT_STARTED_FUNC]();
        }
    }
    /// not required here
    // else if (NRF_SAADC->EVENTS_CALIBRATEDONE)
    // {
    //     // clear the interrupt
    //     NRF_SAADC->EVENTS_CALIBRATEDONE = 0;
    //     if (adc_event_function[EVENT_CALIBRATIONDONE_FUNC] != NULL)
    //     {
    //         adc_event_function[EVENT_CALIBRATIONDONE_FUNC]();
    //     }
    // }
    else if (NRF_SAADC->EVENTS_STOPPED)
    {
        // clear the interrupt
        NRF_SAADC->EVENTS_STOPPED = 0;
        if (adc_event_function[EVENT_STOPPED_FUNC] != NULL)
        {
            adc_event_function[EVENT_STOPPED_FUNC]();
        }
    }

#if defined(ENABLE_ADC_LIMIT_H_INTERUPT)
    // execute the channel_high limit event
    for (uint8_t i = 0; i < 8; i++)
    {
        if (NRF_SAADC->EVENTS_CH[i].LIMITH)
        {
            // clear the interrupt
            NRF_SAADC->EVENTS_CH[i].LIMITH = 0;
            // check if any function is assigned or not
            if (adc_channel_high_limit_func[i] != NULL)
            {
                adc_channel_high_limit_func[i]();
            }
        }
    }

#endif

#if defined(ENABLE_ADC_LIMIT_L_INTERRUPT)

    // execute the channel low limit function
    for (uint8_t i = 0; i < 8; i++)
    {
        if (NRF_SAADC->EVENTS_CH[i].LIMITL)
        {
            // clear the interrupt
            NRF_SAADC->EVENTS_CH[i].LIMITL = 0;
            // check if any function is assigned or not
            if (adc_channel_low_limit_func[i] != NULL)
            {
                adc_channel_low_limit_func[i]();
            }
        }
    }

#endif
}

/// @brief init the ADC module
void adc_init(void)
{
    // clear the int enable registers
    NRF_SAADC->INTENCLR = U32_MAX;

    //////// Nullify the irq handlers

    // there are 6 events functions for the ADC core
    for (uint8_t i = 0; i < 6; i++)
    {
        adc_event_function[i] = NULL;
    }

    /// @brief  there are 16 channels interrupts for the ADC_channel
    // 8 channels interrupt for the HIGH limit event
    for (uint8_t i = 0; i < 8; i++)
    {
        adc_channel_high_limit_func[i] = NULL;
    }

    // 8 channels interrupt for the HIGH limit event
    for (uint8_t i = 0; i < 8; i++)
    {
        adc_channel_low_limit_func[i] = NULL;
    }

    // enable the ADC module this will acquire the GPIO pins of the ADC
    NRF_SAADC->ENABLE = 1UL;
}

/// @brief disable and deinit the ADC module
///
void adc_deinit(void)
{
    // disable the ADC module this will release  the GPIO pins of the ADC
    NRF_SAADC->ENABLE = 0;

    // clear the int enable registers
    NRF_SAADC->INTENCLR = U32_MAX;
}

/// @brief config the ADC module according to the configuration
uint8_t adc_config(const my_adc_config *adc_cfg)
{
    if ((adc_cfg->sample_cfg.adc_sampling_rate < 80) || (adc_cfg->sample_cfg.adc_sampling_rate > 2047))
        return nrf_ERR_INVALID_PARAM;

    // write the resolution register
    NRF_SAADC->RESOLUTION = adc_cfg->adc_resolution;

    // write the oversampling register
    NRF_SAADC->OVERSAMPLE = adc_cfg->adc_oversamplnig;

    // write the sample cfg
    NRF_SAADC->SAMPLERATE = (adc_cfg->sample_cfg.adc_trigger_mode << SAADC_SAMPLERATE_MODE_Pos) | (adc_cfg->sample_cfg.adc_sampling_rate);
    // write the DMA buffer address
    NRF_SAADC->RESULT.PTR = (uint32_t)adc_cfg->ram_buff_ptr;
    /// the maxcounter should be in words
    NRF_SAADC->RESULT.MAXCNT = ((adc_cfg->ram_buff_size) % ADC_BUFF_SIZE_UNIT) + 1;

    return nrf_OK;
}

/// @brief adc channel configuration
void adc_channel_config(uint8_t channel_no, const my_adc_channel_config *adc_chn_cfg)
{
    // configure the ADC pins
    NRF_SAADC->CH[channel_no].PSELP = adc_chn_cfg->pos_sel_pin;
    NRF_SAADC->CH[channel_no].PSELN = adc_chn_cfg->neg_sel_pin;

    NRF_SAADC->CH[channel_no].CONFIG = (adc_chn_cfg->adc_burst_mode << SAADC_CH_CONFIG_BURST_Pos) |
                                       (adc_chn_cfg->chnl_config->adc_diff_mode << SAADC_CH_CONFIG_MODE_Pos) |
                                       (adc_chn_cfg->adc_acq_time << SAADC_CH_CONFIG_TACQ_Pos) |
                                       (adc_chn_cfg->chnl_config->adc_refernce << SAADC_CH_CONFIG_REFSEL_Pos) |
                                       (adc_chn_cfg->chnl_config->channel_gain << SAADC_CH_CONFIG_GAIN_Pos) |
                                       (adc_chn_cfg->neg_resistor_sel << SAADC_CH_CONFIG_RESN_Pos) |
                                       (adc_chn_cfg->pos_resistor_sel << SAADC_CH_CONFIG_RESP_Pos);
}

//// @brief adc_add_irq_handler is used to add an irq handler to the event
void adc_add_irq_handler(uint8_t event_type, void (*func)(void))
{
    adc_event_function[event_type] = func;
}

/// @brief to remove the event irq handler , must also disable the interupt
/// @param event_type
void adc_remove_irq_handler(uint8_t event_type)
{
    adc_event_function[event_type] = NULL;
}

/// @brief event interrupt enable
/// @param event_type
void adc_int_enable_event(uint8_t event_type)
{
    NRF_SAADC->INTENSET = _BV(event_type);
}

/// @brief disable the interrupt
/// @param  event_Type
void adc_int_disable_event(uint8_t event_type)
{
    NRF_SAADC->INTENCLR = _BV(event_type);
}

/// @brief to configure the interrupt for the limit channel
/// @param channel_no
/// @param high_limit
/// @param low_limit
void adc_config_channel_int(uint8_t channel_no, int high_limit, int low_limit)
{
    NRF_SAADC->CH[channel_no].LIMIT = (uint32_t)((high_limit & U16_MAX) << 16) | (low_limit & U16_MAX);
}

/// @brief to enable the interrupt for the high or low limit
void adc_int_enable(uint8_t channel_no, uint8_t high_or_low)
{
    uint8_t mask_val = 0;
    if (high_or_low == CHANNEL_HIGH_LIMIT)
    {
        mask_val = (2 * channel_no) + 7;
    }
    else
    {
        mask_val = (2 * channel_no) + 8;
    }

    NRF_SAADC->INTENSET = _BV(mask_val);
}

/// @brief to disable the interrupt in int register
/// @param channel_no
/// @param high_or_low
void adc_int_disable(uint8_t channel_no, uint8_t high_or_low)
{
    uint8_t mask_val = 0;
    if (high_or_low == CHANNEL_HIGH_LIMIT)
    {
        mask_val = (2 * channel_no) + 7;
    }
    else
    {
        mask_val = (2 * channel_no) + 8;
    }

    NRF_SAADC->INTENCLR = _BV(mask_val);
}

void adc_add_channel_irq_handler(uint8_t chn_no, uint8_t high_or_low, void (*func)(void))
{
    if (high_or_low == CHANNEL_HIGH_LIMIT)
    {
        adc_channel_high_limit_func[chn_no] = func;
    }
    else // low event
    {
        adc_channel_low_limit_func[chn_no] = func;
    }
}

void adc_remove_channel_irq_handler(uint8_t chn_no, uint8_t high_or_low)
{
    if (high_or_low == CHANNEL_HIGH_LIMIT)
    {
        adc_channel_high_limit_func[chn_no] = NULL;
    }
    else // low event
    {
        adc_channel_low_limit_func[chn_no] = NULL;
    }
}

// deprecated
// void adc_get_int_src(void);

/// @brief enable the NVIC side interrupt
void adc_enable_isr(void)
{
    NVIC_SetPriority(SAADC_IRQn, GPIOTE_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(SAADC_IRQn);
}

void adc_disable_isr(void)
{
    NVIC_DisableIRQ(SAADC_IRQn);
}

void adc_continous_conversion(uint8_t);

void adc_start(void)
{
    NRF_SAADC->TASKS_START = 1UL;
}
void adc_stop(void)
{
    NRF_SAADC->TASKS_STOP = 1UL;
}

FORCE_INLINE void adc_take_asample(void)
{
    NRF_SAADC->TASKS_SAMPLE = 1UL;
}
void adc_start_calibration(void)
{
    NRF_SAADC->TASKS_CALIBRATEOFFSET = 1UL;
}
FORCE_INLINE bool is_adc_calibrated(void)
{
    if (NRF_SAADC->EVENTS_CALIBRATEDONE)
    {
        NRF_SAADC->EVENTS_CALIBRATEDONE = 0;
        return 1;
    }
    return 0;
    // check for the event
    // when the calibration start func triggered then wait for te interrupt and then exit the function

    // __WFI(); can be used outsied the function
}
FORCE_INLINE bool adc_check_busy(void)
{
    return NRF_SAADC->STATUS;
}

FORCE_INLINE float adc_get_voltage(uint16_t adc_digi_out, const adc_channel_prop_config *chn_cfg)
{
    // the adc_digi_out is a 2's complement , we have to remove the complement
    int val = 0;
    if (adc_digi_out & 0x8000) // the value is -ve
    {
        val = ~(adc_digi_out - 1);
        val = -val;
    }
    else
    {
        val = adc_digi_out;
    }
    float res = (val * GET_REF_VOLTAGE(chn_cfg->adc_refernce)) / (get_gain(chn_cfg->channel_gain) * get_adc_max_counter(chn_cfg->adc_resolution, chn_cfg->adc_diff_mode));
    return res;
}

///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

uint32_t *adc_get_task_addr(uint8_t task_type)
{
    uint32_t *addr = NULL;
    switch (task_type)
    {
    case ADC_TASK_START:
        addr = (uint32_t *)&NRF_SAADC->TASKS_START;
        break;

    case ADC_TASK_SAMPLE:
        addr = (uint32_t *)&NRF_SAADC->TASKS_SAMPLE;
        break;

    case ADC_TASK_STOP:
        addr = (uint32_t *)&NRF_SAADC->TASKS_STOP;
        break;

    case ADC_TASK_CALIBRATION:
        addr = (uint32_t *)&NRF_SAADC->TASKS_CALIBRATEOFFSET;
        break;
    }
    return addr;
}

uint32_t *adc_get_event_addr(uint8_t event_type)
{
    uint32_t *addr = NULL;
    switch (event_type)
    {
    case ADC_EVENT_STARTED:
        addr = (uint32_t *)&NRF_SAADC->EVENTS_STARTED;
        break;
    case ADC_EVENT_END:
        addr = (uint32_t *)&NRF_SAADC->EVENTS_END;
        break;
    case ADC_EVENT_DONE:
        addr = (uint32_t *)&NRF_SAADC->EVENTS_DONE;
        break;
    case ADC_EVENT_RESULTDONE:
        addr = (uint32_t *)&NRF_SAADC->EVENTS_RESULTDONE;
        break;
    case ADC_EVENT_CALIBRATEDONE:
        addr = (uint32_t *)&NRF_SAADC->EVENTS_CALIBRATEDONE;
        break;
    case ADC_EVENT_STOPPED:
        addr = (uint32_t *)&NRF_SAADC->EVENTS_STOPPED;
        break;
    }
    return addr;
}

uint32_t *adc_get_event_high_limit_addr(uint8_t chn_no)
{
    return (uint32_t *)&NRF_SAADC->EVENTS_CH[chn_no].LIMITH;
}

uint32_t *adc_get_event_low_limit_addr(uint8_t chn_no)
{
    return (uint32_t *)&NRF_SAADC->EVENTS_CH[chn_no].LIMITL;
}

///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
