#include "nrf_accelromter.h"

////// include for the i2c thread safe API
#include "i2c_thread_safe.h"
//// include for the gpio interrupt handler 
#include "gpiote.h"

/////// include for the freertos specific 

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

////// incluede the main helper library 
#include "main_helper.h"



// function to read the register 
static uint8_t read_reg( uint8_t reg_addr)
{
    // uint8_t data;
    // if (i2c_master_write_read_device(I2C_HOST_0, ADXL_SENS_ADDR, &reg_addr, _1byte , &data, _1byte , i2c_wait_time) == ESP_OK)
    // return data;
    return 0;

}

// function to write the register 
static void write_reg(uint8_t reg_addr, uint8_t data)
{
    // uint8_t buff[] ={reg_addr, data};

    // if (i2c_master_write_to_device(I2C_HOST_0, ADXL_SENS_ADDR, buff, _2byte , i2c_wait_time) != ESP_OK)
    // assert(0);

}

// function to init the bus 
static void i2c_bus_init(void)
{

    // i2c_config_t conf = {
    //     .mode = I2C_MODE_MASTER,
    //     .sda_io_num = ADXL_SDA_IO,
    //     .scl_io_num = ADXL_SCL_IO,
    //     .sda_pullup_en = GPIO_PULLUP_DISABLE,
    //     .scl_pullup_en = GPIO_PULLUP_DISABLE,
    //     .master.clk_speed = ADXL_FREQ_HZ,
    // };

    // i2c_param_config(I2C_HOST_0, &conf);
    
    // i2c_driver_install(I2C_HOST_0 , conf.mode, ADXL_MASTER_RX_BUF_DISABLE, ADXL_MASTER_TX_BUF_DISABLE, 0);
}

// function to init the accelro
void adxl_init(void)
{
    i2c_bus_init();
    printf("devid is 0xe5 act id is %x\r\n",read_reg(DEVID));
}


void adxl_deinit(void)
{
    // i2c_driver_delete(I2C_HOST_0);
}


void adxl_cfg_(adxl_config cfg)
{
    uint8_t val = read_reg(POWER_CTL);
    if(read_bit(val, 3))
    {
        // clear the measure bit to configure the accel make the register to 0
        write_reg(POWER_CTL ,0x00);
    }
    
    ////// configure the rate of the acclero
    val =0;
    set_bit(val ,(cfg->low_pwr << 4) | (cfg->rate));
    write_reg(BW_RATE, val);

    /// /// configure the int map
    // //// data ready , watermark and overrun are mapped to int 2 pin 
    // //// single, double ,act, inact, freefall are mapped to int 1 pin

    write_reg(INT_MAP, 0x82);

    // //// write the data format reg  , set the int to active low  and set fullres mode 
    val =0; 
    set_bit(val, _bv(5) | _bv(3) | cfg->output_range);
    write_reg(DATA_FORMAT, val);


    val =0;
    set_bit(val, _bv(3));
    if(cfg->link_autosleep)
    set_bit(val , 0x30 );

    write_reg(POWER_CTL, val);


}

void adxl_cfg_taps(taps_cfg cfg)
{
   
    // // check if measurement bit is set 
     uint8_t val = read_reg(POWER_CTL);
    if(read_bit(val, 3))
    {
        // clear the measure bit to configure the accel
        clear_bit(val,3);
        write_reg(POWER_CTL ,val);
    }

    // now configure the register tap thresh
    write_reg(THRESH_TAP , cfg->tap_thresh);
    // tap duration 
    write_reg(DUR , cfg->tap_durat);
  
    // // if double tap is enabled then do this config 
    if(cfg->tap_type & double_tap)
    {
        write_reg(LATENT, cfg->double_tap_lattency);
        write_reg(WINDOW, cfg->double_tap_window);
        // set the interrupt bit as well 
        // uint8_t int_val =0;
        // int_val = read_reg(INT_ENABLE);

        // set_bit(int_val, _bv(5));
        // write_reg(INT_ENABLE, int_val);
    }

    // configure axis 
    val =0; // the supress bit is already set 
    set_bit(val, cfg->tap_axes ); // you can add axis by logical OR x_axis | y_axis | z_axis
    write_reg(TAP_AXES , val);

    // // now put the adxl back in masurement mode 
    val = read_reg(POWER_CTL);
    set_bit(val, _bv(3));
    write_reg(POWER_CTL, val);

}

