import tkinter as tk
import numpy as np

class DrawingApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Drawing App")

        # Create canvas
        self.canvas = tk.Canvas(root, width=128, height=64, bg="white")
        self.canvas.pack()

        # Initialize grid state
        self.grid_state = np.zeros((128, 64), dtype=int)

        # Create a button to convert drawing to byte array
        convert_button = tk.Button(root, text="Convert to Byte Array", command=self.convert_to_byte_array)
        convert_button.pack()

        # Bind mouse events to the canvas
        self.canvas.bind("<B1-Motion>", self.draw)
        self.canvas.bind("<ButtonRelease-1>", self.stop_drawing)

        # Drawing state
        self.drawing = False

    def draw(self, event):
        if self.drawing:
            x, y = event.x, event.y
            col, row = x // 2, y // 2
            if 0 <= col < 128 and 0 <= row < 64:
                self.grid_state[col, row] = 1
                self.canvas.create_rectangle(col * 2, row * 2, (col + 1) * 2, (row + 1) * 2, fill="black")

    def stop_drawing(self, event):
        self.drawing = False

    def convert_to_byte_array(self):
        byte_array = bytearray()
        for col in range(0, 128, 8):
            for row in range(0, 64):
                byte = 0
                for i in range(8):
                    if self.grid_state[col + i, row] == 1:
                        byte |= (1 << (7 - i))
                byte_array.append(byte)
        print("Byte Array:", byte_array)

if __name__ == "__main__":
    root = tk.Tk()
    app = DrawingApp(root)
    root.mainloop()
