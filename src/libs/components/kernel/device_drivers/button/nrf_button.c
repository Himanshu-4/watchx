#include "nrf_button.h"

#include "gpiote.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


////////// create the qoueue storage area 
//////////////////// error code queue data structure 
#define NRF_BUTTON_EVENT_Q_LENGTH       NRF_BUTTON_EVENTQ_ITEM_LEN 
#define NRF_BUTTON_EVENT_Q_ITEM_SIZE    NRF_BUTTON_EVENTQ_ITEM_SIZE

/* The variable used to hold the queue's data structure. */
static StaticQueue_t xstatic_q_for_button_events;

/* The array to use as the queue's storage area.  This must be at least
uxQueueLength * uxItemSize bytes. */
static uint8_t ucQueueStorageArea_for_btneventq[ NRF_BUTTON_EVENT_Q_ITEM_SIZE * NRF_BUTTON_EVENT_Q_LENGTH ];

static xQueueHandle nrf_btn_evtq_handle = NULL;



//////////// define the button event q irw handler 
static void gpio_int_handler_for_up(void);
static void gpio_int_handler_for_down(void);
static void gpio_int_handler_for_middle(void);
static void gpio_int_handler_for_home(void);


/// @brief this is to init the nrf button library for the watchx button functionality 
/// @param  void 
/// @return succ/failure of the event 
void nrf_button_evt_lib_init(void)
{
    /// init the gpio with proper parameters 
    uint32_t ret_code = nrf_OK;
    /// init the status q 
     nrf_btn_evtq_handle = xQueueCreateStatic(NRF_BUTTON_EVENT_Q_LENGTH ,
                                 NRF_BUTTON_EVENT_Q_ITEM_SIZE,
                                 ucQueueStorageArea_for_btneventq,
                                 &xstatic_q_for_button_events );


     const my_gpio_cfg nrf_buttons_gpio_type =
    {
        .gpio_dir = GPIO_PIN_DIR_INPUT,
        .gpiopull_mode = GPIO_PIN_PULLUP,
        .gpio_sense_type = GPIO_PIN_SENSE_LOW,
        .gpio_input_buff = GPIO_PIN_INPUT_BUFF_CONNECT
    };

    // config the gpio hardware 
    gpio_config(NRF_UP_BUTTON_PIN , &nrf_buttons_gpio_type);
    gpio_config(NRF_DOWN_BUTTON_PIN, &nrf_buttons_gpio_type);
    gpio_config(NRF_MIDDLE_BUTTON_PIN, &nrf_buttons_gpio_type);
    gpio_config(NRF_HOME_BUTTON_PIN, &nrf_buttons_gpio_type);
    
    //// up pin gpiote 
    const my_gpiote_cfg up_pin_int_cfg =
    {
        .mode = GPIO_EVENT_MODE,
        .pinsel = NRF_UP_BUTTON_PIN,
        .outlevel = TASK_OUT_INIT_LEVEL_HIGH,
        .polarity = NRF_BUTTON_INTERRUPT_POLARITY,
    };

    const my_gpiote_cfg down_pin_int_cfg =
    {
        .mode = GPIO_EVENT_MODE,
        .pinsel = NRF_DOWN_BUTTON_PIN,
        .outlevel = TASK_OUT_INIT_LEVEL_HIGH,
        .polarity = NRF_BUTTON_INTERRUPT_POLARITY,
    };

    const my_gpiote_cfg middle_pin_int_cfg =
    {
        .mode = GPIO_EVENT_MODE,
        .pinsel = NRF_MIDDLE_BUTTON_PIN,
        .outlevel = TASK_OUT_INIT_LEVEL_HIGH,
        .polarity = NRF_BUTTON_INTERRUPT_POLARITY,
    };

    const my_gpiote_cfg home_pin_int_cfg =
    {
        .mode = GPIO_EVENT_MODE,
        .pinsel = NRF_HOME_BUTTON_PIN,
        .outlevel = TASK_OUT_INIT_LEVEL_HIGH,
        .polarity = NRF_BUTTON_INTERRUPT_POLARITY
    };
    
    //// config the gpio int channel 
    gpio_config_channel( NRF_UP_BUTTON_GPIOTE_CHANNEL,&up_pin_int_cfg );
    gpio_config_channel( NRF_DOWN_BUTTON_GPIOTE_CHANNEL,&down_pin_int_cfg );
    gpio_config_channel(NRF_MIDDLE_BUTTON_GPIOTE_CHANNEL,&middle_pin_int_cfg );
    gpio_config_channel(NRF_HOME_BUTTON_GPIOTE_CHANNEL, &home_pin_int_cfg);

    //// config the gpio irq handler 
    gpio_add_irq_handler(NRF_UP_BUTTON_GPIOTE_CHANNEL,gpio_int_handler_for_up );
    gpio_add_irq_handler(NRF_DOWN_BUTTON_GPIOTE_CHANNEL,gpio_int_handler_for_down);
    gpio_add_irq_handler(NRF_MIDDLE_BUTTON_GPIOTE_CHANNEL,gpio_int_handler_for_middle);
    gpio_add_irq_handler(NRF_HOME_BUTTON_GPIOTE_CHANNEL, gpio_int_handler_for_home);

    //////// enable the int for the gpio 
    gpio_int_enable(NRF_UP_BUTTON_GPIOTE_CHANNEL);
    gpio_int_enable(NRF_DOWN_BUTTON_GPIOTE_CHANNEL);
    gpio_int_enable(NRF_MIDDLE_BUTTON_GPIOTE_CHANNEL);
    gpio_int_enable(NRF_HOME_BUTTON_GPIOTE_CHANNEL);

}


    // // config the chaanel interrupt 
    // const my_gpiote_cfg ads_ddry_gpte_cfg =
    // {
    //     .mode = GPIO_EVENT_MODE,
    //     .pinsel = EEG_DATA_RDY_PIN,
    //     .outlevel = TASK_OUT_INIT_LEVEL_LOW,
    //     .polarity = EEG_DDRY_INT_TYPE,
    // };
    // gpio_config_channel(EEG_DDRY_CHANNEL , &ads_ddry_gpte_cfg);


