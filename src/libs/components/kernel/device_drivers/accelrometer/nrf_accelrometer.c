#include "nrf_accelromter.h"

#include "adxl345_lib.h"

/// @brief this is the accelrometer event lib 
/// @param  void 
void nrf_accel_evt_lib_init(void)
{

}

/// @brief this is the accelrometer lib deinit 
/// @param  void
void nrf_accel_evt_lib_deinit(void)
{

}

/// @brief this is to get event from accelrometer evtq 
/// @param void
/// @return event 
uint32_t nrf_accel_get_evtq(void);

/// @brief this is the get event from accelrometer evtq in wait state
/// @param time
/// @return event 
uint32_t nrf_accel_get_evtq_wait(uint32_t time);

/// @brief this is to reset the event q from the accelrometer 
/// @param void 
void nrf_accel_reset_evtq(void);

/// @brief 
/// @param evt 
/// @return 
uint32_t nrf_accel_put_in_evtq(uint8_t evt);

/// @brief 
/// @param  
void nrf_accel_pasue_events(void);

/// @brief 
/// @param  
void nrf_accel_resume_events(void);
