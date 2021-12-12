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
    const euphoria::core::Plane plane = euphoria::core::Plane::from_points
    (Vec3f(5, 6, 7), Vec3f(6, 5, 4), Vec3f(1, 2, 3));
    const std::vector<Vec3f> points =
    {
        Vec3f(0.0f, 0.0f, 0.0f),
        Vec3f(2.0f, 6.0f, 1.0f),
        Vec3f(3.0f, 1.0f, -3.0f),
        Vec3f(-2.0f, -1.0f, 2.0f),
        Vec3f(7.0f, 7.0f, -7.0f),
        Vec3f(3.0f, 2.0f, -1.0f),
        Vec3f(10.0f, 3.0f, -10.0f),
        Vec3f(2.0f, 8.0f, -7.0f),
        Vec3f(-1.632993f, 3.265986f, -1.632993f),
        Vec3f(-1.020621f, 3.265986f, -2.245366f)
    };

    void
    distance_to_plane() const
    {
        auto d = Dumper{ "coldet-distance-to-plane.html" };

        d.add_plane(plane, NamedColor::white);
        d.add_arrow(Ray3f(Vec3f::zero(), plane.normal), NamedColor::green);
        for (const auto& p : points)
        {
            const auto dist = get_distance_between(plane, p);
            d.add_sphere(p, 0.1f, euphoria::core::abs(dist) < 0.01f ? NamedColor::yellow : (dist < 0 ? NamedColor::black : NamedColor::white));
        }
    }

    void
    point_to_plane() const
    {
        auto d = Dumper{ "coldet-point-to-plane.html" };

        d.add_plane(plane, NamedColor::white);
        d.add_arrow(Ray3f(Vec3f::zero(), plane.normal), NamedColor::green);
        for (const auto& p : points)
        {
            const auto dist = get_distance_between(plane, p);
            if (euphoria::core::abs(dist) < 0.01f) { continue; }

            const auto pp = get_closest_point(plane, p);
            d.add_arrow(Ray3f::from_to(p, pp), NamedColor::black);
        }
    }
};

struct ray_demo
{
    const Ray3f ray = {Vec3f(-3, -2, -1), Vec3f(3, 2, 1)};
    const std::vector<Vec3f> points =
    {
        Vec3f(-3, -2, -1),
        Vec3f(12, 8, 4),
        Vec3f(0, 0, 0),
        Vec3f(-18, -12, -6),
        Vec3f(-4, -7, -8),
        Vec3f(7, 8, 5),
        Vec3f(1, 5, -5),
        Vec3f(-6, 5, 7),
        Vec3f(1, 6, 8),
        Vec3f(-7, -10, -4),
        Vec3f(-4.5f, -3.0f, -1.5f)
    };

    void
    point_on_ray() const
    {
        auto d = Dumper{ "coldet-point-on-ray.html" };

        d.add_grid();
        d.add_arrow(ray, NamedColor::black);

        for (const auto p : points)
        {
            const auto dist = get_distance_between(ray.get_normalized(), p);
            d.add_sphere(p, 0.1f, dist < 0.001f ? NamedColor::white : NamedColor::black);
        }
    }

    void
    closest_point_on_ray() const
    {
        auto d = Dumper{ "coldet-closest-point-on-ray.html" };

        d.add_grid();
        d.add_arrow(ray, NamedColor::black);

        for (const auto p : points)
        {
            const auto dist = get_distance_between(ray.get_normalized(), p);
            if (dist < 0.001f) { continue; }

            const auto pp = get_closest_point(ray.get_normalized(), p);
            d.add_arrow(Ray3f::from_to(p, pp), NamedColor::red);
        }
    }

};

void
ray_sphere()
{
    const auto test = []
    (
        float ray_x, float ray_y, float ray_z,
        float norm_x, float norm_y, float norm_z,
        float sphere_x, float sphere_y, float sphere_z,
        float rad,
        bool res
    )
    {
        // todo(Gustav): add scene support to dumper...
        const auto ray = Ray3f(Vec3f(ray_x, ray_y, ray_z), Vec3f(norm_x, norm_y, norm_z).get_normalized());
        const auto sphere = ::Sphere{rad};
        const auto sphere_center = Vec3f(sphere_x, sphere_y, sphere_z);
        const auto collision = get_intersection(ray.get_normalized(), sphere, sphere_center);
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
