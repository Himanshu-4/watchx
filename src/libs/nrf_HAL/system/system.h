#ifndef _SYSTEM_H_
#define _SYSTEM_H_

/// include standard library
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// # include the  math library for arm math operations
// #include <math.h>
#include "arm_math.h"

// include the library file for the register deifnations
#include "nrf52.h"
#include "nrf52_bitfields.h"
#include "nrf52832_peripherals.h"

#include "app_util_platform.h"

////////// include the ARM  CMSIS files for core related operations
#include "cmsis_gcc.h"
#include "core_cm4.h"

/// include sysconfig file
#include "sys_config.h"


////---------------- include the err file 
#include "nrf_err_types.h"


//======================================== Bits/Bytes banding macros ===================================
///-----------------------------------------------------------------------------------------------------

#define SET 1
#define RESET 0

#define _BV(x) (1UL << (x))

#define SETBIT(x,y) ((x) |= (y))
#define READBIT(x,y) (((x)>>(y)) & 0x01)
#define CLEARBIT(x,y) ( (x) &= ~(1<<(y)))

#define LSB16(a) ((a) & 0xFF)
#define MSB16(a) (((a)>>8 )& 0xFF)

/// @brief this is used to extract the bytes from a 32 byte number 
#define LSB32(a) ((a) & 0xFF)
#define MIDB32(a) (((a)>>8) & 0xFF)
#define MID_B32(a) (((a)>>16) & 0xFF)
#define MSB32(a) (((a)>>24) & 0xFF)

#define MIN_OF(a,b) ((a)>(b)?(b):(a))
#define MAX_OF(a,b) ((a)>(b)?(a):(b))

#define s(x) sizeof(x)

#define u8(x) ((uint8_t *)&(x))

#define u8_ptr (uint8_t *)

/// @brief modulus function map to positive 
#define MOD(x) ((x>=0)?((x)):(-(x)))

#define NEGLECT_ZERO(x,y) (((x-y) == 0)? (1): (MOD((x-y))))

///////////// get the size of the buffer of uint32_t  from uint8_t buff
// #define buff_size8_to_32(size) (((size)/4) + (((size)%4)==0?(0):(1)) )

#define GET_NO_OF_PERFECT_DIVIDE(input,div)  (((input)/(div)) + (((input)%(div))?(1):(0)))

#define GET_REMNDER(divident,divisor) (((divident)%(divisor)))
//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
///////// delay APIs defined in the time.h
extern void delay_milisec(uint16_t delay_time);
extern void delay_microsec(uint16_t delay_time);

#define nrf_delay_ms(x) (delay_milisec(x))
#define nrf_delay_us(x) (delay_microsec(x))



// the delay API

// if freertos is defined than use the vtaskdelay , if not than use nrf_delay time API
#if defined(FREERTOS_ENV)
// the RTOS tick rate
#define rtos_tick_rate (1000 / configTICK_RATE_HZ)

#define delay(x) (vTaskDelay(x / rtos_tick_rate))

#else

#define delay(x) (delay_milisec(x))
#endif


#define U32_MAX (0xFFFFFFFFUL)
#define U16_MAX (0xFFFFU)

#define U_0 (0UL)

#define NO_OF_NIBBLE_BITS 4 
#define LOWER_NIBBLE_MASK 0x0f
#define HIGHER_NIBBLE_MASK 0xf0

////////////////////////////////////////////////////////////////////
//////////////// compiler attributes 

#define __FORCE_INLINE inline __attribute__((always_inline))
#define __NAKED __attribute__((naked))
#define __NO_OPTIMISATION __attribute__((optimize("O0")))
#define __MAX_OPTIMISATION __attribute__((optimize("O3")))



#define ERR_CHK(x)         \
    do                     \
    {                      \
        uint8_t val = x;   \
        if (val != nrf_OK) \
            return val;    \
    } while (0)


// #define NRF_LOG_ERROR(str, ...)    printf(BRED str  "\r\n"   ANSI_RESET, ##__VA_ARGS__)

// #define CALL_FUNC(x, ...)     
// if(x != NULL)   
// {x(##__VA_ARGS__);} 

