#include "flash_write.h"

// this will hang the processor while waiting for the nvmc to be ready
static inline void wait_for_flash_ready(void)
{
    while (NRF_NVMC->READY == NVMC_IS_BUSY)
    {
        ;
    }
}

void flash_page_erase(uint32_t addr)
{
    // Enable erase.
    NRF_NVMC->CONFIG = NVMC_ERASE_ENABLE;
    __ISB(); // instruction synchronixation barrier
    __DSB(); // data synchronization barrier

    // Erase the page page address must be correct and is the starting addr of the page
    NRF_NVMC->ERASEPAGE = addr;
    wait_for_flash_ready();

    NRF_NVMC->CONFIG = NVMC_READ_ENABLE;
    __ISB();
    __DSB();
}

void flash_write_byte(uint32_t addr, uint8_t value)
{
    uint32_t byte_shift = addr & (uint32_t)0x03;
    uint32_t address32 = addr & ~byte_shift; // Address to the word this byte is in.
    uint32_t value32 = (*(uint32_t *)address32 & ~((uint32_t)0xFF << (byte_shift << (uint32_t)3)));
    value32 = value32 + ((uint32_t)value << (byte_shift << 3));

    // Enable write.
    NRF_NVMC->CONFIG = NVMC_WRITE_ENABLE;
    __ISB();
    __DSB();

    *(uint32_t *)address32 = value32;
    wait_for_flash_ready(); // wait for flash to be ready

    NRF_NVMC->CONFIG = NVMC_READ_ENABLE; // make the read enable again
    __ISB();
    __DSB();
}

void flash_Write_word(uint32_t addr, uint32_t value)
{
    // Enable write.
    NRF_NVMC->CONFIG = NVMC_WRITE_ENABLE;
    __ISB();
    __DSB();

    *(uint32_t *)addr = value;
    wait_for_flash_ready(); // wait for flash , it must be doing the erasing and writing operations

    NRF_NVMC->CONFIG = NVMC_READ_ENABLE;
    __ISB();
    __DSB();
}

void flash_write_bytes(uint32_t address, const uint8_t *src, uint32_t num_bytes)
{
    uint32_t i;
    for (i = 0; i < num_bytes; i++)
    {
        flash_write_byte(address + i, src[i]);
    }
}

void flash_write_word_arr(uint32_t address, const uint32_t *src, uint32_t num_words)
{
    uint32_t i;

    // Enable write.
    NRF_NVMC->CONFIG = NVMC_WRITE_ENABLE;
    __ISB();
    __DSB();

    for (i = 0; i < num_words; i++)
    {
        ((uint32_t *)address)[i] = src[i];
        wait_for_flash_ready();
    }

    NRF_NVMC->CONFIG = NVMC_READ_ENABLE;
    __ISB();
    __DSB();
}

////////////////////////////////////////////////////////////////////////////////////
////////////// for writing anything to flash you must follow the sequnce .
/////// first calculate the total size to be written in the flash and then devide it by
/////// 4KB we get the how many pages we have to use after that starting from the page .
/// we have to use

///'/ Flash read APIs are pending

/// @brief used to get one word from the flash
/// @param addr
/// @return
uint32_t flash_read_word(uint32_t addr)
{
    uint32_t data = 0;
    NRF_NVMC->CONFIG = NVMC_READ_ENABLE;
    __ISB();
    __DSB();
    data = *((uint32_t *)addr);
    __ISB();
    __DSB();

    return data;
}

void flash_read_word_arr(uint32_t addr, uint32_t *buff, uint32_t size)
{

    NRF_NVMC->CONFIG = NVMC_READ_ENABLE;
    __ISB();
    __DSB();
    for (uint32_t i = 0; i < size; i++)
    {
        buff[i] = ((uint32_t *)addr)[i];
    }
    __ISB();
    __DSB();
}

////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////        Flash APIs to read and write UICR register

void flash_write_uicr(uint8_t type, uint8_t index_no, uint32_t value)
{
    uint32_t *preg = NULL;

    /// get the register pointer and then assigned it to preg
    switch (type)
    {
    case UICR_TYPE_FIRMWARE:
        if (index_no > FIRMWARE_MAX_INDEX_NO)
        {
            return;
        }
        preg = (uint32_t *)&NRF_UICR->NRFFW[index_no];
        break;

    case UICR_TYPE_HARDWARE:
        if (index_no > HARDWARE_MAX_INDEX_NO)
        {
            return;
        }
        preg = (uint32_t *)&NRF_UICR->NRFHW[index_no];
        break;

    case UICR_TYPE_CUSTOM:
        if (index_no > CUSTOM_MAX_INDEX_NO)
        {
            return;
        }
        preg = (uint32_t *)&NRF_UICR->CUSTOMER[index_no];
        break;
    }

    // Enable write.
    NRF_NVMC->CONFIG = NVMC_WRITE_ENABLE;
    __ISB();
    __DSB();

    *(uint32_t *)preg = value;
    wait_for_flash_ready(); // wait for flash , it must be doing the erasing and writing operations

    NRF_NVMC->CONFIG = NVMC_READ_ENABLE;
    __ISB();
    __DSB();

    /// apply system reset
    system_soft_reset();
}

uint32_t flash_read_uicr(uint8_t type, uint8_t index_no)
{
    uint32_t val = 0;

    /// get the register pointer and then assigned it to preg
    switch (type)
    {
    case UICR_TYPE_FIRMWARE:
        if (index_no > FIRMWARE_MAX_INDEX_NO)
        {
            return 0;
        }
        val = NRF_UICR->NRFFW[index_no];
        break;

    case UICR_TYPE_HARDWARE:
        if (index_no > HARDWARE_MAX_INDEX_NO)
        {
            return 0;
        }
        val = NRF_UICR->NRFHW[index_no];
        break;

    case UICR_TYPE_CUSTOM:
        if (index_no > CUSTOM_MAX_INDEX_NO)
        {
            return 0;
        }
        val = NRF_UICR->CUSTOMER[index_no];
        break;
    }

    // Enable write.
    NRF_NVMC->CONFIG = NVMC_READ_ENABLE;

    wait_for_flash_ready(); // wait for flash , it must be doing the erasing and writing operations

    __ISB();
    __DSB();

    return val;
}

void flash_erase_uicr(void)
{
    // Enable erase.
    NRF_NVMC->CONFIG = NVMC_ERASE_ENABLE;
    __ISB(); // instruction synchronixation barrier
    __DSB(); // data synchronization barrier

    // Erase the page page address must be correct and is the starting addr of the page
    NRF_NVMC->ERASEUICR = 1UL;
    wait_for_flash_ready();

    NRF_NVMC->CONFIG = NVMC_READ_ENABLE;
    __ISB();
    __DSB();
}