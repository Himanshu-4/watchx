

// #define FREERTOS
///////////////////////////////////////////////////////////////////
//////////////////// include the freertos librarys
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

////////// include user libs here
#include "system.h"
#include "gpio.h"
#include "gpiote.h"
#include "uart_logs.h"
#include "nrf_time.h"

///// include the external sensor libraries
#include "st_accelrometer.h"
#include "fuel_gauge.h"
#include "eeg.h"
#include "tdcs.h"

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// task 1 functions
xTaskHandle task_1_handle = NULL;

void task_1_func(void *);

#define task_1_stack_size 256 // 1KB memory
#define task_1_name "task1"
#define task_1_param NULL
#define task_1_priority 4

// higher peiorirty number means lower priority

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// task 2 functions
xTaskHandle task_runner_handle = NULL;

void task_runner_func(void *);

#define task_2_stack_size 1024
#define task_2_name "task2"
#define task_2_param NULL
#define task_2_priority 4 // lower priority than

//////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////// data is recv in this q
extern xQueueHandle eeg_queue_handle;

uint8_t eeg_q_buff[EEG_ONE_SAMPLE_SIZE] = {0};

typedef struct PACKED
{
    uint8_t code;
    uint8_t rate;
    uint32_t time;

} eeg_data_struct;

/// @brief /////////////////////////////////////////////////////////////////
typedef struct PACKED
{
    uint8_t code;
    uint8_t opcode;
    uint16_t current;
    uint32_t ramp_time;
    uint32_t constant_curr_time;

} tdcs_data_struct;

/// @brief /////////////////////////////////////////////////////////////////
typedef struct PACKED
{
    uint8_t code;
    uint32_t time;
} fuel_data_struct;

typedef struct PACKED
{
    uint8_t code;
    uint32_t time;

}st_accel_struct;


/// @brief //////////////////////////////////////////////////////////////////////////
/// @return /////////////////////////////////////////////////////////////////////////
int main()
{

    if (sys_init() != nrf_OK)
    {
        error_handler("clk init failed");
    }

    // install all the communication drivers
    driver_install();

//////////// init the puinout of the sensor module 
    // ini the eeg module
    eeg_pins_init();
    // init the accelration module
    st_accel_pins_init();
    //////////// intialise the tdcs pinoout
    tdcs_pins_init();


    uint8_t res = pdFAIL;
    res = xTaskCreate(task_1_func, task_1_name, task_1_stack_size, task_1_param, task_1_priority, &task_1_handle);
    if (res != pdPASS)
    {
        error_handler("task_1 not created");
    }

    // start the kernel
    vTaskStartScheduler();
    for (;;)
    {
        // printf("dev id is %x\r\n ", st_get_device_id());
        // printf("the vcell is %f V,soc %d %%, the remcap %dmAh,tte is %lds, current %ldmA, temperature %f*C status %X \r\n",
        // fgage_get_vcell(), fgage_get_soc(), fgage_get_remcap(), fgage_get_tte(), fgage_get_curent(), (float)fgage_get_temp(), fgage_get_status());

        __WFI();
        __NOP();
        // delay(500);
        // // no delay at al
        // uart_log_bytes((uint8_t *)"hye there\r\n",12 );

        nrf_delay_ms(1000);
    }

    return 0;
}

