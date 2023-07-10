#include "kernel_mem_manager.h"

//// include the freertos library
#include "semphr.h"
#include "FreeRTOS.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief this is to init the kernel memory space, its mutex is inited separtely
/// @param kernelmem_instanace
/// @param memory buffer
/// @param size of the memory
/// @param instnace mutex buffer
/// @return succ/failure of the funcction
kernel_mem_err_type kernel_mem_init(kernel_mem_instance *kernel_inst_ptr, uint8_t *mem_inst, uint16_t size, StaticSemaphore_t *mutexbuffer_ptr, uint16_t timeout)
{
    kernel_inst_ptr->kernel_mem_mutex_handle = xSemaphoreCreateMutexStatic(mutexbuffer_ptr);

    kernel_inst_ptr->mutex_timeout = timeout;
    kernel_inst_ptr->mem_size = size;
    ///// give the mutex at starting
    xSemaphoreGive(kernel_inst_ptr->kernel_mem_mutex_handle);

    kernel_inst_ptr->mem_ptr = mem_inst;
    memset(mem_inst, 0, size);
    return KERNEL_MEM_OP_SUCCESS;
}


/// @brief this is to init the kernel memory space, its mutex is inited separtely
/// @param kernelmem_instanace
/// @param memory buffer
/// @param size of the memory
/// @param instnace mutex buffer
/// @return succ/failure of the funcction
kernel_mem_err_type kernel_mem_deinit(const kernel_mem_instance *kernel_inst_ptr)
{
    ///// give the mutex at starting
    xSemaphoreGive(kernel_inst_ptr->kernel_mem_mutex_handle); 
    memset(kernel_inst_ptr->mem_ptr, 0, kernel_inst_ptr->mem_size);
    return KERNEL_MEM_OP_SUCCESS;
}



/// @brief this function is to add the data in the uid
/// @param kernel mem instance
/// @param uid
/// @param data
/// @param size
/// @return succ/ err codes
kernel_mem_err_type kernel_mem_add_data(const kernel_mem_instance *instance, uint32_t uid, const uint8_t *data_ptr, uint16_t size)
{
    //// acquire the lock and start operations
    if (xSemaphoreTake(instance->kernel_mem_mutex_handle, pdMS_TO_TICKS(instance->mutex_timeout)) != pdPASS)
    {
        return KERNEL_MEM_ERR_MUTEX_TIMEOUT;
    }
    uint32_t err = KERNEL_MEM_OP_SUCCESS;
    //// check that if size exceed total avialable mem
    if (instance->mem_size <= size)
    {
        err = KERNEL_MEM_ERR_OUT_OF_MEM;
        goto return_mech;
    }

    uint8_t *arr = instance->mem_ptr;

    /// get the memory pointer to add the data;
    while (arr[KERNEL_MEM_STRUCT_SOD] == KERNEL_MEMORY_START_OF_DATA_DELIMITER)
    {
        kernel_mem_storage_type *my_inst = (kernel_mem_storage_type *)&arr[KERNEL_MEM_STRUCT_SOD];

        /// check that if this uid already present or not
        if (my_inst->kernel_UID == uid)
        {
            err = KERNEL_MEM_ERR_UID_ALRDY_PRESENT;
            goto return_mech;
        }

        /// get the len of the data and add it in the pointer
        // increment the arr to this length
        arr += (uint32_t)my_inst->kernel_len;
    }

    /// check that if after here the pending size can be enough to fit  this data
    uint16_t rem_size = instance->mem_size -
                        (uint16_t)(arr - instance->mem_ptr);

    if (rem_size <= size)
    {
        err = KERNEL_MEM_ERR_OUT_OF_MEM;
        goto return_mech;
    }

    kernel_mem_storage_type *data_store = (kernel_mem_storage_type *)&arr[KERNEL_MEM_STRUCT_SOD];

    data_store->kernel_sod = KERNEL_MEMORY_START_OF_DATA_DELIMITER;
    data_store->kernel_len = KERNEL_MEMORY_META_DATA_SIZE + size;
    data_store->kernel_UID = uid;

    // data_store->kernel_data[3];

    /// from here ownwards we can start copying the data
    // memcpy(&arr[KERNEL_MEM_STRUCT_DATA], data_ptr, size);
    memcpy(data_store->kernel_data, data_ptr, size);

return_mech:
    // give back the mutex
    xSemaphoreGive(instance->kernel_mem_mutex_handle);
    return err;
}

