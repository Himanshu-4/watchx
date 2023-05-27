#ifndef _BPROT_H
#define _BPROT_H

#include "system.h"

//// there are 128 protected blocks 

/****
 * @name protect_flash_block
 * @param block number 
 * @brief this will proteect the flash blcok from erase and return 
 * only debugger have access to write and erase the flash so dont set DISABLEDEBUG register
*/
void protect_flash_block(uint8_t );

/***
 * @name reset_flash_protect
 * @brief reset the micro so the register get to default state 0
*/
void reset_flash_protect(void);
#endif