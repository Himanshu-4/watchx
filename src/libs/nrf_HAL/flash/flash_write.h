#ifndef _FLASH_WRITE_H
#define _FLASH_WRITE_H

#include "system.h"

enum
{
    NVMC_IS_BUSY,
    NVMC_IS_READY
};


enum
{
    NVMC_READ_ENABLE =0UL,
    NVMC_WRITE_ENABLE = 1UL,
    NVMC_ERASE_ENABLE = 2UL
};

#define Page_ADDR_OFFSET 0x00001000UL


enum
{   
    PAGE_0_ADDR = 0x00000000UL,
    PAGE_1_ADDR = 0x00001000UL,
    PAGE_2_ADDR = 0x00002000UL,
    PAGE_3_ADDR = 0x00003000UL,
    PAGE_4_ADDR = 0x00004000UL,
    PAGE_5_ADDR = 0x00005000UL,
    PAGE_6_ADDR = 0x00006000UL,
    PAGE_7_ADDR = 0x00007000UL,
    PAGE_8_ADDR = 0x00008000UL,
    PAGE_9_ADDR = 0x00009000UL,
    PAGE_10_ADDR = 0x0000A000UL,
    PAGE_11_ADDR = 0x0000B000UL,
    PAGE_12_ADDR = 0x0000C000UL,
    PAGE_13_ADDR = 0x0000D000UL,
    PAGE_14_ADDR = 0x0000E000UL,
    PAGE_15_ADDR = 0x0000F000UL,
    PAGE_16_ADDR = 0x00010000UL,
    PAGE_17_ADDR = 0x00011000UL,
    PAGE_18_ADDR = 0x00012000UL,
    PAGE_19_ADDR = 0x00013000UL,
    PAGE_20_ADDR = 0x00014000UL,
    PAGE_21_ADDR = 0x00015000UL,
    PAGE_22_ADDR = 0x00016000UL,
    PAGE_23_ADDR = 0x00017000UL,
    PAGE_24_ADDR = 0x00018000UL,
    PAGE_25_ADDR = 0x00019000UL,
    PAGE_26_ADDR = 0x0001A000UL,
    PAGE_27_ADDR = 0x0001B000UL,
    PAGE_28_ADDR = 0x0001C000UL,
    PAGE_29_ADDR = 0x0001D000UL,
    PAGE_30_ADDR = 0x0001E000UL,
    PAGE_31_ADDR = 0x0001F000UL,
    PAGE_32_ADDR = 0x00020000UL,
    PAGE_33_ADDR = 0x00021000UL,
    PAGE_34_ADDR = 0x00022000UL,
    PAGE_35_ADDR = 0x00023000UL,
    PAGE_36_ADDR = 0x00024000UL,
    PAGE_37_ADDR = 0x00025000UL,
    PAGE_38_ADDR = 0x00026000UL,
    PAGE_39_ADDR = 0x00027000UL,
    PAGE_40_ADDR = 0x00028000UL,
    PAGE_41_ADDR = 0x00029000UL,
    PAGE_42_ADDR = 0x0002A000UL,
    PAGE_43_ADDR = 0x0002B000UL,
    PAGE_44_ADDR = 0x0002C000UL,
    PAGE_45_ADDR = 0x0002D000UL,
    PAGE_46_ADDR = 0x0002E000UL,
    PAGE_47_ADDR = 0x0002F000UL,
    PAGE_48_ADDR = 0x00030000UL,
    PAGE_49_ADDR = 0x00031000UL,
    PAGE_50_ADDR = 0x00032000UL,
    PAGE_51_ADDR = 0x00033000UL,
    PAGE_52_ADDR = 0x00034000UL,
    PAGE_53_ADDR = 0x00035000UL,
    PAGE_54_ADDR = 0x00036000UL,
    PAGE_55_ADDR = 0x00037000UL,
    PAGE_56_ADDR = 0x00038000UL,
    PAGE_57_ADDR = 0x00039000UL,
    PAGE_58_ADDR = 0x0003A000UL,
    PAGE_59_ADDR = 0x0003B000UL,
    PAGE_60_ADDR = 0x0003C000UL,
    PAGE_61_ADDR = 0x0003D000UL,
    PAGE_62_ADDR = 0x0003E000UL,
    PAGE_63_ADDR = 0x0003F000UL,
    PAGE_64_ADDR = 0x00040000UL,
    PAGE_65_ADDR = 0x00041000UL,
    PAGE_66_ADDR = 0x00042000UL,
    PAGE_67_ADDR = 0x00043000UL,
    PAGE_68_ADDR = 0x00044000UL,
    PAGE_69_ADDR = 0x00045000UL,
    PAGE_70_ADDR = 0x00046000UL,
    PAGE_71_ADDR = 0x00047000UL,
    PAGE_72_ADDR = 0x00048000UL,
    PAGE_73_ADDR = 0x00049000UL,
    PAGE_74_ADDR = 0x0004A000UL,
    PAGE_75_ADDR = 0x0004B000UL,
    PAGE_76_ADDR = 0x0004C000UL,
    PAGE_77_ADDR = 0x0004D000UL,
    PAGE_78_ADDR = 0x0004E000UL,
    PAGE_79_ADDR = 0x0004F000UL,
    PAGE_80_ADDR = 0x00050000UL,
    PAGE_81_ADDR = 0x00051000UL,
    PAGE_82_ADDR = 0x00052000UL,
    PAGE_83_ADDR = 0x00053000UL,
    PAGE_84_ADDR = 0x00054000UL,
    PAGE_85_ADDR = 0x00055000UL,
    PAGE_86_ADDR = 0x00056000UL,
    PAGE_87_ADDR = 0x00057000UL,
    PAGE_88_ADDR = 0x00058000UL,
    PAGE_89_ADDR = 0x00059000UL,
    PAGE_90_ADDR = 0x0005A000UL,
    PAGE_91_ADDR = 0x0005B000UL,
    PAGE_92_ADDR = 0x0005C000UL,
    PAGE_93_ADDR = 0x0005D000UL,
    PAGE_94_ADDR = 0x0005E000UL,
    PAGE_95_ADDR = 0x0005F000UL,
    PAGE_96_ADDR = 0x00060000UL,
    PAGE_97_ADDR = 0x00061000UL,
    PAGE_98_ADDR = 0x00062000UL,
    PAGE_99_ADDR = 0x00063000UL,
    PAGE_100_ADDR = 0x00064000UL,
    PAGE_101_ADDR = 0x00065000UL,
    PAGE_102_ADDR = 0x00066000UL,
    PAGE_103_ADDR = 0x00067000UL,
    PAGE_104_ADDR = 0x00068000UL,
    PAGE_105_ADDR = 0x00069000UL,
    PAGE_106_ADDR = 0x0006A000UL,
    PAGE_107_ADDR = 0x0006B000UL,
    PAGE_108_ADDR = 0x0006C000UL,
    PAGE_109_ADDR = 0x0006D000UL,
    PAGE_110_ADDR = 0x0006E000UL,
    PAGE_111_ADDR = 0x0006F000UL,
    PAGE_112_ADDR = 0x00070000UL,
    PAGE_113_ADDR = 0x00071000UL,
    PAGE_114_ADDR = 0x00072000UL,
    PAGE_115_ADDR = 0x00073000UL,
    PAGE_116_ADDR = 0x00074000UL,
    PAGE_117_ADDR = 0x00075000UL,
    PAGE_118_ADDR = 0x00076000UL,
    PAGE_119_ADDR = 0x00077000UL,
    PAGE_120_ADDR = 0x00078000UL,
    PAGE_121_ADDR = 0x00079000UL,
    PAGE_122_ADDR = 0x0007A000UL,
    PAGE_123_ADDR = 0x0007B000UL,
    PAGE_124_ADDR = 0x0007C000UL,
    PAGE_125_ADDR = 0x0007D000UL,
    PAGE_126_ADDR = 0x0007E000UL,
    PAGE_127_ADDR = 0x0007F000UL,
    
};

