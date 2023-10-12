#ifndef _KERNEL_MEM_MANAGER_H_
#define _KERNEL_MEM_MANAGER_H_

#include "system.h"

/// include the freertos file 
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "semphr.h"

#define KERNEL_MEM_SECTION __section(".kernel_mem_data")


//// assign some random number to inits and deinits 
#define KERNEL_MEM_POOL_INITED 0x10

#define KERNEL_MEM_POOL_DEINITED 0x20



/// @brief  this is the memory instance strucutre type , means that the kernel mem is allocate as this
typedef struct _KERNEL_MEM_INSTANCE_
{
    uint8_t *mem_ptr;
    
    /// @brief  size of the mem pool
    uint16_t mem_size;
    uint16_t used_size;

    /// @brief total no of uid present 
    uint16_t total_uid;
    uint8_t kernel_mem_pool_inited;

    /// @brief mutex reltaed info 
    SemaphoreHandle_t kernel_mem_mutex_handle;
    uint16_t mutex_timeout;
} kernel_mem_instance;

/// this will help you to initialize the memory in any particular file
/// @brief this is to init the mem a-> kernel mem instnace name , b -> memory pool name , c-> memory size ,d -> kernel memory mutex buffer
#define KERNEL_MEM_INSTANTIATE(a, b, c, d)          \
    static uint8_t KERNEL_MEM_SECTION __USED b[c]; \
    static StaticSemaphore_t d;                    \
    static kernel_mem_instance a;           \

/// @brief nvs
enum _KERNEL_MEM_STRUCTURE_
{
    KERNEL_MEM_STRUCT_SOD, /// the start of the data is delimiter here
    /// @brief this is the kernel memory data len stored in uint16_t
    KERNEL_MEM_STRUCT_LEN_1,
    KERNEL_MEM_STRUCT_LEN_2,
    /// @brief the mem kernel memory uid uint32_t types
    KERNEL_MEM_STRUCT_UID_1,
    KERNEL_MEM_STRUCT_UID_2,
    KERNEL_MEM_STRUCT_UID_3,
    KERNEL_MEM_STRUCT_UID_4,
    /// @brief the start of the actual data uint8_t types
    KERNEL_MEM_STRUCT_DATA,
};

//////////// this is the
typedef PACKED_STRUCT _KERNEL_MEM_STORAGE_TYPE_
{

    uint8_t kernel_sod;     // this is to start of the data frame present
    uint16_t kernel_len;    //// the length is in bytes that contains how many further components are there
                            ///// len = 4(UID) + len(data) + 1(sod) + 2(len)
                            ///// len cannot be 0xffffffffUL
    uint32_t kernel_UID;    //// this is a uniqueley distributed id that can help us to get the data from the memory
    uint8_t kernel_data[1]; // the 1 is only placeholder for compilation actual data can be any len
}
kernel_mem_storage_type;

/// @brief this is the memory delimiter , for the differnt uid memory types
#define KERNEL_MEMORY_START_OF_DATA_DELIMITER 0x12

/// @brief this is the meta data memory size in bytes for the kernel mem storage
#define KERNEL_MEMORY_META_DATA_SIZE 7

/// @brief these are the error  code that returns of function faliures
typedef enum _KERNEL_MEM_ERR_CODE_
{
    KERNEL_MEM_OP_SUCCESS,
    KERNEL_MEM_ERR_MUTEX_TIMEOUT,
    KERNEL_MEM_ERR_MEMPOOL_DEINITED,
    KERNEL_MEM_ERR_INVALID_PARAM,
    KERNEL_MEM_ERR_UID_ABSENT,
    KERNEL_MEM_ERR_UID_ALRDY_PRESENT,
    KERNEL_MEM_ERR_OUT_OF_MEM,
} kernel_mem_err_type;

/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////// define the function here for the memory allocator

/// @brief this is to init the kernel memory space, its mutex is inited separtely
/// @param kernelmem_instanace
/// @param memory buffer
/// @param size of the memory
/// @param instnace mutex buffer
/// @return succ/failure of the funcction
kernel_mem_err_type kernel_mem_init(kernel_mem_instance *kernel_inst_ptr, uint8_t *mem_inst, uint16_t size, StaticSemaphore_t *mutexbuffer_ptr, uint16_t timeout);

/// @brief this is to deinit the kernel memory space,
/// @param kernelmem_instanace
/// @return succ of the funcction
kernel_mem_err_type kernel_mem_deinit( kernel_mem_instance *kernel_inst_ptr);

/// @brief this function is to add the data in the uid
/// @param mem_inst
/// @param uid
/// @param data
/// @param size
/// @return succ/ err codes
kernel_mem_err_type kernel_mem_add_data( kernel_mem_instance *kernel_inst_ptr, uint32_t uid, const uint8_t *data, uint16_t size);

/// @brief this function is used to modify the data
/// @param mem_inst
/// @param uid
/// @param data
/// @param size
/// @return succ/err codes
kernel_mem_err_type kernel_mem_modify_data( kernel_mem_instance *kernel_inst_ptr, uint32_t uid, const uint8_t *data, uint16_t size);

/// @brief this function is used to get the data pointer where the data start
/// @param mem_inst
/// @param uid
/// @param ptr
/// @return pointer to return 
uint8_t * kernel_mem_get_Data_ptr(const kernel_mem_instance *kernel_inst_ptr, uint32_t uid);

/// @brief this function is to read the data from that uid
/// @param mem_inst
/// @param uid
/// @param data
/// @param size
/// @return succ/err codes
kernel_mem_err_type kernel_mem_read_data(const kernel_mem_instance *kernel_inst_ptr, uint32_t uid, uint8_t *data, uint16_t size);

/// @brief this is to delete the data of that uid
/// @param mem_inst
/// @param uid
/// @return succ/err codes
kernel_mem_err_type kernel_mem_delete_data( kernel_mem_instance *kernel_inst_ptr, uint32_t uid);

/// @brief read the data size of the memory that is in the uid
/// @param mem_inst
/// @param uid
/// @param size pointer
/// @return succ/err codes
kernel_mem_err_type kernel_mem_get_data_size(const kernel_mem_instance *kernel_inst_ptr, uint32_t uid, uint16_t *size);

/// @brief to get the remaining data size from the kernel memory
/// @param kernel_inst_ptr
/// @return succ/err codes
kernel_mem_err_type kernel_mem_get_rem_data_size(const kernel_mem_instance *kernel_inst_ptr, uint16_t *size);

/// @brief to get the used data size in the memory , the size occupied by the uids
/// @param kernel_inst_ptr
/// @param size
/// @return succ/err codes
kernel_mem_err_type kernel_mem_get_used_data_size(const kernel_mem_instance *kernel_inst_ptr, uint16_t *size);

/// @brief to get the total no of the uids present in that memory space
/// @param kernel_inst_ptr
/// @param uid_total
/// @return succ/err codes
kernel_mem_err_type kernel_mem_get_total_no_of_uids(const kernel_mem_instance *kernel_inst_ptr, uint16_t *uid_total);

/// @brief this function is get the uid from the data pointer
/// @param buff
/// @param mem_ptr
/// @param uid pointer
/// @return succ/err codes
kernel_mem_err_type kernel_mem_get_uid_from_pointer(const kernel_mem_instance *kernel_inst_ptr, uint8_t *mem_ptr, uint32_t *uid);

#endif