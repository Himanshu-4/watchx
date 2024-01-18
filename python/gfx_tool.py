import tkinter as tk

from py_logs import LOGGING

import serial


gfx_cmds = {
        "CMD_NOP"              :0 ,
        "CMD_RESET"            :1 ,
        "CMD_SEND_COORDINATES" :2 ,
        "CMD_SEND_DATA"        :3 ,
        "CMD_SEND_DATA_SIZE"   :4 ,
        "CMD_SHOW_BITMAP"      :5 ,
        "CMD_CLEAR_BITMAP"     :6 ,

    # //  * rest coomnads can be added but later
}

gfx_cmd_to_str = {
            0 : "CMD_NOP",
            1 : "CMD_RESET",
            2 : "CMD_SEND_COORDINATES",
            3 : "CMD_SEND_DATA",
            4 : "CMD_SEND_DATA_SIZE",
            5 : "CMD_SHOW_BITMAP",
            6 : "CMD_CLEAR_BITMAP",
            10 : "CMD_NOT_DEFINED"
}

gfx_cmd_rsp = {
         0 : "RSP_ACK",
         1 : "RSP_NACK",
         2 : "RSP_Invalid_cmd",
         3 : "RSP_Invalid_state",
         4 : "RSP_Invalid_param",
         5 : "RSP_data_overflw",
         6 : "RSP_Busy",
         7 : "RSP_err_occured",
         8 : "RSP_OP_failed",
}

PREAMBLE_HEADER = 0x89
##------------ define the logger here -----------------------------
log = LOGGING(__name__)


