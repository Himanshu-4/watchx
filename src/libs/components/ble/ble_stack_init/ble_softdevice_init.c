#include "ble_softdevice_init.h"

/////// include the softdevice util headers 
#include "ble_softdevice_util.h"

#include "nrf_custom_log.h"

#include "ble_config.h"

#include "FreeRTOS.h"
#include "task.h"

///////////////// define the ram configuration here //////////////////////////
////////////////// this is defined by the compiler 
#if defined(__GNUC__)
    extern uint32_t  __data_start__;
    uint32_t const * const m_ram_start = &__data_start__;
#endif


#define RAM_START_ADDR       (0x20000000)

#define RAM_END_ADDR ((NRF_FICR->INFO.RAM * 1024) + RAM_START_ADDR)

#define APP_RAM_START_ADDR   (uint32_t)m_ram_start


/// @brief configure the spftdevice with proper cfg id and cfg structure
/// @param cfg_id 
/// @param cfg 
static void nrf_ble_sd_config(uint32_t cfg_id , ble_cfg_t * cfg)
{
    uint32_t ret_code;
     ret_code = sd_ble_cfg_set(cfg_id , cfg , APP_RAM_START_ADDR);
    if(ret_code != 0)
    {
        NRF_LOG_ERROR( "cfg_id %d  err %d",cfg_id , ret_code );
    } 

    //// reset the structure after use 
    memset(cfg , 0, sizeof(ble_cfg_t));
    
}


static void  nrf_ble_set_configuration(void)
{
    // Overwrite some of the default settings of the BLE stack.
    // If any of the calls to sd_ble_cfg_set() fail, log the error but carry on so that
    // wrong RAM settings can be caught by nrf_sdh_ble_enable() and a meaningful error
    // message will be printed to the user suggesting the correct value.
    ble_cfg_t ble_cfg;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////// BLE connection configuration  

    // gap connection  config .
    memset(&ble_cfg, 0, sizeof(ble_cfg));
    ble_cfg.conn_cfg.conn_cfg_tag                     = NRF_SOFTDEVICE_DEFAULT_CONFIG_TAG;
    ble_cfg.conn_cfg.params.gap_conn_cfg.conn_count   = NRF_BLE_TOTAL_LINK_COUNT;
    ble_cfg.conn_cfg.params.gap_conn_cfg.event_length = NRF_BLE_GAP_EVENT_LENGTH_TIME;

    nrf_ble_sd_config(BLE_CONN_CFG_GAP, &ble_cfg);

    // gatt client connection configuration 
     ble_cfg.conn_cfg.conn_cfg_tag                     = NRF_SOFTDEVICE_DEFAULT_CONFIG_TAG;
     ble_cfg.conn_cfg.params.gattc_conn_cfg.write_cmd_tx_queue_size = NRF_BLE_GATTC_WRITE_CMD_TX_Q_SIZE;

    nrf_ble_sd_config(BLE_CONN_CFG_GATTC, &ble_cfg);

    //// gatt server hvn config 
     ble_cfg.conn_cfg.conn_cfg_tag                     = NRF_SOFTDEVICE_DEFAULT_CONFIG_TAG;
    ble_cfg.conn_cfg.params.gatts_conn_cfg.hvn_tx_queue_size = NRF_BLE_GATT_S_HVN_TX_Q_SIZE;
    
    nrf_ble_sd_config(BLE_CONN_CFG_GATTS ,&ble_cfg);

 ////// gatt mtu connection configuration 
     ble_cfg.conn_cfg.conn_cfg_tag                     = NRF_SOFTDEVICE_DEFAULT_CONFIG_TAG;
     ble_cfg.conn_cfg.params.gatt_conn_cfg.att_mtu = NRF_BLE_GATT_MTU_CONFIG;

    nrf_ble_sd_config(BLE_CONN_CFG_GATT , &ble_cfg);
    
  //////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////// BLE common configuration  

    ble_cfg.common_cfg.vs_uuid_cfg.vs_uuid_count = NRF_BLE_NO_OF_VENDOR_SPECIFIC_UUID;

    nrf_ble_sd_config(BLE_COMMON_CFG_VS_UUID, &ble_cfg);

    ////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////// BLE gatts configuration  

    ble_cfg.gatts_cfg.attr_tab_size.attr_tab_size = NRF_BLE_GATT_ATTRIBUTE_TABLE_SIZE;
    nrf_ble_sd_config(BLE_GATTS_CFG_ATTR_TAB_SIZE, &ble_cfg);

    ble_cfg.gatts_cfg.service_changed.service_changed = NRF_BLE_GATT_SERVER_SERVICE_CHANGE;
    nrf_ble_sd_config(BLE_GATTS_CFG_SERVICE_CHANGED, &ble_cfg);

     ////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////// BLE GAP sconfiguration 
    
    /// role count configuration 
    ble_cfg.gap_cfg.role_count_cfg.adv_set_count = NRF_BLE_GAP_CONFIG_ROLE_COUNT_ADV_SET_COUNT;
    ble_cfg.gap_cfg.role_count_cfg.central_role_count = NRF_BLE_GAP_CONFIG_ROLE_COUNT_CENTRAL_ROLE_COUNT;
    ble_cfg.gap_cfg.role_count_cfg.central_sec_count = NRF_BLE_GAP_CONFIG_ROLE_COUNT_CENTRAL_SEC_COUNT;
    ble_cfg.gap_cfg.role_count_cfg.periph_role_count = NRF_BLE_GAP_CONFIG_ROLE_COUNT_PERIPH_ROLE_COUNT;
    ble_cfg.gap_cfg.role_count_cfg.qos_channel_survey_role_available = NRF_BLE_GAP_CONFIG_ROLE_COUNT_QOS_CHANEL_SEURVEY_ROLE_COUNT;

    nrf_ble_sd_config(BLE_GAP_CFG_ROLE_COUNT, &ble_cfg);

    //  Peripheral Preferred Connection Parameters characteristic include
    ble_cfg.gap_cfg.ppcp_include_cfg.include_cfg = NRF_BLE_GAP_CHAR_PERIPHERAL_PREFERD_CONN_INCLUDE;
    nrf_ble_sd_config(BLE_GAP_CFG_PPCP_INCL_CONFIG, &ble_cfg);

    /// Central Address Resolution characteristic include configuration,
    ble_cfg.gap_cfg.car_include_cfg.include_cfg = NRF_BLE_GAP_CHAR_CENTRAL_ADDR_RESOLUTION_INCLUDE;
    nrf_ble_sd_config(BLE_GAP_CFG_CAR_INCL_CONFIG, &ble_cfg);

    //// you can add the device name here 


}



