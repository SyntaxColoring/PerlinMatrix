import time

import board
import displayio
import framebufferio
import rgbmatrix
from ulab import numpy as np

import perlin
import xxhash

WIDTH = 32
HEIGHT = 32

# Copied from:
# https://learn.adafruit.com/rgb-led-matrices-matrix-panels-with-circuitpython/matrixportal
displayio.release_displays()

matrix = rgbmatrix.RGBMatrix(
    width=WIDTH,
    height=HEIGHT,
    # rgbmatrix takes colors in a 5-6-5 format.
    # https://www.sparkfun.com/news/2650 suggests that this is
    # a limitation of the rgbmatrix library and not an inherent fact
    # of the display.
    #
    # Anecdotally, cranking this up to 5 doesn't substantially impact framerate.
    # Our bottleneck is Perlin noise generation, etc.
    #
    # We could do 6, but then green would have a little more precision, and
    # that weirds me out.
    bit_depth=5,
    rgb_pins=[
        board.MTX_R1,
        board.MTX_G1,
        board.MTX_B1,
        board.MTX_R2,
        board.MTX_G2,
        board.MTX_B2
    ],
    addr_pins=[
        board.MTX_ADDRA,
        board.MTX_ADDRB,
        board.MTX_ADDRC,
        board.MTX_ADDRD
    ],
    clock_pin=board.MTX_CLK,
    latch_pin=board.MTX_LAT,
    output_enable_pin=board.MTX_OE
)

output_buffer = memoryview(matrix)

matrix.refresh()

x_cells = 6
y_cells = 6

perlin_field = perlin.PerlinField(
    x_cells=x_cells,
    y_cells=x_cells,
)

print("Filling buffer...")

z = 0.0

while True:
    start_time = time.monotonic()
    perlin_field.set_z(z)

    for x in range(WIDTH):
        for y in range(HEIGHT):
            global_position = np.array([
                x/WIDTH * x_cells,
                y/HEIGHT * y_cells,
            ])

            value = perlin_field.get(global_position)

            # Gamma correction 2.8 stolen from:
            # https://learn.adafruit.com/led-tricks-gamma-correction/the-longer-fix
            gamma = 2.8
            red_value = int(round(0b11111 * (value**gamma)))
            pixel_value = red_value << 6 << 5

            output_buffer[WIDTH*y + x] = pixel_value

    print("Pushing to matrix...")
    matrix.refresh()
    print(f"Completed frame in {time.monotonic()-start_time} seconds.")

    z += 0.2

print("Done. Entering busy loop.")
while True:
    pass



