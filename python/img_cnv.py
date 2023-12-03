import tkinter as tk

root = tk.Tk()
canvas = tk.Canvas(root, width=200, height=200)
canvas.pack()

# Create a rectangle and get its item ID
rectangle_id = canvas.create_rectangle(50, 50, 150, 150, fill="blue")

print("Item ID of the rectangle:", rectangle_id)

root.mainloop()
