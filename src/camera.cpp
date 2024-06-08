#include "camera.hpp"
#include "material.hpp"

Color Camera::ray_color(const Ray &r, int depth, const Hittable &world)
{
    if (depth <= 0)
        return Color(0, 0, 0);

    hit_record_t rec;
    if (world.hit(r, Interval(0.001, utils::infinity), rec))
    {
        Ray scattered;
        Color attenuation;
        if (rec.mat->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, depth - 1, world);
        return Color(0, 0, 0);
        // Vec3 direction = rec.normal + random_unit_vector();
        // return 0.5 * ray_color(Ray(rec.p, direction), depth - 1, world);
    }

    Vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    // blendedValue = (1−a) * startValue + a * endValue
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

void Camera::init()
{
    image_height = int(m_image_width / m_aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    pixel_samples_scale = 1.0 / m_samples_per_pixel;

    center = Point3(0, 0, 0);

    // Determine viewport dimensions.
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(m_image_width) / image_height);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    auto viewport_u = Vec3(viewport_width, 0, 0);
    auto viewport_v = Vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    pixel_delta_u = viewport_u / m_image_width;
    pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = center - Vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
}

Ray Camera::get_ray(int i, int j) const
{

    auto offset = sample_square();
    auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

    auto ray_origin = center;
    auto ray_direction = pixel_sample - ray_origin;
    return Ray(ray_origin, ray_direction);
}

Vec3 Camera::sample_square() const { return Vec3(utils::random_double() - .5, utils::random_double() - .5, 0); }

void Camera::render(const Hittable &world)
{
    init();

    std::cout << "P3\n" << m_image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++)
    {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < m_image_width; i++)
        {
            Color pixel_color(0, 0, 0);
            for (int s = 0; s < m_samples_per_pixel; s++)
            {
                Ray r = get_ray(i, j);
                pixel_color += ray_color(r, m_max_depth, world);
            }
            Color::write_color(std::cout, pixel_color * pixel_samples_scale);
        }
    }

    std::clog << "\rDone.                 \n";
}