void nrf_button_evt_lib_deinit(void)
{
    if(nrf_btn_evtq_handle != NULL)
    {
        vQueueDelete(nrf_btn_evtq_handle);
        nrf_btn_evtq_handle = NULL;
    }

    ///// disable the gpio int 
    gpio_int_disable(NRF_MIDDLE_BUTTON_GPIOTE_CHANNEL);
    gpio_int_disable(NRF_UP_BUTTON_GPIOTE_CHANNEL);
    gpio_int_disable(NRF_DOWN_BUTTON_GPIOTE_CHANNEL);
    gpio_int_disable(NRF_MIDDLE_BUTTON_GPIOTE_CHANNEL);

    ///// remove the gpio channel 
    gpio_remove_channel(NRF_MIDDLE_BUTTON_GPIOTE_CHANNEL);
    gpio_remove_channel(NRF_UP_BUTTON_GPIOTE_CHANNEL);
    gpio_remove_channel(NRF_DOWN_BUTTON_GPIOTE_CHANNEL);
    gpio_remove_channel(NRF_MIDDLE_BUTTON_GPIOTE_CHANNEL);

}

/// @brief this function is to get the event from the button q 
/// @return event , 0 if no event present 
uint8_t nrf_get_btn_evtq(void)
{
    uint8_t evt = NRF_BUTTON_NONE_EVT;
    if(xQueueReceive(nrf_btn_evtq_handle , &evt , 0) != pdPASS)
    {
        evt =NRF_BUTTON_NONE_EVT;
    }
    return evt;
}

