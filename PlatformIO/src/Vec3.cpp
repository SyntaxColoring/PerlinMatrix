#include <cmath>
#include "Vec3.hpp"

float Vec3::norm() const
{
  return std::sqrt(x*x + y*y + z*z);
}

Vec3 Vec3::operator/(const float divisor) const {
  return Vec3{x/divisor, y/divisor, z/divisor};
}

Vec3 Vec3::operator-(const Vec3 other) const {
  return Vec3{x-other.x, y-other.y, z-other.z};
}

float Vec3::dot(const Vec3 other) const {
  return x*other.x + y*other.y + z*other.z;
}