/***
 * @name flash_page_Erase
 * @param pageaddress
 * @brief used to erase the flash pages at once 
*/
void flash_page_erase(uint32_t addr);

///////////////////////////////////////////////////////////////////
////////////////// Flash write APis 


/*****
 * @name flash_write_byte
 * @param[1] adddress of the flash
 * @param[2] value to be written 
 * @brief use to write one byte in the flash
*/
void flash_write_byte(uint32_t addr, uint8_t value);
/****
 * @name flash_write_word
 * @param[1] address of the flash
 * @param[2] value of the word
 * @brief used for write only one owrd to the flash
*/
void flash_Write_word(uint32_t , uint32_t );
/****
 * @name flash_Write_bytes
 * @param[1] the page address 
 * @param[2] the byte array to be written in the address
 * @param[3] size 
 * @brief please not use this instead use the write word array function in writing the flash
*/
void flash_write_bytes(uint32_t , const uint8_t * , uint32_t );
/****
 * @name flash_write_word_arr
 * @param[1] the page address 
 * @param[2] the words array to be write 
 * @param[3] size of the word array
*/
void flash_write_word_arr(uint32_t , const uint32_t * , uint32_t );




/////////////////////////////////////////////////////////////////
/////////////////////       Flash Read APis 

/***
 * @name flash_read_word
 * @param flash address 
 * @brief used to read one word from the flash 
*/
uint32_t flash_read_word(uint32_t addr);

/***
 * @name flash_read_word_arr
 * @param the address of the flash 
 * @param buffer address 
 * @param size of the buffer 
 * @brief used to get the data stored in the flash and read it in the buffer 
*/
void  flash_read_word_arr(uint32_t addr, uint32_t *buff, uint32_t size);

///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///////////////////   UICR register 

enum
{
    UICR_TYPE_FIRMWARE,
    UICR_TYPE_HARDWARE,
    UICR_TYPE_CUSTOM
};

#define     FIRMWARE_MAX_INDEX_NO 15
#define     HARDWARE_MAX_INDEX_NO 12
#define     CUSTOM_MAX_INDEX_NO     32


/***
 * @name flash_write_uicr
 * @param register type
 * @param index number 
 * @return value stored at uicr 
 * @brief used to get the value stored at the uicr register  
*/
uint32_t flash_read_uicr(uint8_t type, uint8_t index_no);

/***
 * @name flash_write_uicr 
 * @param register type 
 * @param index number 
 * @param value put  at index 
 * @brief used to put the value at the uicr register with the type and index number 
*/
void flash_write_uicr(uint8_t type, uint8_t index_no, uint32_t value);

////////////////////////
//// before writing anything to the uicr you must have to erse the whole uicr and then 
///  write anything to it after that reset the whole device to take effect  

/**
 * @name flash_erase_uicr
 * @param void
 * @brief used to erase the uicr register 
*/
void flash_erase_uicr(void);


#endif