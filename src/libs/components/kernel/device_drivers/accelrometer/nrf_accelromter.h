#ifndef _NRF_ACCELROMETER_H_
#define _NRF_ACCELROMETER_H_

#include "adxl345_lib.h"

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
    NRF_ACCEL_WATERMARK_EVT,
    NRF_ACCEL_OVERRUN_EVT
};

/// these are the axis of the accelrometer where we get the events 
#define ACCEL_X_AXIS 0x10
#define ACCEL_Y_AXIS 0x20
#define ACCEL_Z_AXIS 0x30



#endif