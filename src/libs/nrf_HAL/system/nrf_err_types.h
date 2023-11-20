
#ifndef _NRF_ERR_TYPE_H
#define _NRF_ERR_TYPE_H


/// errors for the anonymus  enum
enum _NRF_ERROR_TYPES_
{
    nrf_OK = 0x00UL + NRF_SYSTEM_ERR_BASE,
    
    /// @brief operations related error
    nrf_ERR_INTERNAL, // if there is an internal error in the system 
    nrf_ERR_NO_MEMORY,
    nrf_ERR_NOT_FOUND,
    nrf_ERR_TIMEOUT,
    nrf_ERR_NOT_SUPPORTED,
    nrf_ERR_SYSTEM_BUSY,
    nrf_ERR_RESOURCE_NOT_FREE,
    
    /// @brief parameters regarding error 
    nrf_ERR_INVALID_PARAM,
    nrf_ERR_INVALID_STATE,
    nrf_ERR_INVALID_LENGTH,
    nrf_ERR_INVALID_FLAGS,
    nrf_ERR_INVALID_DATA,
    

};




#endif