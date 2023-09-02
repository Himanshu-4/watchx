#ifndef _ADXL_H
#define _ADXL_H

#include "system.h"

// follows the NXP I2C guide put the CS to high to put the IC in I2C mode
// the SDO can be use as ALT address pin in I2C mode when high I2c addr is 0x1D , LOw ->0x53

#define I2C_HOST_USED NRF_ACCEL_I2C_HOST_USED
// #define weak_alias ( __attribute__ ((weak, alias ("default_handler"))) )

////////// sensor address  with the ALT addr pin HIGH
// #define ADXL_SENS_ADDR 0x1D
// with the ALT addr pin LOW
#define ADXL_SENS_ADDR 0x53

#define ADXL_REG_DEVID 0x00
#define ADXL_REG_THRESH_TAP 0x1D
#define ADXL_REG_OFSX 0x1E
#define ADXL_REG_OFSY 0x1F
#define ADXL_REG_OFSZ 0x20
#define ADXL_REG_DUR 0x21
#define ADXL_REG_LATENT 0x22
#define ADXL_REG_WINDOW 0x23
#define ADXL_REG_THRESH_ACT 0x24
#define ADXL_REG_THRESH_INACT 0x25
#define ADXL_REG_TIME_INACT 0x26
#define ADXL_REG_ACT_INACT_CTL 0x27
#define ADXL_REG_THRESH_FF 0x28
#define ADXL_REG_TIME_FF 0x29
#define ADXL_REG_TAP_AXES 0x2A
#define ADXL_REG_ACT_TAP_STATUS 0x2B
#define ADXL_REG_BW_RATE 0x2C
#define ADXL_REG_POWER_CTL 0x2D
#define ADXL_REG_INT_ENABLE 0x2E
#define ADXL_REG_INT_MAP 0x2F
#define ADXL_REG_INT_SOURCE 0x30
#define ADXL_REG_DATA_FORMAT 0x31
#define ADXL_REG_DATAX0 0x32
#define ADXL_REG_DATAX1 0x33
#define ADXL_REG_DATAY0 0x34
#define ADXL_REG_DATAY1 0x35
#define ADXL_REG_DATAZ0 0x36
#define ADXL_REG_DATAZ1 0x37
#define ADXL_REG_FIFO_CTL 0x38
#define ADXL_REG_FIFO_STATUS 0x39

#define ADXL_DEVID 0xE5
/////////////////////////////////////////////////////////////////////
//////////////////////////////////// structures //////////////////////////////

