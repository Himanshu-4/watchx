#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "system.h"
//// define the time format for the kernel , this will be managed by the kernel 

// #define KERNEL_TIME_AM 0x10UL
// #define KERNEL_TIME_PM 0x20UL 

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

/// @brief 
/// @param my_time 
/// @return 
uint32_t kernel_get_current_time(kernel_time_struct_t * my_time);

/// @brief 
/// @param my_date 
/// @return 
uint32_t kernel_get_current_date(kernel_date_struct_t * my_date);

/// @brief 
/// @param my_time 
/// @return 
uint32_t kernel_set_current_time(kernel_time_struct_t * my_time);

/// @brief 
/// @param my_date 
/// @return 
uint32_t kernel_set_current_date(kernel_date_struct_t * my_date);


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// //////////////////////////////////////////////////////
///////////////////////////////////// you can also create timers based event in this kernel 


#endif 