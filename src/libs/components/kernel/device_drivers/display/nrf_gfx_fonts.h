#ifndef _NRF_GFX_FONTS_H_
#define _NRF_GFX_FONTS_H_

#include "system.h"

extern const char *days[7];

/// @brief  days index for the strings
enum _DAYS_INDEX_
{
    font_day_monday,
    font_day_tuesday,
    font_day_wednesday,
    font_day_thursday,
    font_day_friday,
    font_day_saturday,
    font_day_sunday

};

extern const char *months[12];
/// @brief the month index for the strings
enum _MONTHS_INDEX_
{
    font_month_jan,
    font_month_feb,
    font_month_march,
    font_month_april,
    font_month_june,
    font_month_july,
    font_month_august,
    font_month_september,
    font_month_october,
    font_month_november,
    font_month_december,
};

extern const uint8_t midFont[10][57];

extern const uint8_t small2Font[][22];

extern const uint8_t ascii_day_char_big_font[][8];

extern const uint8_t ascii_char[][5];

//// thsi value need to be seperated
#define ASCII_CHAR_ARR_OFFSET_VALUE 32

#endif