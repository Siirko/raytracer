#include "camera.hpp"
#include "material.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <future>
#include <mutex>
#include <stb_image_write.h>

Color Camera::ray_color(const Ray &r, int depth, const Hittable &world) const
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
    }

    Vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    // blendedValue = (1−a) * startValue + a * endValue
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

void Camera::init()
{
    m_image_height = int(m_image_width / m_aspect_ratio);
    m_image_height = (m_image_height < 1) ? 1 : m_image_height;

    m_pixel_samples_scale = 1.0 / m_samples_per_pixel;

    m_center = m_lookfrom;

    // Determine viewport dimensions.
    auto theta = utils::degrees_to_radians(m_vfov);
    auto h = std::tan(theta / 2);
    auto viewport_height = 2.0 * h * m_focus_dist;
    auto viewport_width = viewport_height * (double(m_image_width) / m_image_height);

    // Calculate the camera basis vectors
    m_w = unit_vector(m_lookfrom - m_lookat);
    m_u = unit_vector(cross(m_vup, m_w));
    m_v = cross(m_w, m_u);
    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    auto viewport_u = viewport_width * m_u;
    auto viewport_v = viewport_height * -m_v;

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    m_pixel_delta_u = viewport_u / m_image_width;
    m_pixel_delta_v = viewport_v / m_image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = m_center - (m_focus_dist * m_w) - viewport_u / 2 - viewport_v / 2;
    m_pixel00_loc = viewport_upper_left + 0.5 * (m_pixel_delta_u + m_pixel_delta_v);

    auto dof_radius = m_focus_dist * std::tan(utils::degrees_to_radians(m_dof_angle) / 2);
    m_dof_disk_u = dof_radius * m_u;
    m_dof_disk_v = dof_radius * m_v;
}

Point3 Camera::dof_disk_sample() const
{
    // Returns a random point in the camera defocus disk.
    auto p = random_in_unit_disk();
    return m_center + (p[0] * m_dof_disk_u) + (p[1] * m_dof_disk_v);
}

Ray Camera::get_ray(int i, int j) const
{

    auto offset = sample_square();
    auto pixel_sample = m_pixel00_loc + ((i + offset.x()) * m_pixel_delta_u) + ((j + offset.y()) * m_pixel_delta_v);

    auto ray_origin = (m_dof_angle <= 0) ? m_center : dof_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;
    return Ray(ray_origin, ray_direction);
}

Vec3 Camera::sample_square() const { return Vec3(utils::random_double() - .5, utils::random_double() - .5, 0); }

void Task::render_block(const Task::block &b, const Hittable &world, std::vector<unsigned char> &image_data,
                        int channels, const Camera &cam)
{
    for (int j = b.y0; j < b.y1; j++)
    {
        // progress[thread_id] = int(100 * (j - b.y0) / (b.y1 - b.y0));
        for (int i = b.x0; i < b.x1; i++)
        {
            Color pixel_color(0, 0, 0);
            for (int s = 0; s < cam.m_samples_per_pixel; s++)
            {
                Ray r = cam.get_ray(i, j);
                pixel_color += cam.ray_color(r, cam.m_max_depth, world);
            }
            auto c = Color::prepare_color(pixel_color * cam.m_pixel_samples_scale);
            int index = (j * cam.m_image_width + i) * channels;
            image_data[index] = c.x();
            image_data[index + 1] = c.y();
            image_data[index + 2] = c.z();
        }
    }
}

std::vector<Task::block> Camera::create_tasks(int width, int height, int block_size)
{
    std::vector<Task::block> blocks;
    for (int j = 0; j < height; j += block_size)
    {
        for (int i = 0; i < width; i += block_size)
        {
            Task::block b;
            b.x0 = i;
            b.y0 = j;
            b.x1 = std::min(i + block_size, width);
            b.y1 = std::min(j + block_size, height);
            blocks.push_back(b);
        }
    }
    return blocks;
}
void Camera::render(const Hittable &world)
{
    init();

    // According to image dimension (w*h) do blocks for threads
    int block_size = 128;
    std::vector<Task::block> blocks = create_tasks(m_image_width, m_image_height, block_size);
    // Image dimensions and parameters
    const int channels = 3;
    std::vector<unsigned char> image_data(m_image_width * m_image_height * channels);
    std::vector<std::future<void>> futures;
    for (auto &s : blocks)
        futures.push_back(std::async(std::launch::async, &Task::render_block, Task(), s, std::ref(world),
                                     std::ref(image_data), channels, std::ref(*this)));

    while (!futures.empty())
    {
        std::clog << "\rBlocks remaining: " << futures.size() << ' ' << std::flush;
        futures.back().get();
        futures.pop_back();
    }

    std::clog << "\rDone.                 \n";
    stbi_write_png("image.png", m_image_width, m_image_height, channels, image_data.data(), m_image_width * channels);
}