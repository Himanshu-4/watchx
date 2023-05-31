#ifndef _NRF_BUTTON_H_
#define _NRF_BUTTON_H_

#include "system.h"


/// @brief this is the enum about the button event q 
enum _BUTTON_EVENT_Q_
{
    NRF_BUTTON_NONE_EVT,
    NRF_BUTTON_UP_EVT,
    NRF_BUTTON_DOWN_EVT,
    NRF_BUTTON_MIDD_EVT,
    NRF_BUTTON_UP_MID_EVT,
    NRF_BUTTON_DOWN_MID_EVT
};

/// @brief this is to init the nrf button library for the watchx button functionality 
/// @param  void 
void nrf_button_evt_lib_init(void);

/// @brief this fuction is used to deinit the button event q 
/// @param  
void nrf_button_evt_lib_deinit(void);

/// @brief this function is to get the event from the button q 
/// @return event , 0 if no event present 
uint8_t nrf_get_btn_evtq(void);

/// @brief this function is to wait for the event q to have an event 
/// @param the wait time for tthe eventq to wait 
/// @return this will wait for the specified time and return 0 if the time expires as event dont happen
uint8_t nrf_get_btn_evtq_wait(uint32_t );

/// @brief this is to reset the btn event q 
/// @param  void 
void nrf_reset_btn_evtq(void);

/// @brief this is to put the data in the event q 
/// @param  the data to put in event q
/// @return succ/failure of the function
uint8_t nrf_put_in_btn_evtq(uint8_t );


#endif