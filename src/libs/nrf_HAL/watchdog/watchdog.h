#ifndef _WATCHDOG_H
#define _WATCHDOG_H

#include "system.h"

/** @brief WDT register reload value. */
#define WATCHDOG_RELOAD_VALUE       0x6E524635UL /* Fixed value; should not be modified. */

#define LSE_CLK 32768UL


/***
 * @name watchdog_init
 * @param the reload timer value
 * @note it only init the watchdog but not start it 
*/
void watchdog_init(uint16_t);

/***
 * @name watchdog_start
 * @note start the watchdog timer must call init first 
*/
void watchdog_start(void);


/***
 * @name watchdog_reset_counter
 * @note reset the watchdog timer before it reset the system
 * note that it reset the counter based on current value of crv
*/
void watchdog_reset(void);

/// @brief check if watchdog is stareted or not 
bool is_watchdog_started(void);
#endif