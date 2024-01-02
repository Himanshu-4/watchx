
/******
 * Write a test program for sending the command and frames to the OLED screen
 * and also to erase, generate, send the bitmaps to the OLED screen
 *
 *
 *
 * ==============================================================================
 * ----------------- deifne the Command structure -------------------------------
 *
 * Host --> Python,   device --> NRF OLED,
 * Host --> send cmd and data , Slave --> only send responses.
 *
 * so the Header information would be like this
 *
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * type,format          |    description               | Direction
 *  1 byte (meta start) | to signal the header start   | Host --> slave
 *    Preamble info     | information of any cmd       |Slave --> Host
 * ----------------------------------------------------------------------
 * 1 byte               |the actual command send by the| Host <=> Slave
 * (actual command)     |host                          |
 * -----------------------------------------------------------------------
 * 2 byte (length )     |the len of the data or the cmd| Host <=> slave
 *                      |  data len if any             |
 * -----------------------------------------------------------------------
 * rest are data        | data can be long as deifined |
 *                      | in the Length Section        |
 * ------------------------------------------------------------------------
 * Preamble info --> 0x34 ( can be composed of header version, etc)
 *
 * ---------------- define the commands here------------------------------
 * reset                    1
 * send coordinates         2
 * send data                3
 * send data size           4
 * show bitmap              5
 * clear command            6
 * rest coomnads can be added but later
 *
 *
 *
 * ------------------- Data size -----------------------------------------
 * the data size limit is 240 bytes during a transaction i.e. the size cannot exceed
 * 240 bytes + cmd Header size
 *
 *
 * ------------- Response of the command -----------------------------------
 * ACK              0
 * NACK             1
 * Invalid commad   2
 * Invalid Param    3
 * data overflow    4
 * Busy             5
 * operation failed 6
 *
                      |
*/

///////////////////////////////////////////////////////////////////
//////////////////// include the freertos librarys
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

////////// include user libs here
#include "system.h"
#include "nrf_custom_log.h"

#include "kernel_task.h"

#include "nrf_button.h"
#include "ble_gap_func.h"

#include "nrf_gfx.h"

#include "uart_logs.h"

#define GFX_CMD_ARR_MAX_SIZE 30

typedef enum _CMD_TYPES_
{
    GFX_CMD_NOP,
    GFX_CMD_RESET = 1,
    GFX_CMD_SEND_COORDINATES,
    GFX_CMD_SEND_DATA,
    GFX_CMD_SEND_DATA_SIZE,
    GFX_CMD_SHOW_BITMAP,
    GFX_CMD_CLEAR_BITMAP,

    //  * rest coomnads can be added but later
} gfx_cmd_type;

typedef enum _CMD_RESPS_
{
    RSP_ACK,
    RSP_NACK,
    RSP_Invalid_cmd,
    RSP_Invalid_param,
    RSP_data_overflw,
    RSP_Busy,
    RSP_OP_failed,

} gfx_cmd_rsp;

typedef struct __packed _CMD_STRUCT_
{
    uint8_t len;
    uint8_t preamble_info;
    uint8_t cmd_type;
    uint8_t data[1]; // 1 is only used for placeholder for compilation
    // this is treated as a pointer, the array is decayed into pointer
} gfx_cmd_struct_t;

void test_oled_anim_prog(void* param)
{
    UNUSED_PARAMETER(param);
}

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////// general Task Function decleartions /////////////////////////////////////

void general_task_function(void*);

#define genral_task_stack_size 256 // 1KB memory
#define genral_task_name       "gen_task"
#define genral_task_param      NULL
#define genral_task_priority   5

/* Structure that will hold the TCB of the task being created. */
static StaticTask_t gen_task_buffer;

/* Buffer that the task being created will use as its stack.  Note this is
   an array of StackType_t variables.  The size of StackType_t is dependent on
   the RTOS port. */
//// this will reserve the 1KB for the freertos task
static StackType_t gen_task_stack[genral_task_stack_size];

xTaskHandle genral_task_handle = NULL; //!< Reference to SoftDevice FreeRTOS task.

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////// defination of main task