static void nrf_enable_ble_stack(void)
{
    
    uint32_t app_ram_start_addr = APP_RAM_START_ADDR;

        ///// enable the ble stack 
    uint32_t ret_code = sd_ble_enable(&app_ram_start_addr);
    
    if (app_ram_start_addr > APP_RAM_START_ADDR)
    {
        NRF_LOG_WARNING("Insufficient RAM allocated for the SoftDevice.");

        NRF_LOG_WARNING("Change the RAM start location to  0x%x.", app_ram_start_addr);
        NRF_LOG_WARNING("Maximum RAM size for application 0x%x.", RAM_END_ADDR - app_ram_start_addr );
    }
    else
    {
        NRF_LOG_DEBUG("RAM starts at 0x%x", APP_RAM_START_ADDR);
        if ( APP_RAM_START_ADDR != app_ram_start_addr)
        {
            NRF_LOG_DEBUG("RAM start location can be adjusted to 0x%x.", app_ram_start_addr);

            NRF_LOG_DEBUG("RAM size for application can be adjusted to 0x%x.",
                          RAM_END_ADDR - app_ram_start_addr);
        }
    }

    if (ret_code != NRF_SUCCESS)
    {
       NRF_LOG_ERROR("sd_ble_enable() returned %d", ret_code);
    }
    
    
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////// define the events handler below for softdevice 

//////////////////////////////////////////////////////
/////////////////////////////// function handlers for the 

extern void ble_gap_event_handler(ble_evt_t const *);

extern void ble_gatt_client_handler(ble_evt_t const *);

extern void ble_gatt_server_handler(ble_evt_t const *);



/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused. it is set NULL
 */
static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context)
{

    UNUSED_VARIABLE(p_context);

    
    NRF_LOG_DEBUG("BEVT %x",p_ble_evt->header.evt_id);

    /// handle the gap events 
    ble_gap_event_handler(p_ble_evt);

    /// handle the client gatt event 
    ble_gatt_client_handler(p_ble_evt);

    /// handle the gatt server events 
    ble_gatt_server_handler(p_ble_evt);


}



/**@brief       Function for polling BLE events.
 *
 * @param[in]   p_context   Context of the observer.
 */
