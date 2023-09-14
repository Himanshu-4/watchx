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


#include "nrf_gfx_fonts.h"

/// @brief to test the gfx library for testing 
/// @param  
void nrf_gfx_lib_test(void)
{
    ////send data to oled and turn it on
    /// set the horizontal mode and page addrss and column addresss 
    nrf_oled_flip_180(1);
    // nrf_oled_invert_display(OLED_DISPLAY_SET_INVERT);

    nrf_gfx_lib_clear_display();

    nrf_oled_set_addressing_mode(OLED_HORIZONTAL_ADDR_MODE);
    nrf_oled_set_page_addr(SSD_OLED_PAGE_0,SSD_OLED_PAGE_7);
    nrf_oled_set_column_addr(OLED_COLUMN_ADDR_START,OLED_COLUMN_ADDR_END);

    // uint8_t img[1024] = {0x01, [1022] = 0xff};

    // nrf_oled_send_img_data_from_ram(img,sizeof(img))
    nrf_oled_send_img_data_from_flash(ascii_char[4],sizeof(ascii_char[4]) * 20);

    nrf_oled_set_contrast_ratio(1);
    nrf_delay_ms(5000);
    nrf_oled_set_contrast_ratio(0xff);


}



/// @brief clear the display ram 
/// @param  
void nrf_gfx_lib_clear_display(void)
{
    
    nrf_oled_set_addressing_mode(OLED_HORIZONTAL_ADDR_MODE);
    nrf_oled_set_page_addr(SSD_OLED_PAGE_0,SSD_OLED_PAGE_7);
    nrf_oled_set_column_addr(OLED_COLUMN_ADDR_START,OLED_COLUMN_ADDR_END);
    uint8_t clear_ram[1024] ={0};
    nrf_oled_send_img_data_from_ram(clear_ram,sizeof(clear_ram));
}



