#ifndef _TIMER_H
#define _TIMER_H

#include "system.h"

// we are not using the TIMEr 0 IQR handler as it is used by radio drivers 
extern void TIMER1_IRQHandler(void);
extern void TIMER2_IRQHandler(void);
extern void TIMER3_IRQHandler(void);
extern void TIMER4_IRQHandler(void);

// #define TIMER_0 0 never use timer 0 as it is used by radio module 
#define TIMER_1 1 // only have 4 cc register
#define TIMER_2 2 // only have 4 cc register 
#define TIMER_3 3 // only have 6 cc register
#define TIMER_4 4 // only have 6 cc register

/////////////////////// used to get the timer functions through array of callbacks 
#define TIMER_1_FUNC 0
#define TIMER_2_FUNC 1
#define TIMER_3_FUNC 2
#define TIMER_4_FUNC 3

///// sed to disable all the timer at once 
#define TIMER_ALL 8

enum
{
    TIMER_MODE =0,
    LOW_POWER_COUNTER_MODE =2,
};
enum
{
    TIMER_16_BIT_WIDTH,
    TIMER_8_BIT_WIDTH,
    TIMER_24_BIT_WIDTH,
    TIMER_32_BIT_WIDTH
};

/** @brief Timer prescalers. */
enum
{
    TIMER_FREQ_16MHz = 0, ///< Timer frequency 16 MHz.
    TIMER_FREQ_8MHz,      ///< Timer frequency 8 MHz.
    TIMER_FREQ_4MHz,      ///< Timer frequency 4 MHz.
    TIMER_FREQ_2MHz,      ///< Timer frequency 2 MHz.
    TIMER_FREQ_1MHz,      ///< Timer frequency 1 MHz.
    TIMER_FREQ_500kHz,    ///< Timer frequency 500 kHz.
    TIMER_FREQ_250kHz,    ///< Timer frequency 250 kHz.
    TIMER_FREQ_125kHz,    ///< Timer frequency 125 kHz.
    TIMER_FREQ_62_5KHz,   ///< Timer frequency 62.5 KHz.
    TIMER_FREQ_31_25KHz,  ///< Timer frequency 31.25 KHz.
    TIMER_FREQ_15_63KHz,  /// < timer frequency 15.63 KHZ
    TIMER_FREQ_7_81KHz,   //// timer frequency 7.81khz
    TIMER_FREQ_3_9KHZ,    //// timer_Frequency 3.9khz
    TIMER_FREQ_1_95KHz,   /// timer frequency 1.95khz
    TIMER_FREQ_976HZ,    //// 976 hz 
    TIMER_FREQ_488Hz    //// 488 hz 
};


typedef struct __PACKED 
{
    uint8_t mode;
    uint8_t timer_tick_rate; // timer tick rate  
    uint8_t timer_width;
}my_timer_cfg;


/*****
 * @name timer_config
 * @param[1] timer_number
 * @param[2] timer_config
 * @brief used to configre the timer 
 * 
*/
void timer_config(uint8_t, const my_timer_cfg  *);
/******
 * @name timer_module_init
 * @param timer number 
 * @brief must called it once before using any timer function 
*/
void timer_module_init(uint8_t);

/******
 * @name timer_module_unint
 * @param timer number 
 * @brief  disable the timer module 
 * 
*/
void timer_module_deint(uint8_t);


/****
 * @name  timer_get_counter_val
 * @param timer_number
 * @return counter_value
 * 
*/
uint32_t timer_get_counter_val(uint8_t );

/***
 * @name timer_start
 * @param timer number
 * @brief to start a timer / counter
*/
void timer_start(uint8_t);

/*****
 * @name timer_stop
 * @param timer number
 * @brief to stop a timer / counter 
*/
void timer_stop(uint8_t );

/****
 * @name timer_clear
 * @param timer number
 * @brief to clear a timer /counter value 
*/
void timer_clear(uint8_t);



enum
{
    // CAPTURE_COMPARE_0,  /// only use this in getting the counter value or getting the timer value 
    CAPTURE_COMPARE_1=1,
    CAPTURE_COMPARE_2,
    CAPTURE_COMPARE_3,
    CAPTURE_COMPARE_4,
    CAPTURE_COMPARE_5,
    CAPTURE_COMPARE_NONE =9
};

