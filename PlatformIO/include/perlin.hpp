#ifndef PERLIN_HPP_INCLUDED
#define PERLIN_HPP_INCLUDED

#include <cmath>
#include <cstdint>
#include "perlin_gradient_generator.hpp"
#include "perlin_interpolation.hpp"
#include "Vec3.hpp"

template <unsigned int XCells, unsigned int YCells>
class PerlinSlice2D
{
	// Vec3 gradients_[XCells][YCells][2];
	GradientGenerator gradient_generator_;
	float z_;

	public:

	PerlinSlice2D(const float z) // z in [0, +inf), cell boundaries at every integer
	: gradient_generator_(GradientGenerator(XCells, YCells)), z_(z)
	{
		// for (unsigned int gradient_x = 0; gradient_x < XCells; gradient_x++)
		// {
		// 	for (unsigned int gradient_y = 0; gradient_y < YCells; gradient_y++)
		// 	{
		// 		for (unsigned int gradient_z = 0; gradient_z < 2; gradient_z++)
		// 		{
		// 			const Vec3 gradient = gradient_generator.generate(lower_z+gradient_z, gradient_x, gradient_y);
		// 			gradients_[gradient_x][gradient_y][gradient_z] = gradient;
		// 		}
		// 	}
		// }
	}

	// x and y in [0, 1].
	// Returns a value in [0, 1].
	float value_at(const float x, const float y) const
	{
		const unsigned int lower_z = unsigned(z_);
		const float local_z = z_ - lower_z;

		const float scaled_x = x * XCells;
		const float scaled_y = y * YCells;

		const auto low_x = (unsigned int)(scaled_x);
		const auto low_y = (unsigned int)(scaled_y);

		Vec3 gradients_at_corners[2][2][2];
		for (unsigned int x_face = 0; x_face < 2; x_face++)
		{
			for (unsigned int y_face = 0; y_face < 2; y_face++)
			{
				for (unsigned int z_face = 0; z_face < 2; z_face++)
				{
					const unsigned int gradient_x = low_x + x_face;
					const unsigned int gradient_y = low_y + y_face;
					const unsigned int gradient_z = lower_z + z_face;

					const Vec3 gradient = gradient_generator_.generate(gradient_z, gradient_x, gradient_y);

					// const auto& gradient = gradients_[low_x+x_face][low_y+y_face][z_face];
					gradients_at_corners[x_face][y_face][z_face] = gradient;
				}
			}
		}

		const float local_x = scaled_x - low_x;
		const float local_y = scaled_y - low_y;
		const float perlin_value = interpolate_perlin(gradients_at_corners, Vec3{local_x, local_y, local_z});
		return perlin_value;
	}
};

#endif
