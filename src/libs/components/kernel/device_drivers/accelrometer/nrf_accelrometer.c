#include "nrf_accelromter.h"

#include "adxl345_lib.h"

/// include the gpiote lib
#include "gpiote.h"

/// include the freertos implemenatation
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define NRF_ACCEL_EVENT_Q_LENGTH  ACCEL_EVENT_Q_LEN
#define NRF_ACCEL_EVENT_Q_ITEM_SIZE ACCEL_EVENT_Q_ITEM_SIZE

static StaticQueue_t xstatic_q_for_accel_events;
static uint8_t ucQueueStorageArea_for_acceleventq [NRF_ACCEL_EVENT_Q_LENGTH * NRF_ACCEL_EVENT_Q_ITEM_SIZE ];

static xQueueHandle nrf_accel_evtqhandle = NULL;



/// @brief this is the accelrometer event lib 
/// @param  void 
void nrf_accel_evt_lib_init(void)
{
    nrf_accel_evtqhandle = xQueueCreateStatic(NRF_ACCEL_EVENT_Q_LENGTH,
    NRF_ACCEL_EVENT_Q_ITEM_SIZE, ucQueueStorageArea_for_acceleventq, 
    &xstatic_q_for_accel_events);

    
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