class BinaryGridApp:
    def __init__(self, master):
        self.master = master
        self.master.title("Binary Grid App")

        self.cell_size = 10  # Set the desired cell size
        self.grid_size = (128, 64)
        # self.cells = [[False] * self.grid_size[1] for _ in range(self.grid_size[0])]

        self.canvas = tk.Canvas(self.master, width=self.grid_size[0] * self.cell_size, height=self.grid_size[1] * self.cell_size, bg="white")
        self.canvas.pack()

        self._rects = [[0 ] * self.grid_size[1] for _ in range(self.grid_size[0]) ]
        
  
        #########-================================================================================================
        ############========================= input of the start and end of the matrix  ======================== 
        # self.validate_cmd = self.master.register( lambda P:  (P.isdigit() or P == ""))
        
        ##============= Tkinter start  position of the  matrix ====================================================
        self.__start_point_lbl = tk.Label(self.master, text="Start points (0-16,0-8)")
        self.__start_point = tk.Entry(self.master, border=4,width=10)
        
        self.__start_point_lbl.pack(side='left', padx=10,pady=5)
        self.__start_point.pack(side='left', padx=10, pady=5)
        
        ##++++++++++++++++++++++++++ Tkinter end position of the matrix ++++++++++++++++++++++++++++++++++
        self.__end_point_lbl = tk.Label(self.master, text="End points (0-16,0-8)")
        self.__end_point = tk.Entry(self.master, border=4,width=10)
        self.__end_point_lbl.pack(side='left', padx=10,pady=5)
        self.__end_point.pack(side='left', padx=10,pady=5)
    
              
             
        #########-================================================================================================
        ############========================= buttons for the application functionalities ======================== 
        ### left aligned button 
        self.generate_button = tk.Button(self.master, text="Generate Bitmap", command=self.generate_bitmap)
        self.generate_button.pack(side='left', padx=30,pady=2)
        self.clear_bitmap_btn = tk.Button(self.master, text="clear", command=self.clear_chart)
        self.clear_bitmap_btn.pack(side="left", padx=30,pady=2)
        
        ## right aligned buttons 
        self.__raw_cmd = tk.Entry(self.master, border=4,width=10)
        self.__raw_cmd.pack(side='right', padx=15)
        
        self.send_raw_cmd_btn = tk.Button(self.master, text="send_raw", command=self.send_raw_cmd)
        self.send_raw_cmd_btn.pack(side= "right",padx=30,pady=2)
        
        self.send_bitmap_btn = tk.Button(self.master, text="send_bitmap", command=self.send_bitmap )
        self.send_bitmap_btn.pack(side= 'right', padx=30,pady=2)
        
        self.send_clear_btn =tk.Button(self.master, text="clear_bitmap",command=self.send_clear_bitmap)
        self.send_clear_btn.pack(side='right',padx=40,pady=10)
        
        
        
        ### draw the grid this func is defined below
        self.draw_grid()

        self.canvas.bind("<Control-Motion>", self.set_cells)
        self.canvas.bind("<Shift-Motion>",self.unset_cells)


        #######===================================================================================================
        ###=============================== serial communication inits
        
        # space is used as a seperator and it will send the command to directly 

        self.ser = serial.Serial()

        # config the serial port paramters 
        self.ser.baudrate = 921600
        self.ser.port = 'COM6'
        # turn off hardware flow control
        self.ser.xonxoff = True
        self.ser.rtscts = False
        self.ser.dsrdtr = False
        self.ser.stopbits = serial.STOPBITS_ONE
        self.ser.parity =  serial.PARITY_NONE

        self.ser.write_timeout = 2
        # specify the read timeout, if deivce not respond then err condition  
        self.ser.timeout = 2

        self.ser.bytesize  = serial.EIGHTBITS

        ### try to open the connection 
        try:
            self.ser.open()
        except Exception as e:
            log.error("cant able to open the serial port")
        else:
            log.info("device connected succesfully")
            
        log.info("Python GFX testing tool")
        
    def __del__(self):
        
        ## close the serial connection 
        self.ser.close()
        
        log.critical("closing the Python GFX app")
        
        
    def reset_pipe(self):
        ''' reset the communicatoin pipe '''
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()
        
    def handle_serial_cmds(self):
        '''' have to handle the serial command here '''
        
    ##------------------ create a grid of the specified length for the oled representation 
    def draw_grid(self):
        
        ''' @info create rectangles for the canvas  '''
        for i in range(0, self.grid_size[0] ):
            for j in range(0, self.grid_size[1] ):
                self._rects[i][j] = self.canvas.create_rectangle(i * self.cell_size,  j* self.cell_size, (i* self.cell_size) + self.cell_size, (j * self.cell_size)+ self.cell_size, outline="gray", fill="white")
                
        ###+++++++++++++++++++ create the horizontal lines _________________++++++++++++++++
        for i in range(0, self.grid_size[0] * self.cell_size, self.cell_size):
            if i % (8 * self.cell_size) == 0:
                self.canvas.create_line(i, 0, i, self.grid_size[1] * self.cell_size, fill="black")
            # else :
            #     self.canvas.create_line(i,0, i, self.grid_size[1] * self.cell_size, fill="gray")
                
                
        ###_++++++++++++++++++++ create the vertical lines ________________+++++++++++++++++++++
        for j in range(0, self.grid_size[1] * self.cell_size, self.cell_size):
            if j % (8 * self.cell_size) == 0:
                self.canvas.create_line(0, j, self.grid_size[0] * self.cell_size,j, fill="black")
            # else :
            #     self.canvas.create_line(0, j, self.grid_size[0] * self.cell_size,j, fill="gray")
                
        
                
    def clear_chart(self):
        ''''' this will clear the drawing from the screen '''
        ####-------clear every rectangle -------------------
        for i in range(self.grid_size[0]):
            for j in range(self.grid_size[1]):
                self.canvas.itemconfigure(self._rects[i][j], fill="white")

                
    def set_cells(self, event ):
        ''' the event would be of the type as state, x,y '''
        x, y = event.x, event.y
        cell_x = x // self.cell_size
        cell_y = y // self.cell_size
        # self.cells[cell_x][cell_y] = not self.cells[cell_x][cell_y]
        # color = "black" if self.cells[cell_x][cell_y] else "white"
        if cell_x >= self.grid_size[0] or cell_y >= self.grid_size[1]:
            log.error("out of index setcells")
            return
        
        self.canvas.itemconfigure(self._rects[cell_x][cell_y], fill='black')
        
        # self.canvas.create_rectangle(cell_x * self.cell_size, cell_y * self.cell_size,
        #                              (cell_x + 1) * self.cell_size, (cell_y + 1) * self.cell_size,
        #                              fill=color, outline="gray")
        
    def unset_cells(self,event):
        ''' the event would be of the type as state, x,y '''
        x, y = event.x, event.y
        cell_x = x // self.cell_size
        cell_y = y // self.cell_size
        # self.cells[cell_x][cell_y] = not self.cells[cell_x][cell_y]
        # color = "black" if self.cells[cell_x][cell_y] else "white"
        if cell_x >= self.grid_size[0] or cell_y >= self.grid_size[1]:
            log.error("out of index unset cells ")
            return
        
        self.canvas.itemconfigure(self._rects[cell_x][cell_y], fill='white')
        
        
        
           
    def generate_bitmap(self):
        
        #----------- get the start and end of the matrix -------------------------------------
        start_x,start_y = [ int(x) for x in self.__start_point.get().split(',')]
        end_x,end_y = [ int(x) for x in self.__end_point.get().split(',')]
        
        # for x in range(start_x,end_x, 1):
        #     for y in range(start_y,end_y,1):
        data = []
        ##------------------- get the one byte info 

        start_x *= 8
        end_x *= 8
                
        for y in range(start_y,end_y ,1): 
            for x in range(start_x,end_x ,1):
                one_byte =0
                for i in range(8):
                    one_byte |= (   ( 1 if (self.canvas.itemcget(self._rects[x][(y* 8)  + i], "fill") == "black") else 0 ) << i)
                data.append(one_byte)

        log.info(f"data of the list is {len(data)} ")
        log.info(data)


    def send_bitmap(self):
        ''' send the bitmap to the oled disaplay'''
        
        
        
    def send_clear_bitmap(self):
        ''' clear the bitmap display '''
        # send the reset operation 
        #after sending read the return 
        ret =0
        try :
            val = [PREAMBLE_HEADER, 7,gfx_cmds["CMD_CLEAR_BITMAP"],
               *[int(x) for x in self.__start_point.get().split(',')], 
               *[int(x) for x in self.__end_point.get().split(',') ]] # unpack the list 
            
            self.ser.write(bytes(val))
            ret = self.ser.read_until(b"\n")
        except Exception as e:
            log.error("cant read device while clearing bitmap")
        else :
            log.info(f"cleared bitmap return value is {ret}")    
        
    def  send_raw_cmd(self):
        '''send the raw command to the device'''  
        try :
            log.info(f"sending command {gfx_cmd_to_str.get(int(self.__raw_cmd.get()), 10)} ")
            val = [PREAMBLE_HEADER,3,int(self.__raw_cmd.get())]
            self.ser.write(bytes(val))
        except Exception as e:
            log.error(f"cannt load the cmd as {e} ")
        else :
            ### get the response and show it 
            rsp = self.ser.read_until(b"\n")
            log.info(f"rsp is {rsp}")
            
if __name__ == "__main__":
    root = tk.Tk()
    app = BinaryGridApp(root)
    root.mainloop()
