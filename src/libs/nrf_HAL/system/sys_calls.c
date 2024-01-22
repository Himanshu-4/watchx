/**
 ******************************************************************************
 * @file      syscalls.c
 * @author    Auto-generated by STM32CubeIDE
 * @brief     STM32CubeIDE Minimal System calls file
 *
 *            For more information about which c-functions
 *            need which of these lowlevel functions
 *            please consult the Newlib libc-manual
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Includes */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <string.h>

////// include the system.h file 
#include "system.h"

/////m to color code the output errors 
#include "ansii_color_codes.h"


/// @brief log the bytes to the UART ring buffer and make to ready it for transmission
/// @param buff
/// @param size
/// @param override
/// @return succ/err code
extern uint32_t logger_transmit_bytes(const uint8_t* pbuff, uint16_t size);


// /* Variables */
// extern int __io_putchar(int ch) __attribute__((weak));
// extern int __io_getchar(void) __attribute__((weak));


// char *__env[1] = { 0 };
// char **environ = __env;


// /* Functions */
// void initialise_monitor_handles()
// {
// }

int _getpid(void)
{
	return 1;
}

int _kill(int pid, int sig)
{
	// errno = EINVAL;
	return -1;
}

// void _exit (int status)
// {
// 	_kill(status, -1);
// 	while (1) {}		/* Make sure we hang here */
// }

 int _read(int file, char *ptr, int len)
{
	

return len;
}

 int _write(int file, char *ptr, int len)
{
	
    logger_transmit_bytes((uint8_t *)ptr , len);
	return len;
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/////////////// here are some low level system calls 


int _close(int file)
{
	return -1;
}

//  error: 'struct stat' declared inside parameter list will not be visible outside of this definition or declaration [-Werror]
//   103 | int _fstat(int file, struct stat *st
// this is needed to resolve above error

struct stat *st;

int _fstat(int file, struct stat *st)
{
	// st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	return 1;
}

int _lseek(int file, int ptr, int dir)
{
	return 0;
}

// int _open(char *path, int flags, ...)
// {
// 	/* Pretend like we always fail */
// 	return -1;
// }

// int _wait(int *status)
// {
// 	errno = ECHILD;
// 	return -1;
// }

// int _unlink(char *name)
// {
// 	errno = ENOENT;
// 	return -1;
// }

// int _times(struct tms *buf)
// {
// 	return -1;
// }

// int _stat(char *file, struct stat *st)
// {
// 	st->st_mode = S_IFCHR;
// 	return 0;
// }

// int _link(char *old, char *new)
// {
// 	errno = EMLINK;
// 	return -1;
// }

// int _fork(void)
// {
// 	errno = EAGAIN;
// 	return -1;
// }

// int _execve(char *name, char **argv, char **env)
// {
// 	errno = ENOMEM;
// 	return -1;
// }




/// enable the defualt handlers 
void enable_fault_handlers(void)
{
#if defined(ENABLE_HARDFAULT_EXCEPTION)
NVIC_EnableIRQ(HardFault_IRQn);
#endif

#if defined(ENABLE_MEM_MANAGE_EXCEPTION)
NVIC_EnableIRQ(MemoryManagement_IRQn);
#endif

#if defined(ENABLE_BUSFAULT_EXCEPTION )
NVIC_EnableIRQ(BusFault_IRQn);
#endif

#if defined(ENABLE_USAGE_FAULT_EXCEPTION)
NVIC_EnableIRQ(UsageFault_IRQn);
#endif
	
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//////                 define the fault handlers 

static void default_handler(void)
{
	system_soft_reset();
}

void HardFault_Handler(void)
{
default_handler();
}
void MemoryManagement_Handler(void)
{
default_handler();
}

void BusFault_Handler(void)
{
default_handler();
}

void UsageFault_Handler(void)
{
default_handler();
}



// void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
// {
// 	printf(BRED"errcode  %ld,line %ld, filename %s\r\n",error_code,line_num,p_file_name);
// 	while(1)
// 	{
// 		printf("reseting after 2 sec\r\n");
// 		nrf_delay_ms(2000);
// 	}
// 	// system_soft_reset();
// 	while(1);
// }





void softdevice_err_handler(uint32_t id, uint32_t pc, uint32_t info)
{

	printf(BRED"apperror->id %ld,pc %ld, info %ld\r\n",id,pc,info);
	nrf_delay_ms(2000);
	system_soft_reset();
	
	while(1);


}
