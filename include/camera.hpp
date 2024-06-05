#pragma once
#include "color.hpp"
#include "hittable.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include <iostream>

class Camera
{
  private:
    int image_height;           // Rendered image height
    Point3 center;              // Camera center
    Point3 pixel00_loc;         // Location of pixel 0, 0
    Vec3 pixel_delta_u;         // Offset to pixel to the right
    Vec3 pixel_delta_v;         // Offset to pixel below
    double pixel_samples_scale; // Color scale factor for a sum of pixel samples

    Ray get_ray(int i, int j) const;
    Vec3 sample_square() const;
    Color ray_color(const Ray &r, const Hittable &world);

    void init();

  public:
    double m_aspect_ratio = 16.0 / 9.0;
    int m_image_width = 1280;
    int m_samples_per_pixel = 10; // Count of random samples for each pixel

    void render(const Hittable &world);
};