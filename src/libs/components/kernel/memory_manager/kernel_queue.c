#include "kernel_queue.h"

#define VERIFY_INIT(x)                                  \
  if ((q_inst->kernel_q_inited != KERNEL_QUEUE_INITED)) \
  {                                                     \
    return kernel_q_err_not_inited;                     \
  }

///======= Queue management API

/// @brief to create the q
/// @param  q_instance
/// @param mem_inst
/// @param size
/// @param mutexbuff_ptr
/// @param timeout
/// @param size_of_one_node
kernel_q_err_type kernel_q_create(kernel_q_instance *q_inst,
                                  uint8_t *mem_inst, uint16_t size, StaticSemaphore_t *mutexbuff_ptr,
                                  uint16_t timeout, uint16_t size_of_node)
{
  if (size % size_of_node != 0)
  {
    return kernel_q_err_invalid_param;
  }
  q_inst->kernel_q_mutex_handle = xSemaphoreCreateMutexStatic(mutexbuff_ptr);
  NRF_ASSERT(q_inst->kernel_q_mutex_handle == NULL);

  //// timeout of the
  q_inst->mutex_Timeout = timeout;

  /// assigning the memory
  q_inst->mem_ptr = mem_inst;
  q_inst->used_size = 0;
  q_inst->total_mem_size = size;

  q_inst->size_of_node = size_of_node;
  /// index of the q
  q_inst->front_index = 0;
  q_inst->back_index = 0;

  memset(mem_inst, 0, size);
  xSemaphoreGive(q_inst->kernel_q_mutex_handle);
  q_inst->kernel_q_inited = KERNEL_QUEUE_INITED;

  return kernel_q_op_suces;
}

/// @brief delete the q and its resources, freeing the memory
/// @param  q_instance
kernel_q_err_type kernel_q_delete(kernel_q_instance *q_inst)
{
  VERIFY_INIT(q_inst);

  memset(q_inst->mem_ptr, 0, q_inst->total_mem_size);

  xSemaphoreGive(q_inst->kernel_q_mutex_handle);
  vSemaphoreDelete(q_inst->kernel_q_mutex_handle);
  q_inst->kernel_q_mutex_handle = NULL;

  q_inst->kernel_q_inited = KERNEL_QUEUE_DEINITED;

  // memset();
  return kernel_q_op_suces;
}

/// @brief reset the Q and its resources, free the locks
/// @param q_inst
kernel_q_err_type kernel_q_reset(const kernel_q_instance *q_inst)
{
  VERIFY_INIT(q_inst);
  /// give the semaphore and reset the memory
  memset(q_inst->mem_ptr, 0, q_inst->total_mem_size);
  xSemaphoreGive(q_inst->kernel_q_mutex_handle);
  return kernel_q_op_suces;
}

/// @brief get the free size in the q
/// @param q_inst
/// @return free size
uint16_t kernel_q_get_free_size(const kernel_q_instance *q_inst)
{
  if (q_inst->kernel_q_inited != KERNEL_QUEUE_INITED)
  {
    return 0;
  }
  return (q_inst->total_mem_size - q_inst->used_size);
}

/// @brief get the used size in the q
/// @param q_inst
/// @return used size
uint16_t kernel_q_get_used_size(const kernel_q_instance *q_inst)
{
  if (q_inst->kernel_q_inited != KERNEL_QUEUE_INITED)
  {
    return 0;
  }
  return q_inst->used_size;
}

/// @brief get the total index present in the q , it will be usedsize/node_size
/// @param q_inst
/// @return total_index present in the q
uint16_t kernel_q_get_total_index(const kernel_q_instance *q_inst)
{
  if (q_inst->kernel_q_inited != KERNEL_QUEUE_INITED)
  {
    return 0;
  }
  return (q_inst->used_size / q_inst->size_of_node);
}

//========== sending API =========================================================
///====== Queue Data sending APIs

