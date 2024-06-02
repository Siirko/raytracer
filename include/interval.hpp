#pragma once

#include "utils.hpp"

class Interval
{
  public:
    double min, max;

    Interval() : min(+utils::infinity), max(-utils::infinity) {} // Default interval is empty
    Interval(double min, double max) : min(min), max(max) {}
    double size() const { return max - min; }
    bool contains(double x) const { return min <= x && x <= max; }
    bool surrounds(double x) const { return min < x && x < max; }
    static const Interval empty, universe;
};

const Interval Interval::empty = Interval(+utils::infinity, -utils::infinity);
const Interval Interval::universe = Interval(-utils::infinity, +utils::infinity);