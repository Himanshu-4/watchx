#include "nvs.h"

////////// include the nrf logging library
#include "nrf_custom_log.h"

/////////// include the softdevice state mansger library and the soc management

#include "nrf_sdm.h"

/////////// include the freertos sepcific library
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "FreeRTOSConfig.h"

////////////////////////////////////////////////////////
/////////////// global variable to handle the suceessful operations of nvs flash
static bool volatile nvs_init = false;

static bool volatile softdevice_enabled = false;

///// mutex handle for the nvs functions
///////// create a semaphore buffer to store the semaphore data
static StaticSemaphore_t nvs_semaphore_buffer;

static SemaphoreHandle_t nvs_mutex_handle = NULL;

//////////////////////////////////////////////////
//// nvs task handle
static volatile xTaskHandle nvs_task_handle = NULL;

static volatile uint32_t nvs_op_timeout = 0;
///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////// static functions //////////////////////////////////////////////////////////////////////////////

/// @brief this func is used to erase a page in flash and it is asynchronus event
/// @param page_num
/// @return it wait for the ACK from the soc event handler and return the succ/failure of the function
static uint32_t nvs_erase_page(uint8_t page_num)
{
    if (nvs_init != true)
    {
        return nrf_ERR_INVALID_STATE;
    }
    if (xSemaphoreTake(nvs_mutex_handle, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
    {
        return nrf_ERR_TIMEOUT;
    }

    uint32_t err = nrf_OK;
    /////// get the current task handle
    nvs_task_handle = xTaskGetCurrentTaskHandle();

    if (softdevice_enabled)
    {
        err = sd_flash_page_erase(page_num);
        if (err != NRF_SUCCESS)
        {
            goto return_mech;
        }
    }

    else
    {
    }

    /////////// wait for the task notification from the API callback
    if (xTaskNotifyWait(0x00, U32_MAX, &err, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
    {
        err = nrf_ERR_TIMEOUT;
        //////// go to return mechanism
        goto return_mech;
    }

return_mech:

    nvs_task_handle = NULL;
    // give back the mutex
    xSemaphoreGive(nvs_mutex_handle);
    ////// nullify the task handle
    return err;
}

/// @brief this function you can get the is this uid present in nvs storage
/// @param addr
/// @param uid
/// @return 1/0 if this uid is presnt or not
static bool nvs_is_uid_present(uint32_t addr, uint32_t uid)
{
    uint32_t *ptr = (uint32_t *)addr;

    while (ptr[NVS_STRUCT_SOD] == START_OF_DATA)
    {

        /// check that uid match or not
        if (ptr[NVS_STRUCT_UID] == uid)
        {
            return 1;
        }
        // jump to next nvs data
        ptr += ptr[NVS_STRUCT_LEN];
    }

    return 0;
}

static uint32_t nvs_get_size_of_data(uint32_t addr, uint32_t uid)
{
    uint32_t *ptr = (uint32_t *)addr;

    while (ptr[NVS_STRUCT_SOD] == START_OF_DATA)
    {

        /// check that uid match or not
        if (ptr[NVS_STRUCT_UID] == uid)
        {
            return (ptr[NVS_STRUCT_LEN] - NVS_META_DATA_SIZE_WORDS);
        }
        // jump to next nvs data
        ptr += ptr[NVS_STRUCT_LEN];
    }

    return 0;
}

/// @brief this function gives the size of stored nvs data in terms of words
/// @param  addr
/// @return size in terms of words
static uint16_t nvs_get_size_of_stored_data(uint32_t addr)
{
    /// this is to get the no of words sotred in flash , if none data is present then return 0
    uint32_t *ptr = (uint32_t *)addr;
    uint32_t len = 0;

    // if (ptr[NVS_STRUCT_SOD] != START_OF_DATA)
    // {
    //     goto return_len;
    // }

    while (ptr[NVS_STRUCT_SOD] == START_OF_DATA)
    {
        len += ptr[NVS_STRUCT_LEN];
        ptr += ptr[NVS_STRUCT_LEN];
    }

    return len;
}

/// @brief this is to get the pointer to the last loocation where we can write new data
/// @param addr
/// @return
static uint32_t *nvs_get_ptr_last_loction(uint32_t addr)
{
    uint32_t *ptr = (uint32_t *)addr;

    while (ptr[NVS_STRUCT_SOD] == START_OF_DATA)
    {
        ptr += ptr[NVS_STRUCT_LEN];
    }

    return ptr;
}

static uint16_t nvs_get_total_uid_present(uint32_t addr)
{
    uint32_t *ptr = (uint32_t *)addr;

    uint16_t total_uids =0;
    while (ptr[NVS_STRUCT_SOD] == START_OF_DATA)
    {
        ptr += ptr[NVS_STRUCT_LEN];
        total_uids++;
    }

    return total_uids;
}

/// @brief this function is to get the location of data from the uid 
/// @param addr 
/// @param uid 
/// @return pointer location 
static uint32_t * nvs_get_uid_pointer(uint32_t addr, uint32_t uid)
{
    uint32_t *ptr = (uint32_t *)addr;
    
    while (ptr[NVS_STRUCT_SOD] == START_OF_DATA)
    {
        if(ptr[NVS_STRUCT_UID] == uid)
        {
            return &ptr[NVS_STRUCT_DATA];
        }
        ptr += ptr[NVS_STRUCT_LEN];
    }

    return NULL;
}
// /// @brief this is to switch the next link of the last data to link present than link absent
// /// @param buff
// /// @param size
// static void nvs_turn_on_next_link(uint32_t *buff, uint16_t size)
// {
//     uint16_t size_jump = 0;

//     while (buff[NVS_STRUCT_NXTLINK + size_jump] == NEXT_LINK_PRESENT)
//     {
//         size_jump += buff[size_jump + NVS_STRUCT_LEN];
//     }
//     if (size_jump >= size)
//     {
//         return;
//     }

//     // update the link
//     buff[NVS_STRUCT_NXTLINK + size_jump] = NEXT_LINK_PRESENT;
// }

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief this is used to copy the data from the data buffer into flash with proper nvs formatting
/// @param data_buff
/// @param size in bytes
/// @return succs/failure of the function
static uint32_t nvs_write_raw_data(uint32_t uid, uint8_t *data_buff, uint16_t size)
{
    if (!nvs_init)
    {
        return nrf_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(nvs_mutex_handle, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
    {
        return nrf_ERR_TIMEOUT;
    }

    ////// create a err variable
    uint32_t err = nrf_OK;

    /////// get the current task handle
    nvs_task_handle = xTaskGetCurrentTaskHandle();

    if (softdevice_enabled)
    {
        //////////// start the write operation

        /////create a buffer and store the data on it
        // uint16_t nvs_data_len = nvs_get_size_of_stored_data(NVS_PARTITION_START_ADDR);
        uint16_t len = 0;
        len = NVS_META_DATA_SIZE_WORDS + GET_NO_OF_PERFECT_DIVIDE(size,4); // buff_size8_to_32(size);

        ///////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////// here we are allocating memory ///////////////////
        //// reserve memory for the data
        uint32_t arr_data[len];

        memset(arr_data, 0, sizeof(arr_data));

        // uint32_t *data_strt = (uint32_t *)NVS_PARTITION_START_ADDR;

        uint32_t *data_strt = nvs_get_ptr_last_loction(NVS_PARTITION_START_ADDR);

        ///// now start modifying the data with respect to nvs
        /////// store the next link to absent
        arr_data[NVS_STRUCT_SOD] = START_OF_DATA;
        ///////// fisrt store the len of the data
        arr_data[NVS_STRUCT_LEN] = NVS_META_DATA_SIZE_WORDS + GET_NO_OF_PERFECT_DIVIDE(size,4); //buff_size8_to_32(size);
        ////// then store the new uid
        arr_data[NVS_STRUCT_UID] = uid;
        /////then store the RFU
        arr_data[NVS_STRUCT_RFU0] = 0;
        //// store the rfu
        arr_data[NVS_STRUCT_RFU1] = 0;

        ///////// copy the remaining data into the buffer
        memcpy((uint8_t *)(&arr_data[NVS_STRUCT_DATA]), (uint8_t *)data_buff, size);

        /////// if everything goes well here then program the data
        /////////// program the new data
        err = sd_flash_write(data_strt, arr_data, len);

        if (err != NRF_SUCCESS)
        {
            goto return_mech;
        }

        /////////// wait for the task notification from the API callback
        if (xTaskNotifyWait(0x00, U32_MAX, &err, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
        {
            err = nrf_ERR_TIMEOUT;
            //////// go to return mechanism
            goto return_mech;
        }
    }

    else
    {
    }

return_mech:
    ////// nullify the task handle
    nvs_task_handle = NULL;
    // give back the mutex
    xSemaphoreGive(nvs_mutex_handle);

    return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief put the data pointer of data into the inpput buffer pointer
/// @param uid
/// @param buff
/// @param size (dont copy the data if requested size != len-5
/// @return return the errr code 
static uint32_t nvs_read_raw_data(uint32_t uid, uint8_t *buff, uint16_t size)
{
    if (!nvs_init)
    {
        return nrf_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(nvs_mutex_handle, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
    {
        return nrf_ERR_TIMEOUT;
    }

    if (softdevice_enabled)
    {
        ///// iterate through the uid and copy the data into the buffer
        uint32_t *ptr = (uint32_t *)NVS_PARTITION_START_ADDR;

        while (ptr[NVS_STRUCT_SOD] == START_OF_DATA)
        {

            /// check that uid match or not
            if (ptr[NVS_STRUCT_UID] == uid)
            {
                /// chec if requested size is larger then stored size
                if (size > ((ptr[NVS_STRUCT_LEN] - NVS_META_DATA_SIZE_WORDS) * 4))
                {
                    return nrf_ERR_OUT_OF_MEM;
                }
                //// copy the data into the buffer
                memcpy(buff, (uint8_t *)&ptr[NVS_STRUCT_DATA], size);
                goto return_mech;
            }
            // jump to next nvs data
            ptr += ptr[NVS_STRUCT_LEN];
        }
    }
    else
    {
    }

return_mech:
    // give back the mutex
    xSemaphoreGive(nvs_mutex_handle);
    return nrf_OK;
}

/// @brief this is to delete the data from the nvs storage
/// @param uid
/// @return succ/failure of the function
static uint32_t nvs_raw_delete_data(uint32_t uid)
{
    if (!nvs_init)
    {
        return nrf_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(nvs_mutex_handle, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
    {
        return nrf_ERR_TIMEOUT;
    }

    uint32_t err = 0;

    uint32_t *nvs_part = (uint32_t *)NVS_PARTITION_START_ADDR;

    if (softdevice_enabled)
    {
        uint16_t len = nvs_get_size_of_stored_data(NVS_PARTITION_START_ADDR) - nvs_get_size_of_data(NVS_PARTITION_START_ADDR, uid) - NVS_META_DATA_SIZE_WORDS;

        uint32_t arr_data[len];
        uint32_t arr_shift = 0;
        //// make the array to 0
        memset(arr_data, 0, sizeof(arr_data));

        uint32_t *ptr = (uint32_t *)NVS_PARTITION_START_ADDR;
        ////// start copying the data
        while (ptr[NVS_STRUCT_SOD] == START_OF_DATA)
        {
            //// this will only execute once
            if (ptr[NVS_STRUCT_UID] == uid)
            {
                ptr += ptr[NVS_STRUCT_LEN];
                continue;
            }

            //// copy the data using memcpy
            memcpy(u8(arr_data[arr_shift]), u8(ptr[NVS_STRUCT_SOD]), ptr[NVS_STRUCT_LEN] * 4);

            //// shift the array to that position
            arr_shift += ptr[NVS_STRUCT_LEN];

            // also shift the ptr
            ptr += ptr[NVS_STRUCT_LEN];
        }

        // only for debug // show the content
        // for (int i = 0; i < len; i++)
        // {
        //     printf("%x ", arr_data[i]);
        // }

        /////// start erasing the partitoin
        /////// get the current task handle
        nvs_task_handle = xTaskGetCurrentTaskHandle();

        err = sd_flash_page_erase(NVS_PARTITION_PAGE_INDEX);
        if (err != NRF_SUCCESS)
        {
            goto return_mech;
        }

        /////////// wait for the task notification from the API callback
        if (xTaskNotifyWait(0x00, U32_MAX, &err, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
        {
            err = nrf_ERR_TIMEOUT;
            //////// go to return mechanism
            goto return_mech;
        }

        /// if there is nothing to flash then  no need to do 
        if(len <=0)
        {
            goto return_mech;
        }
        // program the data
        /////// if everything goes well here then program the data
        /////////// program the new data
        err = sd_flash_write(nvs_part, arr_data, len);

        if (err != NRF_SUCCESS)
        {
            goto return_mech;
        }

        /////////// wait for the task notification from the API callback
        if (xTaskNotifyWait(0x00, U32_MAX, &err, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
        {
            err = nrf_ERR_TIMEOUT;
            //////// go to return mechanism
            goto return_mech;
        }
    }

    else
    {
    }

return_mech:
    /// nullify the task handle
    nvs_task_handle = NULL;
    // give back the mutex
    xSemaphoreGive(nvs_mutex_handle);
    return nrf_OK;
}

/// @brief this is to modify the data of that uid , after the modification the uid got placed to last destination
/// @param uid
/// @param buff
/// @param size
/// @return succ/faliure of function
static uint32_t nvs_raw_modify_data(uint32_t uid, uint8_t *buff, uint16_t size)
{
    if (!nvs_init)
    {
        return nrf_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(nvs_mutex_handle, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
    {
        return nrf_ERR_TIMEOUT;
    }

    uint32_t err = 0;

    uint32_t *nvs_part = (uint32_t *)NVS_PARTITION_START_ADDR;

    if (softdevice_enabled)
    {
        uint16_t len = nvs_get_size_of_stored_data(NVS_PARTITION_START_ADDR) - nvs_get_size_of_data(NVS_PARTITION_START_ADDR, uid) +    GET_NO_OF_PERFECT_DIVIDE(size,4); /* buff_size8_to_32(size) */ 

        uint32_t arr_data[len];
        uint32_t arr_shift = 0;
        //// make the array to 0
        memset(arr_data, 0, sizeof(arr_data));

        uint32_t *ptr = (uint32_t *)NVS_PARTITION_START_ADDR;
        ////// start copying the data
        while (ptr[NVS_STRUCT_SOD] == START_OF_DATA)
        {
            //// this will only execute once
            if (ptr[NVS_STRUCT_UID] == uid)
            {
                ptr += ptr[NVS_STRUCT_LEN];
                continue;
            }

            //// copy the data using memcpy
            memcpy(u8(arr_data[arr_shift]), u8(ptr[NVS_STRUCT_SOD]), ptr[NVS_STRUCT_LEN] * 4);

            //// shift the array to that position
            arr_shift += ptr[NVS_STRUCT_LEN];

            // also shift the ptr
            ptr += ptr[NVS_STRUCT_LEN];
        }

        // add the content of modified data 
        arr_data[arr_shift + NVS_STRUCT_SOD] = START_OF_DATA;
        arr_data[arr_shift + NVS_STRUCT_LEN] = GET_NO_OF_PERFECT_DIVIDE(size,4) /*buff_size8_to_32(size)*/ + NVS_META_DATA_SIZE_WORDS;
        arr_data[arr_shift + NVS_STRUCT_UID] = uid;
        arr_data[arr_shift + NVS_STRUCT_RFU0] = 0;
        arr_data[arr_shift + NVS_STRUCT_RFU1] =0;

        memcpy( u8(arr_data[arr_shift + NVS_STRUCT_DATA]), buff, size );

        //only for debug // show the content
        // for (int i = 0; i < len; i++)
        // {
        //     printf("%x ", arr_data[i]);
        // }

        /////// start erasing the partitoin
        /////// get the current task handle
        nvs_task_handle = xTaskGetCurrentTaskHandle();

        err = sd_flash_page_erase(NVS_PARTITION_PAGE_INDEX);
        if (err != NRF_SUCCESS)
        {
            goto return_mech;
        }

        ///////// wait for the task notification from the API callback
        if (xTaskNotifyWait(0x00, U32_MAX, &err, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
        {
            err = nrf_ERR_TIMEOUT;
            //////// go to return mechanism
            goto return_mech;
        }
        /////program the data
        ///// if everything goes well here then program the data
        ///////// program the new data
        err = sd_flash_write(nvs_part, arr_data, len);

        if (err != NRF_SUCCESS)
        {
            goto return_mech;
        }

        ///////// wait for the task notification from the API callback
        if (xTaskNotifyWait(0x00, U32_MAX, &err, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
        {
            err = nrf_ERR_TIMEOUT;
            //////// go to return mechanism
            goto return_mech;
        }
    }

    else
    {
    }

return_mech:
    /// nullify the task handle
    nvs_task_handle = NULL;
    // give back the mutex
    xSemaphoreGive(nvs_mutex_handle);
    return nrf_OK;
}
////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  nvs library functions  ////////////////////////////////////////
/// @brief init the flash module
/// @return return the success or failure of the system
uint32_t nvs_flash_init(uint32_t timeout)
{
    uint8_t softdevice_State = 0;

    if (nvs_init)
    {
        return nrf_ERR_INVALID_PARAM;
    }
    ////// check the softdevice state
    sd_softdevice_is_enabled(&softdevice_State);
    if (!softdevice_State)
    {
        return nrf_ERR_INVALID_STATE;
    }

    /////// check if it is already inited
    softdevice_enabled = true;

    if (nvs_mutex_handle == NULL)
    {
        nvs_mutex_handle = xSemaphoreCreateMutexStatic(&nvs_semaphore_buffer);
    }

    if (nvs_mutex_handle == NULL)
    {
        NRF_LOG_ERROR("nvs handle 0");
        return nrf_ERR_OUT_OF_MEM;
    }

    
    nvs_op_timeout = timeout;
    //// switch the global flag
    nvs_init = true;

    ///// give the mutex at starting
    xSemaphoreGive(nvs_mutex_handle);
    return nrf_OK;
}

/// @brief func to write the data in the flash region
/// @param uid
/// @param buff
/// @param size
/// @return succ/failure of function
uint32_t nvs_add_data(uint32_t uid, uint8_t *buff, uint16_t size)
{
    uint32_t err = 0;

    /// check here is the uid present or not
    if (!nvs_is_uid_present(NVS_PARTITION_START_ADDR, uid))
    {
        //// make the new buffer

        err = nvs_write_raw_data(uid, buff, size);
    }
    else
    {
        err = NVS_ERR_UID_ALRDY_PRESENT;
    }

    return err;
}

/// @brief this is to get the size of stored data
/// @param uid
/// @return 0 if uid is absent
uint32_t nvs_get_size_data(uint32_t uid)
{
    return nvs_get_size_of_data(NVS_PARTITION_START_ADDR, uid) * 4;
}


/// @brief this function is used to get the pointer value or location of data 
/// @param uid 
/// @return null if err / pointer value is success
void * nvs_get_data_pointer(uint32_t uid)
{
    if (!nvs_init)
    {
        return NULL;
    }

    if (xSemaphoreTake(nvs_mutex_handle, pdMS_TO_TICKS(nvs_op_timeout)) != pdPASS)
    {
        return NULL;
    }

    uint32_t *ptr =  nvs_get_uid_pointer(NVS_PARTITION_START_ADDR,uid);

    ///// give the mutex at starting
    xSemaphoreGive(nvs_mutex_handle);
    return ptr;

}

/// @brief this function is used to get the total no of uid presnet 
/// @param  void 
/// @return total no of uid present 
uint16_t nvs_Get_total_no_of_uid(void)
{
    return nvs_get_total_uid_present(NVS_PARTITION_START_ADDR);
}

/// @brief func to read the data from the flash
/// @param uid
/// @param buff
/// @param size
/// @return succ/failure of function
uint32_t nvs_read_data(uint32_t uid, uint8_t *buff, uint16_t size)
{

    uint32_t err = 0;

    if (!nvs_is_uid_present(NVS_PARTITION_START_ADDR, uid))
    {
        return NVS_ERR_UID_ABSENT;
    }
    //// this is not here it should be handled properly
    err = nvs_read_raw_data(uid, buff, size);

    return err;
}

/// @brief it will erase the complete nvs partition
/// @param  void
/// @return succ, faliure of the function
uint32_t __FORCE_INLINE nvs_erase_partition(void)
{
    return nvs_erase_page(NVS_PARTITION_PAGE_INDEX);
}

/// @brief this is to remove the uid data from the flash
/// @param uid
/// @return succ/failure of the function
uint32_t nvs_delete_data(uint32_t uid)
{
    uint32_t err = 0;

  
        ///// check that is uid present or not
        if (!nvs_is_uid_present(NVS_PARTITION_START_ADDR, uid))
        {
            return NVS_ERR_UID_ABSENT;
        }

        err = nvs_raw_delete_data(uid);
    

    return err;
}

/// @brief used to modify the data of that uid to the new data
/// @param uid
/// @param buff
/// @param size
/// @return succ/failure of function
uint32_t nvs_modify_data(uint32_t uid, uint8_t *buff, uint16_t size)
{
    uint32_t err = 0;

        ///// check that is uid present or not
        if (!nvs_is_uid_present(NVS_PARTITION_START_ADDR, uid))
        {
            return NVS_ERR_UID_ABSENT;
        }

        err = nvs_raw_modify_data(uid, buff, size);
    
    return err;
}

/// @brief the nrf soc event handler for handling the soc events
/// @param event_id
/// @param context
 void nrf_soc_event_handler(uint32_t event_id, void *context)
{
    //// the context is NULL assigned at defineing the handler
    UNUSED_VARIABLE(context);

    // NRF_LOG_DEBUG("s0x%x.", event_id);

    switch (event_id)
    {
    case NRF_EVT_FLASH_OPERATION_SUCCESS:
    {
        if (nvs_task_handle != NULL)
        {
            xTaskNotify(nvs_task_handle, FLASH_OP_SUCESS, eSetValueWithOverwrite);
        }
    }
    break;

    case NRF_EVT_FLASH_OPERATION_ERROR:
    {
        NRF_LOG_ERROR("flash oper");
        if (nvs_task_handle != NULL)
        {
            xTaskNotify(nvs_task_handle, FLASH_OP_FAILED, eSetValueWithOverwrite);
        }
    }
    break;

    default:
        break;
    }
}
