#include "watchdog.h"

void watchdog_init(uint16_t time_sec)
{
    // donot configure watchdog when active state 
    if(NRF_WDT->RUNSTATUS == 1UL)return;
    
    uint32_t crv_val = (time_sec * LSE_CLK)-1;

    NRF_WDT->CONFIG = 0UL;
    NRF_WDT->CRV = crv_val;
    // enable the reload register 0 and 1
    NRF_WDT->RREN = 1UL;
}

/***
 * @name watchdog_start
 * @note start the watchdog timer must call init first 
*/
void watchdog_start(void)
{
    // check if the watchdog is running or not 
    if(NRF_WDT->RUNSTATUS == 0UL)
    {
        NRF_WDT->TASKS_START = 1UL;
    }
}



/***
 * @name watchdog_reset_counter
 * @note reset the watchdog timer before it reset the system
 * note that it reset the counter based on current value of crv
*/
void watchdog_reset(void)
{
    NRF_WDT->RR[0] = WATCHDOG_RELOAD_VALUE;
}

bool is_watchdog_started(void)
{
    return (volatile uint8_t)NRF_WDT->RUNSTATUS;
}



/// include this in the code 

// Watchdog::ResetReasons Watchdog::ActualResetReason() const {
//   uint32_t reason = NRF_POWER->RESETREAS;
//   NRF_POWER->RESETREAS = 0xffffffff;

//   if (reason & 0x01u)
//     return ResetReasons::ResetPin;
//   if ((reason >> 1u) & 0x01u)
//     return ResetReasons::Watchdog;
//   if ((reason >> 2u) & 0x01u)
//     return ResetReasons::SoftReset;
//   if ((reason >> 3u) & 0x01u)
//     return ResetReasons::CpuLockup;
//   if ((reason >> 16u) & 0x01u)
//     return ResetReasons::SystemOff;
//   if ((reason >> 17u) & 0x01u)
//     return ResetReasons::LpComp;
//   if ((reason) &0x01u)
//     return ResetReasons::DebugInterface;
//   if ((reason >> 19u) & 0x01u)
//     return ResetReasons::NFC;
//   return ResetReasons::HardReset;
// }

// const char* Watchdog::ResetReasonToString(Watchdog::ResetReasons reason) {
//   switch (reason) {
//     case ResetReasons::ResetPin:
//       return "Reset pin";
//     case ResetReasons::Watchdog:
//       return "Watchdog";
//     case ResetReasons::DebugInterface:
//       return "Debug interface";
//     case ResetReasons::LpComp:
//       return "LPCOMP";
//     case ResetReasons::SystemOff:
//       return "System OFF";
//     case ResetReasons::CpuLockup:
//       return "CPU Lock-up";
//     case ResetReasons::SoftReset:
//       return "Soft reset";
//     case ResetReasons::NFC:
//       return "NFC";
//     case ResetReasons::HardReset:
//       return "Hard reset";
//     default:
//       return "Unknown";
//   }
// }
