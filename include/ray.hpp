#pragma once
#include "vec3.hpp"
class Ray
{
  private:
    Point3 m_orig;
    Vec3 m_dir;

  public:
    Ray() {}
    Ray(const Point3 &orig, const Vec3 &dir) : m_orig(orig), m_dir(dir) {}

    Point3 origin() const { return m_orig; }
    Vec3 direction() const { return m_dir; }

    Point3 at(double t) const { return m_orig + t * m_dir; }
};