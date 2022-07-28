#include <cmath>

#include <Arduino.h>

#include "Vec3.hpp"
#include "perlin_interpolation.hpp"

namespace
{
	// https://digitalfreepen.com/2017/06/20/range-perlin-noise.html
	const float PERLIN_3D_OUTPUT_MIN = -std::sqrt(3.0/4.0);
	const float PERLIN_3D_OUTPUT_MAX = std::sqrt(3.0/4.0);

	float smoothstep(const float x)
	{
		if (x < 0) return 0;
		else if (x > 1) return 1;
		else return x*x*(3 - 2*x);
	}

	float lerp(const float a, const float b, const float x)
	{
		return a + (b-a)*x;
	}

	float interpolate_3d(
		const float values_at_cell_corners[2][2][2], // [x][y][z] order.
		const Vec3 local_position
	)
	{
		const float u = smoothstep(local_position.x);
		const float v = smoothstep(local_position.y);
		const float w = smoothstep(local_position.z);

		// linear interpolation
		float a = lerp(values_at_cell_corners[0][0][0], values_at_cell_corners[1][0][0], u);
		float b = lerp(values_at_cell_corners[0][1][0], values_at_cell_corners[1][1][0], u);
		float c = lerp(values_at_cell_corners[0][0][1], values_at_cell_corners[1][0][1], u);
		float d = lerp(values_at_cell_corners[0][1][1], values_at_cell_corners[1][1][1], u);

		float e = lerp(a, b, v);
		float f = lerp(c, d, v);

		return lerp(e, f, w);  //g
	}
}

float interpolate_perlin(
	Vec3 gradients_at_cell_corners[2][2][2], // [x][y][z] order.
	Vec3 local_position
)
{
	float dot_products[2][2][2];

	for (unsigned int x = 0; x < 2; x++)
	{
		for (unsigned int y = 0; y < 2; y++)
		{
			for (unsigned int z = 0; z < 2; z++)
			{
				const Vec3 offset_to_corner = Vec3{float(x), float(y), float(z)} - local_position;
				const Vec3 gradient_at_corner = gradients_at_cell_corners[x][y][z];
				const float dot_product = gradient_at_corner.dot(offset_to_corner);
				dot_products[x][y][z] = dot_product;
			}
		}
	}

	float interpolated_value = interpolate_3d(dot_products, local_position);
	return (interpolated_value - PERLIN_3D_OUTPUT_MIN)/(PERLIN_3D_OUTPUT_MAX-PERLIN_3D_OUTPUT_MIN);
}
