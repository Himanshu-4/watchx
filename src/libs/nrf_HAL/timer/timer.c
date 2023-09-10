#include "timer.h"

// this is the arry of the function callbacks for the four timers
static void (*timer_callback_fun[4])(void) = {NULL};

void TIMER1_IRQHandler(void)
{
    if (timer_callback_fun[TIMER_1_FUNC] != NULL)
    {
        timer_callback_fun[TIMER_1_FUNC]();
    }
}
void TIMER2_IRQHandler(void)
{
    if (timer_callback_fun[TIMER_2_FUNC] != NULL)
    {
        timer_callback_fun[TIMER_2_FUNC]();
    }
}
void TIMER3_IRQHandler(void)
{
    if (timer_callback_fun[TIMER_3_FUNC] != NULL)
    {
        timer_callback_fun[TIMER_3_FUNC]();
    }
}
void TIMER4_IRQHandler(void)
{
    if (timer_callback_fun[TIMER_4_FUNC] != NULL)
    {
        timer_callback_fun[TIMER_4_FUNC]();
    }
}

// ////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////  static functions here ///////////////////////////////////////
//   pls dont touch timer 0 at any cost

/// @brief enable the timer interrupt in nvic
/// @param timer_no
static void timer_int_enable(uint8_t timer_no)
{
    // set the nvic priority bits and enable the interrupt in nvic

    switch (timer_no)
    {
    case TIMER_1:
        /* code */
        NVIC_SetPriority(TIMER1_IRQn, TIMER_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(TIMER1_IRQn);
        break;

    case TIMER_2:
        NVIC_SetPriority(TIMER2_IRQn, TIMER_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(TIMER2_IRQn);
        break;

    case TIMER_3:
        NVIC_SetPriority(TIMER3_IRQn, TIMER_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(TIMER3_IRQn);
        break;

    case TIMER_4:
        NVIC_SetPriority(TIMER4_IRQn, TIMER_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(TIMER4_IRQn);

        break;
    }
}

/// @brief disable the timer interrupt in nvic side
/// @param timer_no
static void timer_int_disable(uint8_t timer_no)
{

    switch (timer_no)
    {
    case TIMER_1:
        /* code */
        NVIC_DisableIRQ(TIMER1_IRQn);
        break;

    case TIMER_2:
        NVIC_DisableIRQ(TIMER2_IRQn);
        break;

    case TIMER_3:
        NVIC_DisableIRQ(TIMER3_IRQn);
        break;

    case TIMER_4:
        NVIC_DisableIRQ(TIMER4_IRQn);
        break;
    default:
        // disable all irq here
        NVIC_DisableIRQ(TIMER1_IRQn);
        NVIC_DisableIRQ(TIMER2_IRQn);
        NVIC_DisableIRQ(TIMER3_IRQn);
        NVIC_DisableIRQ(TIMER4_IRQn);
        break;
    }
}

static uint32_t timer_get_addr_from_no(uint8_t number)
{
    uint32_t addr = 0;
    switch (number)
    {
    case TIMER_1:
        /* code */
        addr = NRF_TIMER1_BASE;
        break;

    case TIMER_2:
        addr = NRF_TIMER2_BASE;
        break;

    case TIMER_3:
        addr = NRF_TIMER3_BASE;
        break;

    case TIMER_4:
        addr = NRF_TIMER4_BASE;
        break;
    default:
        break;
    }

    return addr;
}
//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void timer_module_init(uint8_t timer_no)
{

    timer_int_disable(timer_no);


    // // make all the callbacks to NULL
    // for (uint8_t i = 1; i <= 4; i++)
    // {
    //     // make sure all the timer is in stop condition
    //     timer_stop(i);
    //     // make the callback of every function to Null
    //     timer_callback_fun[i-1] = NULL;
    // }
    timer_stop(timer_no);

    timer_callback_fun[timer_no] = NULL;

}

void timer_module_deint(uint8_t timer_no)
{
    // disable all the interrupt timers
     NRF_TIMER_Type *timer_inst = (NRF_TIMER_Type *)(timer_get_addr_from_no(timer_no));

    timer_int_disable(timer_no);
    // // stop all the timers
    // for (int i = 1; i <= 4; i++)
    // {
    //     // stop all the timer
    //     
    // }
    timer_stop(timer_no);
    timer_callback_fun[timer_no] = NULL;
            // clear the shortcut and interrupt register
        timer_inst->INTENCLR = (0xFFUL << TIMER_INT_BIT_Msk);
        timer_inst->SHORTS = 0;

}

void timer_config(uint8_t timer_number, const my_timer_cfg *cfg)
{

    // disale the timer interrupt
    timer_int_disable(timer_number);
    timer_stop(timer_number);

    NRF_TIMER_Type *timer_inst = (NRF_TIMER_Type *)(timer_get_addr_from_no(timer_number));

         timer_inst->MODE = cfg->mode;
        timer_inst->PRESCALER = (0x0F & cfg->timer_tick_rate);
        timer_inst->BITMODE = cfg->timer_width;

        // clear the shortcut and interrupt register
        timer_inst->INTENCLR = (0xFFUL << TIMER_INT_BIT_Msk);
        timer_inst->SHORTS = 0;
    // configure the timer
   
}

__FORCE_INLINE uint32_t timer_get_counter_val(uint8_t timer_number)
{
    // it will always use the timer counter_capture 0 register to get the value
    uint32_t number = 0;
    NRF_TIMER_Type *timer_inst = (NRF_TIMER_Type *)(timer_get_addr_from_no(timer_number));
    timer_inst->TASKS_CAPTURE[0] = 1UL;
    __NOP();
    __NOP();
    number = timer_inst->CC[0];
   return number;
}

void timer_start(uint8_t timer_number)
{
    NRF_TIMER_Type *timer_inst = (NRF_TIMER_Type *)(timer_get_addr_from_no(timer_number));

    timer_inst->TASKS_START = 1UL;
}

void timer_stop(uint8_t timer_number)
{
    NRF_TIMER_Type *timer_inst = (NRF_TIMER_Type *)(timer_get_addr_from_no(timer_number));

    timer_inst->TASKS_STOP = 1UL;
}

void timer_clear(uint8_t timer_number)
{
    NRF_TIMER_Type *timer_inst = (NRF_TIMER_Type *)(timer_get_addr_from_no(timer_number));

    timer_inst->TASKS_CLEAR = 1UL;
}

// this will add the irq handler
void timer_add_irq_handler(uint8_t timer_number, void (*func)(void))
{
    timer_callback_fun[timer_number - 1] = func;
    // enable the interrupt in nvic
    timer_int_enable(timer_number);
}

void timer_remove_irq_handler(uint8_t timer_number)
{
    // make the callback to null since array is indexed through 0 so we have to do -1
    timer_callback_fun[timer_number - 1] = NULL;
    timer_int_disable(timer_number);
}

void timer_add_interupt_time(uint8_t timer_number, uint8_t capt_comp_id, uint32_t counter_val)
{
    // you can add an interrupt time in any of the compare register but, never use comp_0
    NRF_TIMER_Type *timer_inst = (NRF_TIMER_Type *)(timer_get_addr_from_no(timer_number));

    timer_inst->CC[capt_comp_id] = counter_val;
}

void timer_interrupt_enable(uint8_t timer_number, uint8_t capt_cmp_id)
{
    NRF_TIMER_Type *timer_inst = (NRF_TIMER_Type *)(timer_get_addr_from_no(timer_number));

    timer_inst->INTENSET = _BV(capt_cmp_id + TIMER_INT_BIT_Msk);
}

void timer_interrupt_disable(uint8_t timer_number, uint8_t capt_cmp_id)
{
    NRF_TIMER_Type *timer_inst = (NRF_TIMER_Type *)(timer_get_addr_from_no(timer_number));

    timer_inst->INTENCLR = _BV(capt_cmp_id + TIMER_INT_BIT_Msk);
}

uint8_t timer_get_int_src(uint8_t timer_number)
{
    uint8_t cap_comp_id = CAPTURE_COMPARE_NONE;

    if (timer_number > 2) // that means the timer have 6 cc register
    {
        switch (timer_number)
        {
        case TIMER_3:
            for (uint8_t i = 0; i < 6; i++)
            {
                if (NRF_TIMER3->EVENTS_COMPARE[i])
                {
                    // also check if the interrupt of that is enable dor not
                    cap_comp_id = i;
                    // clear the event interrupt
                    NRF_TIMER3->EVENTS_COMPARE[i] = 0;
                    break;
                }
            }
            break;

        case TIMER_4:

            for (uint8_t i = 0; i < 6; i++)
            {
                if (NRF_TIMER4->EVENTS_COMPARE[i])
                {
                    cap_comp_id = i;
                    // clear the event interrupt
                    NRF_TIMER4->EVENTS_COMPARE[i] = 0;
                    break;
                }
            }
            break;
        }
    }
    else // timer have only 4 cc register
    {
        switch (timer_number)
        {
        case TIMER_1:

            for (uint8_t i = 0; i < 4; i++)
            {
                if (NRF_TIMER1->EVENTS_COMPARE[i])
                {
                    cap_comp_id = i;
                    // clear the event interrupt
                    NRF_TIMER1->EVENTS_COMPARE[i] = 0;
                    break;
                }
            }
            break;

        case TIMER_2:

            for (uint8_t i = 0; i < 4; i++)
            {
                if (NRF_TIMER2->EVENTS_COMPARE[i])
                {
                    cap_comp_id = i;
                    // clear the event interrupt
                    NRF_TIMER2->EVENTS_COMPARE[i] = 0;
                    break;
                }
            }
            break;
        }
    }
    return cap_comp_id;
}

void timer_set_auto_reload_val(uint8_t timer_number, uint8_t cap_comp_id, uint32_t counter_val)
{
    // never set the cap_compare id 0
    if (cap_comp_id == 0)
        return;

    NRF_TIMER_Type *timer_inst = (NRF_TIMER_Type *)(timer_get_addr_from_no(timer_number));

    timer_inst->CC[cap_comp_id] = counter_val;
    timer_inst->SHORTS |= _BV(cap_comp_id);
}

void timer_set_stop_timer_time(uint8_t timer_number, uint8_t cap_comp_id, uint32_t counter_val)
{
    // never set the cap_compare id 0
    if (cap_comp_id == 0)
        return;

    NRF_TIMER_Type *timer_inst = (NRF_TIMER_Type *)(timer_get_addr_from_no(timer_number));

    timer_inst->CC[cap_comp_id] = counter_val;
    timer_inst->SHORTS |= _BV(cap_comp_id + TIMER_SHORT_STOP_BIT_Msk);
}

///////// used in PPI module
////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

uint32_t *timer_get_task_addr(uint8_t timer_number, uint8_t cap_comp_id)
{

    uint32_t *addr = NULL;
    switch (timer_number)
    {
    case TIMER_1:
        addr = (uint32_t *)&NRF_TIMER1->TASKS_CAPTURE[cap_comp_id];
        break;
    case TIMER_2:
        addr = (uint32_t *)&NRF_TIMER2->TASKS_CAPTURE[cap_comp_id];
        break;
    case TIMER_3:
        addr = (uint32_t *)&NRF_TIMER3->TASKS_CAPTURE[cap_comp_id];
        break;
    case TIMER_4:
        addr = (uint32_t *)&NRF_TIMER4->TASKS_CAPTURE[cap_comp_id];
        break;
    }

    return addr;
}

uint32_t *timer_get_event_addr(uint8_t timer_number, uint8_t cap_comp_id)
{
    uint32_t *addr = NULL;
    switch (timer_number)
    {
    case TIMER_1:
        addr = (uint32_t *)&NRF_TIMER1->EVENTS_COMPARE[cap_comp_id];
        break;
    case TIMER_2:
        addr = (uint32_t *)&NRF_TIMER2->EVENTS_COMPARE[cap_comp_id];
        break;
    case TIMER_3:
        addr = (uint32_t *)&NRF_TIMER3->EVENTS_COMPARE[cap_comp_id];
        break;
    case TIMER_4:
        addr = (uint32_t *)&NRF_TIMER4->EVENTS_COMPARE[cap_comp_id];
        break;
    }

    return addr;
}

uint32_t *timer_get_counter_addr(uint8_t timer_number)
{
    uint32_t *addr = NULL;
    switch (timer_number)
    {
    case TIMER_1:
        addr = (uint32_t *)&NRF_TIMER1->TASKS_COUNT;
        break;
    case TIMER_2:
        addr = (uint32_t *)&NRF_TIMER2->TASKS_COUNT;
        break;
    case TIMER_3:
        addr = (uint32_t *)&NRF_TIMER3->TASKS_COUNT;
        break;
    case TIMER_4:
        addr = (uint32_t *)&NRF_TIMER4->TASKS_COUNT;
        break;
    }

    return addr;
}

///////// used in PPI module
////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

// pending
uint32_t timer_us_to_counter(uint8_t timer_number, uint32_t time_us)
{
    NRF_TIMER_Type *timer_inst = (NRF_TIMER_Type *)(timer_get_addr_from_no(timer_number));

    uint32_t prescalar = 0;

    prescalar = timer_inst->PRESCALER;

    return ((time_us * 16UL) >> prescalar);
}

uint32_t timer_ms_to_counter(uint8_t timer_number, uint32_t time_ms)
{

    NRF_TIMER_Type *timer_inst = (NRF_TIMER_Type *)(timer_get_addr_from_no(timer_number));

    uint32_t prescalar = 0;

    prescalar = timer_inst->PRESCALER;

    return ((time_ms * 16000ULL) >> prescalar);
}