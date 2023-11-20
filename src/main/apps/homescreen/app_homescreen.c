#include "ble_ams.h"

#include "app_homescreen.h"

////// include the nrf fonts 
#include "nrf_gfx_fonts.h"

#include "nrf_gfx.h"


void app_homescreen_init(void)
{
    
    nrf_gfx_lib_test();
    // nrf_gfx_lib_screen_refresh();
    NRF_LOG_INFO("homescreen");
    nrf_gfx_lib_set_bitmap(2,10,30,largeFont[0],48 );
}


void app_homescreen_window(void *param)
{

    UNUSED_PARAMETER(param);



    for (;;)
    {


    }
    
}


