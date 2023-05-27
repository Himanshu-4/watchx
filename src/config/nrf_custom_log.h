#ifndef _NRF_LOG_H
#define _NRF_LOG_H

#include "ansii_color_codes.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"



#if DEBUG_LOG

#define NRF_LOG_ERROR(str, ...)    printf(BRED str  "\r\n"   ANSI_RESET, ##__VA_ARGS__)
#define NRF_LOG_WARNING(str,...)   printf(BYEL str  "\r\n"  ANSI_RESET, ##__VA_ARGS__)
#define NRF_LOG_DEBUG(str, ...)    printf(BCYN str  "\r\n"  ANSI_RESET, ##__VA_ARGS__)
#define NRF_LOG_INFO(str, ...)     printf(BGRN str  "\r\n"  ANSI_RESET,  ##__VA_ARGS__)

#else 

#define NRF_LOG_ERROR(str, ...)  // printf(BRED str  "\r\n"   ANSI_RESET, ##__VA_ARGS__)
#define NRF_LOG_WARNING(str,...)  //printf(BYEL str  "\r\n"  ANSI_RESET, ##__VA_ARGS__)
#define NRF_LOG_DEBUG(str, ...)   //printf(BCYN str  "\r\n"  ANSI_RESET, ##__VA_ARGS__)
#define NRF_LOG_INFO(str, ...)   // printf(BGRN str  "\r\n"  ANSI_RESET,  ##__VA_ARGS__)

#endif


#endif