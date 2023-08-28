#ifndef _NRF_ACCELROMETER_H_
#define _NRF_ACCELROMETER_H_

#include "system.h"


/// @brief these are the accelrometer events 
enum _ACCELROMETER_EVENTS_
{
    NRF_ACCEL_NONE_EVT,
    NRF_ACCEL_SINGLE_TAP_EVT,
    NRF_ACCEL_DOUBLE_TAP_EVT,
    NRF_ACCEL_FREEFALL_EVT,
    NRF_ACCEL_ACTIVITY_EVT,
    NRF_ACCEL_INACTIVITY_EVT,

};

enum _ACCELROMETER_FIFO_EVENTS_
{
    NRF_ACCEL_FIFO_NONE_EVT,
    NRF_ACCEL_FIFO_DATARDY_EVT,
    NRF_ACCEL_FIFO_WATERMARK_EVT,
    NRF_ACCEL_FIFO_OVERRUN_EVT
};

/// these are the axis of the accelrometer where we get the events 
#define ACCEL_X_AXIS 0x10
#define ACCEL_Y_AXIS 0x20
#define ACCEL_Z_AXIS 0x30


/// @brief this is the accelrometer event lib 
/// @param  void 
void nrf_accel_evt_lib_init(void);

/// @brief this is the accelrometer lib deinit 
/// @param  void
void nrf_accel_evt_lib_deinit(void);

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


#endif