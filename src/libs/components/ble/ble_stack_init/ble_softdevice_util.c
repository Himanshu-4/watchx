
#include "ble_softdevice_util.h"

/////// include the nordic softdevice libraries 
#include "nrf_svc.h"
#include "nrf_error.h"
#include "nrf_soc.h"
#include "nrf_sdm.h"

#include "ble_config.h"

#ifdef SOFTDEVICE_PRESENT
/* Global nvic state instance, required by nrf_nvic.h */
nrf_nvic_state_t nrf_nvic_state;
#endif

/// @brief this is a variable used to store that how many times the func is called 
static uint32_t m_in_critical_region = 0;

static void app_diable_interrupts(void)
{
    __disable_irq();
    m_in_critical_region++;
}

static void app_enable_interrupts(void)
{
    m_in_critical_region--;
    if (m_in_critical_region == 0)
    {
        __enable_irq();
    }
}

void app_enter_critical_region(uint8_t *p_nested)
{
#if __CORTEX_M == (0x04U)
    ASSERT(APP_LEVEL_PRIVILEGED == privilege_level_get())
#endif

#if defined(SOFTDEVICE_PRESENT)
    /* return value can be safely ignored */
    (void) sd_nvic_critical_region_enter(p_nested);
#else
    app_diable_interrupts();
#endif
}

void app_exit_critical_region(uint8_t nested)
{
#if __CORTEX_M == (0x04U)
    ASSERT(APP_LEVEL_PRIVILEGED == privilege_level_get())
#endif

#if defined(SOFTDEVICE_PRESENT)
    /* return value can be safely ignored */
    (void) sd_nvic_critical_region_exit(nested);
#else
    app_enable_interrupts();
#endif
}




uint8_t cpu_get_privilige_level(void)
{
#if __CORTEX_M == (0x00U)
    /* the Cortex-M0 has no concept of privilege */
    return APP_LEVEL_PRIVILEGED;
    
#elif __CORTEX_M >= (0x04U)
    uint32_t isr_vector_num = __get_IPSR() & IPSR_ISR_Msk ;
    if (0 == isr_vector_num)
    {
        /* Thread Mode, check nPRIV */
        int32_t control = __get_CONTROL();
        return control & CONTROL_nPRIV_Msk ? APP_LEVEL_UNPRIVILEGED : APP_LEVEL_PRIVILEGED;
    }
    else
    {
        /* Handler Mode, always privileged */
        return APP_LEVEL_PRIVILEGED;
    }
#endif
}


uint8_t cpu_get_priority(void)
{
    uint32_t isr_vector_num = __get_IPSR() & IPSR_ISR_Msk ;
    if (isr_vector_num > 0)
    {
        int32_t irq_type = ((int32_t)isr_vector_num - EXTERNAL_INT_VECTOR_OFFSET);
        return (NVIC_GetPriority((IRQn_Type)irq_type) & 0xFF);
    }
    else
    {
        return APP_IRQ_PRIORITY_THREAD;
    }
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/// @brief this is to enable the softdevice interrupt 
/// @param  void 
static void softdevice_evt_interrupt_enable(void)
{
#ifdef SOFTDEVICE_PRESENT
    uint32_t ret_code = sd_nvic_EnableIRQ((IRQn_Type)SD_EVT_IRQn);
    NRF_ASSERT(ret_code);
#else
    // In case of serialization, NVIC must be accessed directly.
    NVIC_EnableIRQ(SD_EVT_IRQn);
#endif
}

/// @brief this is to disable the softdevice interrupt 
/// @param  void 
static void softdevice_evt_interrupt_disable(void)
{
#ifdef SOFTDEVICE_PRESENT
    uint32_t  ret_code = sd_nvic_DisableIRQ((IRQn_Type)SD_EVT_IRQn);
    NRF_ASSERT(ret_code);
#else
    // In case of serialization, NVIC must be accessed directly.
    NVIC_DisableIRQ(SD_EVT_IRQn);
#endif
}




//////////////// this function is called when a softdevice error occured 

extern void softdevice_err_handler(uint32_t id, uint32_t pc, uint32_t info);


static bool nrf_softdevice_enabled = false;
static bool nrf_softdevice_events_suspended =false;


uint32_t softdevice_enable_req(void)
{
    if(nrf_softdevice_enabled)
    {
        return nrf_ERR_INVALID_STATE;
    }

       nrf_clock_lf_cfg_t const clock_lf_cfg =
    {
        .source       = NRF_SDH_CLOCK_LF_SRC,
        .rc_ctiv      = NRF_SDH_CLOCK_LF_RC_CTIV,
        .rc_temp_ctiv = NRF_SDH_CLOCK_LF_RC_TEMP_CTIV,
        .accuracy     = NRF_SDH_CLOCK_LF_ACCURACY
    };

    uint32_t ret_code =0;

    SOFTDEVICE_CRITICAL_REGION_ENTER();
    ret_code = sd_softdevice_enable(&clock_lf_cfg, softdevice_err_handler);
    SOFTDEVICE_CRITICAL_REGION_EXIT();

    //////////// enable the nrf_event irq 

    if(ret_code != 0)
    {
        return nrf_ERR_OPERATION_FAILED;
    }

    nrf_softdevice_enabled = true;

    //// enable the softdevice events interrupts 
    softdevice_evt_interrupt_enable();

    return nrf_OK;

}


uint32_t softdevice_disable_req(void)
{
    if(!nrf_softdevice_enabled)
    {
        return nrf_ERR_INVALID_STATE;
    }

    uint32_t ret_code =0;

    SOFTDEVICE_CRITICAL_REGION_ENTER();
    ret_code = sd_softdevice_disable();
    SOFTDEVICE_CRITICAL_REGION_EXIT();

    if(ret_code != 0)
    {
        return nrf_ERR_OPERATION_FAILED;
    }

    nrf_softdevice_enabled = false ;

    /// disable the softdevice interrupts 
    softdevice_evt_interrupt_disable();
    
    return nrf_OK;
}

bool softdevice_is_enabled(void)
{
    uint8_t sd_state =0;
      sd_softdevice_is_enabled(&sd_state);
    return sd_state;

}

uint32_t  softdevice_suspend_events(void)
{
    if(nrf_softdevice_events_suspended)
    {
        return nrf_ERR_INVALID_STATE;
    }

    softdevice_evt_interrupt_disable();

    nrf_softdevice_events_suspended = true;
    return nrf_OK;
}

uint32_t softdevice_resume_events(void)
{
    if(!nrf_softdevice_events_suspended)
    {
        return nrf_ERR_INVALID_STATE;
    }

    /// set the pending interrupt to trigger it before enabling the interrupt 
    sd_nvic_SetPendingIRQ((IRQn_Type)SD_EVT_IRQn);

    softdevice_evt_interrupt_enable();

    nrf_softdevice_events_suspended = false;
    return nrf_OK;

}




