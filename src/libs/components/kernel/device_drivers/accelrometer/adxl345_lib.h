#ifndef _ADXL_H
#define _ADXL_H


#include "system.h"



// follows the NXP I2C guide put the CS to high to put the IC in I2C mode 
// the SDO can be use as ALT address pin in I2C mode when high I2c addr is 0x1D , LOw ->0x53


// #define weak_alias ( __attribute__ ((weak, alias ("default_handler"))) )

////////// sensor address  with the ALT addr pin HIGH 
// #define ADXL_SENS_ADDR 0x1D
// with the ALT addr pin LOW 
#define ADXL_SENS_ADDR 0x53


#define _1byte  1
#define _2byte  2
#define _3byte  3
#define _6byte  6

#define i2c_wait_time 20  ///100 msec 

#define DEVID             0x00
#define THRESH_TAP        0x1D
#define OFSX              0x1E
#define OFSY              0x1F
#define OFSZ              0x20
#define DUR               0x21
#define LATENT            0x22
#define WINDOW            0x23
#define THRESH_ACT        0x24
#define THRESH_INACT      0x25
#define TIME_INACT        0x26
#define ACT_INACT_CTL     0x27
#define THRESH_FF         0x28
#define TIME_FF           0x29
#define TAP_AXES          0x2A
#define ACT_TAP_STATUS    0x2B
#define BW_RATE           0x2C
#define POWER_CTL         0x2D
#define INT_ENABLE        0x2E
#define INT_MAP           0x2F
#define INT_SOURCE        0x30
#define DATA_FORMAT       0x31
#define DATAX0            0x32
#define DATAX1            0x33
#define DATAY0            0x34
#define DATAY1            0x35
#define DATAZ0            0x36
#define DATAZ1            0x37
#define FIFO_CTL          0x38
#define FIFO_STATUS       0x39



/////////////////////////////////////////////////////////////////////
//////////////////////////////////// structures //////////////////////////////

//// the data rate of the accelrometer and based upon that current is given 
enum
{
    _010 = 0x00,  ///23ua
    _020,         //23
    _039,          //23
    _078,           //23
    _15_6,          //34
    _3_13,         //40
    _6_25,         //45
    _12_5,       //50
    _25,        // 60
    _50,        //90
    _100,       //140
    _200,       //140
    _400,       //140
    _800,       //140
    _1600,      //90
    _3200       //140

};

//// data rate vs current in low power mode // it have higher noise 
enum
{
    _l400 = 0x0c,
    _l200 = 0x0b,
    _l100 = 0x0a,
    _l50 = 0x09,
    _l25 = 0x08,
    _l12_5 = 0x07
};

// ///// output range of the accel
enum
{
    _2g = 0x00,
    _4g,
    _8g,
    _16g
};
////////////////// config for the adxl
typedef struct __packed _CONFIG_ 
{
    uint8_t output_range  :2;
    uint8_t low_pwr  :1; /////only haave one bit
    uint8_t rate     :4; ///// 4bits for rate 
    uint8_t link_autosleep    :1; /// 1bit 

}adxl_cfg;

////////// pointer of the adxl config typen
typedef adxl_cfg * adxl_config;


uint8_t adxl_init(void);

void adxl_deinit(void);

void adxl_cfg_(adxl_config);


#define singletap  0x01
#define double_tap 0x03

#define x_axis 0x01
#define y_axis 0x02
#define z_axis 0x04

typedef struct __packed
{
    uint8_t tap_type;  // could be single or double tap
    uint8_t tap_axes; // could be x,y,z
    uint8_t tap_thresh; // 62.5 mg/LSB
    uint8_t tap_durat; // units 625us/LSB
    uint8_t double_tap_lattency; // 1.25ms/LSB
    uint8_t double_tap_window;  // 1.25ms/LSB

}taps_configurations;

typedef  taps_configurations * taps_cfg;

void adxl_cfg_taps(taps_cfg);

#define act_x_en 0x40
#define act_y_en 0x20
#define act_z_en 0x10
#define dc_oper 0
#define ac_oper 1


typedef struct __packed 
{
    uint8_t thresh_act;  /// 62.5mg/LSB
    uint8_t axes; // could be x, y, z,axes
    uint8_t _ac_dc; /// ac or dc coupled operation 
}activity;


#define inact_x_en 0x04
#define inact_y_en 0x02
#define inact_z_en 0x01

typedef struct __packed 
{
    uint8_t thresh_inact; ///// 62.5mg/LSB
    uint8_t time_inact; //// 1 sec/ LSB 
    uint8_t axes; // axes of interest x,y,z
    uint8_t _ac_dc; /// ac or dc coupled operations 
}inactivity;

typedef struct __packed 
{
    activity act; // for act configurations 
    inactivity inact; // for inactivity configurations 

}activity_inact_config;

typedef activity_inact_config * act_inact_cfg;

void adxl_cfg_act_inact(act_inact_cfg);

/// @brief this is used to configure the frefall 
/// @param  accleration value 62.5mg/LSB 300 to 600mg recommend
/// @param  time value 5ms/LSB 100 ms and 350 ms recommend 
void adxl_cfg_freefall(uint8_t , uint8_t );

enum
{
 _bypass, 
 _fifo,
 _stream,
 _trigger
};
/// @brief configure the fifo
/// @param  fifo_mode 
/// @param  samples //  the max number of samples is 32
void adxl_cfg_fifo(uint8_t , uint8_t );


enum 
{
    _overrun = 0x00, // overrun interrupt set 
    _watermark, //  watermark interrupt set 
    _freefall, // freefall detection max time 1.25 sec
    _inactivity, // inactivity 
    _activity, // activity 
    _double_tap, // double tap
    _single_tap,// single tap innterrupt 
    _data_ready  //  this interrupt fires when data is ready according to the data rate 
};

#define _enable 1

#define _disable 0

void cfg_int(uint8_t , uint8_t);

// disable all the interrupts and then enable according to your use case 
void disable_all_ints(void);

uint8_t read_int_type(void);

bool adxl_read_data(uint8_t *, uint16_t size);

/***
 * @name read_acccelration 
 * @param pointer to a buffer 
 * @param size of the buffer
 * @return if reading is succesfull or not 
 * @note it will automatically calculate the accelration in terms of g 
 * 
*/
bool read_accelration(float * );

//////////// define the gain of the acclerometer 
#define gain 3.874f /// mg/LSB

#endif