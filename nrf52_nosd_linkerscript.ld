
/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

/* //// this linkerscript will not be using softdevice so has to be used without softdevice  */
/* MEMORY
{
  FLASH (rx) : ORIGIN = 0x0, LENGTH = 0x80000
  RAM (rwx) :  ORIGIN = 0x20000000, LENGTH = 0x10000 

} */

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000, LENGTH = 0x78000
  RAM (rwx) :  ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS
{
  .noinit(NOLOAD):
  {
    PROVIDE(__start_noinit_data = .);
    *(.noinit)
    PROVIDE(__stop_noinit_data = .);
  } > RAM
} INSERT AFTER .bss


SECTIONS
{
   . = ALIGN(4);
  .mem_section_dummy_ram :
  {
  }
 
  /* describing a section to place into ram and not to load it from flash  */
   .kernel_memory_types(NOLOAD) :
  {
    /* processor can read write faster at 4 byte align address  */
    /* here kernel memory allocater region */
    . = ALIGN(4);
    PROVIDE(__start_kernel_mem_allocated_data = .);
    KEEP(*(SORT(.kernel_mem_data*)))
    PROVIDE(__end_kernel_mem_allocated_data = .);

  /* kernel linklist alloccated region */
    . = ALIGN(4);
    PROVIDE(__start_kernel_ll_allocated_data = .);
    KEEP(*(SORT(.kernel_linklist_data*)))
    PROVIDE(__end_kernel_ll_allocated_data = .);
    . = ALIGN(4);

  /* kernel Queue allocated region */
  . = ALIGN(4);
  PROVIDE(__start_kernel_queue_allocated_data = .);
  KEEP(*(SORT(.kernel_queue_data*)))
  PROVIDE(__end_kernel_queue_allocated_data = .);
  . = ALIGN(4);


  /* we can use the location pointer for tracking the  */
  } > RAM

  .fifo_uart_buffers(NOLOAD) :
  {
        /* fifo memeory allocated for uart buffers  */
    . = ALIGN(4);
    PROVIDE(__start_fifo_allocated_data = .);
    KEEP(*(SORT(.fifo_buffer*)))
    PROVIDE(__end_fifo_allocated_data = .);
    . = ALIGN(4);
    
  /* we can use the location pointer for tracking the  */
  } > RAM

} INSERT AFTER .data;


SECTIONS
{
  .mem_section_dummy_rom :
  {
  }
} INSERT AFTER .text


INCLUDE "nrf_common.ld"


