#include "nvs.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////// these are the OTA specific functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// erase the page in the flash

// /// =============================================
// ////     this is the address used when calling the func nvs_write_ota_bin_seq
// /// ============================================
// static uint32_t *ota_part_write_addr;

// /// ==============================================
// ///       this is the address used when calling the func nvs_read_bin_seq
// /// =============================================
// static uint32_t ota_part_read_addr;

// /// ==============================================
// ///  this is the base page value of the ota partition
// //// the app only tell sequentially about how many page have to be erased
// /// =============================================
// static uint8_t ota_part_base_page_val;

// /// @brief this is used to set the ota update partition
// /// so that it can be succesfully update the ota content
// void nvs_set_ota_update_part(void)
// {
//     ///// get the partiion table
//     uint32_t *part_table = (uint32_t *)PARTITION_TABLE_FLASH_ADDR;

//     ota_part_write_addr = (uint32_t *)part_table[OLDER_BOOT_PART_ADDR];
//     ota_part_read_addr = part_table[OLDER_BOOT_PART_ADDR];

//     NRF_LOG_WARNING("ota addr %x", part_table[OLDER_BOOT_PART_ADDR]);

//     if (ota_part_write_addr == (uint32_t *)OTA_1_PARTITION_ADDR)
//     {
//         ota_part_base_page_val = OTA_1_START_PAGE_VAL;
//     }
//     else
//     {
//         ota_part_base_page_val = OTA_0_START_PAGE_VAL;
//     }
// }

// /// @brief this is used to erase the ota page with sequential write
// uint32_t nvs_ota_erase_page(uint8_t val)
// {
//     //// if ota partition is 0 make sure that val + ota_part is between ota 0 range (start, end) and vice versa
//     if (ota_part_base_page_val == OTA_0_START_PAGE_VAL)
//     {
//         if ((val + ota_part_base_page_val) <= OTA_0_END_PAGE_VAL)
//         {
//             return nvs_erase_page(ota_part_base_page_val + val);
//         }
//     }
//     else
//     {
//         if ((ota_part_base_page_val + val) <= OTA_1_END_PAGE_VAL)
//         {

//             return nvs_erase_page(ota_part_base_page_val + val);
//         }
//     }
//     return nrf_ERR_OUT_OF_MEM;
// }

// /// @brief this func is used to flash new binaries in flash pages , ota data
// /// @param buff /// buffer of be flashed
// /// @param size /// size of the input buffer this must be in words
// /// @return /// succ failure of the function
// uint32_t nvs_write_ota_bin_seq(uint32_t const *buff, uint32_t size)
// {

//     if (nvs_init != true)
//     {
//         return nrf_ERR_INVALID_STATE;
//     }

//     if (xSemaphoreTake(nvs_mutex_handle, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
//     {
//         return nrf_ERR_TIMEOUT;
//     }

//     uint32_t err = nrf_OK;
//     /////// get the current task handle
//     nvs_task_handle = xTaskGetCurrentTaskHandle();

//     if (softdevice_enabled)
//     {
//         err = sd_flash_write(ota_part_write_addr, buff, size);
//         if (err != NRF_SUCCESS)
//         {
//             goto return_mech;
//         }
//     }

//     else
//     {
//     }
//     /////////// wait for the task notification from the API callback
//     if (xTaskNotifyWait(0x00, U32_MAX, &err, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
//     {
//         err = nrf_ERR_TIMEOUT;
//         //////// go to return mechanism
//         goto return_mech;
//     }
//     // also check that err is nrf_ok

//     /// if the operation is succesfull the code will reach here
//     ota_part_write_addr += size;

// return_mech:

//     nvs_task_handle = NULL;
//     // give back the mutex
//     xSemaphoreGive(nvs_mutex_handle);
//     ////// nullify the task handle
//     return err;
// }

// /// @brief  this func is used to read the firmware binaries sequentially ,
// ///// you just have to give the start address once and its start incremented automatically
// /// @param buff /// buffer pointer to be read from flash
// /// @param size /// size of the buffer in terms of words (this is to check the boundary condition )
// /// @return  /// when the boundary condition fails like sequential exceeds the partition size limit it returns error .
// uint32_t nvs_read_ota_bin_seq(uint8_t *buff, uint8_t size)
// {

