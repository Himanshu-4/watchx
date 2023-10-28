#include "kernel_link_list.h"
#include "nrf_custom_log.h"

#define VERIFY_INIT(x)                                   \
    if ((x->linklist_inited != KERNEL_LINK_LIST_INITED)) \
    {                                                    \
        return KERNEL_LL_ERR_NOT_INITED;                 \
    }

/// @note please make sure that size% (sizeoofonenode + KERNEL_LINK_LIST_META_DATA_SIZE)  ==0
/// @brief this is the init function for the link list lib
/// @param instance
/// @param mem_inst
/// @param size
/// @param mutexbuff_ptr
/// @param timeout
/// @param sizeof_one_node
/// @return succ/failure
kernel_LL_err_type kernel_ll_init(kernel_linklist_instance *instance, uint8_t *mem_inst, uint16_t size, StaticSemaphore_t *mutexbuff_ptr, uint16_t timeout, uint16_t sizeof_one_node)
{
    /// verify that we get the total size
    if (size % sizeof_one_node != 0)
    {
        return KERNEL_LL_ERR_INVALID_PARAM;
    }
    
    instance->kernel_linklist_mutexhandle = xSemaphoreCreateMutexStatic(mutexbuff_ptr);
    NRF_ASSERT((instance->kernel_linklist_mutexhandle == NULL));
    instance->kernel_linklist_mutex_timeout = timeout;

    /// head pointer
    instance->head_ptr = NULL;
    instance->total_link_nodes = 0;

    /// size of the node
    instance->sizeof_one_node = sizeof_one_node;

    instance->mem_ptr = mem_inst;
    instance->total_mem_size = size;

    /// we have to explicitly memset the memory to zero as it is not done by startup file because not fall in .data or .bss section
    memset(mem_inst, 0, instance->total_mem_size);

    instance->linklist_inited = KERNEL_LINK_LIST_INITED;

    xSemaphoreGive(instance->kernel_linklist_mutexhandle);

    return nrf_OK;
}

/// @brief link list deinit structure
/// @param instnace
/// @return succ/failure
kernel_LL_err_type kernel_ll_deinit(kernel_linklist_instance *instnace)
{
    xSemaphoreGive(instnace->kernel_linklist_mutexhandle);
    memset(instnace->mem_ptr, 0, instnace->total_mem_size);
    // memset(&instnace, 0, sizeof(kernel_linklist_instance));
    return KERNEL_LL_OP_SUCESS;
}

/// @brief to add the data in the ll
/// @param inst
/// @param data
/// @param size
/// @return succ/err codes
kernel_LL_err_type kernel_ll_add_data(kernel_linklist_instance *inst, const uint8_t *data, uint16_t size)
{
    VERIFY_INIT(inst);

    // check that size is greater than one node size or there is space avalable in mem
    if ((inst->sizeof_one_node < size) ||
        ((inst->total_link_nodes * inst->sizeof_one_node) >= inst->total_mem_size))
    {
        return KERNEL_LL_ERR_OUT_OF_MEM;
    }

    if (xSemaphoreTake(inst->kernel_linklist_mutexhandle, inst->kernel_linklist_mutex_timeout) != pdPASS)
    {
        return KERNEL_LL_ERR_MUTEX_TIMEOUT;
    }

    kernel_ll_node *node = NULL;

    if (inst->head_ptr == NULL)
    {
        /// get the first node from the starting of memory
        /// @todo have to check that is this is evry time true
        inst->head_ptr = (kernel_ll_node *)inst->mem_ptr;
        node = inst->head_ptr;
    }
    else
    {
        ////step 1 serach for empty node
        uint16_t total_poss_node = inst->total_mem_size / inst->sizeof_one_node;
        kernel_ll_node_union temp_u;
        temp_u.empty_node = inst->mem_ptr;

        while (total_poss_node > 0)
        {
            if (temp_u.ll_node->next_link == NULL)
            {
                break;
            }
            temp_u.empty_node += inst->sizeof_one_node;
            total_poss_node--;
        }
        node = temp_u.ll_node;

        // step 2 search for the last link of ll
        kernel_ll_node *temp = (kernel_ll_node *)inst->head_ptr;
        while (temp->next_link != LINK_TERMINATED)
        {
            /// change the pointer to the next link
            temp = temp->next_link;
        }
        /// extend the lin node
        temp->next_link = node;
    }

    node->next_link = LINK_TERMINATED;
    memcpy(node->data, data, size);
    inst->total_link_nodes++;

    xSemaphoreGive(inst->kernel_linklist_mutexhandle);
    return nrf_OK;
}

