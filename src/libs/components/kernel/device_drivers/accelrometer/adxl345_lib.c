#include "adxl345_lib.h"

////// include for the i2c thread safe API
#include "i2c_thread_safe.h"

///=================================================================================
///=================================================================================
///==================== static functions here 
///////// these are the read and wrirte functions 
// function to read the register 
static uint8_t read_reg( uint8_t reg_addr)
{
    uint8_t read_buff[1];
    uint8_t write_buff[] = {reg_addr};

    i2c_xfr_config xfrcfg = 
    {
        .tx_buff = write_buff,
        .tx_size = sizeof(write_buff),
        .rx_buff = read_buff,
        .rx_size = sizeof(read_buff)
    };
    if(nrf_OK !=  i2c_write_and_read_data_arr(I2C_HOST_USED, ADXL_SENS_ADDR, &xfrcfg))
    {
       return 0; 
    }
    return read_buff[0];
}

// function to write the register 
static void write_reg(uint8_t reg_addr, uint8_t data)
{
    uint8_t buff[] = {reg_addr, data};
    i2c_write_data_arr(I2C_HOST_USED, ADXL_SENS_ADDR, buff, sizeof(buff));
}


//// we always set the link+autosleep in the adxl345

/// @brief put the accelrometer in stnadby 
#define ADXL_STANDBY()   \
write_reg(ADXL_REG_POWER_CTL ,0x30)

//// @brief clear the standby of the accelrometer 
#define ADXL_CLEAR_STANDBY()   \
write_reg(ADXL_REG_POWER_CTL ,0x38)


///=================================================================================
///=================================================================================

/// @brief this is to configure the accelormeter based on the paramter 
/// @param adxl_configure *
/// @return succ/failure  
uint32_t adxl_configure(const adxl_config cfg)
{
    /// check the device id first 
    if(read_reg(ADXL_REG_DEVID) != ADXL_DEVID )
        return nrf_ERR_OCCURED;
   
    ADXL_STANDBY();

    ////// configure the rate of the acclero
    uint8_t val =0;
    SETBIT(val ,(cfg->low_pwr << 4) | (cfg->rate));
    write_reg(ADXL_REG_BW_RATE, val);

    /// /// configure the int map
    // //// data ready , watermark and overrun are mapped to int 2 pin 
    // //// single, double ,act, inact, freefall are mapped to int 1 pin

    write_reg(ADXL_REG_INT_MAP, 0x82);
    write_reg(ADXL_REG_INT_ENABLE, 0);

    write_reg(ADXL_REG_FIFO_CTL, 0);
    // //// write the data format reg  , set the int to active low  and set fullres mode 
    val =0; 
    SETBIT(val, _BV(5) | _BV(3) | (cfg->output_range & 0x03));
    write_reg(ADXL_REG_DATA_FORMAT, val);

    ADXL_CLEAR_STANDBY();

    return 0;
}


void adxl_cfg_taps(const taps_cfg cfg)
{
   
    ADXL_STANDBY();

    // now configure the register tap thresh
    write_reg(ADXL_REG_THRESH_TAP , cfg->tap_thresh);
    // tap duration 
    write_reg(ADXL_REG_DUR , cfg->tap_durat);
  
    // // if double tap is enabled then do this config 
    if(cfg->tap_type & tap_en_double_tap)
    {
        write_reg(ADXL_REG_LATENT, cfg->double_tap_lattency);
        write_reg(ADXL_REG_WINDOW, cfg->double_tap_window);
    }

    // configure axis 
    uint8_t val =0; // the supress bit is already set 
    SETBIT(val, cfg->tap_axes ); // you can add axis by logical OR x_axis | y_axis | z_axis
    write_reg(ADXL_REG_TAP_AXES , val);

    ADXL_CLEAR_STANDBY();

}

void adxl_cfg_act_inact(const act_inact_cfg cfg)
{
    // // check if measurement bit is set 
    ADXL_STANDBY();

    // /// configure activity 
    write_reg(ADXL_REG_THRESH_ACT ,cfg->act.thresh_act );

    // configure inactivity 
    write_reg(ADXL_REG_THRESH_INACT , cfg->inact.thresh_inact);
    write_reg(ADXL_REG_TIME_INACT , cfg->inact.time_inact);
    
    uint8_t val = 0;
    SETBIT(val, (cfg->act._ac_dc <<7 ) |cfg->act.axes | (cfg->inact.axes) | (cfg->inact._ac_dc << 3));
    write_reg(ADXL_REG_ACT_INACT_CTL , val);
    
    ADXL_CLEAR_STANDBY();

}

