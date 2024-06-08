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

class Dielectric : public Material
{
  private:
    double m_refraction_index;
    double reflectance(double cosine, double refraction_ratio) const
    {
        // Use Schlick's approximation for reflectance.
        double r0 = (1 - refraction_ratio) / (1 + refraction_ratio);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }

  public:
    Dielectric(double refraction_index) : m_refraction_index(refraction_index) {}
    bool scatter(const Ray &ray, const hit_record_t &hit, Color &attenuation, Ray &scattered) const override
    {
        attenuation = Color(1.0, 1.0, 1.0);
        double refraction_ratio = hit.front_face ? (1.0 / m_refraction_index) : m_refraction_index;

        Vec3 unit_direction = unit_vector(ray.direction());
        double cos_theta = fmin(dot(-unit_direction, hit.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > utils::random_double())
            direction = reflect(unit_direction, hit.normal);
        else
            direction = refract(unit_direction, hit.normal, refraction_ratio);

        scattered = Ray(hit.p, direction);
        return true;
    }
};