/// @brief send data to the back of the q
/// @param  q_instance
/// @param src
/// @return succ/failure
kernel_q_err_type kernel_q_send_to_back(kernel_q_instance *q_inst, const uint8_t *src, const uint16_t size)
{
  VERIFY_INIT(q_inst);

  /// first check is there any space available
  if (q_inst->total_mem_size == q_inst->used_size)
  {
    return kernel_q_err_out_of_mem;
  }

  if ((src == NULL) || (size == 0))
  {
    return kernel_q_err_invalid_param;
  }
  /// take mutex
  if (xSemaphoreTake(q_inst->kernel_q_mutex_handle, q_inst->mutex_Timeout) != pdPASS)
  {
    return kernel_q_err_mutex_timeout;
  }

  //// check that is back is at the end of memory
  if (q_inst->back_index == q_inst->total_mem_size)
  {
    q_inst->back_index = 0;
  }
  //// copy new data
  memcpy(&q_inst->mem_ptr[q_inst->back_index], src, MIN_OF(size, q_inst->size_of_node));
  /// increment back index
  q_inst->back_index += q_inst->size_of_node;

  /// increment the size
  q_inst->used_size += q_inst->size_of_node;

  //// give the mutex back
  xSemaphoreGive(q_inst->kernel_q_mutex_handle);
  return 0;
}

/// @brief send data to the front of the q
/// @param q_inst
/// @param src
/// @return succ/failure
kernel_q_err_type kernel_q_send_to_front(kernel_q_instance *q_inst, const uint8_t *src, const uint16_t size)
{
  VERIFY_INIT(q_inst);
  /// first check is there any space available
  if (q_inst->total_mem_size == q_inst->used_size)
  {
    return kernel_q_err_out_of_mem;
  }

  if ((src == NULL) || (size == 0))
  {
    return kernel_q_err_invalid_param;
  }
  /// take mutex
  if (xSemaphoreTake(q_inst->kernel_q_mutex_handle, q_inst->mutex_Timeout) != pdPASS)
  {
    return kernel_q_err_mutex_timeout;
  }
  /// move the index
  if (q_inst->front_index == 0)
  {
    // q_inst->front_ahead_back =1;
    q_inst->front_index = q_inst->total_mem_size - q_inst->size_of_node;
  }
  else
  {
    q_inst->front_index -= q_inst->size_of_node;
  }
  memcpy(&q_inst->mem_ptr[q_inst->front_index], src, MIN_OF(size, q_inst->size_of_node));

  /// increment the size
  q_inst->used_size += q_inst->size_of_node;

  //// give the mutex back
  xSemaphoreGive(q_inst->kernel_q_mutex_handle);
  return 0;
}

//================= reciveing API ============================================
/// @brief recieve data from the front of the q
/// @param q_inst
/// @param dest
/// @return succ/failure
kernel_q_err_type kernel_q_recieve_from_front(kernel_q_instance *q_inst, uint8_t *dest, uint16_t size)
{
  VERIFY_INIT(q_inst);
  /// first check is there any data available
  if (0 == q_inst->used_size)
  {
    return kernel_q_err_q_empty;
  }
  if ((dest == NULL))
  {
    return kernel_q_err_invalid_param;
  }
  /// take mutex
  if (xSemaphoreTake(q_inst->kernel_q_mutex_handle, q_inst->mutex_Timeout) != pdPASS)
  {
    return kernel_q_err_mutex_timeout;
  }

  /// copy the data
  memcpy(dest, &q_inst->mem_ptr[q_inst->front_index], MIN_OF(size, q_inst->size_of_node));

  /// after change the front index
  q_inst->front_index += q_inst->size_of_node;
  /// check after modify
  if (q_inst->front_index == q_inst->total_mem_size)
    q_inst->front_index = 0;

  /// increment the size
  q_inst->used_size -= q_inst->size_of_node;

  //// give the mutex back
  xSemaphoreGive(q_inst->kernel_q_mutex_handle);
  return 0;
}

