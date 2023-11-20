#ifndef _NRF_OLED_DRIVER_H_
#define _NRF_OLED_DRIVER_H_

#include "system.h"


///// define the OLEd registers here 
#define SSD13X_REG_SET_CONTRAST_CONTROL 0x81
#define SSD13X_REG_DISPLAY_ON_FOLLOWRAM 0xA4
#define SSD13X_REG_DISPLAY_ON_IGNORERAM 0xA5
#define SSD13X_REG_NORMAL_DISPLAY_MODE 0xA6
#define SSD13X_REG_INVERSE_DISPLAY_MODE 0xA7
#define SSD13X_REG_OLED_DRIVER_OFF 0xAE
#define SSD13X_REG_OLED_DRIVER_ON 0xAF

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
#define SSD13X_REG_SET_COLUMN_ADDRESSING 0x21 /// only for horizontal and vertical addessing mode
#define SSD13X_REG_SET_PAGE_ADDRESSING 0x22 /// only for horizontal and vertical addessing mode 
#define SSD13X_REG_SET_PAGE_START_ADDRESS_FOR_PAGE_ADDRESSING_MASK 0xB0 // page 0 - page 7 only 

//// hardware configuration (panel resolution and layout related) commnand table 
#define SSD13X_REG_SET_DISPLAY_START_LINE_S 0x40 
#define SSD13X_REG_SET_DISPLAY_START_LINE_E 0x7F

#define SSD13X_REG_SEG0_MAP_TO_COL0 0xA0 /// reset beahviour
#define SSD13X_REG_SEG0_MAP_TO_COL127 0xA1 

#define SSD13X_REG_SET_COM_OUT_SCAN_DIR_C0_TO_CN 0xC0 /// reset 
#define SSD13X_REG_SET_COM_OUT_SCAN_DIR_CN_TO_C0 0xC8

#define SSD13X_REG_SET_MULTIPLEX_RATIO 0xA8 /// set mux ratio to n+1 mux reset is 64 mux

#define SSD13X_REG_SET_DISPLAY_OFFSET 0xD3 
#define SSD13X_REG_SET_COM_PINS_HARDWARE_CONF 0xDA 


//// timming and driving scheme setting command table 

#define SSD13X_REG_SET_DISPLAY_CLOCK_DIVIDE_FREQ 0xD5
#define SSD13X_REG_SET_PRECHARGE_PERIOD 0xD9
#define SSD13X_REG_SET_VCOMH_LEVEL  0xDB
#define SSD13X_REG_NOP 0xE3


#define SSD13X_ENABLE_CHARGE_PUMP 0x14U
#define SSD13X_DISABLE_CHARGE_PUMP 0x00U

#define SSD13X_REG_CHARGE_PUMP_REGULATOR 0x8D 



/// @brief contains the ssd oled pages 
enum _NRF_OLED_PAGES_ADDR_
{
    SSD_OLED_PAGE_0,
    SSD_OLED_PAGE_1,
    SSD_OLED_PAGE_2,
    SSD_OLED_PAGE_3,
    SSD_OLED_PAGE_4,
    SSD_OLED_PAGE_5,
    SSD_OLED_PAGE_6,
    SSD_OLED_PAGE_7,
    SSD_OLED_PAGE_MAX
};

#define OLED_COLUMN_ADDR_START 0x00
#define OLED_COLUMN_ADDR_END 0x7f
//// these addresses are starting from zero 

#define SSD_OLED_COLUMN_ADDR_MAX 128
#define SSD_OLED_ROW_ADDR_MAX 64


///////////////// fundametnal OLed APIs 

/// @brief this will init the oled module 
/// @param  void 
void nrf_oled_screen_init(void);

/// @brief set contrast ratio for the oled 
/// @param contrast 
void nrf_oled_set_contrast_ratio(uint8_t contrast);

#define NRF_OLED_FOLLOW_RAM 0x01u
#define NRF_OLED_IGNORES_RAM 0x00U


/// @brief turn on the oled display and start following ram 
/// @param follow_ram 
void nrf_oled_display_on(uint8_t follow_ram);

/// @brief  wakeup the ssd1306 from sleep mode  
/// @param  void 
void nrf_oled_driver_on(void);

