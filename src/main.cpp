#include "camera.hpp"
#include "color.hpp"
#include "hittable.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "utils.hpp"
#include "vec3.hpp"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

int main(int argc, char const *argv[])
{
    HittableList world;

    world.add(std::make_shared<Sphere>(Point3(0, 0, -1), 0.5));
    world.add(std::make_shared<Sphere>(Point3(0, -100.5, -1), 100));

    Camera cam;

    cam.m_aspect_ratio = 16.0 / 9.0;
    cam.m_image_width = 1280;

    cam.render(world);
}