/// @brief recieve data from the back of the q
/// @param q_inst
/// @param dest
/// @return succ/failure
kernel_q_err_type kernel_q_recieve_from_back(kernel_q_instance *q_inst, uint8_t *dest, uint16_t size)
{
  VERIFY_INIT(q_inst);
  /// first check is there any data available
  if (0 == q_inst->used_size)
  {
    return kernel_q_err_q_empty;
  }
  if ((dest == NULL))
  {
    return kernel_q_err_invalid_param;
  }
  /// take mutex
  if (xSemaphoreTake(q_inst->kernel_q_mutex_handle, q_inst->mutex_Timeout) != pdPASS)
  {
    return kernel_q_err_mutex_timeout;
  }

  /// after change the front index
  q_inst->back_index -= q_inst->size_of_node;

  /// copy the data
  memcpy(dest, &q_inst->mem_ptr[q_inst->back_index], MIN_OF(size, q_inst->size_of_node));

  /// check after modify
  if (0 == q_inst->back_index)
    q_inst->back_index = q_inst->total_mem_size;

  /// increment the size
  q_inst->used_size -= q_inst->size_of_node;

  //// give the mutex back
  xSemaphoreGive(q_inst->kernel_q_mutex_handle);
  return 0;
}

// //======================== more API ===========================================

/// @brief get the data pointer from teh q
/// @param q_inst
/// @param index
/// @return data ptr/NULL
uint8_t *kernel_q_get_Data_ptr(const kernel_q_instance *q_inst, uint16_t index)
{
  if (q_inst->kernel_q_inited != KERNEL_QUEUE_INITED)
  {
    return NULL;
  }
  if (q_inst->used_size == 0)
    return NULL;
  /// check invalid index
  if ((index <= 0) || (index > (q_inst->used_size / q_inst->size_of_node) ))
    return NULL;

  uint8_t *ptr = NULL;

  if (q_inst->front_index >= q_inst->back_index)
  {
    /// get the index between front_index -> memend , 0->backindex
    uint16_t ff_index = (q_inst->total_mem_size - q_inst->front_index) / q_inst->size_of_node;
    //  uint16_t bb_index = (q_inst->back_index)/q_inst->size_of_node;

    if (index <= ff_index)
    {
      ptr = &q_inst->mem_ptr[q_inst->front_index + ((index - 1) * q_inst->size_of_node)];
    }
    else
    {
      ptr = &q_inst->mem_ptr[(index - ff_index - 1) * q_inst->size_of_node];
    }
  }
  else
  {
    // we have to only do one way search only
    ptr = &q_inst->mem_ptr[q_inst->front_index + ((index - 1) * q_inst->size_of_node)];
  }
  return ptr;
}

