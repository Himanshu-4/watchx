#ifndef NRF_TIME_H
#define NRF_TIME_H


#include "timer.h"

/****
 * @name time_init
 * @brief used to initialize timer 1 and used its cc1 register for getting the time 
*/
void time_init(void);

/****
 * @name time_deinit
 * @brief used to unintailize timer 1  
*/
void time_deinit(void);

/***
 * @name millis
 * @brief used to get the millisseconds from the start of the timer 
*/
uint32_t millis(void);

/**
 * @name micros 
 * @brief used to get the microseconds from the start of the timer 
*/
// uint32_t micros(void);

/***
 * @name seconds 
 * @brief used to get the seconds from the start of the timer 
*/
uint32_t seconds(void);

/***
 * @name minute 
 * @brief used to get minute from the start of the timer 
*/
uint32_t minute(void);


/**
 * @name delay_milisec
 * @brief used to delay the processor 
*/
void  delay_milisec(uint16_t);

/***
 * @name delay_microsecond
 * @brief used to delay the processor
*/
// void delay_microsec(uint16_t);

#endif