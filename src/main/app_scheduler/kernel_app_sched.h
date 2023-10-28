#ifndef _NRF_KERNEL_APP_SCHEDULER_H_
#define _NRF_KERNEL_APP_SCHEDULER_H_

#include "system.h"

/// @brief pre init the kernel app scheduler 
/// @param  void 
void kernel_app_schd_preinit(void);


uint32_t kernel_app_schd_get_mem_usage(void);

uint32_t kernel_app_schd_switch(void);

uint32_t kernel_app_schd_lock(void);

uint32_t kernel_app_schd_unlock(void);

uint32_t kernel_app_schd_get_status(void);


typedef void (*schd_app_Type)(void);

uint32_t kernel_app_schd_send_app(schd_app_Type new_apps);

schd_app_Type kernel_app_schd_get_current_App(void);

uint8_t kernel_app_schd_get_total_apps_in_q(void);

uint32_t kernel_app_schd_free_q(void);

schd_app_Type kernel_app_schd_get_app(uint8_t index);

#endif