/// @brief this will map the input to a particular slope y2 - max out y1 - min output , x1 = min input x2 max input 
///  Linear mapping function
__STATIC_INLINE double map_d(double input, double input_min, double input_max, double output_min, double output_max) {
    // Ensure that the input value is within the input range
    if (input < input_min) {
        input = input_min;
    } else if (input > input_max) {
        input = input_max;
    }
    // Calculate the mapped value
    double mapped_value = ((input - input_min)  * (output_max - output_min))/(input_max - input_min) + output_min;

    return mapped_value;
}


/// @brief this will map the input to a particular slope y2 - max out y1 - min output , x1 = min input x2 max input 
///  Linear mapping function
__STATIC_INLINE uint32_t map_i(uint32_t input, uint32_t input_min, uint32_t input_max, uint32_t output_min, uint32_t output_max) 
{
    // Ensure that the input value is within the input range
    if (input < input_min) {
        input = input_min;
    } else if (input > input_max) {
        input = input_max;
    }
    // Calculate the mapped value
    uint32_t mapped_value = ((input - input_min)  * (output_max - output_min))/(input_max - input_min) + output_min;

    return mapped_value;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

/****
 * @name sys_init
 * @param void
 * @return check and verify if system init is sucesfull
 * @return true or false
 */
bool sys_init(void);


/*****
 * @name system_soft_reset
 * @brief reset the microcontroller via the soft reset technique
 * aka NVIC_sysytem_Reset()
 */
void __NO_RETURN system_soft_reset(void);



/***
 * @name system_shutdown 
 * @param void
 * @note used to shutdown the system
*/
void system_shutdown(void);



/// @brief this you can get the reset reason of the chip 
enum _RESET_RESAONS_ 
{
    RESET_D2_NONE =0x00,
    RESET_D2_RESETPIN =0x01, 
    RESET_D2_WATCHDOG,
    RESET_D2_SOFTRESET,
    RESET_D2_CPU_LOCKUP,
    RESET_D2_SYSOFF_GPIODETECT,
    RESET_D2_SYSOFF_LPCOMP,
    RESET_D2_SYSOFF_DEBUGINF,
    RESET_D2_SYSOFF_NFC_FLD,

};


/// @brief this function is to get the reset reason of the chip 
/// @param  
/// @return return the number of the reset 
uint32_t get_reset_reason(void);

/****
 * @name check_assrt
 * @param trueorfalse 
 * @param srting to print when there is a failure 
 * @note better to use this instead of assert 
*/
void check_assrt(uint32_t , const char *, uint32_t line);

// /// @brief the system delay API must be defined later
// /// @param x
// static inline void system_delayms( uint32_t x)
// {
//     while (x-- > 1)
//     {
//         __NOP();

//     }

// }

#ifdef NRF_ASSERT_WITH_RETURN_ON_FAIL

#define NRF_ASSERT(x) \
check_assrt(x, __FUNCTION__ , __LINE__ ); \
return nrf_ERR_OPERATION_FAILED \

#else 

#define NRF_ASSERT(x) \
check_assrt(x, __FUNCTION__ , __LINE__ ) \

#endif

/// @brief this is a constanr mem buff type 
typedef PACKED_STRUCT _CONST_BUFFER_TYPE_
{
    const uint8_t * ptr;
    uint16_t size;
}const_memory_buff;

/// @brief this is the memory buffer for pointing to a buffer and size 
typedef PACKED_STRUCT _BUFFER_TYPE_ 
{
    uint8_t *buff;
    uint16_t size;
}memory_buff;

////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/// @brief convert the asccii value to integer
/// @param str
/// @param stringlength
/// @return integer
int str_to_int(char const  *str , uint16_t len);

/// @brief reverse a string .
/// @param str
/// @param len
void reverse(char *str, int len);

/// @brief  convert a integer to a string
/// @param x
/// @param str
/// @param d
/// @return return the str
int int_to_str(int x, char str[], int d);

/// @brief used to convert the float value in an ascii strinf to send it to uart
/// @param n
/// @param res
void ftoa(float n, char *res, int afterpoint);

///// @todo this algo is not working , 
/// @brief convert a string to float 
/// @param str 
/// @return float value 
float str_to_float(char const *str , uint16_t len);

/// @brief to get the power of the number 
/// @param base 
/// @param power 
/// @return result of the operation 
uint32_t POWER(uint32_t base  , uint32_t power);


/**
 * @name error_handler 
 * @param error type
 * @param error string  
*/
void error_handler(uint32_t code, char const *function );



#define APP_ERROR_HANDLER(x)\
 error_handler( x, __FUNCTION__) \


#endif