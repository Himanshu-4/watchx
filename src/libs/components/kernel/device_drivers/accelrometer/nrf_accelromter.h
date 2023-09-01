#ifndef _NRF_ACCELROMETER_H_
#define _NRF_ACCELROMETER_H_

#include "system.h"


/// @brief these are the accelrometer events 
enum _ACCELROMETER_EVENTS_
{
    NRF_ACCEL_EVT_NONE,
    NRF_ACCEL_EVT_SINGLE_TAP,
    NRF_ACCEL_EVT_DOUBLE_TAP,
    NRF_ACCEL_EVT_FREEFALL,
    NRF_ACCEL_EVT_ACTIVITY,
    NRF_ACCEL_EVT_INACTIVITY,
    NRF_ACCEL_EVT_MAX_LIMIT,

};

/// @brief  these are the fifo events 
enum _ACCELROMETER_FIFO_EVENTS_
{
    NRF_ACCEL_EVT_FIFO_NONE,
    NRF_ACCEL_EVT_FIFO_DATARDY,
    NRF_ACCEL_EVT_FIFO_WATERMARK,
    NRF_ACCEL_EVT_FIFO_OVERRUT
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

/// @brief put the event in the evtq forcefully aka overwriting if the q is full 
/// @param evt 
/// @return succ/errcode
uint32_t nrf_accel_put_in_evtq(uint8_t evt);

/// @brief put the event in the event q forcefully and wait for the time if the q is full
/// @param evt 
/// @param time 
/// @return succ/failure 
uint32_t nrf_accel_put_in_evtq_wait(uint8_t evt, uint32_t time);

/// @brief this will pause the events 
/// @param  void 
void nrf_accel_pasue_events(void);

/// @brief this will resume the events 
/// @param  void 
void nrf_accel_resume_events(void);


/// @brief to get the axis of the event like if x axis triggers the event in activity or taps 
/// @param  void 
/// @return or combination axis of the event (x | Y | Z)
uint32_t nrf_accel_get_event_axis_type(void);


#endif