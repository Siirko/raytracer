#include "vec3.hpp"
#include "color.hpp"

Vec3::operator Color() const { return Color(e[0], e[1], e[2]); }