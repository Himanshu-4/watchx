#include "nrf_oled_driver.h"

#include "gpio.h"
#include "spi_thread_safe.h"

#include "nrf_custom_log.h"
#include "FreeRTOS.h"
#include "task.h"

#define OLED_SPI_USED NRF_CONFIG_OLED_SPI_HOST_USED

/// max amount transfer by spi at one go 
#define OLED_SPI_MAX_XFR_BYTES 255

#define OLED_RESET_PIN NRF_CONFIG_OLED_MOD_RESET_PIN
#define OLED_DATA_PIN NRF_CONFIG_OLED_DC_OUT_PIN
#define OLED_CS_PIN NRF_CONFIG_OLED_CHIP_SELECT_PIN

///========================================================================================================================
///================================================ macros functions here ============================================
///////////////////////////////////  oled mode ///////////////////////////////////////////////////
//// put the oled in commnad mode

//// this will put the oled in the commnad mode
#define NRF_OLED_CMD_MODE() \
    gpio_pin_reset(OLED_DATA_PIN);

//// this will put the oled in data mode
#define NRF_OLED_DATA_MODE() \
    gpio_pin_set(OLED_DATA_PIN);

//// this macro function will hard reset the OLEd module
#define NRF_OLED_HARD_RESET()       \
    gpio_pin_reset(OLED_RESET_PIN); \
    nrf_delay_ms(5);                \
    gpio_pin_set(OLED_RESET_PIN);

///========================================================================================================================
///================================================ static functions definations here ============================================

/// @brief  send the data to the oled through SPI
/// @param data
/// @param size
static void oled_send_cmd(const uint8_t *data, uint16_t size)
{
    NRF_OLED_CMD_MODE();
    uint8_t ret = 0;
    spi_xfr_buff buff =
        {
            .rx_buff = NULL,
            .rx_size = 0,
            .tx_buff = data,
            .tx_size = size};
    ret = spi_poll_xfr_thread_safe(OLED_SPI_USED, NRF_CONFIG_OLED_CHIP_SELECT_PIN, &buff);
    if (ret != nrf_OK)
    {
        NRF_LOG_ERROR("oled %d , cmd %d", ret, data[0]);
    }
}

/// @brief send data to the oled ram
/// @param data
/// @param size
static void oled_send_data(const uint8_t *data, uint16_t size)
{
    NRF_OLED_DATA_MODE();
    uint8_t ret = 0;
    //// split the transactions in multiple spi transfers into 256 bytes
    uint8_t split = GET_NO_OF_PERFECT_DIVIDE(size, OLED_SPI_MAX_XFR_BYTES);
    for (size_t i = 0; i < split; i++)
    {
        spi_xfr_buff buff =
            {
                .rx_buff = NULL,
                .rx_size = 0,
                .tx_buff = (data + i * OLED_SPI_MAX_XFR_BYTES),
                .tx_size = ((i == (split-1)) ? ( GET_REMNDER(size,OLED_SPI_MAX_XFR_BYTES) ?GET_REMNDER(size,OLED_SPI_MAX_XFR_BYTES):(OLED_SPI_MAX_XFR_BYTES) ) : (OLED_SPI_MAX_XFR_BYTES))};

        ret = spi_poll_xfr_thread_safe(OLED_SPI_USED, NRF_CONFIG_OLED_CHIP_SELECT_PIN, &buff);
      if (ret != nrf_OK)
        {
            NRF_LOG_ERROR("oled %d , data %d", ret, data[0]);
            return;
        }
    }
}



static void oled_pre_init(void)
{
    //// init all the pins here
    const my_gpio_cfg gpio_cfg =
        {
            .gpio_dir = GPIO_PIN_DIR_OUTPUT,
            .gpio_drive_type = GPIO_PIN_H0H1,
            .gpio_input_buff = GPIO_PIN_INPUT_BUFF_DISCONNECT,
            .gpio_sense_type = GPIO_PIN_NOSENSE,
            .gpiopull_mode = GPIO_PIN_NOPULL,
        };

    gpio_config(OLED_RESET_PIN, &gpio_cfg);
    gpio_config(OLED_DATA_PIN, &gpio_cfg);
    gpio_config(OLED_CS_PIN, &gpio_cfg);

    gpio_pin_set(OLED_RESET_PIN);
    gpio_pin_set(OLED_CS_PIN);
    gpio_pin_set(OLED_DATA_PIN);
}
///========================================================================================================================
///================================================ functions definations here ============================================


