#pragma once

#include "hittable.hpp"
#include "ray.hpp"
#include "utils.hpp"

struct hit_record_t;
class Material
{
  public:
    virtual ~Material() = default;
    virtual bool scatter(const Ray &ray, const hit_record_t &hit, Color &attenuation, Ray &scattered) const = 0;
};

class Lambertian : public Material
{
  private:
    Color m_albedo;

  public:
    Lambertian(const Color &albedo) : m_albedo(albedo) {}
    bool scatter(const Ray &ray, const hit_record_t &hit, Color &attenuation, Ray &scattered) const override
    {
        Vec3 scatter_direction = hit.normal + random_unit_vector();
        if (scatter_direction.near_zero())
            scatter_direction = hit.normal;

        scattered = Ray(hit.p, scatter_direction);
        attenuation = m_albedo;
        return true;
    }
};

class Metal : public Material
{
  private:
    Color m_albedo;
    double m_fuzz;

  public:
    Metal(const Color &albedo, double fuzz) : m_albedo(albedo), m_fuzz(fuzz < 1 ? fuzz : 1) {}
    bool scatter(const Ray &ray, const hit_record_t &hit, Color &attenuation, Ray &scattered) const override
    {
        Vec3 reflected = reflect(unit_vector(ray.direction()), hit.normal);
        scattered = Ray(hit.p, reflected + m_fuzz * random_in_unit_sphere());
        attenuation = m_albedo;
        return dot(scattered.direction(), hit.normal) > 0;
    }
};