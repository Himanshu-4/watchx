#include "ble_gatt_client.h"

//// freertos librares
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/////////// macro functions

#define IS_CLIENT_INITED(x)                 \
  if (x.client_inited != BLE_CLIENT_INITED) \
  {         \
    return ble_client_err_client_not_inited;  \
  }     \

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/////////////////////// extern variables ////////////////////////////////
#define BLE_SEARCH_START_HANDLE BLE_GATT_HANDLE_START

#define BLE_SEARCH_END_HANDLE BLE_GATT_HANDLE_END

#define BLE_GATT_CLIENT_HANDLE_NONE BLE_GATT_HANDLE_INVALID
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/////////////////////// static variables  ////////////////////////////////
////////// create a mutex for the functions

// use semaphore spi handle
static SemaphoreHandle_t ble_client_semphr_handle;

// these are used to create a semaphore buffer
static StaticSemaphore_t ble_client_semphr_buffer;

#define BLE_GATTC_DESC_DISC_HANDLE_EXTEND 5
//////////////////////////////////////////////////////////////////////////
/// create a struct buffer for the client handlers

volatile ble_client_struct client_struct[BLE_NO_OF_CLIENT_SUPPORTED] = {0};

/////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
volatile uint8_t client_buff[BLE_CLIENT_MESSAGE_BUFFER_SIZE];

volatile xTaskHandle client_taskhandle;

//////////////////////////////////////////////////////////////////////////
/////////////////////// static variables  ////////////////////////////////