/// @brief this will init the oled module
/// @param  void
void nrf_oled_screen_init(void)
{
    /// pre init the oled pins
    oled_pre_init();

    NRF_OLED_HARD_RESET();

    delay(10);
    /// software init the oled module
    /// set mux ratio a8/3f
    /// set display start line
    /// set segment remap
    /// set com output scan direction
    /// set com pins hardware config
    /// set contrast control
    /// disable entire display on
    /// set normal display
    /// set osc freq
    /// enable charge pump regulator
    /// dispay on

    const uint8_t cmd[] =
        {
            SSD13X_REG_OLED_DRIVER_OFF,
            SSD13X_REG_SET_DISPLAY_CLOCK_DIVIDE_FREQ, 0x80, /*recommendded clock devider*/
            SSD13X_REG_SET_MULTIPLEX_RATIO, 0x3F,             /*recommednde 128x64*/
            SSD13X_REG_SET_DISPLAY_OFFSET, 0,               /*no offset */
            SSD13X_REG_SET_DISPLAY_START_LINE_S | 0,
            SSD13X_REG_CHARGE_PUMP_REGULATOR, SSD13X_ENABLE_CHARGE_PUMP,
            SSD13X_REG_SET_MEMORY_ADDRESING_MODE, OLED_HORIZONTAL_ADDR_MODE,
            SSD13X_REG_SEG0_MAP_TO_COL127, SSD13X_REG_SET_COM_OUT_SCAN_DIR_CN_TO_C0,
            SSD13X_REG_SET_COM_PINS_HARDWARE_CONF, 0x12, /*recommed enable seq com conf and enabel lef/reight remap*/
            SSD13X_REG_SET_CONTRAST_CONTROL, 0xCF,       /*recommended */
            SSD13X_REG_SET_PRECHARGE_PERIOD, 0xF1,       /*recommende in datasheet*/
            SSD13X_REG_SET_VCOMH_LEVEL, 0x40,            /* recommedn in datasheet*/
            SSD13X_REG_DISPLAY_ON_FOLLOWRAM,
            SSD13X_REG_NORMAL_DISPLAY_MODE,
            SSD13X_REG_DEACTIVATE_SCROLL,
            SSD13X_REG_OLED_DRIVER_ON};

    oled_send_cmd(cmd, sizeof(cmd));

}


/// @brief to reset the oled driver and to reinit it  
/// @param  void 
void nrf_oled_reset_driver(void)
{

    NRF_OLED_HARD_RESET();

    delay(10);
    /// software init the oled module
    /// set mux ratio a8/3f
    /// set display start line
    /// set segment remap
    /// set com output scan direction
    /// set com pins hardware config
    /// set contrast control
    /// disable entire display on
    /// set normal display
    /// set osc freq
    /// enable charge pump regulator
    /// dispay on

    const uint8_t cmd[] =
        {
            SSD13X_REG_OLED_DRIVER_OFF,
            SSD13X_REG_SET_DISPLAY_CLOCK_DIVIDE_FREQ, 0x80, /*recommendded clock devider*/
            SSD13X_REG_SET_MULTIPLEX_RATIO, 63,             /*recommednde 128x64*/
            SSD13X_REG_SET_DISPLAY_OFFSET, 0,               /*no offset */
            SSD13X_REG_SET_DISPLAY_START_LINE_S | 0,
            SSD13X_REG_CHARGE_PUMP_REGULATOR, SSD13X_ENABLE_CHARGE_PUMP,
            SSD13X_REG_SET_MEMORY_ADDRESING_MODE, 0x00,
            SSD13X_REG_SEG0_MAP_TO_COL0, SSD13X_REG_SET_COM_OUT_SCAN_DIR_C0_TO_CN,
            SSD13X_REG_SET_COM_PINS_HARDWARE_CONF, 0x12, /*recommed enable seq com conf and enabel lef/reight remap*/
            SSD13X_REG_SET_CONTRAST_CONTROL, 0xCF,       /*recommended */
            SSD13X_REG_SET_PRECHARGE_PERIOD, 0xF1,       /*recommende in datasheet*/
            SSD13X_REG_SET_VCOMH_LEVEL, 0x40,            /* recommedn in datasheet*/
            SSD13X_REG_NORMAL_DISPLAY_MODE,
            SSD13X_REG_DEACTIVATE_SCROLL,
            SSD13X_REG_DISPLAY_ON_FOLLOWRAM,
            SSD13X_REG_OLED_DRIVER_ON};

    oled_send_cmd(cmd, sizeof(cmd));
}


