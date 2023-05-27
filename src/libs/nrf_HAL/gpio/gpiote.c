#include "gpiote.h"

static void (*func_ptr[8])(void) = {NULL};

// install the isr service
bool gpio_install_isr_servc(void)
{
    // enable the cortex interrupt in nvic

    NVIC_SetPriority(GPIOTE_IRQn, GPIOTE_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(GPIOTE_IRQn);

    // making every pointers to NULL
    for (int i = 0; i < 8; i++)
    {
        func_ptr[i] = NULL;
    }

    return nrf_OK;
}

bool gpio_uninstall_isr_servc(void)
{
    NVIC_DisableIRQ(GPIOTE_IRQn);
    return nrf_OK;
}

uint32_t *task_addr_get_ch_no(uint8_t ch_no, uint8_t type)
{
    uint32_t *addr = NULL;
    switch (type)
    {
    case GPIO_TASK_ADDR_SET:
        addr = (uint32_t *)(NRF_GPIOTE->TASKS_SET + ch_no);
        break;

    case GPIO_TASK_ADDR_OUT:
        addr = (uint32_t *)(NRF_GPIOTE->TASKS_OUT + ch_no);
        break;

    case GPIO_TASK_ADDR_CLR:
        addr = (uint32_t *)(NRF_GPIOTE->TASKS_CLR + ch_no);
        break;

    default:
        addr = NULL;
        break;
    }

    return addr;
}

uint32_t *event_addr_get_ch_no(uint8_t ch_no)
{
    uint32_t *addr = NULL;
    addr = (uint32_t *)&(NRF_GPIOTE->EVENTS_IN[ch_no]);
    return addr;
}

void GPIOTE_IRQHandler(void)
{
    uint32_t val = NRF_GPIOTE->INTENSET;
    // check for the which chaanel resposible for the event
    for (uint8_t i = 0; i < 8; i++)
    {
        // check the event
        if (NRF_GPIOTE->EVENTS_IN[i] == 1UL) // if that is true means event happens
        {
            // clear that event interupt
            NRF_GPIOTE->EVENTS_IN[i] = 0;
            // check if that interrupt line is sett or not
            if ((val >> i) & 1UL)
            {
                // check if any irq handler is assigned or not
                if (func_ptr[i] != NULL)
                {
                    func_ptr[i]();
                }
            }

        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE bool gpio_config_channel(uint8_t ch_no, const my_gpiote_cfg *cfg)
{
    NRF_GPIOTE->CONFIG[ch_no] = ((cfg->mode << GPIOTE_CONFIG_MODE_Pos) |
                                 (cfg->pinsel << GPIOTE_CONFIG_PSEL_Pos) | (cfg->polarity << GPIOTE_CONFIG_POLARITY_Pos) |
                                 (cfg->outlevel << GPIOTE_CONFIG_OUTINIT_Pos));

    return nrf_OK;
}

////////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE void gpio_add_irq_handler(uint8_t ch_no, void (*func)(void))
{
    func_ptr[ch_no] = func;
}

////////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE void gpio_remove_channel(uint8_t ch_no)
{
    NRF_GPIOTE->CONFIG[ch_no] = 0;
}

////////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE void gpio_remove_irq_handler(uint8_t ch_no)
{
    func_ptr[ch_no] = NULL;
}

////////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE void gpio_int_enable(uint8_t ch_no)
{
    NRF_GPIOTE->INTENSET = _BV(ch_no);
}

////////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE void gpio_int_disable(uint8_t ch_no)
{
    NRF_GPIOTE->INTENCLR = _BV(ch_no);
}

////////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE void task_explicit_set(uint8_t ch_no)
{
    // check if task mode is enabled on that pin
    if ((NRF_GPIOTE->CONFIG[ch_no] & 3) == 3)
    {
        // trigger the task
        NRF_GPIOTE->TASKS_SET[ch_no] = 1UL;
    }
}

////////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE void task_explicit_clr(uint8_t ch_no)
{
    // check if task mode is enabled on that pin
    if ((NRF_GPIOTE->CONFIG[ch_no] & 3) == 3)
    {
        // trigger the task
        NRF_GPIOTE->TASKS_CLR[ch_no] = 1UL;
    }
}

////////////////////////////////////////////////////////////////////////////////////
FORCE_INLINE void task_explicit_out(uint8_t ch_no)
{
    // check if task mode is enabled on that pin
    if ((NRF_GPIOTE->CONFIG[ch_no] & 3) == 3)
    {
        // trigger the task
        NRF_GPIOTE->TASKS_OUT[ch_no] = 1UL;
    }
}
