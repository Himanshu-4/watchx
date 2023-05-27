/* Linker script to configure memory regions. */


 /* SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x0, LENGTH = 0x80000
  RAM (rwx) :  ORIGIN = 0x20000000, LENGTH = 0x10000 
} 


SECTIONS
{
} */


SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

/* the ram only avialbale is 42kb
the flash only available is 360kb   */
MEMORY
{
  FLASH (rx) : ORIGIN = 0x26000, LENGTH = 0x5A000
  RAM (rwx) :  ORIGIN = 0x20005968, LENGTH = 0xa698
  /* RAM (rwx) :  ORIGIN = 0x200022f0, LENGTH = 0xdd10 */
}


SECTIONS
{
}

INCLUDE "nrf_common.ld"