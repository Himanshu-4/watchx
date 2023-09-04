#ifndef _NRF_OLED_DRIVER_H_
#define _NRF_OLED_DRIVER_H_

#include "system.h"


///// define the OLEd registers here 
#define SSD13X_REG_SET_CONTRAST_CONTROL 0x81
#define SSD13X_REG_DISPLAY_ON_FOLLOWRAM 0xA4
#define SSD13X_REG_DISPLAY_ON_IGNORERAM 0xA5
#define SSD13X_REG_NORMAL_DISPLAY_MODE 0xA6
#define SSD13X_REG_INVERSE_DISPLAY_MODE 0xA7
#define SSD13X_REG_ENTIRE_DISPLAY_OFF 0xAE
#define SSD13X_REG_ENTIRE_DISPLAY_ON 0xAF

/// scrolling related table 

#define SSD13X_REG_DEACTIVATE_SCROLL 0x2E   // afterdeactivate scrolling ram needs to be rewritten 
#define SSD13X_REG_ACTIVATE_SCROLL 0x2F // after 
#define SSD13X_REG_HORIZONTAL_SCROOL_TO_RIGHT 0x26
#define SSD13X_REG_HORIZONTAL_SCROOL_TO_LEFT 0x27
#define SSD13X_REG_VERTICAL_AND_HORIZONTAL_SCROLL_RIGHT 0x29
#define SSD13X_REG_VERTICAL_AND_HORIZONTAL_SCROLL_LEFT 0x2A
#define SSD13X_REG_SET_VERTICAL_SCROLL_AREA 0xA3

//// Addressing setting table 

#define SSD13X_REG_SET_MEMORY_ADDRESING_MODE 0x20 /// 0x00 horizontal addr mode ,0x01 vertical addr mode , 0x10 page addr mode 
#define SSD13X_REG_SET_LOWER_COLUM_ADDR_FORPAGE_ADDRESSING_MASK 0x00 /// 0 - F only 
#define SSD13X_REG_SET_HIGHER_COLUM_ADDR_FORPAGE_ADDRESSING_MASK 0x10 /// 0 - F only 
#define SSD13X_REG_SET_PAGE_ADDRESSING 0x22 /// only for horizontal and vertical addessing mode 
#define SSD13X_REG_SET_PAGE_START_ADDRESS_FOR_PAGE_ADDRESSING_MASK 0xB0 // page 0 - page 7 only 

//// hardware configuration (panel resolution and layout related) commnand table 
#define SSD13X_REG_SET_DISPLAY_START_LINE_S 0x40 
#define SSD13X_REG_SET_DISPLAY_START_LINE_E 0x7F

#define SSD13X_REG_SEG0_MAP_TO_COL0 0xA0 /// reset beahviour
#define SSD13X_REG_SEG0_MAP_TO_COL127 0xA1 

#define SSD13X_REG_SET_MULTIPLEX_RATIO 0xA8 /// set mux ratio to n+1 mux reset is 64 mux
#define SSD13X_REG_SET_COM_OUT_SCAN_DIR_C0_TO_CN 0xC0 /// reset 
#define SSD13X_REG_SET_COM_OUT_SCAN_DIR_CN_TO_C0 0xC8

#define SSD13X_REG_SET_DISPLAY_OFFSET 0xD3 
#define SSD13X_REG_SET_COM_PINS_HARDWARE_CONF 0xDA 


//// timming and driving scheme setting command table 

#define SSD13X_REG_SET_DISPLAY_CLOCK_DIVIDE_FREQ 0xD5
#define SSD13X_REG_SET_PRECHARGE_PERIOD 0xD9
#define SSD13X_REG_SET_VCOMH_LEVEL  0xDB
#define SSD13X_REG_NOP 0xE3


#define SSD13X_ENABLE_CHARGE_PUMP (_BV(2))
#define SSD13X_DISABLE_CHARGE_PUMP (0x00U)

#define SSD13X_REG_CHARGE_PUMP_REGULATOR 0x8D 


enum _NRF_OLED_PAGES_ADDR_
{
    NRF_OLED_PAGE_0,
    NRF_OLED_PAGE_1,
    NRF_OLED_PAGE_2,
    NRF_OLED_PAGE_3,
    NRF_OLED_PAGE_4,
    NRF_OLED_PAGE_5,
    NRF_OLED_PAGE_6,
    NRF_OLED_PAGE_7,
};


///////////////// fundametnal OLed APIs 

uint32_t nrf_oled_screen_init(void);

uint32_t nrf_oled_set_contrast_ratio(uint8_t contrast);

#define NRF_OLED_FOLLOW_RAM 0x01u
#define NRF_OLED_IGNORES_RAM 0x00U


uint32_t nrf_oled_display_on(uint8_t follow_ram);

uint32_t nrf_oled_entire_display_on(void);

uint32_t nrf_olrf_entire_display_off(void);


uint32_t nrf_oled_set_mem_addressing_mode(uint8_t mode);

uint32_t nrf_oled_set_page_addressing_mode(uint8_t page_addr);


///////////////////  scrolling related APIs 
enum _SCROLL_FRAME_FREQ_
{
    sc_freq_5fps = 0x00,
    sc_freq_64fps,
    sc_freq_128fps,
    sc_freq_256fps,
    sc_freq_3fps,
    sc_freq_4fps,
    sc_freq_25fps,
    sc_freq_2fps,
};


uint32_t nrf_oled_activate_scroll(void);

uint32_t nrf_oled_deactivate_scroll(void);

#define OLED_SCROLL_LEFT 0x10u
#define OLED_SCROLL_RIGHT 0x20u

uint32_t nrf_oled_horizontal_scroll(uint8_t scroll_type, uint8_t start_page
, uint8_t end_page, uint8_t frame_freq);


uint32_t nrf_oled_vert_and_horizontal_scroll(uint8_t scroll_type, uint8_t start_page,
uint8_t end_page , uint8_t frame_freq, uint8_t vertical_row_offset);


uint32_t nrf_oled_vertical_scroll_area(uint8_t start_row, uint8_t end_row);


///////////////// addressing Table APIs 

#define OLED_HORIZONTAL_ADDR_MODE 0x00u
#define OLED_VERTICAL_ADDR_MODE 0x01u
#define OLED_PAGE_ADDR_MODE 0x02u

uint32_t nrf_oled_set_addressing_mode(uint8_t mode);


uint32_t nrf_oled_page_adressing(uint8_t page_start_addr, uint8_t lower_colum_addr, uint8_t higher_colum_addr);

uint32_t nrf_oled_set_page_addr(uint8_t page_start_addr, uint8_t page_end_addr);

uint32_t nrf_oled_set_column_addr(uint8_t colum_start_addr, uint8_t colum_end_addr);


//////// hardware configuration (panel resolution and layout changes ) APIs 





////// timming and driving API for the oled 






#endif