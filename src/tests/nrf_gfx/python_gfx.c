
/******
 * Write a test program for sending the command and frames to the OLED screen
 * and also to erase, generate, send the bitmaps to the OLED screen
 *
 * 
 * 
 * ==============================================================================
 * ----------------- deifne the Command structure -------------------------------
 * 
 * Host --> Python,   device --> NRF OLED,
 * Host --> send cmd and data , Slave --> only send responses.
 * 
 * so the Header information would be like this 
 * 
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * type,format          |    description               | Direction
 *  1 byte (meta start) | to signal the header start   | Host --> slave
 *    Preamble info     | information of any cmd       |Slave --> Host
 * ----------------------------------------------------------------------
 * 1 byte               |the actual command send by the| Host <=> Slave 
 * (actual command)     |host                          |
 * -----------------------------------------------------------------------
 * 2 byte (length )     |the len of the data or the cmd| Host <=> slave
 *                      |  data len if any             | 
 * -----------------------------------------------------------------------
 * rest are data        | data can be long as deifined |
 *                      | in the Length Section        | 
 * ------------------------------------------------------------------------
 * Preamble info --> 0x34 ( can be composed of header version, etc)
 * 
 * ---------------- define the commands here------------------------------
 * reset                    1
 * send coordinates         2
 * send data                3
 * send data size           4
 * show bitmap              5
 * clear command            6
 * rest coomnads can be added but later 
 * 
 * 
 * 
 * ------------------- Data size -----------------------------------------
 * the data size limit is 240 bytes during a transaction i.e. the size cannot exceed 
 * 240 bytes + cmd Header size  
 * 
 * 
 * ------------- Response of the command -----------------------------------
 * ACK              0 
 * NACK             1 
 * Invalid commad   2 
 * Invalid Param    3
 * data overflow    4 
 * Busy             5
 * operation failed 6
 * 
                      | 
*/


typedef enum _CMD_TYPES_
{
    GFX_CMD_NOP,
    GFX_CMD_RESET =1,   
    GFX_CMD_SEND_COORDINATES,
    GFX_CMD_SEND_DATA,
    GFX_CMD_SEND_DATA_SIZE,
    GFX_CMD_SHOW_BITMAP,
    GFX_CMD_CLEAR_BITMAP,
    
//  * rest coomnads can be added but later 
};


typedef enum _CMD_RESPS_
{
    RSP_ACK,
    RSP_NACK,
    RSP_Invalid_cmd,
    RSP_Invalid_param,
    RSP_data_overflw,
    RSP_Busy,
    RSP_OP_failed,

};



typedef struct __packed _CMD_STRUCT_
{
    uint8_t preamble_info;
    uint8_t cmd_type;
    uint16_t len;
    uint8_t data[1]; // 1 is only used for placeholder for compiler
};





void test_oled_anim_prog(void *param)
{



    
}