#ifndef _NRF_GFX_H_
#define  _NRF_GFX_H_


#include "system.h"



//============================================================================================
///===================== basic graphics init library =========================================

/// @brief initalise the graphics library and 
/// @param ROW_MAX 
/// @param COLUMN_MAX 
void nrf_gfx_lib_init(uint16_t ROW_MAX , uint16_t COLUMN_MAX );


/// @brief deinit the gfx library 
/// @param  
void nrf_gfx_lib_deinit(void);

/// @brief reset the graphics library 
/// @param  
void nrf_gfx_lib_reset(void);


/// @brief to test the gfx library for testing 
/// @param  
void nrf_gfx_lib_test(void);

/// @brief send the frame data to the graphics library 
/// @param frame 
/// @param frame_size 
void nrf_gfx_lib_send_frame_data(const uint8_t * frame, uint16_t frame_size);

/// @brief set the pointer to the display to fill the ram buffer 
/// @param x 
/// @param y 
/// @param addressing_mode 
void nrf_gfx_lib_set_pointer_location(uint8_t x, uint8_t y, uint8_t addr_modes);


/// @brief send the bitmap data to the graphics lib 
/// @param start_x must be size (0-7)
/// @param start_y must be (0-127) column 
/// @param end_x must be size (0-7)
/// @param end_y must be (0-127) column 
/// @param img 
/// @param size 
void nrf_gfx_lib_set_bitmap(uint8_t start_x, uint8_t start_y ,uint8_t end_x, uint8_t end_y ,const uint8_t * img, uint16_t size );

/// @brief clear the display ram 
/// @param  void
void nrf_gfx_lib_clear_display(void);

/// @brief referesh the screen and oled driver 
/// @param  
void nrf_gfx_lib_screen_refresh(void);


/// @brief draw string on the oled with start location 
/// @param startx 
/// @param starty 
/// @param string 
/// @param string_size 
void nrf_gfx_lib_draw_string(uint8_t startx,uint8_t starty,const char *string, uint16_t string_size);


/// @brief set the display brightness in terms of percentage 
/// @param percentage 
void nrf_gfx_lib_set_display_brightness(uint8_t percentage);



#endif