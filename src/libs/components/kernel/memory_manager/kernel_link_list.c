#include "kernel_link_list.h"



/// @note please make sure that size% (sizeoofonenode + KERNEL_LINK_LIST_META_DATA_SIZE)  ==0 
/// @brief this is the init function for the link list lib 
/// @param instance 
/// @param mem_inst 
/// @param size 
/// @param mutexbuff_ptr 
/// @param timeout 
/// @param sizeof_one_node 
/// @return succ/failure 
kernel_LL_err_type kernel_ll_init(kernel_linklist_instance * instance, uint8_t *mem_inst, uint16_t size, StaticSemaphore_t *mutexbuff_ptr, uint16_t timeout, uint16_t sizeof_one_node)
{
    
}

/// @brief link list deinit structure 
/// @param instnace 
/// @return succ/failure 
kernel_LL_err_type kernel_ll_deinit(kernel_linklist_instance * instnace);

/// @brief to add the data in the ll 
/// @param inst 
/// @param data 
/// @param size 
/// @return succ/err codes 
kernel_LL_err_type kernel_ll_add_data(kernel_linklist_instance *inst , uint8_t *data, uint8_t size);

/// @brief remove the data from the ll that match the paramter data
/// @param inst 
/// @param start_index 
/// @param data 
/// @param size 
/// @return succ/faliure 
kernel_LL_err_type kernel_ll_remove_data(kernel_linklist_instance *inst , uint8_t start_index, uint8_t *data, uint8_t size);

/// @brief copy the data pointed by the match paramter 
/// @param inst 
/// @param start_index 
/// @param data 
/// @param size 
/// @param copy_buff 
/// @param copy_size 
/// @return succ/fialure 
kernel_LL_err_type kernel_ll_get_data(kernel_linklist_instance *inst, uint8_t start_index, uint8_t *data, uint8_t size, uint8_t *copy_buff, uint8_t copy_size);

/// @brief get the data pointer pointed to the node that have matched data 
/// @param inst 
/// @param start_index 
/// @param data 
/// @param size 
/// @return pointer to that matched data 
uint8_t * kernel_ll_get_data_ptr(kernel_linklist_instance *inst ,uint8_t start_index,uint8_t *data , uint8_t size );

/// @brief total number of nodes present (active ) in the ll
/// @param inst 
/// @return nodes 
uint16_t kernel_ll_get_total_nodes(kernel_linklist_instance *inst);

/// @brief to get the size used by the link list 
/// @param inst 
/// @return size 0 if not inited or empty ll 
uint16_t kernel_ll_get_size_used(kernel_linklist_instance *inst); 
