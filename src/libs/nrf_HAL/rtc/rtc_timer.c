#include "RTC_timer.h"

static void (*rtc_timer_callback_func[2])(void) = {NULL};

extern void RTC1_IRQHandler(void);
extern void RTC2_IRQHandler(void);

void RTC1_IRQHandler(void)
{
    if (rtc_timer_callback_func[0] != NULL)
    {
        rtc_timer_callback_func[0]();
    }
}

void RTC2_IRQHandler(void)
{
    if (rtc_timer_callback_func[1] != NULL)
    {
        rtc_timer_callback_func[1]();
    }
}

//===================================================================================================4
///========================================== static functions here =================================

static void rtc_timer_isr_enbale(uint8_t timer_no)
{
    switch (timer_no)
    {
    case NRF_RTC_TIMER_1:
        NVIC_SetPriority(RTC1_IRQn, RTC_TIMER_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(RTC1_IRQn);
        break;

    case NRF_RTC_TIMER_2:
        NVIC_SetPriority(RTC2_IRQn, RTC_TIMER_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(RTC2_IRQn);
        break;

    default:
        break;
    }
}

static void rtc_timer_isr_disable(uint8_t timer_no)
{
    switch (timer_no)
    {

    case NRF_RTC_TIMER_1:
        NVIC_DisableIRQ(RTC1_IRQn);
        break;

    case NRF_RTC_TIMER_2:
        NVIC_DisableIRQ(RTC2_IRQn);
        break;

    default:
        break;
    }
}

static uint32_t rtc_timer_get_addr_from_no(uint8_t number)
{
    uint32_t addr = 0;
    switch (number)
    {
    case NRF_RTC_TIMER_1:
        addr = NRF_RTC1_BASE;
        break;
    case NRF_RTC_TIMER_2:
        addr = NRF_RTC2_BASE;
        break;
    default:
        break;
    }
    return addr;
}

//===================================================================================================4
///========================================== static functions ends here =================================

//===================================================================================================4
///==========================================  functions definations here =================================

/// @brief configure the rtc timer prescaler
/// @param timer_no
/// @param prescaler
void rtc_Timer_Config(uint8_t timer_no, uint16_t prescaler)
{
    NRF_RTC_Type *timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);

    /// stop the timer first
    timer_inst->TASKS_STOP = 1U;
    /// clear all interrupts
    timer_inst->INTENCLR = U32_MAX;
    timer_inst->EVTENCLR = U32_MAX;
    timer_inst->PRESCALER = prescaler;
}

/// @brief start the rtc timer
/// @param timer_no
void rtc_Timer_start(uint8_t timer_no)
{
    NRF_RTC_Type *timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);
    timer_inst->TASKS_START = 1U;
}

/// @brief stop the rtc timer
/// @param timer_no
void rtc_Timer_stop(uint8_t timer_no)
{
    NRF_RTC_Type *timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);
    timer_inst->TASKS_STOP = 1U;
}

/// @brief clear the rtc timer counter register
/// @param timer_no
void rtc_Timer_clear(uint8_t timer_no)
{
    NRF_RTC_Type *timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);

    timer_inst->TASKS_CLEAR = 1U;
}

/// @brief add an irq handler to the rtc timer isr and enable the nvic isr
/// @param timer_no
/// @param func
void rtc_Timer_add_irq_handler(uint8_t timer_no, void (*func)(void))
{
    /// turn on the isr
    rtc_timer_callback_func[timer_no - 1] = func;
    rtc_timer_isr_enbale(timer_no);
}

/// @brief remove the irq handler from the isr and disable the nvic isr
/// @param timer_no
void rtc_Timer_remove_irq_handler(uint8_t timer_no)
{
    /// stop the isr
    rtc_timer_isr_disable(timer_no);
    rtc_timer_callback_func[timer_no - 1] = NULL;
}

/// @brief enable the interrupt from  particular event
/// @param timer_no
/// @param int_src
void rtc_Timer_int_enable(uint8_t timer_no, uint8_t int_src)
{
    NRF_RTC_Type *timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);
    timer_inst->INTENSET = _BV(int_src);
}

/// @brief disable the event generation from the particular event
/// @param timer_no
/// @param int_src
void rtc_Timer_int_disable(uint8_t timer_no, uint8_t int_src)
{
    NRF_RTC_Type *timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);
    timer_inst->INTENCLR = _BV(int_src);
}

/// @brief enable the event generation from the particular event -> remeber enable int and enable event are differnt things
/// @param timer_no
/// @param event_Type
/// @note you can enable the int generation on that event and also disable the event genration from same event
void rtc_Timer_event_enable(uint8_t timer_no, uint8_t event_Type)
{
    NRF_RTC_Type *timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);
    timer_inst->EVTENSET = _BV(event_Type);
}

/// @brief disable the event genration from the event
/// @param timer_no
/// @param event_Type
void rtc_Timer_event_disable(uint8_t timer_no, uint8_t event_Type)
{
    NRF_RTC_Type *timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);
    timer_inst->EVTENCLR = _BV(event_Type);
}

/// @brief set the compare register of the particular timer
/// @param timer_no
/// @param compare_reg
/// @param value
void rtc_Timer_set_Comapre_reg(uint8_t timer_no, uint8_t compare_reg, uint32_t value)
{
    NRF_RTC_Type *timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);

    if (compare_reg >= RTC_TIMER_COMPARE_MAX)
        return;
    /// first stop the timer
    timer_inst->TASKS_STOP = 1U;

    timer_inst->CC[compare_reg] = value;
}

/// @brief get the timer counter value
/// @param timer_no
/// @return counter value
uint32_t rtc_Timer_get_counter_value(uint8_t timer_no)
{
    NRF_RTC_Type *timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);
    uint32_t counter_value = timer_inst->COUNTER;
    return counter_value;
}


/// @brief get the rtc event and clear it 
/// @param timer_no 
/// @return event value with proper bit shifted 
uint32_t rtc_Timer_get_event_src_and_clear(uint8_t timer_no)
{
    //// check every event and clear the event 
    NRF_RTC_Type *timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);
    uint32_t event =0;

    if(timer_inst->EVENTS_TICK)
    {
        event |= RTC_TIM_EVT_TICK;
        /// clear the event 
        timer_inst->EVENTS_TICK =0;
    }
    if(timer_inst->EVENTS_OVRFLW)
    {
        event |= RTC_TIM_EVT_OVRFLW;
        //// clear the event 
        timer_inst->EVENTS_OVRFLW =0;
    }
    for (uint8_t i = 0; i < RTC_TIMER_COMPARE_MAX; i++)
    {
        if(timer_inst->EVENTS_COMPARE[i])
        {
            event |= RTC_TIM_EVT_COMPARE(i);
            /// clear the event 
            timer_inst->EVENTS_COMPARE[i] = 0;
        }   
    }
    
}