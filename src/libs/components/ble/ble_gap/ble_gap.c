
#include "ble_gap_func.h"

#include "uECC.h"
////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////   GAP related functions  implimented here 

//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////// extern here 

extern const ble_gap_sec_params_t gap_sec_param[ble_gap_security_max_params_supported];

extern void ble_advertise_pre_init(void);

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  global variables here 

  /// @brief  the callbacks associated with this conn handle 
volatile ble_gap_procdeure_callbacks GAP_Callbacks[ble_gap_max_callback_supp] = {NULL};
    

/// define the instance here 

volatile ble_gap_inst_Struct_t gap_inst[BLE_GAP_MAX_NO_OF_DEVICES];


/// @brief this is to init the gap instance for the connection 
/// @param conn_handle 
/// @param index 
/// @return succ/failure 
uint32_t ble_gap_instance_init( uint8_t index )
{
    if(index >= BLE_GAP_MAX_NO_OF_DEVICES) return nrf_ERR_INVALID_PARAM;

    if(gap_inst[index].ble_gap_instnace_inited) return ble_gap_err_instance_already_inited;

    /// init the instnace 
    memset(u8_ptr &gap_inst[index], 0, sizeof(ble_gap_inst_Struct_t));
    gap_inst[index].ble_gap_conn_handle = BLE_CONN_HANDLE_INVALID;

    return nrf_OK;   
}

/// @brief this is to deinit the gap instnace for this conn handle 
/// @param conn_handle 
/// @return succ/failure 
uint32_t ble_gap_instance_deinit(uint8_t index)
{
    if(index >= BLE_GAP_MAX_NO_OF_DEVICES)return nrf_ERR_INVALID_PARAM;
    ;;;;;;;;;;;;;;  /// it is just for fun 😻
memset( (uint8_t *) &gap_inst[index] , 0, sizeof(ble_gap_inst_Struct_t));
gap_inst[index].ble_gap_conn_handle =  BLE_CONN_HANDLE_INVALID;

    return ble_gap_ok;
}

/// @brief this is to get the connection handle of the connected device 
/// @param  index of the device 
/// @return the conection handle , 
uint16_t ble_gap_get_conn_handle(uint8_t index )
{
    if((index-1) > BLE_GAP_MAX_NO_OF_DEVICES)
    {
        return BLE_CONN_HANDLE_INVALID;
    }
    return gap_inst[index -1].ble_gap_conn_handle;
}

/// @brief this function is to set the connection handle of that index 
/// @param index 
/// @param conn_handle 
/// @return 
uint32_t ble_gap_set_conn_handle(uint8_t index, uint16_t conn_handle)
{

}

/// @brief 
/// @param index 
/// @return 
uint32_t ble_gap_remove_conn_handle(uint8_t index)
{


}

/// @brief this will return a valid index based on the conn handle 
/// @param conn_handle 
/// @return index 
uint8_t ble_gap_get_gap_index(uint16_t conn_handle)
{
    uint8_t index =0;

    for(int i=0; i<BLE_GAP_MAX_NO_OF_DEVICES; i++)
    {
        if(gap_inst[i].ble_gap_conn_handle == conn_handle)
        {
            index =i +1;
            break;
        }
    }

    return index;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


/// @brief this is to add the callback to the particular callback 
/// @param callback_type 
/// @param conn_handle 
/// @param callbacks 
void ble_gap_add_callback(uint8_t callback_type, ble_gap_procdeure_callbacks callbacks)
{
    if(callback_type > ble_gap_max_callback_supp) return ;
        //// add the callback 
    GAP_Callbacks[callback_type] = callbacks;
    
}

/// @brief this is to remove the gap callbacks 
/// @param callback_type
void ble_gap_remove_callback( uint8_t callback_type)
{
    if(callback_type > ble_gap_max_callback_supp) return ;
    ///// this is to set the callback 
        //// add the callback 
    GAP_Callbacks[callback_type] = NULL;
    
}





static int random_number_gen(uint8_t *dest, unsigned size) 
{

    /// check that size is less or equal to the pool capacity
    uint8_t pool_cap =0;
    sd_rand_application_pool_capacity_get(&pool_cap);

    if(size> pool_cap)
    {
        NRF_LOG_ERROR("pool cap overflow");
        return 0;
    }
    
    sd_rand_application_bytes_available_get(&pool_cap);
    if(size > pool_cap)
    {
        NRF_LOG_ERROR("bytes overflow");
        return 0;
    }

    // make sure to call nrf_crypto_init and nrf_crypto_rng_init first
    uint32_t ret_code = sd_rand_application_vector_get(dest, size);
    return (ret_code == NRF_SUCCESS) ? 1 : 0;
}

/// @brief preinit the gap so that it can the BLE GAP properly
/// @param  void
/// @return succ/failure 
void ble_gap_pre_init(void)
{
    ble_advertise_pre_init();
    /// set the eng function 
    uECC_set_rng(random_number_gen);

    /// instantize the gap instance 

    for(int i=0; i<BLE_MAX_DEVICE_SUPPORTED; i++)
    {
        memset(u8_ptr &gap_inst[i] , 0, sizeof(ble_gap_inst_Struct_t));
    }

    for(int i=0; i<BLE_MAX_DEVICE_SUPPORTED; i++)
    {
        gap_inst[i].ble_gap_conn_handle = BLE_CONN_HANDLE_INVALID;
    }

}


/// @brief this function is used to init the security procedure 
/// @param conn_handle 
/// @param sec_param_type 
void ble_gap_security_init(uint8_t  index , uint8_t sec_param_type )
{
    if(index >= BLE_GAP_MAX_NO_OF_DEVICES) return ;
    if(gap_inst[index].ble_gap_conn_handle == BLE_CONN_HANDLE_INVALID)return;

    uint32_t ret_code = 0;
    gap_inst[index].ble_gap_security_param_index = sec_param_type;

    ret_code = sd_ble_gap_authenticate(gap_inst[index].ble_gap_conn_handle , &gap_sec_param[sec_param_type] );
    NRF_ASSERT(ret_code);


    /// this will exit when the crypto operations are completed and link is autheticated 

}


