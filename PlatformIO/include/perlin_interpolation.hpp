#ifndef PERLIN_INTERPOLATION_HTTP_INCLUDED
#define PERLIN_INTERPOLATION_HTTP_INCLUDED

#include "Vec3.hpp"

float interpolate_perlin(
  Vec3 gradients_at_cell_corners[2][2][2], // [x][y][z] order.
  Vec3 local_position
);

#endif