//// the data rate of the accelrometer and based upon that current is given
enum
{
    adxl_datarate_010 = 0x00, /// 23ua
    adxl_datarate_020,        // 23
    adxl_datarate_039,        // 23
    adxl_datarate_078,        // 23
    adxl_datarate_15_6,       // 34
    adxl_datarate_3_13,       // 40
    adxl_datarate_6_25,       // 45
    adxl_datarate_12_5,       // 50
    adxl_datarate_25,         // 60
    adxl_datarate_50,         // 90
    adxl_datarate_100,        // 140
    adxl_datarate_200,        // 140
    adxl_datarate_400,        // 140
    adxl_datarate_800,        // 140
    adxl_datarate_1600,       // 90
    adxl_datarate_3200        // 140

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
enum _ACCEL_OUTPUT_RANGE_
{
    adxl_range_2g = 0x00,
    adxl_range_4g,
    adxl_range_8g,
    adxl_range_16g
};
////////////////// config for the adxl
typedef struct __packed _CONFIG_
{
    uint8_t output_range : 2;
    uint8_t low_pwr : 1;        /////only haave one bit
    uint8_t rate : 4;           ///// 4bits for rate
    uint8_t link_autosleep : 1; /// 1bit  this will always 1

} adxl_cfg;

////////// pointer of the adxl config typen
typedef const adxl_cfg *adxl_config;

/// @brief this is to configure the accelormeter based on the paramter
/// @param adxl_configure *
/// @return succ/failure
uint32_t adxl_configure(adxl_config cfg);

#define tap_en_singletap 0x01
#define tap_en_double_tap 0x03

#define tap_axis_en_x 0x01
#define tap_axis_en_y 0x02
#define tap_axis_en_z 0x04

typedef struct __packed
{
    uint8_t tap_type;            // could be single or double tap
    uint8_t tap_axes;            // could be x,y,z
    uint8_t tap_thresh;          // 62.5 mg/LSB
    uint8_t tap_durat;           // units 625us/LSB
    uint8_t double_tap_lattency; // 1.25ms/LSB
    uint8_t double_tap_window;   // 1.25ms/LSB

} taps_configurations;

typedef const taps_configurations *taps_cfg;

void adxl_cfg_taps(taps_cfg);

#define act_x_en 0x40
#define act_y_en 0x20
#define act_z_en 0x10
#define act_dc_oper 0
#define act_ac_oper 1

typedef struct __packed
{
    uint8_t thresh_act; /// 62.5mg/LSB
    uint8_t axes;       // could be x, y, z,axes
    uint8_t _ac_dc;     /// ac or dc coupled operation
} activity;

#define inact_x_en 0x04
#define inact_y_en 0x02
#define inact_z_en 0x01

typedef struct __packed
{
    uint8_t thresh_inact; ///// 62.5mg/LSB
    uint8_t time_inact;   //// 1 sec/ LSB
    uint8_t axes;         // axes of interest x,y,z
    uint8_t _ac_dc;       /// ac or dc coupled operations
} inactivity;

typedef struct __packed
{
    activity act;     // for act configurations
    inactivity inact; // for inactivity configurations

} activity_inact_config;

typedef const activity_inact_config *act_inact_cfg;

void adxl_cfg_act_inact(act_inact_cfg);

/// @brief this is used to configure the frefall
/// @param  accleration value 62.5mg/LSB 300 to 600mg recommend
/// @param  time value 5ms/LSB 100 ms and 350 ms recommend
void adxl_cfg_freefall(uint8_t accel, uint8_t time);

enum _FIFO_CONFIGURETAION_TYPE_
{
    adxl_fifomode_bypass,
    adxl_fifomode_fifo,
    adxl_fifomode_stream,
    adxl_fifomode_trigger
};

/// @brief configure the fifo
/// @param  fifo_mode
/// @param  samples // max samples is 32
void adxl_cfg_fifo(uint8_t, uint8_t);

enum _ACCEL_CFG_INT_TYPE_
{
    accel_int_overrun = 0x00, // overrun interrupt set
    accel_int_watermark,      //  watermark interrupt set
    accel_int_freefall,       // freefall detection max time 1.25 sec
    accel_int_inactivity,     // inactivity
    accel_int_activity,       // activity
    accel_int_double_tap,     // double tap
    accel_int_single_tap,     // single tap innterrupt
    accel_int_data_ready,     //  this interrupt fires when data is ready according to the data rate
    accel_int_max_limit,
};

#define ADXL_INT_MASK_OVERRUN (_BV(accel_int_overrun))
#define ADXL_INT_MASK_WATERMARK (_BV(accel_int_watermark))
#define ADXL_INT_MASK_FREEFALL (_BV(accel_int_freefall))
#define ADXL_INT_MASK_INACTIVITY (_BV(accel_int_inactivity))
#define ADXL_INT_MASK_ACTIVITY (_BV(accel_int_activity))
#define ADXL_INT_MASK_DOUBLE_TAP (_BV(accel_int_double_tap))
#define ADXL_INT_MASK_SINGLE_TAP (_BV(accel_int_single_tap))
#define ADXL_INT_MASK_DATA_READY (_BV(accel_int_data_ready))

#define ACCEL_INT_ENABLE 0x01u
#define ACCEL_INT_DISABLE 0x00u

/// @brief to enable or disable the interrupt
/// @param int_type
/// @param en_dis
void adxl_int_en_dis(uint8_t int_type, uint8_t en_dis);

// disable all the interrupts and then enable according to your use case
void adxl_disable_all_ints(void);

/// @brief this will give you the interupt type of the accel
/// @param  void
/// @return interupt type @ref _ACCEL_CFG_INT_TYPE_
uint8_t adxl_read_int_type(void);

#define ACCEL_GETAXIS_FOR_ACTIVITY 0x10u
#define ACCEL_GETAXIS_FOR_TAPS 0x20u

#define ACCEL_INT_AXIS_NONE 0x0u
#define ACCEL_INT_AXIS_Z 0x01u
#define ACCEL_INT_AXIS_Y 0x02u
#define ACCEL_INT_AXIS_X 0x04u

#define ACCEL_INT_AXIS_MASK_VALUE 0x07u

/// @brief this will give you whch axis give the interrupt
/// @param int_type
/// @return interrupt axis
uint8_t adxl_read_int_axis(uint8_t int_type);

/// @brief this will read the accelration data from the sensor
/// @param data
/// @param size
/// @return succ/failure
bool adxl_read_data(uint8_t *data, uint8_t size);

/// @brief read the accelration values and convert it into proper float arrays value
/// @param buff
uint32_t read_accelration(int16_t *buff, uint8_t *data, uint8_t size);

void adxl_Read_reg(void );
//////////// define the gain of the acclerometer
#define accel_gain_in_mg 3.9f /// mg/LSB
#define accel_gain_in_g 0.0039f /// g/lsb

#endif