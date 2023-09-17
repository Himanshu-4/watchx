#ifndef _NRF_RTC_TIMER_
#define _NRF_RTC_TIMER_

#include "system.h"

/// we can only use rtc 1 and 2 rtc 0 is used by softdevice
#define NRF_RTC_TIMER_1 1
#define NRF_RTC_TIMER_2 2

/// @brief interrupt mask for differnt events
enum _RTC_TIMER_INTS_MASK_
{
    RTC_TIM_EVT_MASK_TICK = 0,
    RTC_TIM_EVT_MASK_OVRFLW,
    RTC_TIM_EVT_MASK_COMPARE0 = 16,
    RTC_TIM_EVT_MASK_COMPARE1,
    RTC_TIM_EVT_MASK_COMPARE2,
    RTC_TIM_EVT_MASK_COMPARE3,

};

/// @brief these are the capture compare register values for the rtc timers
enum _RTC_TIMER_COMPARE_REG_VALUES_
{
    RTC_TIMER_COMPARE_0,
    RTC_TIMER_COMPARE_1,
    RTC_TIMER_COMPARE_2,
    RTC_TIMER_COMPARE_3,
    RTC_TIMER_COMPARE_MAX
};

////================= function declartion

/// @brief configure the rtc timer prescaler
/// @param timer_no
/// @param prescaler
void rtc_Timer_Config(uint8_t timer_no, uint16_t prescaler);

/// @brief start the rtc timer
/// @param timer_no
void rtc_Timer_start(uint8_t timer_no);

/// @brief stop the rtc timer
/// @param timer_no
void rtc_Timer_stop(uint8_t timer_no);

/// @brief clear the rtc timer counter register
/// @param timer_no
void rtc_Timer_clear(uint8_t timer_no);

/// @brief add an irq handler to the rtc timer isr and enable the nvic isr
/// @param timer_no
/// @param func
void rtc_Timer_add_irq_handler(uint8_t timer_no, void (*func)(void));

/// @brief remove the irq handler from the isr and disable the nvic isr
/// @param timer_no
void rtc_Timer_remove_irq_handler(uint8_t timer_no);

/// @brief enable the interrupt from  particular event
/// @param timer_no
/// @param int_src
void rtc_Timer_int_enable(uint8_t timer_no, uint8_t int_src);

/// @brief disable the event generation from the particular event
/// @param timer_no
/// @param int_src
void rtc_Timer_int_disable(uint8_t timer_no, uint8_t int_src);

/// @brief enable the event generation from the particular event -> remeber enable int and enable event are differnt things
/// @param timer_no
/// @param event_Type
/// @note you can enable the int generation on that event and also disable the event genration from same event
void rtc_Timer_event_enable(uint8_t timer_no, uint8_t event_Type);

/// @brief disable the event genration from the event
/// @param timer_no
/// @param event_Type
void rtc_Timer_event_disable(uint8_t timer_no, uint8_t event_Type);

/// @brief set the compare register of the particular timer
/// @param timer_no
/// @param compare_reg
/// @param value
void rtc_Timer_set_Comapre_reg(uint8_t timer_no, uint8_t compare_reg, uint32_t value);

/// @brief get the timer counter value
/// @param timer_no
/// @return counter value
uint32_t rtc_Timer_get_counter_value(uint8_t timer_no);

#endif