/// @brief remove the data from the ll that match the paramter data
/// @param inst
/// @param start_index
/// @param data
/// @param size
/// @return succ/faliure
kernel_LL_err_type kernel_ll_remove_data(kernel_linklist_instance *inst, uint8_t start_index, const uint8_t *data, uint8_t size)
{

    VERIFY_INIT(inst);
    // check that size is greater than one node size or there is space avalable in mem
    if (inst->sizeof_one_node < size)
    {
        return KERNEL_LL_ERR_OUT_OF_MEM;
    }
    if (xSemaphoreTake(inst->kernel_linklist_mutexhandle, inst->kernel_linklist_mutex_timeout) != pdPASS)
    {
        return KERNEL_LL_ERR_MUTEX_TIMEOUT;
    }
    uint32_t ret = 0;

    kernel_ll_node *prev_link = NULL;
    kernel_ll_node *this_link = inst->head_ptr;

    if (this_link == NULL)
    {
        ret = KERNEL_LL_ERR_MATCH_FAILED;
        NRF_LOG_ERROR("ll empty");
        goto return_mech;
    }

    uint16_t total_nodes = inst->total_link_nodes;
    while (total_nodes > 0)
    {

        if (memcmp(this_link->data + start_index, data, size) == 0)
        {
            /// check if this is the head pointer
            if (this_link == inst->head_ptr)
            {
                // also chekck is this is the only node in the ll
                if (this_link->next_link == LINK_TERMINATED)
                {
                    //// make the head to zero
                    inst->head_ptr = NULL;
                    // __SIMD32() this is the simd instructiont
                }
                else
                {
                    inst->head_ptr = this_link->next_link;
                }
            }
            else
            {
                prev_link->next_link = this_link->next_link;
            }
            memset(this_link, 0, inst->sizeof_one_node);
            inst->total_link_nodes--;
            goto return_mech;
        }

        prev_link = this_link;
        this_link = this_link->next_link;
        total_nodes--;
    }

    ret = KERNEL_LL_ERR_MATCH_FAILED;

return_mech:
    xSemaphoreGive(inst->kernel_linklist_mutexhandle);
    return ret;
}

/// @brief this function is used to modify the data of the linklist node
/// @param inst
/// @param start_index
/// @param databuff
/// @param size
/// @param newdata
/// @return succ/failure
kernel_LL_err_type kernel_ll_modify_data(const kernel_linklist_instance *inst, uint8_t start_index, const uint8_t *databuff, uint8_t size, const uint8_t *newdata)
{
    VERIFY_INIT(inst);

    if (inst->sizeof_one_node < size)
    {
        return KERNEL_LL_ERR_OUT_OF_MEM;
    }
      /// serach for the data in the links
    kernel_ll_node *start = (kernel_ll_node *)inst->head_ptr;
    if (start == NULL)
    {
        return KERNEL_LL_ERR_EMPTY_LINKLIST;
    }

    if (xSemaphoreTake(inst->kernel_linklist_mutexhandle, inst->kernel_linklist_mutex_timeout) != pdPASS)
    {
        return KERNEL_LL_ERR_MUTEX_TIMEOUT;
    }

    uint32_t ret = 0;
  
    while (start != LINK_TERMINATED)
    {
        if (memcmp(databuff, (start->data + start_index), size) == 0)
        {
            /// modify  the data to the buffer
            memcpy(start->data, newdata, (inst->sizeof_one_node - KERNEL_LINK_LIST_META_DATA_SIZE));
            ret = KERNEL_LL_OP_SUCESS;
            goto return_mech;
        }
        start = start->next_link;
    }
    ret = KERNEL_LL_ERR_MATCH_FAILED;

return_mech:
    xSemaphoreGive(inst->kernel_linklist_mutexhandle);
    return ret;
}

