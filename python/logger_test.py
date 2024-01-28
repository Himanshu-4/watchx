## This is a UART LOGGER testing library that will test the logger library
# of the Nrf52 Logging over the UART . please note that it can do both the 
## input and output testing. but for that you have to enable it by yourself



import serial

from threading import Timer

from py_logs import LOGGING


log = LOGGING(__name__)


# import threading
##########################################################################3
#########################################################################3
###################### this is the printing with colors 


# class colors:

#     reset = '\033[0m'
#     bold = '\033[01m'
#     disable = '\033[02m'
#     underline = '\033[04m'
#     reverse = '\033[07m'
#     strikethrough = '\033[09m'
#     invisible = '\033[08m'
 
#     class fg:
#         black = '\033[30m'
#         red = '\033[31m'
#         green = '\033[32m'
#         orange = '\033[33m'
#         blue = '\033[34m'
#         purple = '\033[35m'
#         cyan = '\033[36m'
#         lightgrey = '\033[37m'
#         darkgrey = '\033[90m'
#         lightred = '\033[91m'
#         lightgreen = '\033[92m'
#         yellow = '\033[93m'
#         lightblue = '\033[94m'
#         pink = '\033[95m'
#         lightcyan = '\033[96m'
 
#     class bg:
#         black = '\033[40m'
#         red = '\033[41m'
#         green = '\033[42m'
#         orange = '\033[43m'
#         blue = '\033[44m'
#         purple = '\033[45m'
#         cyan = '\033[46m'
#         lightgrey = '\033[47m'


# def prRed(skk): print("\033[91m {}\033[00m" .format(skk))
 
 
# def prGreen(skk): print("\033[92m {}\033[00m" .format(skk))
 
 
# def prYellow(skk): print("\033[93m {}\033[00m" .format(skk))
 
 
# def prLightPurple(skk): print("\033[94m {}\033[00m" .format(skk))
 
 
# def prPurple(skk): print("\033[95m {}\033[00m" .format(skk))
 
 
# def prCyan(skk): print("\033[96m {}\033[00m" .format(skk))
 
 
# def prLightGray(skk): print("\033[97m {}\033[00m" .format(skk))
 
 
# def prBlack(skk): print("\033[98m {}\033[00m" .format(skk))





    
##########################################################################
########################################################################
##############




class SR_LOGGER_TESTING:
    def __init__(self) -> None:
        self.ser = serial.Serial()
        # config the serial port paramters 
        self.ser.baudrate = 1000000
        self.ser.port = 'COM9'
        # turn off hardware flow control
        self.ser.xonxoff = True
        self.ser.rtscts = False
        self.ser.dsrdtr = False
        self.ser.stopbits = serial.STOPBITS_ONE
        self.ser.parity =  serial.PARITY_NONE

        self.ser.write_timeout = 5
        # specify the read timeout, if deivce not respond then err condition  
        self.ser.timeout = 5

        self.ser.bytesize  = serial.EIGHTBITS

        

        log.info("UART logger testing application ")
        

    def __del__(self):
        ### close the port and say goodbye  😃
        log.critical("closing the application")
        if(self.ser.is_open):
            self.ser.close()
            
        
    def reset_buff(self):
        ### flush the buffers 
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()

    
    def test(self):
        try_data_1 = "This is a trial data "
        try_data_2 = "this is a read and write data \r\n"
        while True:
            # first get the user input 
            usr_cmd = input("enter what the cmd is ")
            if usr_cmd == "e":
                break
            elif usr_cmd == "1":
               self.write_data(try_data_1)
            elif usr_cmd == '2':
                self.write_and_read(try_data_2)
               
               
    def write_data(self,data:str):
        # try to open the port and start writing 
        # try to send some data to MCU 
        log.info(f"sending the data of len {len(data)} ")        
        try:
            self.ser.open()
            self.ser.write(bytes(data, encoding='utf-8'))
        except Exception as e:
            log.error(f"cannt write to serial as error-> {e}")
        finally:
            # close the port  
            if(self.ser.is_open):
                self.ser.close()
                
    def write_and_read(self,data):
        # try to open the port and start writing 
        # try to send some data to MCU 
        log.info(f"sending the data and len {len(data)} ")        
        try:
            self.ser.open()
            self.ser.write(bytes(data, encoding='utf-8'))
        except Exception as e:
            log.error(f"cannt write to serial port Error-> {e}")
          
        # try to read that data bback 
        try:
            read_data = self.ser.read(size=len(data))
            
        except Exception as e:
            log.error(f"can't read the data from the serial port as {e}")
            
        else :
            log.info("the data returned by the device is ")
            try:
                decoded_data = read_data.decode('utf-8')
            except UnicodeDecodeError as e:
                log.error(f"Error decoding bytes: {e}")
            else :
                log.info(decoded_data)
        
        # close the port  
        if(self.ser.is_open):
            self.ser.close()
            
            


if __name__ == "__main__":
    TEST  = SR_LOGGER_TESTING()
    TEST.test()
    
    