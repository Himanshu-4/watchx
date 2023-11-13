#ifndef _KERNEL_FIFO_H_
#define _KERNEL_FIFO_H_

#include "system.h"

/// include the freertos file 
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"




/**** +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * //++++++++++=================== Descriptions =================================
 *  we are using a circular buffer 
 * the Q created would first check if (total_mem_size)%sizeof_one_node == 0
 * the head(reader,front,faceindex,FIFO start) and the tail(writer,back,tailindex,FIFO end) is so aligned  
 * such that reading and writing can be done at any node . 
 * 
 * 
 * 
 * 
 * 
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#define KERNEL_QUEUE_SECTION __section(".kernel_queue_data")

#define KERNEL_QUEUE_INITED 0x12
#define KERNEL_QUEUE_DEINITED 0x22



/// @brief the error type of the queue
typedef enum _KERNEL_QUEUE_ERR_TYPE_
{
    kernel_q_op_suces =0,
    kernel_q_err_mutex_timeout,
    kernel_q_err_not_inited,
    kernel_q_err_invalid_param,
    kernel_q_err_out_of_mem,
    kernel_q_err_q_empty,
    kernel_q_err_q_full,


}kernel_q_err_type;


typedef struct  _KERNEL_Q_STRUCT_INSTACES_ 
{
    uint8_t kernel_q_inited; 

    /// @brief memory pool information 
    uint8_t *mem_ptr;
    uint16_t total_mem_size;
    uint16_t used_size;

    /// @brief mutex for the CPU bounded operation
    SemaphoreHandle_t kernel_q_mutex_handle;
    uint16_t mutex_Timeout;

    /// @brief indexing for the Q operations 
    uint16_t front_index;
    uint16_t back_index;

    /// @brief the size of one node would be in bytes 
    uint16_t size_of_node;
    
}kernel_q_instance;

/// @brief a=kernel_qinstance, b=q-memory c=q-memsize d=semphr
#define KERNEL_Q_INSTANTIATE(a,b,c,d)    \
    static uint8_t KERNEL_QUEUE_SECTION __USED b[c];    \
    static StaticSemaphore_t d; \
    static kernel_q_instance a; \

//////////===========================================================================
///=========================   Function declarations ================================

///======= Queue management API

/// @brief to create the q 
/// @param  q_instance
/// @param mem_inst 
/// @param size 
/// @param mutexbuff_ptr 
/// @param timeout 
/// @param size_of_one_node 
kernel_q_err_type kernel_q_create(kernel_q_instance * q_inst, uint8_t *mem_inst, uint16_t size, StaticSemaphore_t *mutexbuff_ptr, uint16_t timeout, uint16_t size_of_node);

/// @brief delete the q and its resources, freeing the memory 
/// @param  q_instance 
kernel_q_err_type kernel_q_delete(kernel_q_instance * q_inst);

/// @brief reset the Q and its resources, free the locks
/// @param q_inst 
kernel_q_err_type kernel_q_reset(const kernel_q_instance * q_inst);

/// @brief get the free size in the q
/// @param q_inst 
/// @return free size
uint16_t kernel_q_get_free_size(const kernel_q_instance * q_inst);

/// @brief get the used size in the q 
/// @param q_inst 
/// @return used size
uint16_t kernel_q_get_used_size(const kernel_q_instance *q_inst);

/// @brief get the total index present in the q , it will be usedsize/node_size
/// @param q_inst 
/// @return total_index present in the q  
uint16_t kernel_q_get_total_index(const kernel_q_instance *q_inst);

///====== Queue Data sending APIs

/// @brief send data to the back of the q 
/// @param  q_instance
/// @param src 
/// @return succ/failure
kernel_q_err_type kernel_q_send_to_back(kernel_q_instance * q_inst, const uint8_t *src,const uint16_t size);

/// @brief send data to the front of the q
/// @param q_inst 
/// @param src 
/// @return succ/failure
kernel_q_err_type kernel_q_send_to_front(kernel_q_instance *q_inst,const uint8_t *src,const uint16_t size);

#define kernel_q_send(a,b,c)    \
    kernel_q_send_to_back(a,b,c);    \

/// @brief recieve data from the front of the q
/// @param q_inst 
/// @param dest 
/// @return succ/failure
kernel_q_err_type kernel_q_recieve_from_front(kernel_q_instance * q_inst , uint8_t *dest,uint16_t size);

/// @brief recieve data from the back of the q 
/// @param q_inst 
/// @param dest 
/// @return succ/failure
kernel_q_err_type kernel_q_recieve_from_back(kernel_q_instance *q_inst, uint8_t *dest,uint16_t size);

#define Kernel_q_recieve(a,b,c) \
    kernel_q_recieve_from_front(a,b,c);   \


/// @brief get the data pointer from teh q 
/// @param q_inst 
/// @param index 
/// @return data ptr/NULL
uint8_t * kernel_q_get_Data_ptr(const kernel_q_instance *q_inst, uint16_t index);

/// @brief remove the particular index from the q 
/// @param q_inst 
/// @param index 
/// @return succ/failure
kernel_q_err_type kernel_q_remove_index(kernel_q_instance *q_inst, uint16_t index);

/// @brief change the data at the particular index
/// @param q_inst 
/// @param index 
/// @param data 
/// @return succ/failure
kernel_q_err_type kernel_q_modify_data_at_index(const kernel_q_instance * q_inst, uint8_t index, uint8_t *data,uint16_t size);


///=================================== end ===========================

#endif