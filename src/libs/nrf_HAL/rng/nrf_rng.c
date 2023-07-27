#include "nrf_ran_gen.h"

static bool rng_init_state =0;

/// @brief this is to init the random genrator module
/// @param  void
void nrf_rng_init(void)
{
    rng_init_state =1;

    /// we dont want interrupt from rng
    NRF_RNG->INTENCLR = 1;

    /// config rng for bias correction
    NRF_RNG->CONFIG = 1;

    /// short register  bw event_Data_Ready and stop task
    NRF_RNG->SHORTS = 1;
}


//// @todo have to put a delay or wait function so that it cant stuck in the loop or give a err code 

/// @brief this is to fill the buffer with random bytes
/// @param buff
/// @param size
/// @return succ/failure of the function
uint32_t nrf_rng_fill_buff(uint8_t *buff, uint8_t size)
{
    /// check the init state 
    if(rng_init_state !=1 )
    {return nrf_ERR_INVALID_STATE;}
        /// clear the event 
        NRF_RNG->EVENTS_VALRDY =0;
   
    for (uint8_t i = 0; i < size; i++)
    {
        //// fill the buffer with the random numbers

        NRF_RNG->TASKS_START = 1;

        while (NRF_RNG->EVENTS_VALRDY != 1)
        {
            __NOP();
        }
        /// clear the event 
        NRF_RNG->EVENTS_VALRDY =0;

        /// copy the data in the buffer
        buff[i] = NRF_RNG->VALUE;
    }

    return nrf_OK;
}
