#ifndef _NVS_H_
#define _NVS_H_

#include "system.h"

//// include the partition table library
// #include "partition_table.h"

enum NVS_STORAGE_STRUCTURE
{
    PARTITION_TABLE_UID = 0x01,
};

///////////only 4kb space is available for the nvs partiotion
#define NVS_PARTITION_START_ADDR 0x7F000UL

#define NVS_PARTITION_SPACE_SIZE_WORDS 1024 //(in words)

#define NVS_PARTITION_SPACE_SIZE_BYTES 4096 // (in bytes)

#define NVS_PARTITION_PAGE_INDEX 127

/////////// define the nvs meta data size 
#define NVS_META_DATA_SIZE_WORDS 5 

#define NVS_META_DATA_SIZE_BYTES 20


/////////////////// define the SOC observer priority
#define APP_SOC_EVENTS_OBSERVER_PRIO 1

#define FLASH_OP_SUCESS 0x00UL

#define FLASH_OP_FAILED nrf_ERR_OPERATION_FAILED



/// @brief nvs 
enum _NVS_STRUCTURE_
{
    NVS_STRUCT_SOD,
    NVS_STRUCT_LEN,
    NVS_STRUCT_UID,
    NVS_STRUCT_RFU0,
    NVS_STRUCT_RFU1,
    NVS_STRUCT_DATA,
};


//////////// this is the
typedef PACKED_STRUCT _NVS_STORAGE_TYPE
{
    
    uint32_t sod; // this is to start of the data frame present  
    uint32_t len; //// the length is in words that contains how many further components are there
                  ///// len = 1(UID) + 2(RFU) + len(data) + 1(sod) + 1(len)
                  ///// len cannot be 0xffffffffUL // U32_max means that the erase word of flash
    uint32_t UID;
    uint32_t RFU[2];
    uint32_t data[1]; // the 1 is only placeholder for compilation actual data can be any len 
}
nvs_storage_type;

/////////// this must be progrmmed correctly to make sure that nvs 
#define START_OF_DATA 0x1010UL

#define START_OF_DATA_ABSENT U32_MAX




/// @brief init the flash module
/// @param timeout of the
/// @return return the success or failure of the system
uint32_t nvs_flash_init(uint32_t);

/// @brief this is used to copy the data from the data buffer into flash with proper nvs formatting
/// @param data_buff
/// @param size in words
/// @return succs/failure of the function
uint32_t nvs_add_data(uint32_t uid, uint8_t *data_buff, uint16_t size);

/// @brief put the data pointer of data into the inpput buffer pointer
/// @param uid
/// @param buff
/// @return return the size of the buffer in the form of bytes
uint32_t nvs_read_data(uint32_t uid, uint8_t *buff , uint16_t size);

/// @brief this is to get the size of the stored data 
/// @param uid 
/// @return return size in bytes of data , 0 if uid is absent  
uint32_t nvs_get_size_data(uint32_t uid);

/// @brief this function is used to get the pointer value or location of data 
/// @param uid 
/// @return null if err / pointer value is success
void * nvs_get_data_pointer(uint32_t uid);

/// @brief this function is used to get the total no of uid presnet 
/// @param  void 
/// @return total no of uid present 
uint16_t nvs_Get_total_no_of_uid(void);

/// @brief used to modify the data of that uid to the new data 
/// @param uid 
/// @param buff 
/// @param size 
/// @return succ/failure of function 
uint32_t nvs_modify_data(uint32_t uid, uint8_t *buff, uint16_t size);

/// @brief this is to remove the uid data from the flash 
/// @param uid 
/// @return succ/failure of the function 
uint32_t nvs_delete_data(uint32_t uid);

/// @brief used to completely erase the nvs partition , when you want to erase everythig 
/// @param  void
/// @return succ/failure of the function 
uint32_t  nvs_erase_partition(void);


/// @brief these are the error  code that returns of function faliures  
enum _NVS_ERR_CODE_
{
    NVS_OP_SUCCESS,
    NVS_ERR_UID_ABSENT,
    NVS_ERR_UID_ALRDY_PRESENT,
    NVS_ERR_FLASH_ERASE_FAILED,
    NVS_ERR_FLASH_WRITE_FAILED,
    NVS_ERR_OUT_OF_MEM,
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////// ota related function for the non volatile library 



// /// @brief this function is used to erase the flash pages
// /// @param val
// /// @return return the err code . succes
// uint32_t nvs_ota_erase_page(uint8_t val);

// /// @brief this is used to set the ota update partition
// /// so that it can be succesfully update the ota content
// void nvs_set_ota_update_part(void);

// ///// write ota data sequentially 

// /// @brief this func is used to flash new binaries in flash pages , ota data
// /// @param buff /// buffer of be flashed
// /// @param size /// size of the input buffer this must be in words
// /// @return /// succ failure of the function
// uint32_t nvs_write_ota_bin_seq(uint32_t const *buff, uint32_t size);

// /// @brief  this func is used to read the firmware binaries sequentially ,
// ///// you just have to give the start address once and its start incremented automatically
// /// @param buff /// buffer pointer to be read from flash
// /// @param size /// size of the buffer in terms of words (this is to check the boundary condition )
// /// @return  /// when the boundary condition fails like sequential exceeds the partition size limit it returns error .
// uint32_t nvs_read_ota_bin_seq(uint8_t* buff, uint8_t size);

// //// for partition table related API

// /// @brief the nvs read partition table gives the part table and
// /// @param partition_table pointer
// /// @return succ. failure of the function 
// uint32_t nvs_read_partition_table(partition_table_struct *buff);

// /// @brief this is used to write the partition table into the flash
// /// @param buff pointer to write
// /// @param size size to write in bytes 
// /// @return /// succ. failure of the operation
// uint32_t nvs_write_partition_table(uint32_t *buff, uint16_t size);

#endif