#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "system.h"
//// define the time format for the kernel , this will be managed by the kernel 

// #define KERNEL_TIME_AM 0x10UL
// #define KERNEL_TIME_PM 0x20UL 

#define KERNEL_TIME_HOUR_MAX 23
#define KERNEL_TIME_MINUTE_MAX 59
#define KERNEL_TIME_SECONDS_MAX 59


/// @brief define the month data like jan,feb,etc
typedef enum
{
	MONTH_JAN = 0,
	MONTH_FEB,
	MONTH_MAR,
	MONTH_APR,
	MONTH_MAY,
	MONTH_JUN,
	MONTH_JUL,
	MONTH_AUG,
	MONTH_SEP,
	MONTH_OCT,
	MONTH_NOV ,
	MONTH_DEC
} month_enum_t;

/// @brief define the day data like monday,tuesday ,etc
typedef enum
{
	DAY_MON =0,
	DAY_TUE ,
	DAY_WED ,
	DAY_THU ,
	DAY_FRI ,
	DAY_SAT ,
	DAY_SUN ,
} day_enum_t;

///@brief onyl 24 hour format is supported for now 
typedef PACKED_STRUCT _KERNEL_TIME_FORMAT_
{
    uint8_t hour;
    uint8_t minutes;
    uint8_t seconds;
}kernel_time_struct_t;

typedef PACKED_STRUCT _KEERNEL_DATE_FORAMT_ 
{
    uint8_t date;
    uint8_t day;
    uint8_t month;
    uint16_t year;
}kernel_date_struct_t;

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////// the set and get time functions 

/// @brief get the current time from the kernel time module
/// @param my_time 
void kernel_time_get_current_time(kernel_time_struct_t * my_time);

/// @brief get the current date from the kernel date module 
/// @param my_date  
void kernel_time_get_current_date(kernel_date_struct_t * my_date);

/// @brief set the current time in the kernel time module 
/// @param my_time 
void kernel_time_set_current_time(kernel_time_struct_t * my_time);

/// @brief get the current data for the 
/// @param my_date 
void kernel_time_set_current_date(kernel_date_struct_t * my_date);

/// @brief pre init the kernel time module
void kernel_time_pre_init(kernel_time_struct_t * time, kernel_date_struct_t * data);


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// you can also create timers based event in this kernel 


#endif 