void task_1_func(void *param)
{
    UNUSED_VARIABLE(param);

    char temp[30] = {0};
    uint8_t i = 0;

//// init the basic structure of the params 
    eeg_data_struct eeg_data =
        {
            .code = 1,
            .rate = 0,
            .time = 0,
        };

    tdcs_data_struct tdcs_data =
        {
            .code = 2,
            .opcode = 4,
            .constant_curr_time = 0,
            .current = 0,
            .ramp_time = 0};

    fuel_data_struct fuel_data =
        {
            .code = 3,
            .time = 0};

    st_accel_struct accel_data=
    {
        .code =4,
        .time =0,
    };
        

    uint8_t count = 0;
    uint8_t index = 0;

    
    // printf(UGRN "count valu is %ld\r\n" , count++);
    for (;;)
    {

        // read the input string
        if (get_num_rx_bytes() > 1)
        {
            // read the input buffer
            while (get_num_rx_bytes() != 0)
            {
                temp[i++] = read_char();
                delay(10);
            }
             
            /////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////
            if (temp[0] == '1') // this is for eeg command
            {
                i = 2;
                uint8_t x = 0;
            decode_eeg_str:
                index = i;
                // first get the rate
                while (temp[i++] != ' ')
                {
                    // increment the x
                    x++;
                }

                // after copy is complete
                count++;
                switch (count)
                {
                case 1:
                    eeg_data.rate = aToi(&temp[index], x);
                    break;

                case 2:
                    eeg_data.time = aToi(&temp[index], x);
                    // copy is complete
                    goto copy_eeg_cmpt;
                    break;
                }

                x = 0;

                goto decode_eeg_str;

            copy_eeg_cmpt:

                count = 0;

                if (task_runner_handle == NULL)
                {
                    // also create the task and start it
                    xTaskCreate(task_runner_func, task_2_name, task_2_stack_size, &eeg_data, task_2_priority, &task_runner_handle);
                    configASSERT(task_runner_handle);
                }
            }
            //////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////////////
            else if (temp[0] == '2') //  this is for tdcs stimulation
            {
                // for tdcs stimulation
                i = 2;
                uint8_t x = 0;
            decode_tdcs_str:
                index = i;
                // first get the opcode
                while (temp[i++] != ' ')
                {
                    // increment the x
                    x++;
                }

                // after copy is complete
                count++;
                switch (count)
                {
                case 1:
                    tdcs_data.opcode = aToi(&temp[index], x);
                    break;

                case 2:
                    tdcs_data.current = aToi(&temp[index], x);
                    break;

                case 3:
                    tdcs_data.ramp_time = aToi(&temp[index], x);
                    break;

                case 4:
                    tdcs_data.constant_curr_time = aToi(&temp[index], x);
                    // copy is complete
                    goto copy_tdcs_cmpt;
                    break;
                }

                x = 0;
                goto decode_tdcs_str;

            copy_tdcs_cmpt:

                count = 0;
                if (task_runner_handle == NULL)
                {
                    // also create the task and start it
                    xTaskCreate(task_runner_func, task_2_name, task_2_stack_size, &tdcs_data, task_2_priority, &task_runner_handle);
                    ASSERT(task_runner_handle);
                }
            }

            /////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////
            else if (temp[0] == '3') // this is for fuel gauge
            {
             
                // for tdcs stimulation
                i = 2;
                uint8_t x = 0;

                index = i;
                // first get the opcode
                while (temp[i++] != ' ')
                {
                    // increment the x
                    x++;
                }

                fuel_data.time = aToi(&temp[index], x);

                count = 0;

                if (task_runner_handle == NULL)
                {
                    // also create the task and start it
                    xTaskCreate(task_runner_func, task_2_name, task_2_stack_size, &fuel_data, task_2_priority, &task_runner_handle);
                    ASSERT(task_runner_handle);
                }
            }
             /////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////
            
            else if (temp[0] == '4') // this is for accelrometer
            {
                    // for tdcs stimulation
                i = 2;
                uint8_t x = 0;

                index = i;
                // first get the opcode
                while (temp[i++] != ' ')
                {
                    // increment the x
                    x++;
                }

                accel_data.time = aToi(&temp[index], x);

                count = 0;

                if (task_runner_handle == NULL)
                {
                    // also create the task and start it
                    xTaskCreate(task_runner_func, task_2_name, task_2_stack_size, &accel_data, task_2_priority, &task_runner_handle);
                    ASSERT(task_runner_handle);
                }
            }
             /////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////
            
            else if(temp[0] == 's')
            {

                system_soft_reset();
            }
            i = 0;

            /// reset the string after usage
            memset(temp, 0, sizeof(temp));
        }

        else
        {
            // give a delay
            delay(500);
            // printf("hello\r\n");
        }


    }

    /////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    /// should never reach here
    vTaskDelete(NULL);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void task_runner_func(void *param)
{
    uint8_t *opcode = param;
    uint8_t ret = 0;

    /// get the elasped time
    uint32_t elapsed_time = 0;

    if (*opcode == 1) // means eeg have to be run
    {

        eeg_data_struct *my_eeg_data = param;
        // init the eeG module
        ret = eeg_init(my_eeg_data->rate , ADS_EEG_ONLY);

        elapsed_time = my_eeg_data->time;
        // // there is a problem in init the function
        if (ret != nrf_OK)
        {
            // send err_code
            printf("2");
            goto terminate_func;
        }
        printf("0");
    }
    else if (*opcode == 2) // means tdcs have to be run
    {
        tdcs_data_struct *my_tdcs_data = param;
        // init the tdcs module //////////////////////////////// multiply by 1000 as time is given as millisec in param
        tdcs_init(my_tdcs_data->opcode, my_tdcs_data->current, 1000 * my_tdcs_data->ramp_time, 1000 *  my_tdcs_data->constant_curr_time);

        elapsed_time = (2 * my_tdcs_data->ramp_time) + my_tdcs_data->constant_curr_time;

        printf("0");
    }
    else if (*opcode == 3) // fuel gauge have to be run
    {
        fuel_data_struct *fgage_data = param;

        ret = fuel_gauge_init();
   
        if (ret != nrf_OK)
        {
            // send err_code
            printf("4");
            goto terminate_func;
        }
        printf("0");
        elapsed_time = fgage_data->time;
    }
    else if (*opcode == 4) // accelrometer have to be run
    {
        st_accel_struct *acc_data = param;

        const my_accel_config acc_cfg =
        {
            .axis_enabled = X_AXIS_EN | Y_AXIS_EN | Z_AXIS_EN,
            .out_data_rate = ST_ODR_50HZ,
            .out_senstvty = ST_ACCEL_SENS_2G
        };

            /// init the st accelrometer 
        ret = st_config_accel(&acc_cfg);
        if(ret != nrf_OK)
        {

            printf("4");
            goto terminate_func;

        }
      
        printf("0");
        elapsed_time = acc_data->time;
    }

   

    uint32_t prev_sec = seconds();
    delay(20);
    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    for (;;)
    {
        if (*opcode == 1) // means eeg have to be run
        {
            // data is recd in the queu of the eeg
            xQueueReceive(eeg_queue_handle ,eeg_q_buff, EEG_WAIT_FOR_RESPONSE);
            // deocde the data and print it
            printf("%6d,%6d,%6d,%6d,\n",eeg_data_to_int(eeg_q_buff),
            eeg_data_to_int(eeg_q_buff +3),eeg_data_to_int(eeg_q_buff+6),eeg_data_to_int(eeg_q_buff+9));
            //
        }
        else if (*opcode == 2) // means tdcs have to be run
        {
            tdcs_run_simulation();
           
        }
        else if (*opcode == 3) // fuel gauge have to be run
        {
            // print the fuel gauge values
            printf("vcell %5ld,soc %3d,r_mah %5d,rette %5ld,I %5ld,temp %3d,stat %2d\r\n",
                   (uint32_t)fgage_get_vcell(), fgage_get_soc(), fgage_get_remcap(), fgage_get_tte(),
                   fgage_get_curent(), fgage_get_temp(), fgage_get_status());

            delay(1000);
        }
        else if (*opcode == 4) // accelrometer have to be run
        {
            // get the accel temp data 
            printf("stid %4x temp %5d\r\n",st_get_device_id(), st_get_temp());
            delay(200);
        }

        // taskYIELD();
        // lets check for the termination string
        if ((seconds() - prev_sec) > elapsed_time)
        {
            break;
        }
    }

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
terminate_func:
    if (*opcode == 1) // means eeg have to be run
    {
        // deint the eeg module
        eeg_deint();
    }
    else if (*opcode == 2) // means tdcs have to be run
    {
        tdcs_deinit();
    }
    else if (*opcode == 3) // fuel gauge have to be run
    {
      
        // we dont have any fuelgauge deinit
    }
    else if (*opcode == 4) // accelrometer have to be run
    {
        /// may be put the accelr. into depp sleep
    }

    // also delete the task
    task_runner_handle = NULL;
    /// should never reach here
    vTaskDelete(NULL);
}