void adxl_cfg_freefall(uint8_t accel , uint8_t time)
{
     ADXL_STANDBY();

    write_reg(ADXL_REG_THRESH_FF , accel);
    write_reg(ADXL_REG_TIME_FF , time);

      // enable the interrupt 
    uint8_t val = read_reg(ADXL_REG_INT_ENABLE);
    SETBIT(val, _BV(2)); // set both the act and inact int
    write_reg(ADXL_REG_INT_ENABLE ,val);
    
    ADXL_CLEAR_STANDBY();
}

/// @brief to enable or disable the interrupt
/// @param int_type
/// @param en_dis
void adxl_int_en_dis(uint8_t int_type, uint8_t en_dis)
{
    ADXL_STANDBY();

    uint8_t val = read_reg(ADXL_REG_INT_ENABLE);

    if (en_dis) // enable the interrupt 
    {
        SETBIT(val, int_type);
    }
    else // clear the interrupt
    {
        CLEARBIT(val, int_type);
    }
    
    write_reg(ADXL_REG_INT_ENABLE, val);

    ADXL_CLEAR_STANDBY();
}

// disable all the interrupts and then enable according to your use case
void adxl_disable_all_ints(void)
{
           // // check if measurement bit is set 
    ADXL_STANDBY();

    write_reg(ADXL_REG_INT_ENABLE, 0);

    ADXL_CLEAR_STANDBY();

}


void adxl_cfg_fifo(uint8_t mode, uint8_t samples)
{
     ADXL_STANDBY();

    uint8_t val =0;
    SETBIT(val, (mode<<6) | samples);

    ADXL_CLEAR_STANDBY();
}


/// @brief this will give you the interupt type of the accel 
/// @param  void
/// @return interupt type @ref _ACCEL_CFG_INT_TYPE_
uint8_t adxl_read_int_type(void)
{
    return read_reg(ADXL_REG_INT_SOURCE);
}

/// @brief this will give you whch axis give the interrupt 
/// @param int_type 
/// @return interrupt axis 
uint8_t adxl_read_int_axis(uint8_t int_type)
{
    uint8_t ret =0;
    //// read the reg vlaue 
    uint8_t reg = read_reg(ADXL_REG_ACT_TAP_STATUS);

    switch (int_type)
    {
    case ACCEL_GETAXIS_FOR_ACTIVITY:
        ret = (reg >> 4) &  ACCEL_INT_AXIS_MASK_VALUE;
        break;
    
    case ACCEL_GETAXIS_FOR_TAPS:
        ret = reg &  ACCEL_INT_AXIS_MASK_VALUE;
        break;

    default:
        ret = ACCEL_INT_AXIS_NONE;
        break;
    }

    return ret;
}


bool adxl_read_data(uint8_t * data , uint16_t size)
{
     uint8_t x0_addr = ADXL_REG_DATAX0;
    // if (i2c_master_write_read_device(I2C_HOST_0, ADXL_SENS_ADDR, &x0_addr, _1byte , data, size , i2c_wait_time) == ESP_OK)
    return 1;
    return 0;
}

bool read_accelration(float *buff )
{
    uint8_t x0_addr = ADXL_REG_DATAX0;
    uint8_t raw_buff[6] ={0};
    // if (i2c_master_write_read_device(I2C_HOST_0, ADXL_SENS_ADDR, &x0_addr, _1byte , raw_buff, 6 , i2c_wait_time) == ESP_OK)
    // {
    //     int16_t  x = (raw_buff[1] <<8 | raw_buff[0] );
    //     int16_t  y = (raw_buff[3] <<8 | raw_buff[2] );
    //     int16_t  z = (raw_buff[5] <<8 | raw_buff[4] );


    //     // calculate the compliment  this is correct 
    //     if(x & 0x8000) // check if x is -ve 
    //     {
    //         buff[0] = -(~(x-1));
    //     }
    //     else // if it is not -ve then asitis
    //     {
    //         buff[0] = x;
    //     }
    //     if(y & 0x8000)
    //     {
    //         buff[1] = -(~(y-1));
    //     }
    //     else 
    //     {
    //         buff[1] = y;
    //     }
    //     if( z & 0x8000)
    //     {
    //         buff[2] = -(~(z-1));
    //     }
    //     else 
    //     {
    //         buff[2] = z;
    //     }
    //     // multiply the data with the  gain factor 
    //     // the gain is 3.876 mg/LSB
    //     for(uint8_t i=0; i<3; i++)
    //     {
    //         buff[i] *= gain;
        // }


        // return 1;
    // }
   
    return 0;
   
}