//     if (!nvs_init)
//     {
//         return nrf_ERR_INVALID_STATE;
//     }

//     if (xSemaphoreTake(nvs_mutex_handle, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
//     {
//         return nrf_ERR_TIMEOUT;
//     }

//     //// point the address to the read addres
//     // buff = (uint32_t *)ota_part_read_addr;
//     uint32_t err = nrf_OK;

//     ////////// do a size check
//     if (ota_part_read_addr >= OTA_1_PARTITION_ADDR)
//     {
//         if ((ota_part_read_addr + size) >= OTA_1_PART_END_ADDR)
//         {
//             err = nrf_ERR_OUT_OF_MEM;
//             goto return_mech;
//         }
//     }
//     else
//     {
//         if ((ota_part_read_addr + size) >= OTA_0_PART_END_ADDR)
//         {
//             err = nrf_ERR_OUT_OF_MEM;
//             goto return_mech;
//         }
//     }

//     /// copy the data to the buffer
//     memcpy(buff, (uint8_t *)ota_part_read_addr, size);

//     ////// the size is words type , so mem increment is 4 bytes
//     ota_part_read_addr += size;

// return_mech:
//     // give back the mutex
//     xSemaphoreGive(nvs_mutex_handle);
//     ////// nullify the task handle
//     return err;
// }

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////

// /// @brief the nvs read partition table gives the part table and
// /// @param partition_table pointer
// /// @return // succ or failure of the function
// uint32_t nvs_read_partition_table(partition_table_struct *buff)
// {
//     if (!nvs_init)
//     {
//         return nrf_ERR_INVALID_STATE;
//     }

//     if (xSemaphoreTake(nvs_mutex_handle, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
//     {
//         return nrf_ERR_TIMEOUT;
//     }

//     uint32_t *data = (uint32_t *)PARTITION_TABLE_FLASH_ADDR;
//     memcpy(buff, data, PARTITION_TABLE_SIZE_BYTES);

//     // give back the mutex
//     xSemaphoreGive(nvs_mutex_handle);
//     ////// nullify the task handle
//     return nrf_OK;
// }

// /// @brief this is used to write the partition table into the flash
// /// @param buff pointer to write the u32 pointer
// /// @param size size to write in words
// /// @return /// succ. failure of the operation
// uint32_t nvs_write_partition_table(uint32_t *buff, uint16_t size)
// {
//     if (nvs_init != true)
//     {
//         return nrf_ERR_INVALID_STATE;
//     }

//     if (xSemaphoreTake(nvs_mutex_handle, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
//     {
//         return nrf_ERR_TIMEOUT;
//     }

//     uint32_t err = nrf_OK;
//     /////// get the current task handle
//     nvs_task_handle = xTaskGetCurrentTaskHandle();

//     //////// erase the partition table block
//     /////// the block number  where partition tabel lies is 126
//     err = sd_flash_page_erase(PARTITION_TABLE_PAGE_NO);
//     if (err != NRF_SUCCESS)
//     {
//         goto return_mech;
//     }

//     /////////// wait for the task notification from the API callback so to confirm that flash erase is completed
//     if (xTaskNotifyWait(0x00, U32_MAX, &err, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
//     {
//         err = nrf_ERR_TIMEOUT;
//         //////// go to return mechanism
//         goto return_mech;
//     }

//     if (err != FLASH_OP_SUCESS)
//     {
//         goto return_mech;
//     }

//     uint32_t *part_addr = (uint32_t *)PARTITION_TABLE_FLASH_ADDR;

//     if (softdevice_enabled)
//     {
//         err = sd_flash_write(part_addr, buff, size);
//         if (err != NRF_SUCCESS)
//         {
//             goto return_mech;
//         }
//     }

//     else
//     {
//     }
//     /////////// wait for the task notification from the API callback
//     if (xTaskNotifyWait(0x00, U32_MAX, &err, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
//     {
//         err = nrf_ERR_TIMEOUT;
//         //////// go to return mechanism
//         goto return_mech;
//     }

// return_mech:

//     nvs_task_handle = NULL;
//     // give back the mutex
//     xSemaphoreGive(nvs_mutex_handle);
//     ////// nullify the task handle
//     return err;
// }

//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//////////////////////// the SOC events callback
