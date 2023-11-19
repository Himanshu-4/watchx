#include "system.h"
#include "ansii_color_codes.h"

#include "FreeRTOS.h"
#include "task.h"

/*****
 * @name power_regultiaon init
 * @param void
 * @note init the DC DC regulator
 * @return succ/failure of operation
 */
static bool power_regulation_init(void)
{
    // enable the DC Dc regulator
    NRF_POWER->DCDCEN = POWER_DCDCEN_DCDCEN_Enabled;

    // set up the brow out reset to 2.0v
    NRF_POWER->POFCON = POWER_POFCON_POF_Enabled | (POWER_POFCON_THRESHOLD_V20 << POWER_POFCON_THRESHOLD_Pos);

    // nrfx_err_t err_code=0;
    // const nrf_drv_power_config_t clck_cfg=
    // {
    //     .dcdcen = 1 // enable the dc dc regulator
    // };

    // err_code = nrf_drv_power_init(&clck_cfg);
    // // it returns the error code if it is not equal to NRF_OK
    // ERR_CHK(err_code);

    // NVIC_SystemReset() reset the processor core if needed
    // if everythings fine return 0;
    return nrf_OK;
}

/*****
 * @name clcok_setup_init
 * @param void
 * @note init the 64 mhz clock , configure external crystal and pll engine e
 * @return succ/failure of operation
 */
static bool clock_setup_init(void)
{
    // The HFINT will be used when HFCLK is requested and HFXO has not been started. The HFXO is started by
    //  triggering the HFCLKSTART task and stopped using the HFCLKSTOP task. A HFCLKSTARTED event will
    //  be generated when the HFXO has started and its frequency is stable

    // check if the task triggered or not
    while (NRF_CLOCK->HFCLKRUN != 1)
    {
        __NOP(); // perform no operations , just waste cycles
        // try again starting the task
        // trigger the task to activate the HSXO
        NRF_CLOCK->TASKS_HFCLKSTART = 1UL;
    }
    // wait for the event to get the status
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED != 1)
    {
        __NOP();
    }
    // check the HF CLK clk status
    if (!(NRF_CLOCK->HFCLKSTAT & 1UL))
    {
        // high frequency external crystal oscillator is not able to start
        return nrf_clock_init_failed;
    }

    // clear the event of hf started
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    // try starting the low speed external oscillator

#if defined(START_LSXO)

    if (NRF_CLOCK->LFCLKRUN & 1UL)
    {
        return nrf_clock_init_failed;
    }

    // set the low spped clock source to xtal
    NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_Xtal;

    // check if the LSXO is started or not
    while (NRF_CLOCK->LFCLKRUN != 1UL)
    {
        __NOP();
        // try starting the LSO task
        //  trigger the LSXO start
        NRF_CLOCK->TASKS_LFCLKSTART = 1UL;
    }

    // wait for the event to trigger and clock become stable
    while (NRF_CLOCK->LFCLKRUN != 1)
    {
        __NOP();
    }
    // wait for the lxfo to start and start the main clck

    // clear the lfclk event
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;

    uint32_t clck_val = NRF_CLOCK->LFCLKSTAT;
    while (((clck_val >> CLOCK_LFCLKSTAT_STATE_Pos) & 1UL) && (clck_val & CLOCK_LFCLKSTAT_SRC_Xtal))
    {
        clck_val = NRF_CLOCK->LFCLKSTAT;
        __NOP();
    }
    // check if LFCK running or not with external osc.

#endif

#if defined(ENABLE_TRACE)
    // enable the trace output on gpio pin 18
    NRF_CLOCK->TRACECONFIG = (CLOCK_TRACECONFIG_TRACEMUX_Serial << CLOCK_TRACECONFIG_TRACEMUX_Pos) | CLOCK_TRACECONFIG_TRACEPORTSPEED_4MHz;
#endif

    while (NRF_CLOCK->TRACECONFIG != 0)
    {
        __NOP();
        // disbale the trace clock
        NRF_CLOCK->TRACECONFIG = 0;
    }

    // nrfx_clock_enable();

    // if everythings fine return 0;
    return nrf_OK;
}

/*****
 * @name Instruction cache enabe
 * @param void
 * @note enable the cache for the instruction fetch for the Cortex M4
 * @return succ/failure of operation
 */
static bool Instruction_cache_init(void)
{
    // wait until NVMC is not ready nvmc is ready or not
    while (NRF_NVMC->READY != NVMC_READY_READY_Ready)
    {
        __NOP();
    }
    // after enable the interuction cache
    NRF_NVMC->ICACHECNF = NVMC_ICACHECNF_CACHEEN_Enabled;

    while (NRF_NVMC->READY != NVMC_READY_READY_Ready)
    {
        __NOP();
    }

    // if everythings fine return 0;
    return nrf_OK;
}

