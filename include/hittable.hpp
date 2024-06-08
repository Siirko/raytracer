#pragma once
#include "interval.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include <memory>
#include <vector>

class Material;
struct hit_record_t
{
    Point3 p;
    Vec3 normal;
    std::shared_ptr<Material> mat;
    double t;
    bool front_face;
    void set_face_normal(const Ray &r, const Vec3 &outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable
{
  public:
    virtual ~Hittable() = default;
    virtual bool hit(const Ray &r, Interval ray_int, hit_record_t &rec) const = 0;
};

class HittableList : public Hittable
{
  private:
    std::vector<std::shared_ptr<Hittable>> m_objects;

  public:
    HittableList() {}
    HittableList(std::shared_ptr<Hittable> object) { add(object); }

    void clear() { m_objects.clear(); }
    void add(std::shared_ptr<Hittable> object) { m_objects.push_back(object); }

    virtual bool hit(const Ray &r, Interval ray_int, hit_record_t &rec) const override
    {
        hit_record_t temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_int.max;

        for (const auto &object : m_objects)
        {
            if (object->hit(r, Interval(ray_int.min, closest_so_far), temp_rec))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
};