/// @brief set contrast ratio for the oled
/// @param contrast
void nrf_oled_set_contrast_ratio(uint8_t contrast)
{
    uint8_t cmd_buff[] = {SSD13X_REG_SET_CONTRAST_CONTROL, contrast};
    oled_send_cmd(cmd_buff, sizeof(cmd_buff));
}

/// @brief turn on the oled display and start following ram
/// @param follow_ram
void nrf_oled_display_on(uint8_t follow_ram)
{
    uint8_t cmd = 0;
    cmd = (follow_ram) ? (SSD13X_REG_DISPLAY_ON_FOLLOWRAM) : (SSD13X_REG_DISPLAY_ON_IGNORERAM);
    oled_send_cmd(&cmd, 1);
}

/// @brief  wakeup the ssd1306 from sleep mode
/// @param  void
void nrf_oled_driver_on(void)
{
    uint8_t cmd = SSD13X_REG_OLED_DRIVER_ON;
    oled_send_cmd(&cmd, 1);
}

/// @brief this will put the ssd1306 in sleep mode
/// @param  void
void nrf_olrf_oled_driver_off(void)
{
    uint8_t cmd = SSD13X_REG_OLED_DRIVER_OFF;
    oled_send_cmd(&cmd, 1);
}

/// @brief invert the display or normal mode .. this will reverse the color ordering 
/// @param mode
void nrf_oled_invert_display(uint8_t mode)
{
    uint8_t cmd = 0;
    cmd = (mode == OLED_DISPLAY_SET_INVERT) ? (SSD13X_REG_INVERSE_DISPLAY_MODE) : (SSD13X_REG_NORMAL_DISPLAY_MODE);
    oled_send_cmd(&cmd, 1);
}

////////////=================================================================================================
///// =======================hardware configuration (panel resolution and layout changes ) APIs =============

/// @brief flip the oled to 180 degree
/// @param  input
void nrf_oled_flip_180(bool input)
{
    //// rotate the display by 180
    if (input)
    {
        uint8_t cmd[] = {SSD13X_REG_SEG0_MAP_TO_COL127, SSD13X_REG_SET_COM_OUT_SCAN_DIR_CN_TO_C0};
        oled_send_cmd(cmd, sizeof(cmd));
    }
    //// dont roatate display
    else
    {
        uint8_t cmd[] = {SSD13X_REG_SEG0_MAP_TO_COL0, SSD13X_REG_SET_COM_OUT_SCAN_DIR_C0_TO_CN};
        oled_send_cmd(cmd, sizeof(cmd));
    }
}


///================================================ scrolling functions definations here ============================================

/// @brief to activate the scrolling 
/// @param  void 
void nrf_oled_activate_scroll(void)
{
    uint8_t cmd = SSD13X_REG_ACTIVATE_SCROLL;
    oled_send_cmd(&cmd,1);
}

/// @brief deactivate the scrolling 
/// @param  void 
void nrf_oled_deactivate_scroll(void)
{
    uint8_t cmd = SSD13X_REG_DEACTIVATE_SCROLL;
    oled_send_cmd(&cmd,1);
}

/// @brief activate the horizontal scroll and setting it 
/// @param scroll_type 
/// @param start_page 
/// @param end_page 
/// @param frame_freq 
void nrf_oled_config_horizontal_scroll(uint8_t scroll_type, uint8_t start_page
, uint8_t end_page, uint8_t frame_freq)
{
    if((start_page >= SSD_OLED_PAGE_MAX) || (end_page >= SSD_OLED_PAGE_MAX ))
    {
        return ;
    }
    if(frame_freq >= scroll_freq_max) return;
    const uint8_t cmd[] = 
    {
        ((scroll_type == OLED_SCROLL_TYPE_LEFT)?
        (SSD13X_REG_HORIZONTAL_SCROOL_TO_LEFT):(SSD13X_REG_HORIZONTAL_SCROOL_TO_RIGHT)),
        0x00,start_page,frame_freq,end_page,0x00,0xff
    };
    oled_send_cmd(cmd, sizeof(cmd));
}

