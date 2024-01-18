import serial

from threading import Timer

# import threading
##########################################################################3
#########################################################################3
###################### this is the printing with colors 


class colors:

    reset = '\033[0m'
    bold = '\033[01m'
    disable = '\033[02m'
    underline = '\033[04m'
    reverse = '\033[07m'
    strikethrough = '\033[09m'
    invisible = '\033[08m'
 
    class fg:
        black = '\033[30m'
        red = '\033[31m'
        green = '\033[32m'
        orange = '\033[33m'
        blue = '\033[34m'
        purple = '\033[35m'
        cyan = '\033[36m'
        lightgrey = '\033[37m'
        darkgrey = '\033[90m'
        lightred = '\033[91m'
        lightgreen = '\033[92m'
        yellow = '\033[93m'
        lightblue = '\033[94m'
        pink = '\033[95m'
        lightcyan = '\033[96m'
 
    class bg:
        black = '\033[40m'
        red = '\033[41m'
        green = '\033[42m'
        orange = '\033[43m'
        blue = '\033[44m'
        purple = '\033[45m'
        cyan = '\033[46m'
        lightgrey = '\033[47m'


def prRed(skk): print("\033[91m {}\033[00m" .format(skk))
 
 
def prGreen(skk): print("\033[92m {}\033[00m" .format(skk))
 
 
def prYellow(skk): print("\033[93m {}\033[00m" .format(skk))
 
 
def prLightPurple(skk): print("\033[94m {}\033[00m" .format(skk))
 
 
def prPurple(skk): print("\033[95m {}\033[00m" .format(skk))
 
 
def prCyan(skk): print("\033[96m {}\033[00m" .format(skk))
 
 
def prLightGray(skk): print("\033[97m {}\033[00m" .format(skk))
 
 
def prBlack(skk): print("\033[98m {}\033[00m" .format(skk))

###########################################################################
###############################################################################
##############################################################################

# space is useed as a seperator and it will send the command to directly 

ser = serial.Serial()

# config the serial port paramters 
ser.baudrate = 115200
ser.port = 'COM9'
# turn off hardware flow control
ser.xonxoff = True
ser.rtscts = False
ser.dsrdtr = False
ser.stopbits = serial.STOPBITS_ONE
ser.parity =  serial.PARITY_NONE

ser.write_timeout = 5
# specify the read timeout, if deivce not respond then err condition  
ser.timeout = 5

ser.bytesize  = serial.EIGHTBITS


##########################################################################
########################################################################
############## global variables 
eeg_data_size = 33

Fuel_data_size = 70

TDCS_data_size = 22

Accel_data_size = 22

en_flag = False
##########################################################################
###############
#########################################################
##############



##########################################################################
########################################################################
##############
def wait_for_stop():
    global en_flag
    en_flag = False
    

def close_app():
    ser.close()
    print(colors.fg.red,colors.bold,"closing the serial app\r\n",colors.reset)

def reset_buff():
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    
##########################################################################
########################################################################
##############


print(colors.fg.yellow,
    "this is the test Application to test the Firmware\r\n"
    "Enter the following number for performing the  following test\r\n"
    ,colors.bold,colors.fg.green,
    "                          \r\n"
    "1 -> for testing the EEG\r\n"
    "2 -> for testing the TDCS\r\n"
    "3 -> for testing the fuel gauge \r\n"
    "4 -> for testing the Accelrometer\r\n"
    "r -> reset the device \r\n"
    "close -> to close the python aPP\r\n"
    "\r\n"
    ,colors.fg.red,
    "enter s for stop immediately any running test "
    ,colors.reset)


# we can unccoment it later 
ser.open()


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print('Interrupted')
        try:
            sys.exit(130)
        except SystemExit:
            os._exit(130)


def main():

    while True:
        # read the acknowledment from the device 
        err  = ser.read(1)
        print(ser)
        

#################################################################################
##################################################################################
#################################################################################
##################################################################################








    # flush the buffer content 
    ser.reset_input_buffer()
    ser.reset_output_buffer()

    print("sending serial data\r\n")
    ser.write(bytes(ser_str , encoding='utf-8'))
    var = ser.read(len(ser_str))
    print(var)
    ser.close()


import serial

Baud_Rate = 115200
Ser = serial.Serial('COM9', Baud_Rate)

while Ser.isOpen():
    input('Error: Ser is already open, please close it manually. if the port is closed, press enter\n')
    try:
        Ser.close()
    except serial.serialutil.PortNotOpenError:
        pass
Ser.open()

Serial_com = [] #a str array with the relevant commands

for i in range(len(Serial_com)):
    ter = Ser.readline()
    while ter != 0xaf:
        #print(str(ter))
        print(str(ter.decode('utf-8').rstrip('\r\n')))
        ter = Ser.readline()
    sleep(1)
    if i == 0:
        print('login: root')
        Ser.write(bytes("root", encoding='utf-8'))
    else:
        print('\n\n\n\n\nroot @ Ser: ~  # ' + str(Serial_com[i]))
        Ser.write(bytes(Serial_com[i], encoding='utf8'))

def print_format_table():
    """
    prints table of formatted text format options
    """
    for style in range(8):
        for fg in range(30, 38):
            s1 = ''
            for bg in range(40, 48):
                format = ';'.join([str(style), str(fg), str(bg)])
                s1 += '\x1b[%sm %s \x1b[0m' % (format, format)
            print(s1)
        print('\n')
 

for the time limit per question
input_time = int(input("Set time limit in seconds: "))
  
# Print the question to display
print("Who is your best friend?")
  
# Set the timer for the specified time and call the
# function to print the message when time is over
t = Timer(input_time, lambda: print(
    "\nYour writing time is over!!\nEnter / to quit the program"))
  
# Start the timer
t.start()
  
# Print a message for user specifying number of seconds
print("You have", str(input_time), " seconds to write the answer")
  
# Get value from user
answer = input()
  
# Stop the timer
t.cancel()

