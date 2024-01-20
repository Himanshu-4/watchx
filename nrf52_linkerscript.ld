/* Linker script to configure memory regions. */




SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

/* the flash only available is 360kb   */
MEMORY
{
  FLASH (rx) : ORIGIN = 0x26000, LENGTH = 0x5A000
  /* RAM (rwx) :  ORIGIN = 0x20005968, LENGTH = 0xa698
  we have 52kb ram available for us the sd only require 12kb  */
  RAM (rwx) :  ORIGIN = 0x20003000, LENGTH = 0xd000
}


SECTIONS
{
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

INCLUDE "nrf_common.ld"