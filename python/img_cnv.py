
from PIL import Image

# Open the image
image_path = "data/new.png"  # Replace with the path to your image
image = Image.open(image_path)

# Resize the image to 128x32 if needed
image = image.resize((128, 32))

# Convert the image to grayscale
image = image.convert("L")

# Get the pixel values as a list
pixels = list(image.getdata())

# Convert pixel values to hex code
hex_code = [format(pixel, '02x') for pixel in pixels]

# Print the hex code
print(hex_code)