bool sys_init(void)
{

    ERR_CHK(power_regulation_init());
    ERR_CHK(clock_setup_init());
    ERR_CHK(Instruction_cache_init());

    // ERR_CHK(UART_debug_init());
    // if everythings fine return 0;
    return nrf_OK;
}


// function to enter low power mode aka system off mode

static void __NO_RETURN enter_low_pwr_mode(void)
{
    NRF_POWER->SYSTEMOFF = POWER_SYSTEMOFF_SYSTEMOFF_Enter;
    __DSB();
    __ISB();

    /* Solution for simulated System OFF in debug mode */
    while (true)
    {
        __WFI();
        __WFE();
    }
}

void system_soft_reset(void)
{
    // reset the system
    NVIC_SystemReset();
}

uint32_t get_reset_reason(void)
{
    uint32_t val = 0xf000f & (NRF_POWER->RESETREAS);
    NRF_POWER->RESETREAS = val;

    switch (val)
    {
    case 0x80000: // sysoff nfc field
        val = RESET_D2_SYSOFF_NFC_FLD;
        break;

    case 0x40000: /// sysoff deug interface
        val = RESET_D2_SYSOFF_DEBUGINF;
        break;

    case 0x20000: // lpcomp
        val = RESET_D2_SYSOFF_LPCOMP;
        break;

    case 0x10000: // sysoff gpiodetect
        val = RESET_D2_SYSOFF_GPIODETECT;
        break;

    case 8: // cpu lockup
        val = RESET_D2_CPU_LOCKUP;
        break;

    case 4: // softreset
        val = RESET_D2_SOFTRESET;
        break;

    case 2: // watchdog
        val = RESET_D2_WATCHDOG;
        break;

    case 1: // reset pin
        val = RESET_D2_RESETPIN;
        break;

    default:
        val = RESET_D2_NONE;
        break;
    }

    return val;
}
 
void system_shutdown(void)
{

    extern void disable_sd(void);

    //// disable the softdevice
    disable_sd();
    /// this is to omit the compilation error
    enter_low_pwr_mode();
}


__FORCE_INLINE int str_to_int(char const *str, uint16_t len)
{
     // Initialize result
    int res = 0;

    // Iterate through all characters
    // of input string and update result
    // take ASCII character of corresponding digit and
    // subtract the code from '0' to get numerical
    // value and multiply res by 10 to shuffle
    // digits left to update running total str[i] != '\0'
    for (int i = 0; i < len; ++i)
    {
    if((str[i] > 57) || (str[i]<48))
    {
      continue;
    }
        res = res * 10 + str[i] - '0';
    }
    // return result.
    return res;
}

///// @todo this algo is the worst algorithm for float conversion . should not use this  
/// @brief convert a string to float 
/// @param str 
/// @return float value 
__FORCE_INLINE float str_to_float(char const *str , uint16_t len)
{ 
    float floatValue = 0.0f;
    int decimalFlag = 0;
    float powerOfTen = 1.0f;

    for (int i = 0; i<len; i++)
    {
        if (str[i] >= '0' && str[i] <= '9') {
            if (!decimalFlag) {
                floatValue = (floatValue * 10.0f) + (str[i] - '0');
            } else {
                floatValue += (float)(str[i] - '0') / powerOfTen;
                powerOfTen *= 10.0f;
            }
        } else if (str[i] == '.') {
            decimalFlag = 1;
            powerOfTen = 10.0f;
        }
    }

    return floatValue;
}

// Reverses a string 'str' of length 'len'
__FORCE_INLINE void reverse(char *str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

__FORCE_INLINE int int_to_str(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
    {
        str[i++] = '0';
    }

    reverse(str, i);
    str[i] = '\0'; //
    return i;
}

// Converts a floating-point/double number to a string.
void ftoa(float n, char *res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = int_to_str(ipart, res, 0);

    res[i] = '.'; // add dot

    // Get the value of fraction part upto given no.
    // of points after dot. The third parameter
    // is needed to handle cases like 233.007
    fpart = fpart * pow(10, afterpoint);

    int_to_str((int)fpart, res + i + 1, afterpoint);
}

uint32_t POWER(uint32_t base, uint32_t power)
{
    uint32_t res = 1;
    for (uint32_t i = 0; i < power; i++)
    {
        res *= base;
    }
    return res;
}

void error_handler(uint32_t code, char const  *function )
{

    printf(RED "ERR %d ,func %s\r\n" ANSI_RESET, code, function);
    for (int i = 0; i < 4; i++)
    {
        // nrf_delay_ms(1000);
        delay(2000);
    }
}

void check_assrt(uint32_t cond, const char *str_print, uint32_t line )
{
    if ((cond) != 0)
    {
        printf(BRED "%d %s->%d\r\n",line, str_print, cond);
        delay(1000);
    }
}
