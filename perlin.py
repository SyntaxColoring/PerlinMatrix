import math

from ulab import numpy as np

import xxhash


INDEX_OFFSETS = np.array(
    [
        [0, 0, 0],
        [0, 0, 1],
        [0, 1, 0],
        [0, 1, 1],
        [1, 0, 0],
        [1, 0, 1],
        [1, 1, 0],
        [1, 1, 1],
    ],
    dtype=np.uint8,
)

class VectorGrid:
    def __init__(self, x_size, y_size, seed):
        self._seed = seed
        self._x_size = x_size
        self._y_size = y_size

    def get_vector(self, x, y, z):
        """
        x is an int [0, x_size).
        y is an int [0, y_size).
        z is an int >= 0.
        """
        grid_point_id = (
            (self._x_size * self._y_size) * z
            +
            self._x_size * y
            +
            x
        )
        grid_point_hash = xxhash.xxh32_int32(seed=self._seed, input=grid_point_id)
        return self._gradient_from_hash(grid_point_hash)

    @staticmethod
    def _gradient_from_hash(int32_hash):
        # To do:
        #
        # This doesn't give a proper uniform distribution.
        #
        # Look into Ken Perlin's simplex noise patent (recently expired!)
        # and check out his bit manipulation.
        #
        # Or, use a precomputed table of some sort.
        #
        # Or, do something with Gaussian distributions to generate it better.

        ten_bits = 0b11_1111_1111

        # [0.0, 1.0]
        x = ((int32_hash >> 10*0) & ten_bits) / ten_bits
        y = ((int32_hash >> 10*1) & ten_bits) / ten_bits
        z = ((int32_hash >> 10*2) & ten_bits) / ten_bits

        # [-1.0, 1.0]
        x = x*2-1
        y = y*2-1
        z = z*2-1

        norm = math.sqrt(x**2 + y**2 + z**2)
        return (x/norm, y/norm, z/norm)


class PerlinField:
    def __init__(self, x_cells, y_cells, seed=0):
        self._x_cells = x_cells
        self._y_cells = y_cells
        self._vector_grid = VectorGrid(
            x_size = x_cells + 1,
            y_size = y_cells + 1,
            seed = seed,
        )

    def get(self, pos) -> float:
        """
        x and y are floats [0, 1].
        z is a float >= 0.
        """

        pos_floored = np.floor(pos)

        position_in_cell = pos - pos_floored

        index_offsets = [
            [0, 0, 0],
            [0, 0, 1],
            [0, 1, 0],
            [0, 1, 1],
            [1, 0, 0],
            [1, 0, 1],
            [1, 1, 0],
            [1, 1, 1],
        ]

        grid_coords_to_sample = pos_floored + index_offsets

        gradients = [
            self._vector_grid.get_vector(
                # Rely on our floats being small enough to be losslessly
                # converted to int.
                int(pos_floored[0]) + x_index_offset,
                int(pos_floored[1]) + y_index_offset,
                int(pos_floored[2]) + z_index_offset,
            )
            for x_index_offset, y_index_offset, z_index_offset in index_offsets
        ]

        corner_positions = [
            [0.0, 0.0, 0.0],
            [0.0, 0.0, 1.0],
            [0.0, 1.0, 0.0],
            [0.0, 1.0, 1.0],
            [1.0, 0.0, 0.0],
            [1.0, 0.0, 1.0],
            [1.0, 1.0, 0.0],
            [1.0, 1.0, 1.0],
        ]

        offsets = corner_positions - position_in_cell
        dot_products = np.sum(gradients*offsets, axis=1)

        interp = _interp_3d(
            dot_products[0],
            dot_products[1],
            dot_products[2],
            dot_products[3],
            dot_products[4],
            dot_products[5],
            dot_products[6],
            dot_products[7],
            position_in_cell[0],
            position_in_cell[1],
            position_in_cell[2],
        )

        dimensions = 3

        max = math.sqrt(dimensions/4)
        min = -math.sqrt(dimensions/4)

        return (interp - min)/(max-min)


def _smoothstep(x: float) -> float:
    # Assume x is in [0, 1].
    return x*x*(3 - 2*x);


def _lerp(a: float, b: float, x: float) -> float:
    return a + x*(b-a)


def _interp_3d(
    val_000: float,
    val_001: float,
    val_010: float,
    val_011: float,
    val_100: float,
    val_101: float,
    val_110: float,
    val_111: float,
    x: float,
    y: float,
    z: float,
) -> float:
    # I have no idea if this is actually an okay way to do interpolation
    # or even what this is doing. Stolen from:
    # https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/perlin-noise-part-2
    u = _smoothstep(x)
    v = _smoothstep(y)
    w = _smoothstep(z)

    a = _lerp(val_000, val_100, u)
    b = _lerp(val_010, val_110, u)
    c = _lerp(val_001, val_101, u)
    d = _lerp(val_011, val_111, u)

    e = _lerp(a, b, v)
    f = _lerp(c, d, v)

    return _lerp(e, f, w)
