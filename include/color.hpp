#pragma once

#include "vec3.hpp"

class Color : public Vec3
{
  public:
    Color() : Vec3() {}
    Color(double r, double g, double b) : Vec3(r, g, b) {}
    Color(uint64_t hex) : Vec3(((hex >> 16) & 0xFF) / 255.0, ((hex >> 8) & 0xFF) / 255.0, (hex & 0xFF) / 255.0) {}
    void write_color(std::ostream &out)
    {
        auto r = this->x();
        auto g = this->y();
        auto b = this->z();

        // Translate the [0,1] component values to the byte range [0,255].
        int rbyte = int(255.999 * r);
        int gbyte = int(255.999 * g);
        int bbyte = int(255.999 * b);

        // Write out the pixel color components.
        out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
    }
};