#ifndef _NRF_RTC_TIMER_
#define _NRF_RTC_TIMER_

#include "system.h"

/// we can only use rtc 1 and 2 rtc 0 is used by softdevice 
#define NRF_RTC_TIMER_1  1
#define NRF_RTC_TIMER_2  2

void rtc_timer_config(uint8_t rtc_timer_no, uint32_t prescalar );




#endif