#include "Vec3.hpp"
#include "xxh32.hpp"

#include "perlin_gradient_generator.hpp"

GradientGenerator::GradientGenerator(const unsigned int row_count, const unsigned int column_count)
: row_count_(row_count), column_count_(column_count)
{ }

Vec3 GradientGenerator::generate(const unsigned int slice, const unsigned int row, const unsigned int column) const
{
  uint32_t hash = hash_location(slice, row, column);

  // To do:
  //
  // This doesn't give a proper uniform distribution.
  //
  // Look into Ken Perlin's simplex noise patent (recently expired!)
  // and check out his bit manipulation.
  //
  // Or, use a precomputed table of some sort.
  //
  // Or, do something with Gaussian distributions to generate it better.

  const uint32_t ten_bits = 0b1111111111;

  // [0.0, 1.0]
  Vec3 result = {
   float((hash >> 10*0) & ten_bits) / ten_bits,
   float((hash >> 10*1) & ten_bits) / ten_bits,
   float((hash >> 10*2) & ten_bits) / ten_bits
  };

  // [-1.0, 1.0]
  result.x = result.x*2-1;
  result.y = result.y*2-1;
  result.z = result.z*2-1;

  return result / result.norm();
}

unsigned int GradientGenerator::hash_location(const unsigned int slice, const unsigned int row, const unsigned int column) const
{
  uint32_t index = row_count_*column_count_*slice + column_count_*row + column;
  return xxh_32(index, 0);
}