/// @brief this function is used to get the data pointer where the data start
/// @param mem_inst
/// @param uid
/// @param ptr
/// @return succ/ err codes
kernel_mem_err_type kernel_mem_get_Data_ptr(const kernel_mem_instance *instance, uint32_t uid, uint8_t *ptr)
{
    //// acquire the lock and start operations
    if (xSemaphoreTake(instance->kernel_mem_mutex_handle, pdMS_TO_TICKS(instance->mutex_timeout)) != pdPASS)
    {
        return KERNEL_MEM_ERR_MUTEX_TIMEOUT;
    }
    uint32_t err = KERNEL_MEM_OP_SUCCESS;

    uint8_t *arr = instance->mem_ptr;
  
    /// get the memory pointer to add the data;
    while (arr[KERNEL_MEM_STRUCT_SOD] == KERNEL_MEMORY_START_OF_DATA_DELIMITER)
    {
        kernel_mem_storage_type *my_inst = (kernel_mem_storage_type *)&arr[KERNEL_MEM_STRUCT_SOD];

        /// check that if this uid already present or not
        if (my_inst->kernel_UID == uid)
        {
            // give back the pointer
            ptr = &arr[KERNEL_MEM_STRUCT_DATA];
            goto return_mech;
        }

        /// get the len of the data and add it in the pointer
        // increment the arr to this length
        arr += (uint32_t)my_inst->kernel_len;
    }
    err = KERNEL_MEM_ERR_UID_ABSENT;

return_mech:
    // give back the mutex
    xSemaphoreGive(instance->kernel_mem_mutex_handle);
    return err;
}

/// @brief read the data size of the memory that is in the uid
/// @param mem_inst
/// @param uid
/// @param size pointer
/// @return succ/err codes
kernel_mem_err_type kernel_read_data_size(const kernel_mem_instance *instance, uint32_t uid, uint16_t *size)
{
    //// acquire the lock and start operations
    if (xSemaphoreTake(instance->kernel_mem_mutex_handle, pdMS_TO_TICKS(instance->mutex_timeout)) != pdPASS)
    {
        return KERNEL_MEM_ERR_MUTEX_TIMEOUT;
    }
    uint32_t err = KERNEL_MEM_OP_SUCCESS;

    uint8_t *arr = instance->mem_ptr;

    /// get the memory pointer to add the data;
    while (arr[KERNEL_MEM_STRUCT_SOD] == KERNEL_MEMORY_START_OF_DATA_DELIMITER)
    {
        kernel_mem_storage_type *my_inst = (kernel_mem_storage_type *)&arr[KERNEL_MEM_STRUCT_SOD];

        /// check that if this uid already present or not
        if (my_inst->kernel_UID == uid)
        {
            // give back the pointer
            *size = my_inst->kernel_len - KERNEL_MEMORY_META_DATA_SIZE;
            goto return_mech;
        }

        /// get the len of the data and add it in the pointer
        // increment the arr to this length as pointer are uint32_t type 
        arr += (uint32_t)my_inst->kernel_len;
    }
    err = KERNEL_MEM_ERR_UID_ABSENT;

return_mech:
    // give back the mutex
    xSemaphoreGive(instance->kernel_mem_mutex_handle);
    return err;
}

/// @brief this function is get the uid from the data pointer
/// @param buff
/// @param mem_ptr
/// @param uid pointer
/// @return succ/err codes
kernel_mem_err_type kernel_get_uid_from_pointer(const kernel_mem_instance *instance, uint8_t *mem_ptr, uint32_t *uid)
{
    //// acquire the lock and start operations
    if (xSemaphoreTake(instance->kernel_mem_mutex_handle, pdMS_TO_TICKS(instance->mutex_timeout)) != pdPASS)
    {
        return KERNEL_MEM_ERR_MUTEX_TIMEOUT;
    }
    uint32_t err = KERNEL_MEM_OP_SUCCESS;

    uint8_t *arr = instance->mem_ptr;

    /// get the memory pointer to add the data;
    while (arr[KERNEL_MEM_STRUCT_SOD] == KERNEL_MEMORY_START_OF_DATA_DELIMITER)
    {
        kernel_mem_storage_type *my_inst = (kernel_mem_storage_type *)&arr[KERNEL_MEM_STRUCT_SOD];

        /// check that if this uid already present or not
        if (my_inst->kernel_data == mem_ptr)
        {
            // give back the uid
            *uid = my_inst->kernel_UID;
            goto return_mech;
        }
        // increment the arr to this length
        arr += (uint32_t)my_inst->kernel_len;
    }
    err = KERNEL_MEM_ERR_UID_ABSENT;

return_mech:
    // give back the mutex
    xSemaphoreGive(instance->kernel_mem_mutex_handle);
    return err;
}

