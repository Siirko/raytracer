#pragma once
#include "color.hpp"
#include "hittable.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include <cmath>
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
    Color ray_color(const Ray &r, int depth, const Hittable &world);

    struct task_t
    {
        struct block
        {
            int x0, y0, x1, y1;
        } first, second;
    };

    std::vector<task_t::block> create_tasks(int width, int height, int block_size);
    void render_block(std::mutex &mtx, const task_t::block &b, const Hittable &world,
                      std::vector<unsigned char> &image_data, int channels);

    void init();

  public:
    double m_aspect_ratio = 16.0 / 9.0;
    int m_image_width = 1280;
    int m_samples_per_pixel = 10; // Count of random samples for each pixel
    int m_max_depth = 10;         // Maximum depth of ray recursion

    void render(const Hittable &world);
};