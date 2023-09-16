#include "RTC_timer.h"



static void (*rtc_timer_callback_func[2])(void) = {NULL};

extern void RTC1_IRQHandler(void);
extern void RTC2_IRQHandler(void);


void RTC1_IRQHandler(void)
{
if(rtc_timer_callback_func[0] != NULL)
{
    rtc_timer_callback_func[0]();
}
}

void RTC2_IRQHandler(void)
{
if(rtc_timer_callback_func[1] != NULL)
{
    rtc_timer_callback_func[1]();
}
}


//===================================================================================================4
///========================================== static functions here =================================

static void rtc_timer_int_enbale(uint8_t timer_no)
{
    switch (timer_no)
    {
    case NRF_RTC_TIMER_1:
        NVIC_SetPriority(RTC1_IRQn, RTC_TIMER_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(RTC1_IRQn);
        break;
    
    case NRF_RTC_TIMER_2:
        NVIC_SetPriority(RTC2_IRQn,RTC_TIMER_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(RTC2_IRQn);
        break;
    
    default:
        break;
    }
}

static void rtc_timer_int_disable(uint8_t timer_no)
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
    uint32_t addr =0;
    switch (number)
    {
    case NRF_RTC_TIMER_1:
        addr = NRF_RTC1_BASE;
        break;
    case NRF_RTC_TIMER_2:
        addr = NRF_RTC2_BASE;
        break;
    default:
        addr =0;
        break;
    }
    return addr;
}

//===================================================================================================4
///========================================== static functions ends here =================================



//===================================================================================================4
///==========================================  functions definations here =================================

void rtc_Timer_start(uint8_t timer_no)
{
    NRF_RTC_Type * timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);
    timer_inst->TASKS_START =1U;
}

void rtc_Timer_stop(uint8_t timer_no)
{
    NRF_RTC_Type * timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);
    timer_inst->TASKS_STOP =1U;
}

void rtc_Timer_clear(uint8_t timer_no)
{
    NRF_RTC_Type * timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);

    timer_inst->TASKS_CLEAR =1U;
}


void rtc_Timer_add_irq_handler(uint8_t timer_no, void (*func)(void))
{
rtc_timer_callback_func[timer_no-1] = func; 
}

void rtc_Timer_remove_irq_handler(uint8_t timer_no )
{
rtc_timer_callback_func[timer_no-1] = NULL; 
}

void rtc_Timer_int_enable(uint8_t timer_no, uint8_t int_src)
{
NRF_RTC_Type * timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);

}

void rtc_Timer_int_disable(uint8_t timer_no, uint8_t int_src)
{
NRF_RTC_Type * timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);
}

void rtc_Timer_event_enable(uint8_t timer_no, uint8_t event_Type)
{
NRF_RTC_Type * timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);

}


void rtc_Timer_event_disable(uint8_t timer_no, uint8_t event_Type)
{
NRF_RTC_Type * timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);

}

__FORCE_INLINE uint32_t rtc_Timer_get_counter_value(uint8_t timer_no)
{
NRF_RTC_Type * timer_inst = (NRF_RTC_Type *)rtc_timer_get_addr_from_no(timer_no);

}
