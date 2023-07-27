#ifndef _NRF_RNG_H_
#define _NRF_RNG_H_

#include "system.h"


/// @brief this is to init the random genrator module 
/// @param  void 
void nrf_rng_init(void);

/// @brief this is to fill the buffer with random bytes 
/// @param buff 
/// @param size 
/// @return succ/failure of the function 
uint32_t nrf_rng_fill_buff(uint8_t *buff, uint8_t size);

#endif