/// @brief configure the vertical and horixontal scroll
/// @param scroll_type 
/// @param start_page 
/// @param end_page 
/// @param frame_freq 
/// @param vertical_row_offset 
void nrf_oled_config_vert_and_horizontal_scroll(uint8_t scroll_type, uint8_t start_page,
uint8_t end_page , uint8_t frame_freq, uint8_t vertical_row_offset)
{
    if((start_page >= SSD_OLED_PAGE_MAX) || (end_page >= SSD_OLED_PAGE_MAX ))
    {
        return ;
    }
    if(frame_freq >= scroll_freq_max) return;
    if(vertical_row_offset >= SSD_OLED_ROW_ADDR_MAX) return;

    const uint8_t cmd[] = 
    {
        ((scroll_type == OLED_SCROLL_TYPE_LEFT)?
        (SSD13X_REG_VERTICAL_AND_HORIZONTAL_SCROLL_LEFT):(SSD13X_REG_VERTICAL_AND_HORIZONTAL_SCROLL_RIGHT)),
        0x00,start_page,frame_freq,end_page,vertical_row_offset
    };
        oled_send_cmd(cmd, sizeof(cmd));
}

/// @brief config the ertical scroll area 
/// @param start_row 
/// @param end_row 
void nrf_oled_config_vertical_scroll_area(uint8_t start_row_fixed, uint8_t total_row_in_scroll)
{
    if((start_row_fixed >= SSD_OLED_ROW_ADDR_MAX) || (total_row_in_scroll >= SSD_OLED_COLUMN_ADDR_MAX))
    return;

    if((start_row_fixed + total_row_in_scroll) >= SSD_OLED_ROW_ADDR_MAX)
    return;

    uint8_t cmd[] =
    {
        SSD13X_REG_SET_VERTICAL_SCROLL_AREA,
        start_row_fixed,total_row_in_scroll
    };

    oled_send_cmd(cmd,sizeof(cmd));
}


///================================================ addressing functions definations here ============================================



/// @brief set the addressing mode of the oled 
/// @param mode 
void nrf_oled_set_addressing_mode(uint8_t mode)
{
    uint8_t cmd[] =
    {
        SSD13X_REG_SET_MEMORY_ADDRESING_MODE,mode
    };
    oled_send_cmd(cmd,sizeof(cmd));
}

/// @brief set the oled page addressing ,it is not self incrementing 
/// @param page_start_addr 
/// @param lower_colum_addr 
/// @param higher_colum_addr 
void nrf_oled_config_page_addressing(uint8_t page_start_addr, uint8_t column_start_Addr)
{
    uint8_t cmd[] =
    {
        (SSD13X_REG_SET_PAGE_START_ADDRESS_FOR_PAGE_ADDRESSING_MASK | page_start_addr),
        (SSD13X_REG_SET_LOWER_COLUM_ADDR_FORPAGE_ADDRESSING_MASK | (LOWER_NIBBLE_MASK & column_start_Addr)),
        (SSD13X_REG_SET_HIGHER_COLUM_ADDR_FORPAGE_ADDRESSING_MASK | ((column_start_Addr >> NO_OF_NIBBLE_BITS) & LOWER_NIBBLE_MASK))
    };
    oled_send_cmd(cmd,sizeof(cmd));
}

/// @brief set column address for hoircontal or vertical scrolling mode 
/// @param colum_start_addr 
/// @param colum_end_addr 
/// @return 
void nrf_oled_set_column_addr(uint8_t colum_start_addr, uint8_t colum_end_addr)
{
       if((colum_start_addr >= SSD_OLED_COLUMN_ADDR_MAX) || (colum_end_addr >= SSD_OLED_COLUMN_ADDR_MAX))
    return;

    uint8_t cmd[] = 
    {
        SSD13X_REG_SET_COLUMN_ADDRESSING, colum_start_addr,colum_end_addr
    };
    oled_send_cmd(cmd,sizeof(cmd));
}


/// @brief to set the page addresse for horizontal or vertical addressing mode 
/// @param page_start_addr 
/// @param page_end_addr 
/// @return err codes 
void nrf_oled_set_page_addr(uint8_t page_start_addr, uint8_t page_end_addr)
{
    if((page_start_addr >= SSD_OLED_PAGE_MAX) || (page_end_addr >= SSD_OLED_PAGE_MAX))
    return;

    uint8_t cmd[] = 
    {
        SSD13X_REG_SET_PAGE_ADDRESSING,page_start_addr,page_end_addr
    };
    oled_send_cmd(cmd,sizeof(cmd));
}



/// @brief to send the oled data to the module 
/// @param data 
/// @param size 
void nrf_oled_send_img_data(const uint8_t * data, uint16_t size)
{
    //// copy the data content to ram buffer 

    /// send the data to the spi driver 
    oled_send_data(data,size);
}