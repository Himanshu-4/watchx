#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "system.h"
//// define the time format for the kernel , this will be managed by the kernel 

#define KERNEL_TIME_AM 10
#define KERNEL_TIME_PM 20 

/// @brief define the month data like jan,feb,etc
typedef enum
{
	MONTH_JAN = 0,
	MONTH_FEB = 1,
	MONTH_MAR = 2,
	MONTH_APR = 3,
	MONTH_MAY = 4,
	MONTH_JUN = 5,
	MONTH_JUL = 6,
	MONTH_AUG = 7,
	MONTH_SEP = 8,
	MONTH_OCT = 9,
	MONTH_NOV = 10,
	MONTH_DEC = 11
} month_enum_t;

/// @brief define the day data like monday,tuesday ,etc
typedef enum
{
	DAY_MON = 0,
	DAY_TUE = 1,
	DAY_WED = 2,
	DAY_THU = 3,
	DAY_FRI = 4,
	DAY_SAT = 5,
	DAY_SUN = 6,
} day_enum_t;

///@brief onyl 12 hour format is supported for now 
typedef PACKED_STRUCT _KERNEL_TIME_FORMAT_
{
    uint8_t hour;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t am_pm;
}kernel_time_struct_t;

typedef PACKED_STRUCT _KEERNEL_DATE_FORAMT_ 
{
    uint8_t date;
    day_enum_t day;
    month_enum_t month;
    uint8_t year;
}kernel_date_struct_t;



#endif 