int main()
{

    if (sys_init() != nrf_OK) {
        NRF_LOG_ERROR("sys init failed");
        // APP_ERROR_HANDLER(nrf_ERR_OPERATION_FAILED);
    }

    genral_task_handle = xTaskCreateStatic(general_task_function,
                                           genral_task_name,
                                           genral_task_stack_size,
                                           genral_task_param,
                                           genral_task_priority,
                                           gen_task_stack,
                                           &gen_task_buffer);
    if (genral_task_handle == NULL) {
        APP_ERROR_HANDLER(nrf_ERR_NO_MEMORY);
    }

    /// init the kernel task preinit
    Kernel_task_preinit();

    // watchdog_init(1);
    // watchdog_start();
    /// @todo have to implement the kernel init file
    // watchx_kernel_init();
    /// init the accelrometer task
    /// init the led , buzzer libs

    // hardfault is diabled . please enable it

    // app_start_scheduler();

    vTaskStartScheduler();
    for (;;) {
        // printf("dev id is %x\r\n ", st_get_device_id());
        // printf("the vcell is %f V,soc %d %%, the remcap %dmAh,tte is %lds, current %ldmA, temperature %f*C status %X \r\n",
        // fgage_get_vcell(), fgage_get_soc(), fgage_get_remcap(), fgage_get_tte(), fgage_get_curent(), (float)fgage_get_temp(),
        // fgage_get_status());

        __WFI();
        __NOP();
        // delay(500);
        //     // // no delay at al
        // uart_log_bytes((uint8_t *)msg, sizeof(msg) );
        // gpio_pin_toogle(LED_1);

        nrf_delay_ms(100);
    }

    system_soft_reset();
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////// ///////////// /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////// static functions define here /////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////// genral task /////////////////////////////////////////////

void general_task_function(void* param)
{

    uint8_t cmd_arr[GFX_CMD_ARR_MAX_SIZE] = {0};

    UNUSED_VARIABLE(param);

    delay(100);

    uint32_t err = 0;

    for (;;) {
        uint8_t evt = nrf_btn_get_evtq();
        if (evt != 0) {
            // NRF_LOG_WARNING("%d", evt);
            if (evt == NRF_BUTTON_UP_EVT) {
                // NRF_LOG_INFO("starting adv %d", ble_gap_start_advertise(0));

            } else if (evt == NRF_BUTTON_DOWN_EVT) {
                // NRF_LOG_INFO("stopping adv%d", ble_gap_stop_advertise());

                /// clear the display of the nrf gfx
                nrf_gfx_lib_clear_display();

            } else if (evt == NRF_BUTTON_MIDD_EVT) {

            } else if (evt == NRF_BUTTON_HOME_EVT) {

                //   NRF_LOG_INFO("timer counter %d",rtc_Timer_get_counter_value(NRF_RTC_TIMER_2));
            }
        }

        gfx_cmd_struct_t* cmd = (gfx_cmd_struct_t*) &cmd_arr;
        /// process the cmd
        switch (cmd->cmd_type) {
            case GFX_CMD_RESET:

                break;

            case GFX_CMD_NOP:
                // send the ACK response
                printf("%d\r\n", RSP_ACK);
                break;

            case GFX_CMD_SEND_COORDINATES:

                break;

            case GFX_CMD_SEND_DATA:

                break;

            case GFX_CMD_SEND_DATA_SIZE:

                break;

            case GFX_CMD_SHOW_BITMAP:

                break;

            case GFX_CMD_CLEAR_BITMAP:

                break;

            default:
                break;
        }

        // /// handle the accelrometer here
        // uint8_t evttype =  nrf_accel_get_evtq();

        // if(evttype != NRF_ACCEL_EVT_NONE)
        // {
        //     NRF_LOG_INFO("evt is %d",evttype) ;
        // }

        // nrf_accel_read_raw();
        // NRF_LOG_INFO("main task 1");
    }

    vTaskDelete(NULL);
}

/// @brief to get the serial command from the serial port
/// @param arr
/// @param size
/// @return succ/err code
uint32_t get_host_cmd(uint8_t* arr, uintt8_t size)
{
    uint32_t err = 0;
    /// check for the number of bytes in the uart buffer
    if (get_num_rx_bytes() >= sizeof(gfx_cmd_struct_t)) {

        /// give a nice delay of 15 msec so that the rx finish getting all the data
        delay(15);

        /// new cmd arrives ,process it
        /// get the cmd
        err = get_rx_data(arr, MIN_OF(size, get_num_rx_bytes()));

        // check succesfull logs
        if (err != 0) {
            /// err occured
            uart_flush_buffer();
            memset(arr, 0, size);
        }
    }
    return err;
}

/// @brief to get the data from the serial port
/// @param arr
/// @param size
/// @return succ/failure
uint32_t get_host_data(uint8_t* arr, uint16_t size)
{   
    uint32_t err = 0;
    /// check for the number of bytes in the uart buffer
    if (get_num_rx_bytes() >= sizeof(gfx_cmd_struct_t)) {

        /// give a nice delay of 15 msec so that the rx finish getting all the data
        delay(15);

        /// new cmd arrives ,process it
        /// get the cmd
        err = get_rx_data(arr, MIN_OF(size, get_num_rx_bytes()));

        // check succesfull logs
        if (err != 0) {
            /// err occured
            uart_flush_buffer();
            memset(arr, 0, size);
        }
    }
    return err;
}