/// @brief to get the remaining data size from the kernel memory
/// @param kernel_inst_ptr
/// @return succ/err codes
kernel_mem_err_type kernel_get_rem_data_size(const kernel_mem_instance *instance, uint16_t *size)
{
    //// acquire the lock and start operations
    if (xSemaphoreTake(instance->kernel_mem_mutex_handle, pdMS_TO_TICKS(instance->mutex_timeout)) != pdPASS)
    {
        return KERNEL_MEM_ERR_MUTEX_TIMEOUT;
    }
    uint32_t err = KERNEL_MEM_OP_SUCCESS;

    uint8_t *arr = instance->mem_ptr;
    uint16_t len = 0;
    /// get the memory pointer to add the data;
    while (arr[KERNEL_MEM_STRUCT_SOD] == KERNEL_MEMORY_START_OF_DATA_DELIMITER)
    {
        kernel_mem_storage_type *my_inst = (kernel_mem_storage_type *)&arr[KERNEL_MEM_STRUCT_SOD];

        /// get the len of the data and add it in the pointer
        // increment the arr to this length
        arr += (uint32_t)my_inst->kernel_len;
    }
    /// check the  remaining  size
    *size = instance->mem_size -
            (uint16_t)(arr - instance->mem_ptr);

    // give back the mutex
    xSemaphoreGive(instance->kernel_mem_mutex_handle);
    return KERNEL_MEM_OP_SUCCESS;
}

/// @brief to get the used data size in the memory , the size occupied by the uids
/// @param kernel_inst_ptr
/// @param size
/// @return succ/err codes
kernel_mem_err_type kernel_get_used_data_size(const kernel_mem_instance *instance, uint16_t *size)
{
    //// acquire the lock and start operations
    if (xSemaphoreTake(instance->kernel_mem_mutex_handle, pdMS_TO_TICKS(instance->mutex_timeout)) != pdPASS)
    {
        return KERNEL_MEM_ERR_MUTEX_TIMEOUT;
    }
    uint32_t err = KERNEL_MEM_OP_SUCCESS;

    uint8_t *arr = instance->mem_ptr;
    uint16_t len = 0;
    /// get the memory pointer to add the data;
    while (arr[KERNEL_MEM_STRUCT_SOD] == KERNEL_MEMORY_START_OF_DATA_DELIMITER)
    {
        kernel_mem_storage_type *my_inst = (kernel_mem_storage_type *)&arr[KERNEL_MEM_STRUCT_SOD];
        
        /// get the len of the data and add it in the pointer
        // increment the arr to this length
        arr += (uint32_t)my_inst->kernel_len;
    }
    /// check the  remaining  size
    *size = (uint16_t)(arr - instance->mem_ptr);

    // give back the mutex
    xSemaphoreGive(instance->kernel_mem_mutex_handle);
    return KERNEL_MEM_OP_SUCCESS;
}

/// @brief to get the total no of the uids present in that memory space
/// @param kernel_inst_ptr
/// @param uid_total
/// @return succ/err codes
kernel_mem_err_type kernel_get_total_no_of_uids(const kernel_mem_instance *instance, uint16_t *uid_total)
{
    //// acquire the lock and start operations
    if (xSemaphoreTake(instance->kernel_mem_mutex_handle, pdMS_TO_TICKS(instance->mutex_timeout)) != pdPASS)
    {
        return KERNEL_MEM_ERR_MUTEX_TIMEOUT;
    }
    uint32_t err = KERNEL_MEM_OP_SUCCESS;

    uint8_t *arr = instance->mem_ptr;

    uint16_t total_uid = 0;
    /// get the memory pointer to add the data;
    while (arr[KERNEL_MEM_STRUCT_SOD] == KERNEL_MEMORY_START_OF_DATA_DELIMITER)
    {
        kernel_mem_storage_type *my_inst = (kernel_mem_storage_type *)&arr[KERNEL_MEM_STRUCT_SOD];
        /// get the len of the data and add it in the pointer
        // increment the arr to this length
        arr += (uint32_t)my_inst->kernel_len;
        total_uid++;
    }
    /// check the  remaining  size
    *uid_total = total_uid;

    // give back the mutex
    xSemaphoreGive(instance->kernel_mem_mutex_handle);
    return KERNEL_MEM_OP_SUCCESS;
}