/// @brief this function is to wait for the event q to have an event 
/// @return this will wait for the specified time and return 0 if the time expires as event dont happen
uint8_t nrf_get_btn_evtq_wait(uint32_t wait_time)
{
    uint8_t evt =NRF_BUTTON_NONE_EVT;
    if(xQueueReceive(nrf_btn_evtq_handle , &evt , pdMS_TO_TICKS(wait_time) ) != pdPASS)
    {
        evt =NRF_BUTTON_NONE_EVT;
    }
    return evt;
}

/// @brief this is to reset the btn event q 
/// @param  void 
void nrf_reset_btn_evtq(void)
{
    xQueueReset(nrf_btn_evtq_handle);
}

/// @brief this is to put the data in the event q 
/// @param  the data to put in event q
/// @return succ/failure of the function
uint8_t nrf_put_in_btn_evtq(uint8_t event )
{
    if(event > NRF_BUTTON_DOWN_MID_EVT)
    {
        return nrf_ERR_INVALID_PARAM;
    }
    xQueueOverwrite(nrf_btn_evtq_handle , &event);
    return nrf_OK;

}

uint8_t nrf_put_in_btn_evtq_wait(uint8_t event)
{
    if(event > NRF_BUTTON_DOWN_MID_EVT)
    {
        return nrf_ERR_INVALID_PARAM;
    }

    if(xQueueSend(nrf_btn_evtq_handle , &event , pdMS_TO_TICKS(NRF_BUTTON_EVENT_WAIT_TIME)) != pdPASS)
    {
        return nrf_ERR_TIMEOUT;
    }
    return nrf_OK;
    
}


//=================================================================================================
//=================================================================================================
//============================== define the interrupt handler here 


////////////// define the gpio int handler 
static void gpio_int_handler_for_up(void)
{
    BaseType_t high_task_woken =pdFALSE;
  
    //// send the event to the queue 
    uint8_t event = NRF_BUTTON_UP_EVT;

    //  //// check is any other event present or not 
    // if(uxQueueMessagesWaiting(nrf_btn_evtq_handle) != 0)
    // {
    //     uint8_t prev_event = NRF_BUTTON_NONE_EVT;
    //     xQueueReceiveFromISR(nrf_btn_evtq_handle , &prev_event , &high_task_woken);

    //     //// check that which event is this 
    //     switch (prev_event)
    //     {
    //     case NRF_BUTTON_MIDD_EVT:
    //         prev_event = NRF_BUTTON_UP_MID_EVT;            
    //         break;
        
    //     default:
    //         prev_event = NRF_BUTTON_UP_EVT;
    //         break;
    //     }

    //     xQueueOverwriteFromISR(nrf_btn_evtq_handle, &event , &high_task_woken);
    // }
    
    
        xQueueOverwriteFromISR(nrf_btn_evtq_handle, &event , &high_task_woken);
    
       // check if we need a task switch 
    portYIELD_FROM_ISR(high_task_woken);
}

static void gpio_int_handler_for_down(void)
{
    BaseType_t high_task_woken =pdFALSE;
    
    //// send the event to the queue 
    uint8_t event = NRF_BUTTON_DOWN_EVT;

    
        xQueueOverwriteFromISR(nrf_btn_evtq_handle, &event , &high_task_woken);
    

       // check if we need a task switch 
    portYIELD_FROM_ISR(high_task_woken);
}

static void gpio_int_handler_for_middle(void)
{
    BaseType_t high_task_woken =pdFALSE;

    //// send the event to the queue 
    uint8_t event = NRF_BUTTON_MIDD_EVT;

        xQueueOverwriteFromISR(nrf_btn_evtq_handle, &event , &high_task_woken);

       // check if we need a task switch 
    portYIELD_FROM_ISR(high_task_woken);
}

static void gpio_int_handler_for_home(void)
{
    BaseType_t high_task_woken =pdFALSE;

    //// send the event to the queue 
    uint8_t event = NRF_BUTTON_HOME_EVT;

        xQueueOverwriteFromISR(nrf_btn_evtq_handle, &event , &high_task_woken);

       // check if we need a task switch 
    portYIELD_FROM_ISR(high_task_woken);
}