/// @brief this will put the ssd1306 in sleep mode 
/// @param  void 
void nrf_olrf_oled_driver_off(void);

#define OLED_DISPLAY_SET_INVERT 0x01
#define OLED_DISPLAY_SET_NORMAL 0x02

/// @brief invert the display or normal mode 
/// @param mode 
void nrf_oled_invert_display(uint8_t mode);

/// @brief flip the oled to 180 degree 
/// @param  input
void nrf_oled_flip_180(bool input);

/// @brief to reset the oled driver and to reinit it  
/// @param  void 
void nrf_oled_reset_driver(void);
////////////=================================================================================================
///// ====================================== to set the address in horizontal and vertical addressing mode 

///////////////////  scrolling related APIs 
enum _SCROLL_FRAME_FREQ_
{
    scroll_freq_5fps = 0x00,
    scroll_freq_64fps,
    scroll_freq_128fps,
    scroll_freq_256fps,
    scroll_freq_3fps,
    scroll_freq_4fps,
    scroll_freq_25fps,
    scroll_freq_2fps,
    scroll_freq_max
};

/// @brief to activate the scrolling 
/// @param  void 
void nrf_oled_activate_scroll(void);

/// @brief deactivate the scrolling 
/// @param  void 
void nrf_oled_deactivate_scroll(void);

#define OLED_SCROLL_TYPE_LEFT 0x10u
#define OLED_SCROLL_TYPE_RIGHT 0x20u

/// @brief activate the horizontal scroll and setting it 
/// @param scroll_type 
/// @param start_page 
/// @param end_page 
/// @param frame_freq 
/// @return succ/failure
uint32_t nrf_oled_config_horizontal_scroll(uint8_t scroll_type, uint8_t start_page
, uint8_t end_page, uint8_t frame_freq);


/// @brief configure the vertical and horixontal scroll
/// @param scroll_type 
/// @param start_page 
/// @param end_page 
/// @param frame_freq 
/// @param vertical_row_offset 
/// @return succ/failure
uint32_t nrf_oled_config_vert_and_horizontal_scroll(uint8_t scroll_type, uint8_t start_page,
uint8_t end_page , uint8_t frame_freq, uint8_t vertical_row_offset);

/// @brief config the ertical scroll area 
/// @param start_row 
/// @param end_row 
/// @return succ/failure
uint32_t nrf_oled_config_vertical_scroll_area(uint8_t start_row_fixed, uint8_t total_row_in_scroll);

////////////=================================================================================================
///// ====================================== to set the address in horizontal and vertical addressing mode 
///////////////// addressing Table APIs 

#define OLED_HORIZONTAL_ADDR_MODE 0x00u
#define OLED_VERTICAL_ADDR_MODE 0x01u
#define OLED_PAGE_ADDR_MODE 0x02u

/// @brief set the addressing mode of the oled 
/// @param mode 
/// @return succ/failure
uint32_t nrf_oled_set_addressing_mode(uint8_t mode);

/// @brief set the oled page addressing ,it is not self incrementing  
/// @param page_start_addr 
/// @param column_start_addr 
/// @return succ/failure
uint32_t nrf_oled_config_page_addressing(uint8_t page_start_addr, uint8_t column_start_Addr);


/// @brief to set the page addresse for horizontal or vertical addressing mode 
/// @param page_start_addr 
/// @param page_end_addr 
/// @return succ/failure
uint32_t nrf_oled_set_page_addr(uint8_t page_start_addr, uint8_t page_end_addr);

/// @brief set column address for hoircontal or vertical scrolling mode 
/// @param colum_start_addr 
/// @param colum_end_addr 
/// @return succ/failure
uint32_t nrf_oled_set_column_addr(uint8_t colum_start_addr, uint8_t colum_end_addr);


//=======================================================================================
//===================== send the image data to the oled driver 

/// @brief to send the oled data to the module 
/// @param data 
/// @param size 
void nrf_oled_send_img_data_from_ram(const uint8_t * data, uint16_t size);


/// @brief to send the img data from the the flash 
/// @param data 
/// @param size 
void nrf_oled_send_img_data_from_flash(const uint8_t * data, uint16_t size);


#endif