/// @brief this function is to read the data from that uid , this function will copy the data from mem_buff to the requested buffer
/// @param mem_inst
/// @param uid
/// @param data
/// @param size
/// @return succ/err codes
kernel_mem_err_type kernel_mem_read_data(const kernel_mem_instance *instance, uint32_t uid, uint8_t *data, uint16_t size)
{
    //// acquire the lock and start operations
    if (xSemaphoreTake(instance->kernel_mem_mutex_handle, pdMS_TO_TICKS(instance->mutex_timeout)) != pdPASS)
    {
        return KERNEL_MEM_ERR_MUTEX_TIMEOUT;
    }
    uint32_t err = KERNEL_MEM_OP_SUCCESS;

    uint8_t *arr = instance->mem_ptr;
   
    /// get the memory pointer to add the data;
    while (arr[KERNEL_MEM_STRUCT_SOD] == KERNEL_MEMORY_START_OF_DATA_DELIMITER)
    {
        kernel_mem_storage_type *my_inst = (kernel_mem_storage_type *)&arr[KERNEL_MEM_STRUCT_SOD];

        /// check that if this uid already present or not
        if (my_inst->kernel_UID == uid)
        {
            /// copy the data
            memcpy(data, my_inst->kernel_data, MIN_OF(size, (my_inst->kernel_len - KERNEL_MEMORY_META_DATA_SIZE)));
            goto return_mech;
        }
        
        /// get the len of the data and add it in the pointer
        // increment the arr to this length
        arr += (uint32_t)my_inst->kernel_len;
    }

    err = KERNEL_MEM_ERR_UID_ABSENT;

return_mech:
    // give back the mutex
    xSemaphoreGive(instance->kernel_mem_mutex_handle);
    return err;
}

/// @brief this is to delete the data of that uid
/// @param mem_inst
/// @param uid
/// @return succ/err codes
kernel_mem_err_type kernel_mem_delete_data(const kernel_mem_instance *instance, uint32_t uid)
{
    //// acquire the lock and start operations
    if (xSemaphoreTake(instance->kernel_mem_mutex_handle, pdMS_TO_TICKS(instance->mutex_timeout)) != pdPASS)
    {
        return KERNEL_MEM_ERR_MUTEX_TIMEOUT;
    }
    uint32_t err = KERNEL_MEM_OP_SUCCESS;

    uint8_t *uid_match_start_ptr;
    uint8_t *uid_match_end_ptr;

    uint16_t rem_data_size = 0;
    uint16_t uid_data_size = 0;
    bool uid_found = 0;

    uint8_t *arr = instance->mem_ptr;
  
    /// get the memory pointer to add the data;
    while (arr[KERNEL_MEM_STRUCT_SOD] == KERNEL_MEMORY_START_OF_DATA_DELIMITER)
    {
        kernel_mem_storage_type *my_inst = (kernel_mem_storage_type *)&arr[KERNEL_MEM_STRUCT_SOD];

        if (uid_found)
        {
            rem_data_size += my_inst->kernel_len;
        }
        /// check that if this uid already present or not
        if (my_inst->kernel_UID == uid)
        {
            uid_found = 1;
            /// get the memory size to copy the content
            uid_match_start_ptr = &my_inst->kernel_sod;
            uid_match_end_ptr = uid_match_start_ptr + my_inst->kernel_len;
            uid_data_size = my_inst->kernel_len;
        }

        /// get the len of the data and add it in the pointer
        // increment the arr to this length
        arr += (uint32_t)my_inst->kernel_len;
    }

    if (uid_found)
    {
        /// there would be case where the rem_data_size would be 0 means that either there
        //// is only one uid or this target uid is the last uid , in that case we have to
        /// instead of copy the data make it zero

        if (rem_data_size == 0)
        {
            memset(uid_match_start_ptr, 0, uid_data_size);
        }
        else
        {
            /// copy the content to a temp memory
            uint8_t *temp = malloc(rem_data_size);
            if (temp == NULL)
            {
                err = KERNEL_MEM_ERR_OUT_OF_MEM;
                goto return_mech;
            }
            memcpy(temp, uid_match_end_ptr, rem_data_size);
            /// now copy the content before the start of that founded uid
            memcpy(uid_match_start_ptr, temp, rem_data_size);

            /// free the malloc memory
            free(temp);
        }
    }
    else
    {
        err = KERNEL_MEM_ERR_UID_ABSENT;
    }

return_mech:
    // give back the mutex
    xSemaphoreGive(instance->kernel_mem_mutex_handle);
    return KERNEL_MEM_OP_SUCCESS;
}

