#include "nrf_gfx.h"

#include "nrf_oled_driver.h"
#include "nrf_custom_log.h"

#include "nrf_gfx_fonts.h"

////==========================================================================================
//====================== static functions ====================================================

const uint8_t clear_ram_content[1024] ={0};


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

/// @brief draw string on the oled with start location 
/// @param startx 
/// @param starty 
/// @param string 
/// @param string_size 
void nrf_gfx_lib_draw_string(uint8_t startx,uint8_t starty,const char *string, uint16_t string_size)
{
    uint32_t ret =0;
    /// set the horizontal addressing mode
    ret =nrf_oled_set_addressing_mode(OLED_PAGE_ADDR_MODE);
    NRF_ASSERT(ret);
    ret =nrf_oled_config_page_addressing(startx,starty);
    NRF_ASSERT(ret);

    char my_string[string_size][5];

    for (size_t i = 0; i < string_size; i++)
    {
        /* code */
        memcpy(my_string[i],ascii_char[(int)string[i] - ASCII_CHAR_ARR_OFFSET_VALUE],5);
    }
    
    nrf_oled_send_img_data_from_ram(u8_ptr my_string,sizeof(my_string));
}

/// @brief send the bitmap data to the graphics lib 
/// @param x 
/// @param y 
/// @param img 
/// @param size 
void nrf_gfx_lib_set_bitmap(uint8_t x, uint8_t y ,uint8_t width , const uint8_t * img, uint16_t size )
{
    uint32_t ret =0;
    /// set the horizontal addressing mode so that it automatically shift to next page
    ret = nrf_oled_set_addressing_mode(OLED_HORIZONTAL_ADDR_MODE);

    NRF_ASSERT(ret);
    /// set the start page and column adddr 

    uint8_t height = (size/width)  - 1;
    

    ret= nrf_oled_set_page_addr(x,height);
    NRF_ASSERT(ret);
    ret = nrf_oled_set_column_addr(y,width);
    NRF_ASSERT(ret);

    /// send the bitmap data 
    nrf_oled_send_img_data_from_flash(img,size);

}


/// @brief to test the gfx library for testing 
/// @param  
void nrf_gfx_lib_test(void)
{
    ////send data to oled and turn it on
    /// set the horizontal mode and page addrss and column addresss 
    nrf_oled_flip_180(1);
    nrf_oled_invert_display(OLED_DISPLAY_SET_INVERT);
    nrf_gfx_lib_clear_display();

    // nrf_oled_set_addressing_mode(OLED_HORIZONTAL_ADDR_MODE);
    // nrf_oled_set_page_addr(SSD_OLED_PAGE_0,SSD_OLED_PAGE_7);
    // nrf_oled_set_column_addr(OLED_COLUMN_ADDR_START,10);

    // uint8_t img[1024] = {0x01, [1022] = 0xff};

  

    // const char * str = "Himanshu jangra ";
    // nrf_gfx_lib_draw_string(10,0,str,strlen(str));
    const char *s = "swapnil broh whats up ";
    nrf_gfx_lib_draw_string(0,10,s,strlen(s));

    // nrf_oled_send_img_data_from_flash(midFont[0], sizeof(midFont[4]));

    //   nrf_oled_config_horizontal_scroll(OLED_SCROLL_TYPE_LEFT, SSD_OLED_PAGE_0,SSD_OLED_PAGE_0,scroll_freq_5fps);

    // nrf_oled_config_horizontal_scroll(OLED_SCROLL_TYPE_RIGHT,SSD_OLED_PAGE_0,SSD_OLED_PAGE_0,scroll_freq_2fps);
    // nrf_oled_config_vert_and_horizontal_scroll(OLED_SCROLL_TYPE_LEFT ,SSD_OLED_PAGE_0,SSD_OLED_PAGE_0,scroll_freq_5fps,8);
    // nrf_oled_config_vertical_scroll_area(0,64);
    // nrf_oled_activate_scroll();
}



/// @brief clear the display ram 
/// @param  
void nrf_gfx_lib_clear_display(void)
{
    
    nrf_oled_set_addressing_mode(OLED_HORIZONTAL_ADDR_MODE);
    nrf_oled_set_page_addr(SSD_OLED_PAGE_0,SSD_OLED_PAGE_7);
    nrf_oled_set_column_addr(OLED_COLUMN_ADDR_START,OLED_COLUMN_ADDR_END);
    
    /// we can ignore the stuff that the spi driver will transmit 0 data because the dma cannt refer to flash addresess
    nrf_oled_send_img_data_from_ram(clear_ram_content,sizeof(clear_ram_content));
}

/// @brief referesh the screen and oled driver 
/// @param  
void nrf_gfx_lib_screen_refresh(void)
{
    nrf_oled_reset_driver();
}


/// @brief set the display brightness in terms of percentage 
/// @param percentage 
void nrf_gfx_lib_set_display_brightness(uint8_t percentage)
{
    // printf("%d ",map_i(percentage,0,100,0,0xff));
    nrf_oled_set_contrast_ratio(map_i(percentage,0,100,0,0xff));
}