/// @brief copy the data pointed by the match paramter
/// @param inst
/// @param start_index
/// @param data
/// @param size
/// @param copy_buff
/// @param copy_size
/// @return succ/fialure
kernel_LL_err_type kernel_ll_get_data(const kernel_linklist_instance *inst, uint8_t start_index, const uint8_t *data, uint8_t size, uint8_t *copy_buff, uint8_t copy_size)
{
    VERIFY_INIT(inst);

    if ((inst->sizeof_one_node < size) || (inst->sizeof_one_node < copy_size))
    {
        return KERNEL_LL_ERR_OUT_OF_MEM;
    }
    if (xSemaphoreTake(inst->kernel_linklist_mutexhandle, inst->kernel_linklist_mutex_timeout) != pdPASS)
    {
        return KERNEL_LL_ERR_MUTEX_TIMEOUT;
    }
    uint32_t ret = 0;

    /// serach for the data in the links
    kernel_ll_node *start = (kernel_ll_node *)inst->head_ptr;
    if (start == NULL)
    {
        ret = KERNEL_LL_ERR_EMPTY_LINKLIST;
        goto return_mech;
    }
    while (start != LINK_TERMINATED)
    {
        if (memcmp(data, (start->data + start_index), size) == 0)
        {
            /// copy the data to the buffer
            memcpy(copy_buff, start->data, copy_size);
            
            ret = KERNEL_LL_OP_SUCESS;
            goto return_mech;
        }
        start = start->next_link;
    }

    ret = KERNEL_LL_ERR_MATCH_FAILED;

return_mech:
    xSemaphoreGive(inst->kernel_linklist_mutexhandle);
    return ret;
}

/// @brief get the data pointer pointed to the node that have matched data
/// @param inst
/// @param start_index
/// @param data
/// @param size
/// @return pointer to that matched data
uint8_t *kernel_ll_get_data_ptr(const kernel_linklist_instance *inst, uint8_t start_index, const uint8_t *data, uint8_t size)
{
    if (inst->linklist_inited != KERNEL_LINK_LIST_INITED)
    {
        return NULL;
    }

    if (inst->sizeof_one_node < size)
    {
        return NULL;
    }
    uint8_t *data_ptr = NULL;
    /// serach for the data in the links
    kernel_ll_node *start = (kernel_ll_node *)inst->head_ptr;
    if (start == NULL)
    {
        return NULL;
    }

    while (start != LINK_TERMINATED)
    {
        if (memcmp(data, (start->data + start_index), size) == 0)
        {
            data_ptr = start->data;
            break;
        }
        start = start->next_link;
    }

    return data_ptr;
}

/// @brief to get the size used by the link list note that index starting from 1
/// @param inst
/// @param index
/// @return data pointer from the index
uint8_t *kernel_ll_get_data_from_index(const kernel_linklist_instance *inst, uint16_t index)
{
    if (inst->linklist_inited != KERNEL_LINK_LIST_INITED)
    {
        return NULL;
    }
    if (inst->total_link_nodes < index)
    {
        return NULL;
    }

    uint8_t *data_ptr = NULL;
    /// serach for the data in the links
    kernel_ll_node *start = (kernel_ll_node *)inst->head_ptr;
    if (start == NULL)
    {
        return NULL;
    }
    uint16_t ind = 0;
    while (start != LINK_TERMINATED)
    {
        if (ind == index)
        {
            data_ptr = start->data;
        }
        ind++;
        start = start->next_link;
    }

    return data_ptr;
}

/// @brief total number of nodes present (active ) in the ll
/// @param inst
/// @return nodes
uint16_t kernel_ll_get_total_nodes(const kernel_linklist_instance *inst)
{
    if (inst->linklist_inited != KERNEL_LINK_LIST_INITED)
    {
        return 0;
    }
    return inst->total_link_nodes;
}

/// @brief to get the size used by the link list
/// @param inst
/// @return size 0 if not inited or empty ll
uint16_t kernel_ll_get_size_used(const kernel_linklist_instance *inst)
{
    if (inst->linklist_inited != KERNEL_LINK_LIST_INITED)
    {
        return 0;
    }
    return (inst->total_link_nodes * (inst->sizeof_one_node));
}
