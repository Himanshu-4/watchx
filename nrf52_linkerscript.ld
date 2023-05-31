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
  /* RAM (rwx) :  ORIGIN = 0x20005968, LENGTH = 0xa698
  we have 52kb ram available for us the sd only require 10kb  */
  RAM (rwx) :  ORIGIN = 0x20003000, LENGTH = 0xd000
}


SECTIONS
{
}

INCLUDE "nrf_common.ld"