uint32_t gatt_client_pre_init(void)
{

  // create the mutex
  if (ble_client_semphr_handle == NULL)
  {
    ble_client_semphr_handle = xSemaphoreCreateMutexStatic(&ble_client_semphr_buffer);
  }

  /// set the client conn handle
  for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
  {
    client_struct[i].conn_handle = BLE_CONN_HANDLE_INVALID;
  }
  // configASSERT(ble_client_semphr_handle);

  // always give the  semaphore after initaling
  xSemaphoreGive(ble_client_semphr_handle);

  return ble_client_ok;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/////////////////////// function declarations  ////////////////////////////////

//////// init the gatt client module // this is to be called when connection is done
uint32_t gatt_client_init(uint16_t conn_handle)
{

  // take the semaphore
  if (xSemaphoreTake(ble_client_semphr_handle, (BLE_CLIENT_FUNCTIONS_MUTEX_WAIT_TIME)) != pdPASS)
  {
    return ble_client_err_timeout;
  }

  if ((conn_handle == BLE_CONN_HANDLE_INVALID))
  {
    return nrf_ERR_INVALID_PARAM;
  }

  uint32_t ret_code = 0;

  uint8_t index = 0;

  /// serach the client instnace 
  for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
  {
    if (client_struct[i].conn_handle == BLE_CONN_HANDLE_INVALID)
    {
      index = i;
      goto init_the_client;
    }
  }
  ret_code = ble_client_err_max_limit_reached;
  goto return_mech;

init_the_client:

  client_struct[index].client_ind_handler = NULL;
  client_struct[index].client_indi_hand_param = NULL;

  client_struct[index].client_notif_handler = NULL;
  client_struct[index].client_notif_hand_param = NULL;

  client_struct[index].client_timeout_handler = NULL;
  client_struct[index].client_timeout_hand_param = NULL;

  client_struct[index].client_err_handler = NULL;
  client_struct[index].client_err_hand_param = NULL;

  client_struct[index].conn_handle = conn_handle;
  client_struct[index].client_inited = BLE_CLIENT_INITED;

  ret_code = ble_client_ok;

  memset((uint8_t *)client_buff, 0, sizeof(client_buff));
  client_taskhandle = NULL;

return_mech:
  xSemaphoreGive(ble_client_semphr_handle);
  return ret_code;
}

/// @brief this is to deinit the gatt client module ,you can call it at gap_disconnecct
/// @param conn_handle
/// @return succ/failure of function
uint32_t gatt_client_deinit(uint16_t conn_handle)
{
  // take the semaphore
  if (xSemaphoreTake(ble_client_semphr_handle, (BLE_CLIENT_FUNCTIONS_MUTEX_WAIT_TIME)) != pdPASS)
  {
    return ble_client_err_timeout;
  }

  uint32_t ret_code = 0;

  uint8_t index = 0;

  /// serach the client instnace 
  for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
  {
    if (client_struct[i].conn_handle == conn_handle)
    {
      index = i;
      goto deinit_the_client;
    }
  }
  ret_code = ble_client_err_conn_handle_not_found;
  goto return_mech;

deinit_the_client:

  client_struct[index].client_ind_handler = NULL;
  client_struct[index].client_indi_hand_param = NULL;

  client_struct[index].client_notif_handler = NULL;
  client_struct[index].client_notif_hand_param = NULL;

  client_struct[index].client_timeout_handler = NULL;
  client_struct[index].client_timeout_hand_param = NULL;

  client_struct[index].client_err_handler = NULL;
  client_struct[index].client_err_hand_param = NULL;

  client_struct[index].conn_handle = BLE_CONN_HANDLE_INVALID;
  client_struct[index].client_inited = 0;

  memset((uint8_t *)client_buff, 0, sizeof(client_buff));

  ret_code = ble_client_ok;

return_mech:
  xSemaphoreGive(ble_client_semphr_handle);
  return ret_code;
}

/// @brief this is to set the server mtu of the connection handle
/// @param conn_hand
/// @param mtu
/// @return succ/failure of function
uint32_t gatt_client_set_server_mtu(uint16_t conn_handle, uint16_t mtu)
{
  // take the semaphore
  if (xSemaphoreTake(ble_client_semphr_handle, (BLE_CLIENT_FUNCTIONS_MUTEX_WAIT_TIME)) != pdPASS)
  {
    return ble_client_mutex_not_avialble;
  }

  uint32_t err = ble_client_ok;
  /// serach the client instnace 
  for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
  {
    if (client_struct[i].conn_handle == conn_handle)
    {
      IS_CLIENT_INITED(client_struct[i]);
      goto request_mtu;
    }
  }

  err = ble_client_err_conn_handle_not_found;
  goto return_mech;

request_mtu:

  //// get the task handle
  client_taskhandle = xTaskGetCurrentTaskHandle();

  err = sd_ble_gattc_exchange_mtu_request(conn_handle, mtu);

  if (err != nrf_OK)
  {
    NRF_LOG_ERROR("mtu");
    goto return_mech;
  }

  if (xTaskNotifyWait(0, U32_MAX, &err, BLE_CLIENT_FUNCTIONS_CLIENT_WAIT_TIME) != pdPASS)
  {
    err = ble_client_err_timeout;
    goto return_mech;
  }

  if (err != nrf_OK)
  {
    err = ble_client_err_invalid_state;
    goto return_mech;
  }

return_mech:
  // nullify the tas handle
  client_taskhandle = NULL;
  xSemaphoreGive(ble_client_semphr_handle);
  return err;
}

/// @brief this is to add the gatt client timeout callback
/// @param conn_hand
/// @param gatt_client_timeout_handler
/// @return succ/failure of function
uint32_t gatt_client_add_timeout_callback(uint16_t conn_handle, gatt_client_timeout_handler handler, void *param)
{
  uint32_t err = ble_client_ok;

  // take the semaphore
  if (xSemaphoreTake(ble_client_semphr_handle, BLE_CLIENT_FUNCTIONS_MUTEX_WAIT_TIME) != pdPASS)
  {
    return ble_client_err_timeout;
  }

  uint8_t index = 0;
  /// add the timeout callback
  /// serach the client instnace 
  for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
  {
    if (client_struct[i].conn_handle == conn_handle)
    {
      index = i;
      IS_CLIENT_INITED(client_struct[i]);
      goto init_callback;
    }
  }
  err = ble_client_err_conn_handle_not_found;
  goto return_mech;

init_callback:
  client_struct[index].client_timeout_handler = handler;
  client_struct[index].client_timeout_hand_param = param;

return_mech:
  xSemaphoreGive(ble_client_semphr_handle);
  return err;
}

/// @brief this is to call when an errr is occured during gatt operations
/// @param conn_handle
/// @param parameter pointer
/// @param handler
/// @return succ/failure of the function
uint32_t gatt_client_add_err_handler_callback(uint16_t conn_handle, gatt_client_error_handler handler, void *param)
{
  uint32_t err = ble_client_ok;

  // take the semaphore
  if (xSemaphoreTake(ble_client_semphr_handle, (BLE_CLIENT_FUNCTIONS_MUTEX_WAIT_TIME)) != pdPASS)
  {
    return ble_client_err_timeout;
  }

  uint8_t index = 0;
  /// add the timeout callback
  /// serach the client instnace 
  for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
  {
    if (client_struct[i].conn_handle == conn_handle)
    {
      index = i;
      IS_CLIENT_INITED(client_struct[i]);
      goto init_callback;
    }
  }
  err = ble_client_err_conn_handle_not_found;
  goto return_mech;

init_callback:
  client_struct[index].client_err_handler = handler;
  client_struct[index].client_err_hand_param = param;

return_mech:
  xSemaphoreGive(ble_client_semphr_handle);
  return err;
}

/// @brief this is to add the indication callback to the client handlers
/// @note this function will be called inside the ble event handler task of the sd , which have a higher priority
//// and limited stack size please dont use memory intensive  task inside this handler
/// @param conn_hand
/// @param callback
/// @return succ/failure of function
uint32_t gatt_client_add_indication_callback(uint16_t conn_handle, gatt_client_indc_callbaclk handler, void *param)
{
  uint32_t err = ble_client_ok;

  // take the semaphore
  if (xSemaphoreTake(ble_client_semphr_handle, (BLE_CLIENT_FUNCTIONS_MUTEX_WAIT_TIME)) != pdPASS)
  {
    return ble_client_err_timeout;
  }

  uint8_t index = 0;
  /// add the timeout callback
  /// serach the client instnace 
  for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
  {
    if (client_struct[i].conn_handle == conn_handle)
    {
      index = i;
      IS_CLIENT_INITED(client_struct[i]);
      goto init_callback;
    }
  }
  err = ble_client_err_conn_handle_not_found;
  goto return_mech;

init_callback:
  client_struct[index].client_ind_handler = handler;
  client_struct[index].client_indi_hand_param = param;

return_mech:
  xSemaphoreGive(ble_client_semphr_handle);
  return err;
}

/// @brief this is to add the notification callback for the notification
/// @note this function will be called inside the ble event handler task of the sd , which have a higher priority
//// and limited stack size please dont use memory intensive  task inside this handler
/// @param conn_hand
/// @param callback
/// @param pointer_param
/// @return succ/failure of function
uint32_t gatt_client_add_notif_callback(uint16_t conn_handle, gatt_client_notif_callbaclk handler, void *param)
{
  uint32_t err = ble_client_ok;

  // take the semaphore
  if (xSemaphoreTake(ble_client_semphr_handle, (BLE_CLIENT_FUNCTIONS_MUTEX_WAIT_TIME)) != pdPASS)
  {
    return ble_client_err_timeout;
  }

  uint8_t index = 0;
  /// add the timeout callback
  /// serach the client instnace 
  for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
  {
    if (client_struct[i].conn_handle == conn_handle)
    {
      index = i;
      IS_CLIENT_INITED(client_struct[i]);
      goto init_callback;
    }
  }
  err = ble_client_err_conn_handle_not_found;
  goto return_mech;

init_callback:
  client_struct[index].client_notif_handler = handler;
  client_struct[index].client_notif_hand_param = param;

return_mech:
  xSemaphoreGive(ble_client_semphr_handle);
  return err;
}

///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
///////////////// database discovery functions

/// @brief for searching the gatt server service
/// @param conn_hand
/// @param ble_service_struct_t pointer
/// @return succ/failure of function
uint32_t gatt_client_discover_service(uint16_t conn_handle, ble_service_struct_t *service_struct)
{

  uint32_t err = ble_client_ok;

  // take the semaphore
  if (xSemaphoreTake(ble_client_semphr_handle, (BLE_CLIENT_FUNCTIONS_MUTEX_WAIT_TIME)) != pdPASS)
  {
    return ble_client_mutex_not_avialble;
  }

  /// serach the client instnace 
  for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
  {
    if (client_struct[i].conn_handle == conn_handle)
    {
      IS_CLIENT_INITED(client_struct[i]);
      goto discover_Data;
    }
  }
  err = ble_client_err_conn_handle_not_found;
  goto copy_null;

discover_Data:

  client_taskhandle = xTaskGetCurrentTaskHandle();
  //// call the serach service function
  err = sd_ble_gattc_primary_services_discover(conn_handle, BLE_SEARCH_START_HANDLE, &service_struct->ble_service.uuid);

  if (err != nrf_OK)
  {
    err = ble_client_err_srvc_not_found;
    goto copy_null;
  }

  /// wait for the task notifxcation from the callback
  if (xTaskNotifyWait(0, U32_MAX, &err, BLE_CLIENT_FUNCTIONS_CLIENT_WAIT_TIME) != pdPASS)
  {
    err = ble_client_err_timeout;
    goto copy_null;
  }
  else
  {
    /// check the err value
    if (err != nrf_OK)
    {
      NRF_LOG_ERROR("srvc disc %d", err);
      goto copy_null;
    }

    /// copy the data from the client buffer
    memcpy(service_struct, (uint8_t *)client_buff, sizeof(ble_gattc_service_t));
    goto return_mech;
  }

copy_null:
{
  service_struct->ble_service.handle_range.start_handle = BLE_GATT_CLIENT_HANDLE_NONE;
  service_struct->ble_service.handle_range.end_handle = BLE_GATT_CLIENT_HANDLE_NONE;
}
return_mech:
{
  client_taskhandle = NULL;
  xSemaphoreGive(ble_client_semphr_handle);
  return err;
}
}

/// @brief to discover the gatt client characteristics of the server, at this time only one by one char discv is avaialble
/// @param conn_hand
/// @param service_struct
/// @param char_struct
/// @return succ/failure of function
uint32_t gatt_client_discover_chars(uint16_t conn_handle, ble_service_struct_t *service_struct, ble_char_struct_t *char_struct)
{

  uint32_t err = ble_client_ok;

  // take the semaphore
  if (xSemaphoreTake(ble_client_semphr_handle, (BLE_CLIENT_FUNCTIONS_MUTEX_WAIT_TIME)) != pdPASS)
  {
    return ble_client_mutex_not_avialble;
  }

/// serach the client instnace 
  for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
  {
    if (client_struct[i].conn_handle == conn_handle)
    {
      IS_CLIENT_INITED(client_struct[i]);
      goto discover_Data;
    }
  }
  err = ble_client_err_conn_handle_not_found;
  goto return_mech;

discover_Data:

  client_taskhandle = xTaskGetCurrentTaskHandle();

  // discover the characteristcs
  // copy the content of the char data
  memcpy((uint8_t *)client_buff, (uint8_t *)char_struct, sizeof(ble_char_struct_t));

  err = sd_ble_gattc_characteristics_discover(conn_handle, &service_struct->ble_service.handle_range);

  if (err != nrf_OK)
  {
    goto copy_null;
  }

  /// wait for the notifcation from the callback
  if (xTaskNotifyWait(0x00, U32_MAX, &err, (BLE_CLIENT_FUNCTIONS_CLIENT_WAIT_TIME)) != pdPASS)
  {
    err = ble_client_err_timeout;
    goto copy_null;
  }
  else
  {
    if (err != nrf_OK)
    {
      err = ble_client_err_char_not_found;
      goto copy_null;
    }

    /// copy the content to the structure
    memcpy(char_struct, (uint8_t *)client_buff, sizeof(ble_char_struct_t));
    goto return_mech;
  }

copy_null:
{
  char_struct->characterstic.handle_decl = BLE_GATT_CLIENT_HANDLE_NONE;
  char_struct->characterstic.handle_value = BLE_GATT_CLIENT_HANDLE_NONE;
}

return_mech:
{
  client_taskhandle = NULL;
  xSemaphoreGive(ble_client_semphr_handle);
  return err;
}
}
/// @brief for seraching the gatt client char descriptor discovery
/// @param conn_hand
/// @param char_struct
/// @param desc_uuid
/// @return succ/failure of function
uint32_t gatt_client_discover_char_desc(uint16_t conn_handle, ble_char_struct_t *char_struct, ble_char_desc_struct_t *desc_struct)
{

  // take the semaphore
  if (xSemaphoreTake(ble_client_semphr_handle, (BLE_CLIENT_FUNCTIONS_MUTEX_WAIT_TIME)) != pdPASS)
  {
    return ble_client_mutex_not_avialble;
  }
  
  uint32_t err = ble_client_ok;

  /// serach the client instnace 
  for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
  {
    if (client_struct[i].conn_handle == conn_handle)
    {
      IS_CLIENT_INITED(client_struct[i]);
      goto discover_Data;
    }
  }
  err = ble_client_err_conn_handle_not_found;
  goto return_mech;

discover_Data:
{
  client_taskhandle = xTaskGetCurrentTaskHandle();
  // discover the characteristcs
  /// set the buffer content to char
  // copy the content of the char data
  memcpy(u8_ptr client_buff, u8_ptr desc_struct, sizeof(ble_char_desc_struct_t));
  ble_gattc_handle_range_t desc_disc_range;

  desc_disc_range.start_handle = char_struct->characterstic.handle_value;
  desc_disc_range.end_handle = char_struct->characterstic.handle_value + BLE_GATTC_DESC_DISC_HANDLE_EXTEND;

  err = sd_ble_gattc_descriptors_discover(conn_handle, &desc_disc_range);

  if (err != nrf_OK)
  {
    NRF_LOG_ERROR("desc disc err");
    goto copy_null;
  }

  /// wait for the notifcation from the callback
  if (xTaskNotifyWait(0x00, U32_MAX, &err, (BLE_CLIENT_FUNCTIONS_CLIENT_WAIT_TIME)) != pdPASS)
  {
    err = ble_client_err_timeout;
    goto copy_null;
  }

  /// check if error or not
  if (err != nrf_OK)
  {
    err = ble_client_err_desc_not_found;
    goto copy_null;
  }
    /// copy the content to the structure
    memcpy((uint8_t *) desc_struct, (uint8_t *)client_buff, sizeof(ble_char_desc_struct_t));
    goto return_mech; 
}

copy_null:
{
    desc_struct->descriptor.handle = BLE_GATT_CLIENT_HANDLE_NONE;
}

return_mech:
{
  client_taskhandle = NULL;
  xSemaphoreGive(ble_client_semphr_handle);
  return err;
}
}

/////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///////////  gatt server operations

/// @brief this is to write a value to the char ,descriptor
/// @param conn_hand
/// @param write_type
/// @param buff
/// @param size
/// @return succ/failure of func
uint32_t gatt_client_char_write(uint16_t conn_handle, ble_char_struct_t *char_struct, uint8_t write_type, uint8_t *buff, uint16_t size)
{

  uint32_t err = ble_client_ok;

  // take the semaphore
  if (xSemaphoreTake(ble_client_semphr_handle, (BLE_CLIENT_FUNCTIONS_MUTEX_WAIT_TIME)) != pdPASS)
  {
    return ble_client_mutex_not_avialble;
  }

  /// serach the client instnace 
  for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
  {
    if (client_struct[i].conn_handle == conn_handle)
    {
      IS_CLIENT_INITED(client_struct[i]);
      goto server_operation;
    }
  }
  err = ble_client_err_conn_handle_not_found;
  goto return_mech;

server_operation:
{
  // wait for the complete notification 
  client_taskhandle = xTaskGetCurrentTaskHandle();
  //// do a write operatioln
  ble_gattc_write_params_t ble_write_param;

  ble_write_param.write_op = (write_type == CHAR_WRITE_WITH_RSP) ? (BLE_GATT_OP_WRITE_REQ) : (BLE_GATT_OP_WRITE_CMD);
  ble_write_param.flags = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_CANCEL;
  ble_write_param.offset = 0;
  ble_write_param.handle = char_struct->characterstic.handle_value;

  // data part
  ble_write_param.p_value = buff;
  ble_write_param.len = size;

  /// call the function
  err = sd_ble_gattc_write(conn_handle, &ble_write_param);

  if (err != nrf_OK)
  {
    goto return_mech;
  }

  /// wait for the notifcation from the callback
  if (xTaskNotifyWait(0x00, U32_MAX, &err, (BLE_CLIENT_FUNCTIONS_CLIENT_WAIT_TIME)) != pdPASS)
  {
    err = ble_client_err_timeout;
    goto return_mech;
  }

  /// check if error or not
  if (err != nrf_OK)
  {
    /// log the erroor
    NRF_LOG_ERROR("write %d", err);
    err = ble_client_err_write_op_failed;
  }
}

return_mech:
{
  client_taskhandle = NULL;
  xSemaphoreGive(ble_client_semphr_handle);
  return err;
}
}

/// @brief this is to read the char value of the service, or u can read descriptor value , the write size must be known
/// @param conn_hand
/// @param char structure
/// @param buffer
/// @param size of the buffer
/// @return succ/failure of function
uint32_t gatt_client_char_read(uint16_t conn_handle, ble_char_struct_t *char_struct, uint8_t *buff, uint8_t size)
{
  // take the semaphore
  if (xSemaphoreTake(ble_client_semphr_handle, (BLE_CLIENT_FUNCTIONS_MUTEX_WAIT_TIME)) != pdPASS)
  {
    return ble_client_mutex_not_avialble;
  }
  
  uint32_t err = ble_client_ok;

  /// serach the client instnace 
  for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
  {
    if (client_struct[i].conn_handle == conn_handle)
    {
      IS_CLIENT_INITED(client_struct[i]);
      goto server_operation;
    }
  }
  err = ble_client_err_conn_handle_not_found;
  goto return_mech;

///// perform the write operations
server_operation:
{
  client_taskhandle = xTaskGetCurrentTaskHandle();
  /// perform the read operation and wait for the notification
  sd_ble_gattc_read(conn_handle, char_struct->characterstic.handle_value, 0);

  if (err != nrf_OK)
  {
    goto return_mech;
  }

  /// wait for the notifcation from the callback
  if (xTaskNotifyWait(0x00, U32_MAX, &err, (BLE_CLIENT_FUNCTIONS_CLIENT_WAIT_TIME)) != pdPASS)
  {
    err = ble_client_err_timeout;
    goto return_mech;
  }

  /// check if error or not
  if (err != nrf_OK)
  {
    /// log the erroor
    NRF_LOG_ERROR("read %x", err);
    err = ble_client_err_read_op_failed;
  }
  else 
  {
        /// copy the content
    memcpy(buff, u8_ptr client_buff, MIN_OF( sizeof(client_buff), size));
  }

}
return_mech:
  client_taskhandle = NULL;
  xSemaphoreGive(ble_client_semphr_handle);
  return err;
}

/// @brief this is to write a value to the gatt client descriptor
/// @param conn_hand
/// @param desc_struct
/// @param buff
/// @param size
/// @return succ/Failure of func
uint32_t gattc_client_char_desc_write(uint16_t conn_handle, ble_char_desc_struct_t *desc_struct, uint8_t *buff, uint16_t size)
{

  // take the semaphore
  if (xSemaphoreTake(ble_client_semphr_handle, (BLE_CLIENT_FUNCTIONS_MUTEX_WAIT_TIME)) != pdPASS)
  {
    return ble_client_err_timeout;
  }
  
  uint32_t err = ble_client_ok;

  /// serach the client instnace 
  for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
  {
    if (client_struct[i].conn_handle == conn_handle)
    {
      IS_CLIENT_INITED(client_struct[i]);
      goto server_operation;
    }
  }
  err = ble_client_err_conn_handle_not_found;
  goto return_mech;

server_operation:
{
  client_taskhandle = xTaskGetCurrentTaskHandle();
  //// do a write operatioln
  ble_gattc_write_params_t ble_write_param;

  ble_write_param.write_op = BLE_GATT_OP_WRITE_CMD;
  ble_write_param.flags = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_CANCEL;
  ble_write_param.offset = 0;
  ble_write_param.handle = desc_struct->descriptor.handle;

  // data part
  ble_write_param.p_value = buff;
  ble_write_param.len = size;

  /// call the function
  err = sd_ble_gattc_write(conn_handle, &ble_write_param);

  if (err != nrf_OK)
  {
    NRF_LOG_ERROR("desc wr");
    goto return_mech;
  }

  /// wait for the notifcation from the callback
  if (xTaskNotifyWait(0x00, U32_MAX, &err, (BLE_CLIENT_FUNCTIONS_CLIENT_WAIT_TIME)) != pdPASS)
  {
    err = ble_client_err_timeout;
    goto return_mech;
  }

  /// check if error or not
  if (err != nrf_OK)
  {
    /// log the erroor
    NRF_LOG_ERROR("write %d", err);
    err = ble_client_err_write_op_failed;
  }
}

return_mech:
client_taskhandle = NULL;
  xSemaphoreGive(ble_client_semphr_handle);
  return err;
}

/// @brief this to read the char descriptor value
/// @param conn_hand
/// @param desc_struct
/// @param buff
/// @param size
/// @return succ/Failure of the func
uint32_t gattc_client_char_desc_read(uint16_t conn_handle, ble_char_desc_struct_t *desc_struct, uint8_t *buff, uint16_t size)
{
  // take the semaphore
  if (xSemaphoreTake(ble_client_semphr_handle, (BLE_CLIENT_FUNCTIONS_MUTEX_WAIT_TIME)) != pdPASS)
  {
    return ble_client_mutex_not_avialble;
  }
  
  uint32_t err = ble_client_ok;

  /// serach the client instnace 
  for (uint8_t i = 0; i < BLE_NO_OF_CLIENT_SUPPORTED; i++)
  {
    if (client_struct[i].conn_handle == conn_handle)
    {
      IS_CLIENT_INITED(client_struct[i]);
      goto server_operation;
    }
  }
  err = ble_client_err_conn_handle_not_found;
  goto return_mech;

///// perform the write operations
server_operation:
{
  client_taskhandle =  xTaskGetCurrentTaskHandle();
  /// perform the read operation and wait for the notification
  err = sd_ble_gattc_read(conn_handle, desc_struct->descriptor.handle, 0);
  if (err != nrf_OK)
  {
    goto return_mech;
  }

  /// wait for the notifcation from the callback
  if (xTaskNotifyWait(0x00, U32_MAX, &err, (BLE_CLIENT_FUNCTIONS_CLIENT_WAIT_TIME)) != pdPASS)
  {
    err = ble_client_err_timeout;
    goto return_mech;
  }
  /// check if error or not
  if (err != nrf_OK)
  {
    /// log the erroor
    NRF_LOG_ERROR("read %d", err);
    err = ble_client_err_read_op_failed;
  }
  else 
  {
        // copy the content
    memcpy(buff, u8_ptr client_buff, MIN_OF(sizeof(client_buff), size));
  }
}
return_mech:
  client_taskhandle = NULL;
  xSemaphoreGive(ble_client_semphr_handle);
  return err;
}
