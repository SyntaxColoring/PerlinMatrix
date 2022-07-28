#ifndef PERLIN_GRADIENT_GENERATOR_HPP_INCLUDED
#define PERLIN_GRADIENT_GENERATOR_HPP_INCLUDED

#include "Vec3.hpp"

class GradientGenerator
{
  unsigned int row_count_;
  unsigned int column_count_;

  unsigned int hash_location(const unsigned int slice, const unsigned int row, const unsigned int column) const;

  public:

  GradientGenerator(const unsigned int row_count, const unsigned int column_count);

  Vec3 generate(const unsigned int slice, const unsigned int row, const unsigned int column) const;
};

#endif
