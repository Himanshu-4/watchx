#ifndef _KERNEL_TASK_H_
#define _KERNEL_TASK_H_


#include "system.h"

/// @brief the task preinit to initalise the task 
/// @param  void
void Kernel_task_preinit(void);

/// @brief to deinit the any pre initalise thinfs 
/// @param  void
void kernel_task_deinit(void);

#endif