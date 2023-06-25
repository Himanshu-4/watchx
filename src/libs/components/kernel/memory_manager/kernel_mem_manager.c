#include "kernel_mem_manager.h"

//// deifne an array of big amount 

static int __used KERNEL_MEM_SECTION arr[200] = {0};

void printfun(void)
{
    printf("%d", arr[2]);
}