void adxl_cfg_act_inact(act_inact_cfg cfg)
{
    // // check if measurement bit is set 
     uint8_t val = read_reg(POWER_CTL);
    if(read_bit(val, 3))
    {
        // clear the measure bit to configure the accel
        clear_bit(val,3);
        write_reg(POWER_CTL ,val);
    }
    // /// configure activity 
    write_reg(THRESH_ACT ,cfg->act.thresh_act );

    // configure inactivity 
    write_reg(THRESH_INACT , cfg->inact.thresh_inact);
    write_reg(TIME_INACT , cfg->inact.time_inact);
    
    val = read_reg(ACT_INACT_CTL);
    set_bit(val, (cfg->act._ac_dc <<7 ) |cfg->act.axes | (cfg->inact.axes) | (cfg->inact._ac_dc << 3));
    write_reg(ACT_INACT_CTL , val);
    

    // // now put the adxl back in masurement mode 
    val = read_reg(POWER_CTL);
    set_bit(val, _bv(3));
    write_reg(POWER_CTL, val);

}

void adxl_cfg_freefall(uint8_t accel , uint8_t time)
{
       // // check if measurement bit is set 
     uint8_t val = read_reg(POWER_CTL);
    if(read_bit(val, 3))
    {
        // clear the measure bit to configure the accel
        clear_bit(val,3);
        write_reg(POWER_CTL ,val);
    }

    write_reg(THRESH_FF , accel);
    write_reg(TIME_FF , time);

    
      // enable the interrupt 
    val = read_reg(INT_ENABLE);
    set_bit(val, _bv(2)); // set both the act and inact int
    write_reg(INT_ENABLE ,val);

     // // now put the adxl back in masurement mode 
    val = read_reg(POWER_CTL);
    set_bit(val, _bv(3));
    write_reg(POWER_CTL, val);

}


void cfg_int(uint8_t int_type, uint8_t value)
{
        // // check if measurement bit is set 
     uint8_t val = read_reg(POWER_CTL);
    if(read_bit(val, 3))
    {
        // clear the measure bit to configure the accel
        clear_bit(val,3);
        write_reg(POWER_CTL ,val);
    }

    val = read_reg(INT_ENABLE);

    if (value) // enable the interrupt 
    {
        set_bit(val, _bv(int_type));
    }
    else // clear the interrupt
    {
        clear_bit(val, int_type);
    }
    
    write_reg(INT_ENABLE, val);

     // // now put the adxl back in masurement mode 
    val = read_reg(POWER_CTL);
    set_bit(val, _bv(3));
    write_reg(POWER_CTL, val);
}

void disable_all_ints(void)
{
           // // check if measurement bit is set 
     uint8_t val = read_reg(POWER_CTL);
    if(read_bit(val, 3))
    {
        // clear the measure bit to configure the accel
        clear_bit(val,3);
        write_reg(POWER_CTL ,val);
    }
    write_reg(INT_ENABLE, 0);

     // // now put the adxl back in masurement mode 
    val = read_reg(POWER_CTL);
    set_bit(val, _bv(3));
    write_reg(POWER_CTL, val);


}
void adxl_cfg_fifo(uint8_t mode, uint8_t samples)
{
      // // check if measurement bit is set 
     uint8_t val = read_reg(POWER_CTL);
    if(read_bit(val, 3))
    {
        // clear the measure bit to configure the accel
        clear_bit(val,3);
        write_reg(POWER_CTL ,val);
    }
    set_bit(val, (mode<<6) | samples);
    // // now put the adxl back in masurement mode 
    val = read_reg(POWER_CTL);
    set_bit(val, _bv(3));
    write_reg(POWER_CTL, val);

}

uint8_t read_int_type(void)
{
    return read_reg(INT_SOURCE);
}

void tap_int_handler(void)
{
    default_handler();
}

void act_inact_handler(void)
{
    default_handler();
}


void default_handler(void)
{
    printf(" this is default handler \r\n");
    // 
}

bool adxl_read_data(uint8_t * data , uint16_t size)
{
     uint8_t x0_addr = DATAX0;
    // if (i2c_master_write_read_device(I2C_HOST_0, ADXL_SENS_ADDR, &x0_addr, _1byte , data, size , i2c_wait_time) == ESP_OK)
    return 1;
    return 0;
}

bool read_accelration(float *buff )
{
    uint8_t x0_addr = DATAX0;
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