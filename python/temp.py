# import tkinter as tk
# import numpy as np

# class DrawingApp:
#     def __init__(self, root):
#         self.root = root
#         self.root.title("Drawing App")

#         # Create canvas
#         self.canvas = tk.Canvas(root, width=128, height=64, bg="white")
#         self.canvas.pack()

#         # Initialize grid state
#         self.grid_state = np.zeros((128, 64), dtype=int)

#         # Create a button to convert drawing to byte array
#         convert_button = tk.Button(root, text="Convert to Byte Array", command=self.convert_to_byte_array)
#         convert_button.pack()

#         # Bind mouse events to the canvas
#         self.canvas.bind("<B1-Motion>", self.draw)
#         self.canvas.bind("<ButtonRelease-1>", self.stop_drawing)

#         # Drawing state
#         self.drawing = False

#     def draw(self, event):
#         if self.drawing:
#             x, y = event.x, event.y
#             col, row = x // 2, y // 2
#             if 0 <= col < 128 and 0 <= row < 64:
#                 self.grid_state[col, row] = 1
#                 self.canvas.create_rectangle(col * 2, row * 2, (col + 1) * 2, (row + 1) * 2, fill="black")

#     def stop_drawing(self, event):
#         self.drawing = False

#     def convert_to_byte_array(self):
#         byte_array = bytearray()
#         for col in range(0, 128, 8):
#             for row in range(0, 64):
#                 byte = 0
#                 for i in range(8):
#                     if self.grid_state[col + i, row] == 1:
#                         byte |= (1 << (7 - i))
#                 byte_array.append(byte)
#         print("Byte Array:", byte_array)

# if __name__ == "__main__":
#     root = tk.Tk()
#     app = DrawingApp(root)
#     root.mainloop()

import tkinter as tk

class BinaryGridApp:
    def __init__(self, master):
        self.master = master
        self.master.title("Binary Grid App")

        self.cell_size = 10  # Set the desired cell size
        self.grid_size = (128, 64)
        self.cells = [[False] * self.grid_size[1] for _ in range(self.grid_size[0])]

        self.canvas = tk.Canvas(self.master, width=self.grid_size[0] * self.cell_size, height=self.grid_size[1] * self.cell_size, bg="white")
        self.canvas.pack()

        
        self.generate_button = tk.Button(self.master, text="Generate Bitmap", command=self.generate_bitmap)
        self.generate_button.pack()

        self._rects = [[]]
        
        
        self.clear_button = tk.Button(self.master, text="clear", command=self.clear_chart)
        self.clear_button.pack()
        self.draw_grid()

        self.canvas.bind("<B1-Motion>", self.toggle_cell)

    ##------------------ create a grid of the specified length for the oled representation 
    def draw_grid(self):
                
        ###+++++++++++++++++++ create the horizontal lines _________________++++++++++++++++
        for i in range(0, self.grid_size[0] * self.cell_size, self.cell_size):
            if i % (8 * self.cell_size) == 0:
                self.canvas.create_line(i, 0, i, self.grid_size[1] * self.cell_size, fill="black")
            else :
                self.canvas.create_line(i,0, i, self.grid_size[1] * self.cell_size, fill="gray")
                
                
        ###_++++++++++++++++++++ create the vertical lines ________________+++++++++++++++++++++
        for j in range(0, self.grid_size[1] * self.cell_size, self.cell_size):
            if j % (8 * self.cell_size) == 0:
                self.canvas.create_line(0, j, self.grid_size[0] * self.cell_size,j, fill="black")
            else :
                self.canvas.create_line(0, j, self.grid_size[0] * self.cell_size,j, fill="gray")
                
        
        
                
    def clear_chart(self):
        self.canvas.delete("rectangle")
                
    def toggle_cell(self, event):
        x, y = event.x, event.y
        cell_x = x // self.cell_size
        cell_y = y // self.cell_size
        self.cells[cell_x][cell_y] = not self.cells[cell_x][cell_y]
        color = "black" if self.cells[cell_x][cell_y] else "white"
        self.canvas.create_rectangle(cell_x * self.cell_size, cell_y * self.cell_size,
                                     (cell_x + 1) * self.cell_size, (cell_y + 1) * self.cell_size,
                                     fill=color, outline="gray")
        
        
    def generate_bitmap(self):
        bitmap_array = [[1 if self.cells[x][y] else 0 for y in range(self.grid_size[1])] for x in range(self.grid_size[0])]
        print(bitmap_array)

if __name__ == "__main__":
    root = tk.Tk()
    app = BinaryGridApp(root)
    root.mainloop()