/// @brief this function is used to modify the data
/// @param mem_inst
/// @param uid
/// @param data
/// @param size
/// @return succ/err codes
kernel_mem_err_type kernel_mem_modify_data(const kernel_mem_instance *instance, uint32_t uid, const uint8_t *data, uint16_t size)
{
    //// acquire the lock and start operations
    if (xSemaphoreTake(instance->kernel_mem_mutex_handle, pdMS_TO_TICKS(instance->mutex_timeout)) != pdPASS)
    {
        return KERNEL_MEM_ERR_MUTEX_TIMEOUT;
    }
    uint32_t err = KERNEL_MEM_OP_SUCCESS;

    uint8_t *uid_match_start_ptr;
    uint8_t *uid_match_end_ptr;

    uint16_t rem_data_size = 0;
    uint16_t uid_data_size = 0;
    uint16_t prev_data_size =0;
    bool uid_found = 0;

    uint8_t *arr = instance->mem_ptr;
 
    /// get the memory pointer to add the data;
    while (arr[KERNEL_MEM_STRUCT_SOD] == KERNEL_MEMORY_START_OF_DATA_DELIMITER)
    {
        kernel_mem_storage_type *my_inst = (kernel_mem_storage_type *)&arr[KERNEL_MEM_STRUCT_SOD];

        if (uid_found)
        {
            rem_data_size += my_inst->kernel_len;
        }
        else 
        {
            prev_data_size += my_inst->kernel_len;
        }
        /// check that if this uid already present or not
        if (my_inst->kernel_UID == uid)
        {
            uid_found = 1;
            /// get the memory size to copy the content
            uid_match_start_ptr = &my_inst->kernel_sod;
            uid_match_end_ptr = uid_match_start_ptr + my_inst->kernel_len;
            uid_data_size = my_inst->kernel_len;
        }
        /// get the len of the data and add it in the pointer
              // increment the arr to this length
        arr += (uint32_t)my_inst->kernel_len;
    }

    if (uid_found)
    {
        /// there would be case where the rem_data_size would be 0 means that either there
        //// is only one uid or this target uid is the last uid , in that case we have to
        /// instead of copy the data make it zero

        if (rem_data_size == 0)
        {
            memset(uid_match_start_ptr, 0, uid_data_size);
        }
        else
        {
            /// copy the content to a temp memory
            uint8_t *temp = malloc(rem_data_size);
            if (temp == NULL)
            {
                err = KERNEL_MEM_ERR_OUT_OF_MEM;
                goto return_mech;
            }
            memcpy(temp, uid_match_end_ptr, rem_data_size);
            /// now copy the content before the start of that founded uid
            memcpy(uid_match_start_ptr, temp, rem_data_size);

            /// free the malloc memory
            free(temp);

        }

        // get the arr pointer ready 
        arr = instance->mem_ptr;
        arr += (prev_data_size - uid_data_size + rem_data_size);

        kernel_mem_storage_type *my_inst = (kernel_mem_storage_type *) arr;
        /// copy the meta data 
        my_inst->kernel_sod = KERNEL_MEMORY_START_OF_DATA_DELIMITER;
        my_inst->kernel_len = KERNEL_MEMORY_META_DATA_SIZE + size;
        my_inst->kernel_UID = uid;

        memcpy(my_inst->kernel_data, data, size);
    }
    else
    {
        err = KERNEL_MEM_ERR_UID_ABSENT;
    }

return_mech:
    // give back the mutex
    xSemaphoreGive(instance->kernel_mem_mutex_handle);
    return KERNEL_MEM_OP_SUCCESS;
}
