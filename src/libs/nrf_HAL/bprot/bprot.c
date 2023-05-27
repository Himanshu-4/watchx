#include "bprot.h"

void protect_flash_block(uint8_t block_no )
{   
    
    if(block_no >=128)
    return;

   if (block_no >= 96)
    {
        // use config 3 register 
        block_no -= 96;
        NRF_BPROT->CONFIG3 = _BV(block_no);
    }
    else if(block_no >= 64)
    {
        // use config 2 register 
        block_no -= 64;
        NRF_BPROT->CONFIG2 = _BV(block_no);
    }
    else if(block_no >= 32)
    {
        // use config 1 register 
        block_no -= 32;
        NRF_BPROT->CONFIG1 = _BV(block_no);
    }
    else
    { 
        // use config 0 register 
        NRF_BPROT->CONFIG0 = _BV(block_no);
    }

}

void reset_flash_protect(void)
{
    system_soft_reset();
}