static void nrf_sdh_ble_evts_poll(void * p_context)
{
    
    ret_code_t ret_code;


    while (true)
    {

        __ALIGN(4) uint8_t evt_buffer[NRF_SDH_BLE_EVT_BUF_SIZE];
        

        ble_evt_t * p_ble_evt;
        uint16_t    evt_len = (uint16_t)sizeof(evt_buffer);

        ret_code = sd_ble_evt_get(evt_buffer, &evt_len);
       
        //// check while we are not remaining with any events 
        if (ret_code != NRF_SUCCESS)
        {
            break;
        }

        p_ble_evt = (ble_evt_t *)evt_buffer;

        ble_evt_handler(p_ble_evt, p_context);
       


    }

    if (ret_code != NRF_ERROR_NOT_FOUND)
    {
        NRF_ASSERT(ret_code);
    }
}



extern void nrf_soc_event_handler(uint32_t event_id, void *context);


/**@brief   Function for polling SoC events.
 *
 * @param[in]   p_context   Context of the observer.
 */
static void nrf_sdh_soc_evts_poll(void * p_context)
{
    ret_code_t ret_code;

    UNUSED_VARIABLE(p_context);

    while (true)
    {
        uint32_t evt_id;

        ret_code = sd_evt_get(&evt_id);
        if (ret_code != NRF_SUCCESS)
        {
            break;
        }

       //// forward the event to softdevice soc handler 
         nrf_soc_event_handler(evt_id, p_context);
    }

    if (ret_code != NRF_ERROR_NOT_FOUND)
    {
        NRF_ASSERT(ret_code);
    }
}






//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////      create the static task to handle the BLE events ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Structure that will hold the TCB of the task being created. */
static StaticTask_t ble_sd_TaskBuffer;

/* Buffer that the task being created will use as its stack.  Note this is
   an array of StackType_t variables.  The size of StackType_t is dependent on
   the RTOS port. */
//// this will reserve the 1KB for the freertos task
static StackType_t ble_sd_stack[NRF_BLE_FREERTOS_SDH_TASK_STACK];

static TaskHandle_t softdevice_evt_handle = NULL; //!< Reference to SoftDevice FreeRTOS task.

////////////////////////////////////////////////////////////////////
/////////////////// IRQ defination ///////////////////////////////////


void SD_EVT_IRQHandler(void)
{
    BaseType_t yield_req = pdFALSE;

    if (softdevice_evt_handle != NULL)
    {
        vTaskNotifyGiveFromISR(softdevice_evt_handle, &yield_req);
    }

    /* Switch the task if required. */
    portYIELD_FROM_ISR(yield_req);
}



/* This function gets events from the SoftDevice and processes them. */
static void softdevice_task(void * pvParameter)
{

    while (true)
    {
        /////////// run the soc event handler first 
        nrf_sdh_soc_evts_poll(pvParameter);

        //// after that run the sdh ble event 
        nrf_sdh_ble_evts_poll(pvParameter);

        (void)ulTaskNotifyTake(pdTRUE,         /* Clear the notification value before exiting (equivalent to the binary semaphore). */
                               portMAX_DELAY); /* Block indefinitely (INCLUDE_vTaskSuspend has to be enabled).*/
    }
}

void nrf_sdh_freertos_init(void)
{
    softdevice_evt_handle = xTaskCreateStatic(softdevice_task,
                                              "BLE",
                                              NRF_BLE_FREERTOS_SDH_TASK_STACK,
                                              NULL,
                                              SOFTDEVICE_TASK_PRIORITY, ble_sd_stack, &ble_sd_TaskBuffer);
    if (softdevice_evt_handle == NULL)
    {
        NRF_LOG_ERROR("SoftDevice task not created.");
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static void nrf_enable_Softdevice(void)
{
    if(softdevice_is_enabled())
    {
        return;
    }
    uint32_t ret_code =0;
    /// enable the softdevice
    ret_code = softdevice_enable_req();

    NRF_ASSERT(ret_code);
    NRF_LOG_INFO("ble_Stack_enabled");
}


////////////// funnction used to init the BLE module

void nrf_softdevice_init_and_start(void)
{
    // softdevice stack initialization
    nrf_enable_Softdevice();
    
    ///// configure the ble stack 
    nrf_ble_set_configuration();

    //// turn the ble stack on 
    nrf_enable_ble_stack();
  
    // this have a deffered interrupt processing mechanism so that
    // as to make the interurpt handler as short as possible and let the processing runs in thread mode 
    nrf_sdh_freertos_init();
}



