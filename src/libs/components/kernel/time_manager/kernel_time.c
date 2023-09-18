#include "kernel_time.h"

#include "RTC_timer.h"

//// create a const mapping for the max days in months

const uint8_t kernel_months_max_Days[12] =
    {
        31, /// Jan
        28, /// Feb
        31, /// March
        30, /// April
        31, /// May
        30, /// June
        31, /// July
        31, /// August
        30, /// sept
        31, /// OCT
        30, /// NOV
        31  /// DEC
};

const uint8_t kernel_months_max_Days_leap_year[12] =
    {
        31, /// Jan
        29, /// Feb
        31, /// March
        30, /// April
        31, /// May
        30, /// June
        31, /// July
        31, /// August
        30, /// sept
        31, /// OCT
        30, /// NOV
        31  /// DEC
};

/// for a leap year the year must be multiple of 4
#define LEAP_YEAR_BASE_NUMBER 4

#define RTC_TIMER_PRESCALER 327
#define RTC_TIMER_COMPARE_VALUE 100

#define RTC_TIMER_USED NRF_RTC_TIMER_2
//========================================= static functions======================================

static volatile kernel_time_struct_t current_time;

static volatile kernel_date_struct_t current_date;

/// @brief RTC itterrupt handler
/// @param
static void rtc_timer_2_handler(void)
{
    /// we should know the handler frequency for now we take 1 seconds

    uint32_t evt = rtc_Timer_get_event_src_and_clear(RTC_TIMER_USED);

    if (evt & RTC_TIM_EVT_COMPARE(0))
    {

        /// based on that update time
        /// update the seconds
        current_time.seconds++;

        /// check the seconds
        if (current_time.seconds > KERNEL_TIME_SECONDS_MAX)
        {
            /// increment the minutes
            current_time.minutes++;
            current_time.seconds = 0;

            /// check the minutes
            if (current_time.minutes > KERNEL_TIME_MINUTE_MAX)
            {
                /// increment the hour
                current_time.hour++;
                current_time.minutes = 0;

                /// check the hour
                if (current_time.hour > KERNEL_TIME_HOUR_MAX)
                {
                    /// restore the hour
                    current_time.hour = 0;

                    //// increment the date and day
                    current_date.day++;
                    current_date.date++;

                    /// safeley increment the day
                    if (current_date.day > DAY_SUN)
                    {
                        current_date.day = DAY_MON;
                    }

                    /// check the month  overflow
                    if (current_date.date > (GET_REMNDER(current_date.year, LEAP_YEAR_BASE_NUMBER) ? (kernel_months_max_Days[current_date.month]) : (kernel_months_max_Days_leap_year[current_date.month])))
                    {
                        current_date.date = 1;

                        /// increment the month
                        current_date.month++;

                        if (current_date.month > MONTH_DEC)
                        {
                            current_date.month = MONTH_JAN;

                            /// increment the year
                            current_date.year++;

                            //// should end here
                        }
                    }
                }
            }
        }

    
    //// clear the timer counter
    rtc_Timer_clear(RTC_TIMER_USED);
    }
}

//-------------------------------- functions definations------------------------------------------

/// @brief pre init the kernel time module
void kernel_time_pre_init(kernel_time_struct_t *time, kernel_date_struct_t *date)
{
    /// init the rtc module
    rtc_Timer_Config(RTC_TIMER_USED, RTC_TIMER_PRESCALER);
    rtc_Timer_add_irq_handler(RTC_TIMER_USED, rtc_timer_2_handler);
    rtc_Timer_int_enable(RTC_TIMER_USED, RTC_TIM_EVT_MASK_COMPARE0);
    rtc_Timer_event_enable(RTC_TIMER_USED, RTC_TIM_EVT_MASK_COMPARE0);

    if (time != NULL)
    {
        memcpy(u8_ptr & current_time, time, sizeof(kernel_time_struct_t));
    }
    else
    {
        current_time.hour = 16;
        current_time.minutes = 26;
        current_time.seconds = 12;
    }
    if (date != NULL)
    {
        memcpy(u8_ptr & current_date, date, sizeof(kernel_date_struct_t));
    }
    else
    {
        current_date.date = 16;
        current_date.day = DAY_SAT;
        current_date.month = MONTH_SEP;
        current_date.year = 2023;
    }

    /// set the timer counter register
    rtc_Timer_clear(RTC_TIMER_USED);
    rtc_Timer_set_Comapre_reg(RTC_TIMER_USED, RTC_TIMER_COMPARE_0, RTC_TIMER_COMPARE_VALUE);

    /// START THE TIMer
    rtc_Timer_start(RTC_TIMER_USED);
}

/// @brief get the current time from the kernel time module
/// @param my_time
void kernel_time_get_current_time(kernel_time_struct_t *my_time)
{
    //// copy the current time
    memcpy(my_time, u8_ptr & current_time, sizeof(kernel_time_struct_t));
}

/// @brief get the current date from the kernel date module
/// @param my_date
void kernel_time_get_current_date(kernel_date_struct_t *my_date)
{
    memcpy(my_date, u8_ptr & current_date, sizeof(kernel_date_struct_t));
}

/// @brief set the current time in the kernel time module
/// @param my_time
void kernel_time_set_current_time(kernel_time_struct_t *my_time)
{
    memcpy(u8_ptr & current_time, my_time, sizeof(kernel_time_struct_t));
}

/// @brief get the current data for the
/// @param my_date
void kernel_time_set_current_date(kernel_date_struct_t *my_date)
{
    memcpy(u8_ptr & current_date, my_date, sizeof(kernel_date_struct_t));
}
