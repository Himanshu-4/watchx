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
#define SSD13X_REG_HORIZONTAL_SCROOL_TO_LLEFT 0x27
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

#endif