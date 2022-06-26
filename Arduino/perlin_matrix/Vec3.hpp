#ifndef VEC3_HPP_INCLUDED
#define VEC3_HPP_INCLUDED

struct Vec3
{
  float x;
  float y;
  float z;

  float norm() const;

  Vec3 operator/(const float divisor) const;
  Vec3 operator-(const Vec3 other) const;
  float dot(const Vec3 other) const;
};

#endif