/****
 * @name timer_add_interupt_time
 * @param[1] timer number
 * @param[2] the compare index 
 * @param[3] the counter value at which the 
 * @brief timer interrupt counter value at what counter value the irq fires
 * user should calculate the time and set the counter value accordingly 
 *
*/
void timer_add_interupt_time(uint8_t ,uint8_t ,  uint32_t);


/*****
 * @name timer_add_irq_handler
 * @param[1] timer number 
 * @param[2] the callback function
 * @brief used to add a timer callback 
*/
void timer_add_irq_handler(uint8_t , void (*)(void));

/*****
 * @name timer_remove_irq_handler
 * @param[1] timer number 
 * @brief used to remove a timer callback it disable the IRQ handler  
*/
void timer_remove_irq_handler(uint8_t );

#define TIMER_INT_BIT_Msk 16UL
/*****
 * @name timer_interrupt_enable
 * @param[1] timer interupt number 
 * @param[2] compare index 
 * @brief enable the interrupt for the particular compare index 
*/
void timer_interrupt_enable(uint8_t, uint8_t );


/*****
 * @name timer_interrupt_disable
 * @param[1] timer interupt number 
 * @param[2] compare index 
 * @brief disable the interrupt for the particular compare index 
*/
void timer_interrupt_disable(uint8_t, uint8_t);

/****
 * @name timer_get_int_src
 * @param timer number 
 * @return the source or the compare register index 
 * @brief used in the irq handler to get the which compare register
 *  is resposible for the event and also clear that event 
*/
uint8_t timer_get_int_src(uint8_t);

/***
 * @name timer_set_auto_reload_val
 * @param[1] timernumber 
 * @param[2] capture_compare ID
 * @param[3] reload value 
 * @brief this will set the shortcut bits and tie the
 *  compare register to reload value and tie the event compare and clear register so when counter reaches the 
 * compare value it automatically reset to zero 
*/
void timer_set_auto_reload_val(uint8_t , uint8_t , uint32_t);

#define TIMER_SHORT_STOP_BIT_Msk 8UL
/****
 * @name timer_set_stop_Timer_time
 * @param[1] timer number 
 * @param[2] the compare index
 * @param[3] the counter value 
 * @brief when the timer counter reaches the counter value then it automatically stop the timer 
*/
void timer_set_stop_timer_time(uint8_t , uint8_t, uint32_t);


/*****
 * @name timer_us_to_counter
 * @param[1] timer_number
 * @param[2] time in microseconds 
 * @return gives the counter value based on the timer parameters 
*/
uint32_t timer_us_to_counter(uint8_t , uint32_t );


/*****
 * @name timer_us_to_counter
 * @param[1] timer_number
 * @param[2] time in milliseconds 
 * @return gives the counter value based on the timer parameters 
*/
uint32_t timer_ms_to_counter(uint8_t , uint32_t );

///////// used in PPI module 
////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
/***
 * @name timer_get_task_capture_addr
 * @param[1] timer_number
 * @param[2] the capture_compare index 
 * @return the address of the task register 
 * @brief used to handle it in PPI event 
*/
uint32_t * timer_get_task_capture_addr(uint8_t, uint8_t);

/***
 * @name timer_get_start_addr
 * @param[1] timer_number
 * @return the address of timer start  register 
 * @brief used to handle it in PPI event 
*/
uint32_t * timer_get_start_addr(uint8_t);

/***
 * @name timer_get_clear_addr
 * @param[1] timer_number
 * @return the address of timer clear register 
 * @brief used to handle it in PPI event 
*/
uint32_t * timer_get_clear_addr(uint8_t);

/***
 * @name timer_get_stop_addr
 * @param[1] timer_number
 * @return the address of timer stop register 
 * @brief used to handle it in PPI event 
*/
uint32_t * timer_get_stop_addr(uint8_t);

/*****
 * @name timer_get_event_addr
 * @param[1] timer_number
 * @param[2] the capture_compare index 
 * @return the adddress of the event register 
 * @brief used to handle the PPI task
 * 
*/
uint32_t * timer_get_event_addr(uint8_t, uint8_t );

/*****
 * @name timer_get_counter_addr
 * @param[1] timer_number 
 * @return the adddress of the counter register 
 * @brief used to handle the PPI task
 * 
*/
uint32_t * timer_get_counter_addr(uint8_t);

////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////




#endif 