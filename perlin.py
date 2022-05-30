import math

from ulab import numpy as np

import xxhash


_CORNER_OFFSETS = np.array(
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


_PERLIN_3D_OUTPUT_MIN = math.sqrt(3/4)
_PERLIN_3D_OUTPUT_MAX = -math.sqrt(3/4)


class GradientGrid:
    def __init__(self, x_size, y_size, seed):
        self._seed = seed
        self._x_size = x_size
        self._y_size = y_size

    def get_gradient(self, x, y, z) -> float:
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
        self._gradient_grid = GradientGrid(
            x_size = x_cells + 1,
            y_size = y_cells + 1,
            seed = seed,
        )

    def set_z(self, z) -> None:
        self._z = z
        z_floored = int(np.floor(z))
        self._z_position_in_cell = z - z_floored
        self._low_z_grid = self._get_gradient_slice(z_index=z_floored)
        self._high_z_grid = self._get_gradient_slice(z_index=z_floored+1)

    def get(self, pos) -> float:
        """
        x and y are floats [0, 1].
        z is a float >= 0.
        """
        pos = np.array([pos[0], pos[1], self._z], dtype=np.float)

        pos_floored = np.array(np.floor(pos), dtype=np.uint16)

        position_in_cell = pos - pos_floored

        gradients = [
            self._low_z_grid[ pos_floored[0]+0][pos_floored[1]+0],
            self._high_z_grid[pos_floored[0]+0][pos_floored[1]+0],
            self._low_z_grid[ pos_floored[0]+0][pos_floored[1]+1],
            self._high_z_grid[pos_floored[0]+0][pos_floored[1]+1],
            self._low_z_grid[ pos_floored[0]+1][pos_floored[1]+0],
            self._high_z_grid[pos_floored[0]+1][pos_floored[1]+0],
            self._low_z_grid[ pos_floored[0]+1][pos_floored[1]+1],
            self._high_z_grid[pos_floored[0]+1][pos_floored[1]+1],
        ]

        offsets = _CORNER_OFFSETS - position_in_cell
        dot_products = np.sum(gradients*offsets, axis=1)

        interpolated_value = _interp_3d(
            dot_products,
            position_in_cell,
        )

        return (interpolated_value - _PERLIN_3D_OUTPUT_MIN)/(_PERLIN_3D_OUTPUT_MAX-_PERLIN_3D_OUTPUT_MIN)

    def _get_gradient_slice(self, z_index: int) -> np.ndarray:
        # Returns a 2D array of 3D gradient vectors.
        x_size = self._x_cells + 1
        y_size = self._y_cells + 1
        result = [
            [
                np.array(self._gradient_grid.get_gradient(x, y, z_index), dtype=np.float)
                for y in range(y_size)
            ]
            for x in range(x_size)
        ]
        return result

def _smoothstep(values: np.ndarray) -> np.ndarray:
    # Assume each value is in [0, 1].
    return values*values*(3 - 2*values);


def _lerp(from_values: np.ndarray, to_values: np.ndarray, pos: float) -> np.ndarray:
    # Assume pos is in [0, 1].
    return from_values + pos*(to_values-from_values)


def _interp_3d(
    corner_values: np.ndarray,
    pos: np.ndarray,
) -> float:
    # I have no idea if this is actually an okay way to do interpolation
    # or even what this is doing.
    #
    # Stolen from:
    # https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/perlin-noise-part-2

    uvw = _smoothstep(pos)

    low_x_values = corner_values[:4]
    high_x_values = corner_values[4:]

    x_interpolated_values = _lerp(low_x_values, high_x_values, uvw[0])

    low_y_values = x_interpolated_values[:2]
    high_y_values = x_interpolated_values[2:]
    xy_interpolated_values = _lerp(low_y_values, high_y_values, uvw[1])

    low_z_value = xy_interpolated_values[:1]
    high_z_value = xy_interpolated_values[1:]
    xyz_interpolated_value = _lerp(low_z_value, high_z_value, uvw[2])

    return xyz_interpolated_value[0]
