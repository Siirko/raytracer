#pragma once
#include <cmath>
#include <random>

namespace utils
{
const double infinity = __DBL_MAX__;

inline double degrees_to_radians(double degrees) { return degrees * M_PI / 180.0; }

inline double random_double(double min = 0.0, double max = 1.0)
{
    static std::uniform_real_distribution<double> distribution(min, max);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_gaussian(double min = 0.0, double max = 1.0)
{
    static std::normal_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}
} // namespace utils