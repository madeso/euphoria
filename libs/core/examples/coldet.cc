#include "core/intersection.h"
#include "core/dump.h"
#include "core/numeric.h"
#include "core/colors.h"
#include "core/ray.h"
#include "core/plane.h"
#include "core/sphere.h"

#include <vector>
#include <string>
#include <iostream>

using namespace euphoria::core;
using namespace euphoria::core::dump3d;

struct plane_demo
{
    const euphoria::core::plane plane = euphoria::core::plane::from_points
    (vec3f(5, 6, 7), vec3f(6, 5, 4), vec3f(1, 2, 3));
    const std::vector<vec3f> points =
    {
        vec3f(0.0f, 0.0f, 0.0f),
        vec3f(2.0f, 6.0f, 1.0f),
        vec3f(3.0f, 1.0f, -3.0f),
        vec3f(-2.0f, -1.0f, 2.0f),
        vec3f(7.0f, 7.0f, -7.0f),
        vec3f(3.0f, 2.0f, -1.0f),
        vec3f(10.0f, 3.0f, -10.0f),
        vec3f(2.0f, 8.0f, -7.0f),
        vec3f(-1.632993f, 3.265986f, -1.632993f),
        vec3f(-1.020621f, 3.265986f, -2.245366f)
    };

    void
    distance_to_plane() const
    {
        auto d = dumper{ "coldet-distance-to-plane.html" };

        d.add_plane(plane, color::white);
        d.add_arrow(ray3f(vec3f::zero(), plane.normal), color::green);
        for (const auto& p : points)
        {
            const auto dist = DistanceBetween(plane, p);
            d.add_sphere(p, 0.1f, Abs(dist) < 0.01f ? color::yellow : (dist < 0 ? color::black : color::white));
        }
    }

    void
    point_to_plane() const
    {
        auto d = dumper{ "coldet-point-to-plane.html" };

        d.add_plane(plane, color::white);
        d.add_arrow(ray3f(vec3f::zero(), plane.normal), color::green);
        for (const auto& p : points)
        {
            const auto dist = DistanceBetween(plane, p);
            if (Abs(dist) < 0.01f) { continue; }

            const auto pp = ClosestPoint(plane, p);
            d.add_arrow(ray3f::from_to(p, pp), color::black);
        }
    }
};

struct ray_demo
{
    const ray3f ray = {vec3f(-3, -2, -1), vec3f(3, 2, 1)};
    const std::vector<vec3f> points =
    {
        vec3f(-3, -2, -1),
        vec3f(12, 8, 4),
        vec3f(0, 0, 0),
        vec3f(-18, -12, -6),
        vec3f(-4, -7, -8),
        vec3f(7, 8, 5),
        vec3f(1, 5, -5),
        vec3f(-6, 5, 7),
        vec3f(1, 6, 8),
        vec3f(-7, -10, -4),
        vec3f(-4.5f, -3.0f, -1.5f)
    };

    void
    point_on_ray() const
    {
        auto d = dumper{ "coldet-point-on-ray.html" };

        d.add_grid();
        d.add_arrow(ray, color::black);

        for (const auto p : points)
        {
            const auto dist = DistanceBetween(ray.get_normalized(), p);
            d.add_sphere(p, 0.1f, dist < 0.001f ? color::white : color::black);
        }
    }

    void
    closest_point_on_ray() const
    {
        auto d = dumper{ "coldet-closest-point-on-ray.html" };

        d.add_grid();
        d.add_arrow(ray, color::black);

        for (const auto p : points)
        {
            const auto dist = DistanceBetween(ray.get_normalized(), p);
            if (dist < 0.001f) { continue; }

            const auto pp = ClosestPoint(ray.get_normalized(), p);
            d.add_arrow(ray3f::from_to(p, pp), color::red);
        }
    }

};

void
ray_sphere()
{
    const auto test = []
    (
        float rayX, float rayY, float rayZ,
        float normX, float normY, float normZ,
        float sphereX, float sphereY, float sphereZ,
        float rad,
        bool res
    )
    {
        // todo(Gustav): add scene support to dumper...
        const auto ray = ray3f(vec3f(rayX, rayY, rayZ), vec3f(normX, normY, normZ).get_normalized());
        const auto sphere = ::sphere{rad};
        const auto sphere_center = vec3f(sphereX, sphereY, sphereZ);
        const auto collision = GetIntersection(ray.get_normalized(), sphere, sphere_center);
        const auto collided = collision >= 0.0f;
        if(collided != res)
        {
            std::cout
                << "Collision was " << ( res ? "expected" : "_not_ expected")
                << ", but ";
            if(collided)
            {
                std::cout << "occured at: " << collision;
            }
            else
            {
                std::cout << "never occured";
            }
            std::cout << ".\n";
        }
    };

    test(-2, 1, 0, 2, 0, 0, 2, 0, 0, 2, true);
    test(-2, 0, 0, 2, 0, 0, 2, 2, 0, 2, true);
    test(-2, 0, 0, 2, 0, 0, 0, 0, 0, 2, true);
    test(-2, 2, 0, 2, -1, 2, 0, 0, 0, 2, true);
    test(2, 1, 0, 2, 0, 0, 2, 0, 0, 2, true);
    test(-2, 1, 0, -1, 0, 0, 2, 0, 0, 2, false);
    test(-5, 1, 0, 2, 0.4f, 0, 2, 0, 0, 2, false);
}

int
main(int, char**)
{
    const auto plane = plane_demo{};
    plane.distance_to_plane();
    plane.point_to_plane();

    const auto ray = ray_demo{};
    ray.point_on_ray();
    ray.closest_point_on_ray();

    ray_sphere();
}
