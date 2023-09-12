#include "nrf_gfx.h"

#include "nrf_oled_driver.h"
#include "nrf_custom_log.h"


////==========================================================================================
//====================== static functions ====================================================

/// @brief send the frame data to the oled 
/// @param frame 
/// @param size 
static void oled_send_frame_data(const uint8_t * frame, uint16_t size)
{

}


//============================================================================================
///===================== basic graphics init library =========================================


/// @brief initalise the graphics library and 
/// @param ROW_MAX 
/// @param COLUMN_MAX 
void nrf_gfx_lib_init(uint16_t ROW_MAX , uint16_t COLUMN_MAX )
{
  
    
}

/// @brief to test the gfx library for testing 
/// @param  
void nrf_gfx_lib_test(void)
{
    ////send data to oled and turn it on
    /// set the horizontal mode and page addrss and column addresss 

    nrf_oled_set_addressing_mode(OLED_HORIZONTAL_ADDR_MODE);
    nrf_oled_set_page_addr(SSD_OLED_PAGE_0,SSD_OLED_PAGE_7);
    nrf_oled_set_column_addr(OLED_COLUMN_ADDR_START,OLED_COLUMN_ADDR_END);

    uint8_t img[] = {0x12,12,123,123,10,10};
    nrf_oled_send_img_data(img,sizeof(img));

}