/// @brief remove the particular index from the q
/// @param q_inst
/// @param index
/// @return succ/failure
kernel_q_err_type kernel_q_remove_index(kernel_q_instance *q_inst, uint16_t index)
{
  VERIFY_INIT(q_inst);
  if (q_inst->kernel_q_inited != KERNEL_QUEUE_INITED)
  {
    return kernel_q_err_not_inited;
  }
  if (q_inst->used_size == 0)
    return kernel_q_err_q_empty;
  /// check invalid index
  if ((index <= 0) || ( index > (q_inst->used_size / q_inst->size_of_node) ))
    return kernel_q_err_invalid_param;

  if (xSemaphoreTake(q_inst->kernel_q_mutex_handle, q_inst->mutex_Timeout) != pdPASS)
  {
    return kernel_q_err_mutex_timeout;
  }  
  /// have to check if
  if (q_inst->front_index >= q_inst->back_index)
  {
    uint16_t ff_index = (q_inst->total_mem_size - q_inst->front_index) / q_inst->size_of_node;
    uint16_t bb_index = (q_inst->back_index)/q_inst->size_of_node;

    /// check if we get the data in the front of front index
    if(index <= ff_index)
    {
      /// find the copy size 
      uint16_t copy_size = (ff_index - index)*q_inst->size_of_node;
       uint8_t *new_ptr = &q_inst->mem_ptr[q_inst->front_index + (index - 1) * q_inst->size_of_node];
        uint8_t *old_ptr =  &q_inst->mem_ptr[q_inst->front_index + (index * q_inst->size_of_node)];
      //// shift the data to the specific positions 
      memcpy(new_ptr,old_ptr,copy_size);
      /// copy last node of front index 
      memcpy(&q_inst->mem_ptr[q_inst->total_mem_size-q_inst->size_of_node], q_inst->mem_ptr,q_inst->size_of_node);

      /// shift the whole array to left 
      memcpy(q_inst->mem_ptr,&q_inst->mem_ptr[q_inst->size_of_node],(bb_index-1)*q_inst->size_of_node);
    }
    else 
    { 
        uint8_t * new_ptr = &q_inst->mem_ptr[(index-1-ff_index) * q_inst->size_of_node];
        uint8_t * old_ptr = &q_inst->mem_ptr[(index-ff_index) * q_inst->size_of_node];
        uint16_t copy_size = (bb_index- (index -ff_index)) * q_inst->size_of_node;
        memcpy(new_ptr,old_ptr,copy_size);
    }
  }
  else
  {
    /// we have to shift back the data
      uint16_t copy_size = ((q_inst->used_size / q_inst->size_of_node) - index) * q_inst->size_of_node;
     uint8_t *new_ptr = &q_inst->mem_ptr[q_inst->front_index + (index - 1) * q_inst->size_of_node];
      uint8_t *old_ptr =  &q_inst->mem_ptr[q_inst->front_index + (index * q_inst->size_of_node)];
      memcpy(new_ptr,old_ptr,copy_size);
  }

  q_inst->back_index -= q_inst->size_of_node;
  if(0 == q_inst->back_index )
  {
    q_inst->back_index = q_inst->total_mem_size;
  }
  q_inst->used_size -= q_inst->size_of_node;

   //// give the mutex back
  xSemaphoreGive(q_inst->kernel_q_mutex_handle);
  return 0;
}

/// @brief change the data at the particular index
/// @param q_inst
/// @param index
/// @param data
/// @return succ/failure
kernel_q_err_type kernel_q_modify_data_at_index(const kernel_q_instance *q_inst, uint8_t index, uint8_t *data, uint16_t size)
{
  VERIFY_INIT(q_inst);
  if (q_inst->kernel_q_inited != KERNEL_QUEUE_INITED)
  {
    return kernel_q_err_not_inited;
  }
  if (q_inst->used_size == 0)
    return kernel_q_err_q_empty;
  /// check invalid index
  if ((index <= 0) || (index > (q_inst->used_size / q_inst->size_of_node)))
    return kernel_q_err_invalid_param;

  if (xSemaphoreTake(q_inst->kernel_q_mutex_handle, q_inst->mutex_Timeout) != pdPASS)
  {
    return kernel_q_err_mutex_timeout;
  } 

  uint8_t *ptr = NULL;

  if (q_inst->front_index >= q_inst->back_index)
  {
    /// get the index between front_index -> memend , 0->backindex
    uint16_t ff_index = (q_inst->total_mem_size - q_inst->front_index) / q_inst->size_of_node;
    //  uint16_t bb_index = (q_inst->back_index)/q_inst->size_of_node;

    if (index <= ff_index)
    {
      ptr = &q_inst->mem_ptr[q_inst->front_index + ((index - 1) * q_inst->size_of_node)];
    }
    else
    {
      ptr = &q_inst->mem_ptr[(index - ff_index - 1) * q_inst->size_of_node];
    }
  }
  else
  {
    // we have to only do one way search only
    ptr = &q_inst->mem_ptr[q_inst->front_index + ((index - 1) * q_inst->size_of_node)];
  }

  /// copy the data at the founded index
  memcpy(ptr, data, MIN_OF(size, q_inst->size_of_node));

   //// give the mutex back
  xSemaphoreGive(q_inst->kernel_q_mutex_handle);
  return 0;
}


