#include "nrf_time.h"

/////////////////// used to get the least count of the timer
__STATIC_INLINE float TIMER_LEAST_COUNT(uint8_t timer_freq)
{
    float val = 0;
    switch (timer_freq)
    {
    case TIMER_FREQ_16MHz:
        val = 0.0000625f;
            break;
    case TIMER_FREQ_8MHz:
        val = 0.000125f;
        break;
    case TIMER_FREQ_4MHz:
        val = 0.00025f;
        break;
    case TIMER_FREQ_2MHz:
        val = 0.0005f;
        break;
    case TIMER_FREQ_1MHz:
        val = 0.001f;
        break;
    case TIMER_FREQ_500kHz: ///< Timer frequency 500 kHz.
        val = 0.002f;
        break;
    case TIMER_FREQ_250kHz: ///< Timer frequency 250 kHz.
        val = 0.004f;
        break;
    case TIMER_FREQ_125kHz: ///< Timer frequency 125 kHz.
        val = 0.008f;
        break;
    case TIMER_FREQ_62_5KHz: ///< Timer frequency 62.5 KHz.
        val = 0.016f;
        break;
    case TIMER_FREQ_31_25KHz: ///< Timer frequency 31.25 KHz.
        val = 0.032f;
        break;
    case TIMER_FREQ_15_63KHz: /// < timer frequency 15.63 KHZ
        val = 0.064f;
        break;
    case TIMER_FREQ_7_81KHz: //// timer frequency 7.81khz
        val = 0.128f;
        break;
    case TIMER_FREQ_3_9KHZ: //// timer_Frequency 3.9khz
        val = 0.256f;
        break;
    case TIMER_FREQ_1_95KHz: /// timer frequency 1.95khz
        val = 0.512f;
        break;
    case TIMER_FREQ_976HZ: //// 976 hz
        val = 1.0246f;
        break;
    case TIMER_FREQ_488Hz: //// 488 hz
        val = 2.05f;
        break;
    default:
        val = 1;
         break;
    }
    return val;
}
/****
 * @name time_init
 * @note used to initialize timer 1 and used its cc1 register for getting the time
 */
void time_init(void)
{
    // init the timer module
    timer_module_init(TIMER_TIME_MODULE);

    // config the timer
    const my_timer_cfg timer_cfg =
        {
            .mode = TIMER_MODE,
            .timer_width = TIMER_32_BIT_WIDTH,
            .timer_tick_rate = TIMER_TICK_FREQ};

    timer_config(TIMER_TIME_MODULE, &timer_cfg);

    timer_set_auto_reload_val(TIMER_TIME_MODULE, CAPTURE_COMPARE_1, U32_MAX);

    timer_start(TIMER_TIME_MODULE);
}

void time_deinit(void)
{
    timer_stop(TIMER_TIME_MODULE);
    timer_module_deint(TIMER_TIME_MODULE);
}

/***
 * @name millis
 * @note used to get the millisseconds from the start of the timer
 */
__FORCE_INLINE uint32_t millis(void)
{
    return timer_get_counter_val(TIMER_TIME_MODULE) * TIMER_LEAST_COUNT(TIMER_TICK_FREQ);
}

/**
 * @name micros
 * @note used to get the microseconds from the start of the timer
 */
__FORCE_INLINE uint32_t micros(void)
{
    return timer_get_counter_val(TIMER_TIME_MODULE);
}

/***
 * @name seconds
 * @note used to get the seconds from the start of the timer
 */
__FORCE_INLINE uint32_t seconds(void)
{
    return (timer_get_counter_val(TIMER_TIME_MODULE) * (TIMER_LEAST_COUNT(TIMER_TICK_FREQ) * 1000)) + 1;
}

/***
 * @name minute
 * @note used to get minute from the start of the timer
 */
__FORCE_INLINE uint32_t minute(void)
{

    return (timer_get_counter_val(TIMER_TIME_MODULE) * TIMER_LEAST_COUNT(TIMER_TICK_FREQ) * 6000) + 1;
}

__FORCE_INLINE void delay_milisec(uint16_t delay_time)
{
    uint32_t start_val = millis();

    uint32_t end_val = millis();
    while ((end_val - start_val) < (delay_time))
    {
        end_val = millis();
        __NOP();
    }
}

__FORCE_INLINE void delay_microsec(uint16_t delay_time)
{

    uint32_t start_val = timer_get_counter_val(TIMER_TIME_MODULE);

    uint32_t end_val = timer_get_counter_val(TIMER_TIME_MODULE);
    while ((end_val - start_val) < (delay_time))
    {
        end_val = timer_get_counter_val(TIMER_TIME_MODULE);
    }
}