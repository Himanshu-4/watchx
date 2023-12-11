# import tkinter as tk

# def create_square():
#     # Get values from entry widgets
#     value1 = entry1.get()
#     value2 = entry2.get()
#     value3 = entry3.get()
#     value4 = entry4.get()

#     # Do something with the values (you can customize this part based on your needs)

#     # For demonstration, let's print the values
#     print("Entry 1:", value1)
#     print("Entry 2:", value2)
#     print("Entry 3:", value3)
#     print("Entry 4:", value4)

# # Create the main window
# root = tk.Tk()
# root.title("Square of Entry Boxes")

# # Create Entry widgets
# entry1 = tk.Entry(root)
# entry2 = tk.Entry(root)
# entry3 = tk.Entry(root)
# entry4 = tk.Entry(root)

# # Arrange Entry widgets in a square using grid
# entry1.grid(row=0, column=0, padx=10, pady=5)
# entry2.grid(row=0, column=1, padx=10, pady=5)
# entry3.grid(row=1, column=0, padx=10, pady=5)
# entry4.grid(row=1, column=1, padx=10, pady=5)

# # Add padding between upper and lower rows
# root.rowconfigure(0, pad=10)
# root.rowconfigure(1, pad=10)

# # Create a button to perform an action (e.g., print values)
# button = tk.Button(root, text="Create Square", command=create_square)
# button.grid(row=2, column=0, columnspan=2)

# # Run the Tkinter event loop
# root.mainloop()

import tkinter as tk

def create_square():
    # Get values from entry widgets
    value1 = entry_var1.get()
    value2 = entry_var2.get()
    value3 = entry_var3.get()
    value4 = entry_var4.get()

    # Do something with the values (you can customize this part based on your needs)

    # For demonstration, let's print the values
    print("Entry 1:", value1)
    print("Entry 2:", value2)
    print("Entry 3:", value3)
    print("Entry 4:", value4)

# Create the main window
root = tk.Tk()
root.title("Square of Entry Boxes")

# Create Entry widgets and associated StringVar variables
entry_var1 = tk.StringVar()
entry_var2 = tk.StringVar()
entry_var3 = tk.StringVar()
entry_var4 = tk.StringVar()

entry1_label = tk.Label(root, text="Entry 1:")
entry2_label = tk.Label(root, text="Entry 2:")
entry3_label = tk.Label(root, text="Entry 3:")
entry4_label = tk.Label(root, text="Entry 4:")

entry1 = tk.Entry(root, textvariable=entry_var1)
entry2 = tk.Entry(root, textvariable=entry_var2)
entry3 = tk.Entry(root, textvariable=entry_var3)
entry4 = tk.Entry(root, textvariable=entry_var4)

# Pack Label and Entry widgets
entry1_label.pack(side="left", padx=10, pady=5)
entry1.pack(side="right", padx=10, pady=5)

entry2_label.pack(side="left", padx=10, pady=5)
entry2.pack(side="right", padx=10, pady=5)

entry3_label.pack(side="left", padx=10, pady=5)
entry3.pack(side="right", padx=10, pady=5)

entry4_label.pack(side="left", padx=10, pady=5)
entry4.pack(side="right", padx=10, pady=5)

# Create a button to perform an action (e.g., print values)
button = tk.Button(root, text="Create Square", command=create_square)
button.pack(side="top", pady=10)

# Run the Tkinter event loop
root.mainloop()
