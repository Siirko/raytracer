#pragma once
#include "hittable.hpp"
#include "vec3.hpp"

class Sphere : public Hittable
{
  private:
    Point3 m_center;
    double m_radius;

  public:
    Sphere(const Point3 &center, double radius) : m_center(center), m_radius(fmax(0, radius)) {}

    bool hit(const Ray &r, Interval ray_int, hit_record_t &rec) const override
    {
        Vec3 oc = m_center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - m_radius * m_radius;

        auto discriminant = h * h - a * c;
        if (discriminant < 0)
            return false;

        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (!ray_int.surrounds(root))
        {
            root = (h + sqrtd) / a;
            if (!ray_int.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        Vec3 outward_normal = (rec.p - m_center) / m_radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }
};