#ifndef SYS_CFG_H
#define SYS_CFG_H


#if defined(BUILD_APPLICATION)


///////// init the freertos enviournment for the system 
#define FREERTOS_ENV  1

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////      define the system init configurations
#define START_LSXO 1

// // enable the trace clock on pin 18 with 2 mhz clock
// #define  ENABLE_TRACE  1

/// @brief in this return type when assert will failed it return from the funcition with an err code 
#define ASSERT_TYPE_WITH_RETURN 0x02

/// @brief in this if assert will fail it just continue to next inst 
#define ASSERT_TYPE_WITHOUT_RETURN 0x03


// #define NRF_ASSERT_TYPE ASSERT_TYPE_WITH_RETURN 
#define NRF_ASSERT_TYPE ASSERT_TYPE_WITHOUT_RETURN


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#if (NRF_ASSERT_TYPE == ASSERT_TYPE_WITH_RETURN)
#define NRF_ASSERT_WITH_RETURN_ON_FAIL
#endif
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////// NRF TIMER BASED DELAY API
//// this is defined to use the nrf_delay_us(x) and nrf_delay_ms(x) macros
#define USE_TIMER_DELAY 1

///////// TImer 0 is used by radio interrrupts 
#define TIMER_TIME_MODULE TIMER_1

// #define TIMER_TICK_FREQ TIMER_FREQ_1MHz
#define TIMER_TICK_FREQ TIMER_FREQ_125kHz

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////       UART Library
#define UART_DRIVER_INSTALL 1

//// config defined in uart library 
//// pin 6 UL
/// pin 8UL
/// baud rate 115200

/// by the way pins 2---5 are ains pins and 6 --- 8 are used for external SPI flash 

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////         UART LOGGING LIB


//// define the nrf log APIs or you can use the uart_log_bytes in application . if you are not using it then
//// you can free 512 bytes of ram
#define USE_NRF_LOGS 1

#define UART_TX_DEFAULT_PIN 6UL 
#define UART_RX_DEFAULT_PIN 8UL

#define UART_LOG_BAUD_RATE UART_Baudrate_115200


#define NRF_LOG_MAX_ERRORS 1 
#define NRF_LOG_MAX_WARNINGS 2
#define NRF_LOG_MAX_INFO  3
#define NRF_LOG_MAX_DEBUG 4


#define DEBUG_LOG_MAX  NRF_LOG_MAX_INFO  

////================================================================================
////================================================================================
////=================================== I2c configurations 
#define I2C_DRIVER_INSTALL 1

#define I2C_HARDWARE_USE I2C0  

#define I2C_SDA_LINE PIN_25 

#define I2C_SCL_LINE PIN_26

#define I2C_CLK_FREQ I2C_FREQ_400KHZ

#define I2C_WAIT_FOR_DATA_RESPONSE 300
////================================================================================
////================================================================================
////=================================== SPI configurations 

#define SPI_DRIVER_INSTALL 1 

#define  SPI_DUMMY_TX_BYTE 0x00 

#define  SPI_MISO_LINE  PIN_13

#define  SPI_MOSI_LINE PIN_12 

#define  SPI_SCK_LINE  PIN_11

#define  SPI_MODE_USED  SPI_MODE_0

#define  SPI_BIT_ORDER  SPI_BIT_ORDER_MSB_FIRST

#define  SPI_SCK_FREQUENCY  SPI_SCK_500K

#define SPI_HARDWARE_USE  SPI2
 
#define SPI_DATA_RESPONSE_TIME 100


////================================================================================
////================================================================================
////======================================== OLED configurations 



////================================================================================
////================================================================================
////========================================  buzzer configurations 

#define NRF_BUZZER_PIN PIN_18

////================================================================================
////================================================================================
////======================================  notification led configurations 

#define NRF_NOTIFICATION_LED_PIN PIN_17 



////================================================================================
////================================================================================
////======================================  battery configuraitions 


////================================================================================
////================================================================================
////====================================== button configurations 

#define NRF_HOME_BUTTON_PIN PIN_16
#define NRF_MIDDLE_BUTTON_PIN  PIN_13 
#define NRF_UP_BUTTON_PIN PIN_14 
#define NRF_DOWN_BUTTON_PIN PIN_15

//////////// define the interrupt type 
#define NRF_BUTTON_INTERRUPT_POLARITY EVENT_RISING_EDGE

////// define the gpiote channel 
#define NRF_MIDDLE_BUTTON_GPIOTE_CHANNEL GPIOTE_CHANNEL_1
#define NRF_UP_BUTTON_GPIOTE_CHANNEL GPIOTE_CHANNEL_2
#define NRF_DOWN_BUTTON_GPIOTE_CHANNEL GPIOTE_CHANNEL_3
#define NRF_HOME_BUTTON_GPIOTE_CHANNEL GPIOTE_CHANNEL_4

