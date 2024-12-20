#include "core/intersection.h"
#include "core/dump.h"
#include "base/numeric.h"
#include "base/colors.h"
#include "base/ray.h"
#include "base/plane.h"
#include "core/sphere.h"
#include "core/argparse.h"
#include "base/os.h"

#include <iostream>


using namespace eu;
using namespace eu::core;
using namespace eu::core::dump3d;


struct PlaneDemo
{
    const eu::Plane plane = eu::Plane::from_points
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
    distance_to_plane(const std::string& cd) const
    {
        auto d = Dumper{ cd + "/coldet-distance-to-plane.html" };

        d.add_plane(plane, NamedColor::white);
        d.add_arrow(Ray3f(zero3f, plane.normal), NamedColor::green);
        for (const auto& p : points)
        {
            const auto dist = get_distance_between(plane, p);
            d.add_sphere(p, 0.1f, eu::abs(dist) < 0.01f ? NamedColor::yellow : (dist < 0 ? NamedColor::black : NamedColor::white));
        }
    }

    void
    point_to_plane(const std::string& cd) const
    {
        auto d = Dumper{ cd + "/coldet-point-to-plane.html" };

        d.add_plane(plane, NamedColor::white);
        d.add_arrow(Ray3f(zero3f, plane.normal), NamedColor::green);
        for (const auto& p : points)
        {
            const auto dist = get_distance_between(plane, p);
            if (eu::abs(dist) < 0.01f) { continue; }

            const auto pp = get_closest_point(plane, p);
            d.add_arrow(Ray3f::from_to(p, pp), NamedColor::black);
        }
    }
};

struct RayDemo
{
    const Ray3f ray = {vec3f(-3, -2, -1), vec3f(3, 2, 1)};
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
    point_on_ray(const std::string& cd) const
    {
        auto d = Dumper{ cd + "/coldet-point-on-ray.html" };

        d.add_grid();
        d.add_arrow(ray, NamedColor::black);

        for (const auto p : points)
        {
            const auto dist = get_distance_between(ray.get_normalized(), p);
            d.add_sphere(p, 0.1f, dist < 0.001f ? NamedColor::white : NamedColor::black);
        }
    }

    void
    closest_point_on_ray(const std::string& cd) const
    {
        auto d = Dumper{ cd + "/coldet-closest-point-on-ray.html" };

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
        const auto ray = Ray3f(vec3f(ray_x, ray_y, ray_z), vec3f(norm_x, norm_y, norm_z).get_normalized());
        const auto sphere = ::Sphere{rad};
        const auto sphere_center = vec3f(sphere_x, sphere_y, sphere_z);
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
main(int argc, char* argv[])
{
    auto parser = argparse::Parser("coldet dump");

    auto current_directory = get_current_directory();
    parser
        .add("-w", &current_directory)
        .set_help("Sets the working direction if it's different from the current folder")
        ;
    const auto parse_result = parser.parse(argparse::NameAndArguments::extract(argc, argv));
    if(parse_result != argparse::ok)
    {
        return parse_result.return_value;
    }

    const auto plane = PlaneDemo{};
    plane.distance_to_plane(current_directory);
    plane.point_to_plane(current_directory);

    const auto ray = RayDemo{};
    ray.point_on_ray(current_directory);
    ray.closest_point_on_ray(current_directory);

    ray_sphere();
}
