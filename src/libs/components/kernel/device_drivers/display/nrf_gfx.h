#ifndef _NRF_GFX_H_
#define  _NRF_GFX_H_


#include "system.h"



//============================================================================================
///===================== basic graphics init library =========================================

/// @brief initalise the graphics library and 
/// @param ROW_MAX 
/// @param COLUMN_MAX 
void nrf_gfx_lib_init(uint16_t ROW_MAX , uint16_t COLUMN_MAX );

/// @brief to test the gfx library for testing 
/// @param  
void nrf_gfx_lib_test(void);

void nrf_gfx_send_frame_data(const uint8_t * frame, uint16_t frame_size);

void nrf_gfx_set_pointer_location(uint8_t x, uint8_t y);

void nrf_gfx_set_bitmap(uint8_t x, uint8_t y , const uint8_t * img, uint16_t size );

void nrf_gfx_clear_screen(void);

void nrf_gfx_screen_refresh();

void nrf_


#endif