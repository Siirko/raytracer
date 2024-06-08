#pragma once

#include "interval.hpp"
#include "vec3.hpp"

class Color : public Vec3
{
  public:
    Color() : Vec3() {}
    Color(double r, double g, double b) : Vec3(r, g, b) {}
    Color(uint64_t hex) : Vec3(((hex >> 16) & 0xFF) / 255.0, ((hex >> 8) & 0xFF) / 255.0, (hex & 0xFF) / 255.0) {}
    static double linear_to_gamma(double linear_component)
    {
        if (linear_component > 0)
            return std::sqrt(linear_component);

        return 0;
    }
    static Color prepare_color(const Color &pixel_color)
    {
        auto r = pixel_color.x();
        auto g = pixel_color.y();
        auto b = pixel_color.z();

        r = linear_to_gamma(r);
        g = linear_to_gamma(g);
        b = linear_to_gamma(b);

        // Translate the [0,1] component values to the byte range [0,255].
        static const Interval intensity(0.0, 0.999);
        int rbyte = int(256 * intensity.clamp(r));
        int gbyte = int(256 * intensity.clamp(g));
        int bbyte = int(256 * intensity.clamp(b));

        return Color(rbyte, gbyte, bbyte);
    }
};