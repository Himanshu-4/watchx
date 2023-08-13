#ifndef _KERNEL_TASK_H_
#define _KERNEL_TASK_H_


#include "system.h"

/// @brief this is the kernel state for the kernel task 
enum _KERNEL_TASK_STATES_
{
    kernel_state_idle =0,
    kernel_state_ble_disconnected,
    kernel_state_ble_connected,
    
    kernel_state_ble_notif_recv,
    kernel_state_ble_indic_recv,

    kernel_state_max_limit,
};


/// @brief the task preinit to initalise the task 
/// @param  void
void Kernel_task_preinit(void);

/// @brief to deinit the any pre initalise thinfs 
/// @param  void
void kernel_task_deinit(void);

#endif