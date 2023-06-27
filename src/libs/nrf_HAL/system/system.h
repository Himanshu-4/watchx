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
// #include "arm_math.h"

// include the library file for the register deifnations
#include "nrf52.h"
#include "nrf52_bitfields.h"
#include "nrf52832_peripherals.h"

////////// include the ARM  CMSIS files for core related operations
#include "cmsis_gcc.h"
#include "core_cm4.h"

/// include sysconfig file
#include "sys_config.h"

#if defined(FREERTOS_ENV)

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#endif


//// include the driver file from the nrfx driver API
// #include "nrfx_power.h"
// #include "nrf_drv_power.h"
// #include "nrf_drv_clock.h"

// #include "nrfx_clock.h"
// #include "nrfx_power.h"

#define SET 1
#define RESET 0

#define _BV(x) (1UL << (x))

#define LSB16(a) ((a) & 0xFF)
#define MSB16(a) (((a)>>8 )& 0xFF)

#define LSB32(a) ((a) & 0xFF)



#define MIN_OF(a,b) ((a)>(b)?(b):(a))
#define MAX_OF(a,b) ((a)>(b)?(a):(b))

#define s(x) sizeof(x)

#define u8(x) ((uint8_t *)&(x))

///////////// get the size of the buffer of uint32_t  from uint8_t buff
#define buff_size8_to_32(size) (((size)/4) + (((size)%4)==0?(0):(1)) )


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

////////////////////////////////////////////////////////////////////
//////////////// forcely inline 
#define FORCE_INLINE inline __attribute__((always_inline))
#define __NAKED __attribute__((naked))


/// errors for the anonymus  enum
enum _NRF_ERROR_TYPES_
{
    nrf_OK = 0x00UL,
    nrf_ERR_TIMEOUT,
    nrf_clock_init_failed,
    nrf_ERR_OCCURED,
    nrf_ERR_INVALID_PARAM,
    nrf_ERR_INVALID_STATE,
    nrf_ERR_OPERATION_FAILED,
    nrf_ERR_OUT_OF_MEM,
};

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

/**
 * @name driver_install
 * @brief used to intall the i2c , spi , uart drivers
 */
void driver_install(void);

/**
 * @name driver_uninstall
 * @brief used to uninstall all the drivers like SPI, I2c and UART
 */
void driver_uninstall(void);

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

////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/// @brief convert the asccii value to integer
/// @param str
/// @param stringlength
/// @return integer
int aToi(char *str , uint16_t len);

/// @brief reverse a string .
/// @param str
/// @param len
void reverse(char *str, int len);

/// @brief  convert a integer to a string
/// @param x
/// @param str
/// @param d
/// @return return the str
int intToStr(int x, char str[], int d);

/// @brief used to convert the float value in an ascii strinf to send it to uart
/// @param n
/// @param res
void ftoa(float n, char *res, int afterpoint);

/// @brief to get the power of the number 
/// @param base 
/// @param power 
/// @return result of the operation 
uint32_t pow(uint32_t base  , uint32_t power);


/**
 * @name error_handler 
 * @param error type
 * @param error string  
*/
void error_handler(uint32_t code, char const *function );



#define APP_ERROR_HANDLER(x)\
 error_handler( x, __FUNCTION__) \


#endif