#include "kernel_task.h"

/// include the driver common files 
#include "device_drivers/drivers_common/drivers_common.h"

/// @brief the task preinit to initalise the task 
/// @param  void
void Kernel_task_preinit(void)
{
    /// init the spi,i2c and gpio drivers 
    Hardware_drivers_install();
}

/// @brief to deinit the any pre initalise thinfs 
/// @param  void
void kernel_task_deinit(void)
{

}