#define NRF_BUTTON_EVENT_WAIT_TIME 2000 /// 2 secs

///// define the item size for the nrf button event q 
#define NRF_BUTTON_EVENTQ_ITEM_SIZE  (sizeof(uint8_t))

////// define the event q length for the buttons event 
#define NRF_BUTTON_EVENTQ_ITEM_LEN 2


////================================================================================
////================================================================================
////==================================== accelrometer adxl345 configurations 

#define NRF_ACCEL_EVENT_WAIT_TIME 1000 /// 1sec 

#define NRF_ACCEL_I2C_HOST_USED I2C_HARDWARE_USE
#define NRF_ADXL_INT1_PIN PIN_27 
// #define ADXL_INT2_PIN PIN_28

#define NRF_ACCEL_INT1_INTERRUPT_POLARITY EVENT_RISING_EDGE
#define NRF_ACCEL_INT1_GPIO_CHANNEL GPIOTE_CHANNEL_5

#define NRF_ACCEL_EVENT_Q_ITEM_SIZE  (sizeof(uint8_t))
#define NRF_ACCEL_EVENT_Q_LEN  5 /// let say we accomodate 3 accel event max 

////================================================================================
////================================================================================
////==================================== Fuel Gauge  configurations 

#define FUEL_GAUGE_WAIT_FOR_RESPONSE 300

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////       install PIn isr handler
#define INSTALL_GPIO_ISR_SERVICE 1


//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////         FLoating point unit 
#define FPU_ENABLE 1

#define USE_FPU_IRQ_HANDLER 1



///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////
#define ENABLE_HARDFAULT_EXCEPTION 1
#define ENABLE_MEM_MANAGE_EXCEPTION 1 
#define ENABLE_BUSFAULT_EXCEPTION  1
#define ENABLE_USAGE_FAULT_EXCEPTION 1

///// fault handlers are defined at a fixed priority 

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// deifne the priorities of the interrupts of the peripherals





#define GPIOTE_INTERRUPT_PRIORITY 6

#define TIMER_INTERRUPT_PRIORITY 6

#define ADC_INTERRUPT_PRIORITY 6

#define UART_INTERRUPT_PRIORITY 6

#define SPI_INTERRUPT_PRIORITY 6

#define I2C_INTERRUPT_PRIORITY 6

#define FPU_INTERRUPT_PRIORITY 6



////================================================================================
////================================================================================
 //////// flash operation timeout time 

 #define NVS_FLASH_OPERATION_TIMEOUT 500 // in millisec

////================================================================================
////================================================================================
////=================================================================================
////===============================================================================
////================================================================================
////================================================================================
////=================================================================================
////===============================================================================


#else // this is used to build the bootloader 


/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////      define the system init configurations


// // enable the trace clock on pin 18 with 2 mhz clock
// #define  ENABLE_TRACE  1


///////////////////////////////////////////////////////////////////////////////////////////
//////////////////// NRF TIMER BASED DELAY API
//// this is defined to use the nrf_delay_us(x) and nrf_delay_ms(x) macros
#define USE_TIMER_DELAY 1

// #define TIMER_TICK_FREQ TIMER_FREQ_1_95KHz
#define TIMER_TICK_FREQ TIMER_FREQ_1MHz


///////// TImer 0 is used by radio interrrupts 
#define TIMER_TIME_MODULE TIMER_1

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////       UART Library
#define UART_DRIVER_INSTALL 1

//// config defined in uart library 
//// pin 6 UL
/// pin 8UL
/// baud rate 115200

/// by the way pins 2---5 are ains pins and 6 --- 8 are used for external SPI flash 

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////         UART LOGGING LIB


//// define the nrf log APIs or you can use the uart_log_bytes in application . if you are not using it then
//// you can free 512 bytes of ram
#define USE_NRF_LOGS 1

#define UART_TX_DEFAULT_PIN 6UL 
#define UART_RX_DEFAULT_PIN 8UL

#define UART_LOG_BAUD_RATE UART_Baudrate_115200


//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////       install PIn isr handler
#define INSTALL_GPIO_ISR_SERVICE 1


//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////         FLoating point unit 
#define FPU_ENABLE 1

#define USE_FPU_IRQ_HANDLER 1




///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////
#define ENABLE_HARDFAULT_EXCEPTION 1
#define ENABLE_MEM_MANAGE_EXCEPTION 1 
#define ENABLE_BUSFAULT_EXCEPTION  1
#define ENABLE_USAGE_FAULT_EXCEPTION 1






#define GPIOTE_INTERRUPT_PRIORITY 6

#define TIMER_INTERRUPT_PRIORITY 6

#define ADC_INTERRUPT_PRIORITY 6

#define UART_INTERRUPT_PRIORITY 6

#define SPI_INTERRUPT_PRIORITY 6

#define I2C_INTERRUPT_PRIORITY 6

#define FPU_INTERRUPT_PRIORITY 6

#endif 

#endif