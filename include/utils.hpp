#pragma once
#include <cmath>

namespace utils
{
const double infinity = __DBL_MAX__;

inline double degrees_to_radians(double degrees) { return degrees * M_PI / 180.0; }
} // namespace utils