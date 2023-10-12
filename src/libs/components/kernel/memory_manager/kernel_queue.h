#ifndef _KERNEL_FIFO_H_
#define _KERNEL_FIFO_H_

#include "system.h"

/// include the freertos file 
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "semphr.h"


#define KERNEL_QUEUE_SECTION __section(".kernel_fifo_queue_data")

#define KERNEL_QUEUE_INITED 0x12
#define KERNEL_QUEUE_DEINITED 0x22



/// @brief the error type of the queue
typedef enum _KERNEL_QUEUE_ERR_TYPE_
{
    kernel_q_op_suces =0,
    kernel_q_err_mutex_timeout,
    kernel_q_err_not_inited,
    kernel_q_err_invalid_param,
    kernel_q_err_match_failed,
    kernel_q_err_out_of_mem,

}kernel_q_err_type;


typedef __packed struct  kernel_Q_struct_instnace 
{
    uint8_t kernel_q_inited; 
    

    /// @brief memory pool information 
    uint8_t *mem_ptr;
    uint16_t mem_size;
    uint16_t used_size;

    SemaphoreHandle_t kernel_q_mutex_handle;
    uint16_t mutex_Timeout;



}kernel_q_instance;


#define KERNEL_Q_INST
//////////===========================================================================
///=========================   Function declarations ================================


kernel_q_err_type kernel_q_create();

kernel_q_err_type kernel_q_delete();

kernel_q_err_type kernel_q_reset();


#endif