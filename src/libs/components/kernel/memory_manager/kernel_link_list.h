#ifndef _KERNEL_LINK_LIST_H_
#define _KERNEL_LINK_LIST_H_

#include "system.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#define KERNEL_LINK_LIST_SECTION __section(".kernel_linklist_data")

#define KERNEL_LINK_LIST_INITED 0x10
#define KERNEL_LINK_LIST_DEINITED 0x20


__PACKED_STRUCT kernel_linklist_node_type 
{
    struct kernel_linklist_node_type *next_link;
    uint8_t data[1];  
    /// 1 is only the placeholder for compilation , actual size is @sizeof_one_node
};

typedef struct kernel_linklist_node_type kernel_ll_node;

/// @brief this is the link list instnace that is unique for every instnace in differnt scopes 
typedef struct _KERNEL_LINKLIST_INST_
{
    kernel_ll_node *head_ptr;
    uint8_t linklist_inited;

    /// @brief size of the link list 
    /// @note the size of one node is including the linklist meta node size 
    uint16_t total_link_nodes;
    uint16_t sizeof_one_node; 

    /// @brief link list memory size allocate for it 
    uint8_t *mem_ptr;
    uint16_t total_mem_size;

    /// @brief kernel_linklist_mutex info 
    SemaphoreHandle_t kernel_linklist_mutexhandle;
    uint16_t kernel_linklist_mutex_timeout;

} kernel_linklist_instance;


#define KERNEL_LINKLIST_INSTANTISE(a,b,c,d) \
    static uint8_t KERNEL_LINK_LIST_SECTION __USED b[c];    \
    static StaticSemaphore_t d; \
    static kernel_linklist_instance a;  \


enum _KERNEL_LINK_LIST_STRUCTURE_
{
    KERNEL_LL_NEXT_LINK_PTR = 0x00,
    /// @brief this is the data buffer starting pooint 
    KERNEL_LL_DATA_BUFF = 4,
};

/// this is the meta data size of the link list 
#define KERNEL_LINK_LIST_META_DATA_SIZE 4

#define LINK_TERMINATED (void *)U32_MAX

/// @brief this is the err tyes in the lin list lib 
typedef enum _KERNEL_LINK_LIST_ERR_CODES_
{
    KERNEL_LL_OP_SUCESS,
    KERNEL_LL_ERR_MUTEX_TIMEOUT,
    KERNEL_LL_ERR_NOT_INITED,
    KERNEL_LL_ERR_INVALID_PARAM,
    KERNEL_LL_ERR_OUT_OF_MEM,
    KERNEL_LL_ERR_MATCH_FAILED,

} kernel_LL_err_type;


/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////// define the function here for the memory allocator


/// @note please make sure that size% (sizeoofonenode + KERNEL_LINK_LIST_META_DATA_SIZE)  ==0 
/// @brief this is the init function for the link list lib 
/// @param instance 
/// @param mem_inst 
/// @param size 
/// @param mutexbuff_ptr 
/// @param timeout 
/// @param sizeof_one_node 
/// @return succ/failure 
kernel_LL_err_type kernel_ll_init(kernel_linklist_instance * instance, uint8_t *mem_inst, uint16_t size, StaticSemaphore_t *mutexbuff_ptr, uint16_t timeout, uint16_t sizeof_one_node);

/// @brief link list deinit structure 
/// @param instnace 
/// @return succ/failure 
kernel_LL_err_type kernel_ll_deinit(kernel_linklist_instance * instnace);

/// @brief to add the data in the ll 
/// @param inst 
/// @param data 
/// @param size 
/// @return succ/err codes 
kernel_LL_err_type kernel_ll_add_data(kernel_linklist_instance *inst , uint8_t *data, uint16_t size);

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

#endif