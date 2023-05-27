#ifndef _BLE_SOFTDEVICE_UTIL_
#define _BLE_SOFTDEVICE_UTIL_


#include "system.h"


// #include <stdint.h>
// #include "compiler_abstraction.h"
// #include "nrf.h"

#include "nrf_soc.h"
#include "nrf_nvic.h"

// #include "nrf_assert.h"
// #include "app_error.h"



// #if __CORTEX_M == (0x00U)
// #define IRQ_PRIO_SD_HIGH       0
// #define IRQ_PRIO_APP_HIGH      1
// #define IRQ_PRIO_APP_MID       1
// #define IRQ_PRIO_SD_LOW        2
// #define IRQ_PRIO_APP_LOW_MID   3
// #define IRQ_PRIO_APP_LOW       3
// #define IRQ_PRIO_APP_LOWEST    3
// #define IRQ_PRIO_THREAD        4

// ////// this is for cm4 
// #elif __CORTEX_M >= (0x04U)
// #define IRQ_PRIO_SD_HIGH       0
// #define IRQ_PRIO_SD_MID        1
// #define IRQ_PRIO_APP_HIGH      2
// #define IRQ_PRIO_APP_MID       3
// #define IRQ_PRIO_SD_LOW        4
// #define IRQ_PRIO_APP_LOW_MID   5
// #define IRQ_PRIO_APP_LOW       6
// #define IRQ_PRIO_APP_LOWEST    7
// #define IRQ_PRIO_THREAD        15
// #else
//     #error "No CPU defined"
// #endif




// /*@brief The privilege levels available to applications in Thread Mode */
// enum _APP_PRIVILGE_LEVEL_
// {
//     APP_LEVEL_UNPRIVILEGED,
//     APP_LEVEL_PRIVILEGED
// };

/**@cond NO_DOXYGEN */
#define EXTERNAL_INT_VECTOR_OFFSET 16
/**@endcond */

/**@brief Macro for setting a breakpoint.
 */
#if defined(__GNUC__)
#define NRF_BREAKPOINT __asm__("BKPT 0");
#else
#define NRF_BREAKPOINT __BKPT(0)
#endif

/** @brief Macro for setting a breakpoint.
 *
 * If it is possible to detect debugger presence then it is set only in that case.
 *
 */
#if __CORTEX_M == 0x04
#define NRF_BREAKPOINT_COND do {                            \
    /* C_DEBUGEN == 1 -> Debugger Connected */              \
    if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)   \
    {                                                       \
       /* Generate breakpoint if debugger is connected */   \
            NRF_BREAKPOINT;                                 \
    } \
    }while (0)
#else
#define NRF_BREAKPOINT_COND NRF_BREAKPOINT
#endif // __CORTEX_M == 0x04



/// @brief this function is used to enter the critical region 
/// @param p_nested 
void app_enter_critical_region(uint8_t *p_nested);

/// @brief this function is used to exit the critical region 
/// @param nested 
void app_exit_critical_region(uint8_t nested);


/// @brief this macro function is used to enter the critical region 
#define SOFTDEVICE_CRITICAL_REGION_ENTER()          \
uint8_t nested_softdevice_critical_region =0;       \
app_enter_critical_region(&nested_softdevice_critical_region) \


/// @brief this function is used to exit the critical region 
#define SOFTDEVICE_CRITICAL_REGION_EXIT() \
app_exit_critical_region(0)               \



/// @brief this is to enable the softdevice 
/// @param  void 
/// @return succ/failure of function 
uint32_t softdevice_enable_req(void);

/// @brief this function is used to disbale the softdevice 
/// @param  void 
/// @return succ/failure of function 
uint32_t softdevice_disable_req(void);

/// @brief this is to check if softdevice is enabled 
/// @param  void
/// @return softdevice state 
bool softdevice_is_enabled(void);

/// @brief this function is to suspend the softdevice events  
/// @param  void 
uint32_t softdevice_suspend_events(void);

/// @brief this is to resume the softdevice events
/// @param  
uint32_t softdevice_resume_events(void);


/**
 * @brief Function for finding the current interrupt level.
 *
 * @return   Current interrupt level.
 */
uint8_t cpu_get_priority(void);


/**
 * @brief Function for finding out the current privilege level.
 *
 * @return   Current privilege level.
 * @retval   APP_LEVEL_UNPRIVILEGED    We are running in unprivileged level.
 * @retval   APP_LEVEL_PRIVILEGED    We are running in privileged level.
 */
uint8_t cpu_get_privilige_level(void);


#endif 