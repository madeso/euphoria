#include "core/intersection.h"
#include "core/dump.h"
#include "core/numeric.h"
#include "core/colors.h"
#include "core/ray.h"
#include "core/plane.h"

#include <vector>
#include <string>
#include <iostream>

using namespace euphoria::core;
using namespace euphoria::core::dump3d;

const auto plane = Plane::FromPoints
    (vec3f(5, 6, 7), vec3f(6, 5, 4), vec3f(1, 2, 3));
const auto points = std::vector<vec3f>
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
distance_to_plane()
{
    auto d = Dumper{"coldet-distance-to-plane.html"};
    // d.AddGrid();

    d.AddPlane(plane, Color::White);
    d.AddArrow(Ray3f(vec3f::Zero(), plane.normal), Color::Green);
    for(const auto& p: points)
    {
        const auto dist = DistanceBetween(plane, p);
        d.AddSphere(p, 0.1f, Abs(dist)<0.01f? Color::Yellow : (dist<0? Color::Black : Color::White) );
    }
}

void
point_to_plane()
{
    auto d = Dumper{"coldet.html"};
    // d.AddGrid();

    d.AddPlane(plane, Color::White);
    d.AddArrow(Ray3f(vec3f::Zero(), plane.normal), Color::Green);
    for(const auto& p: points)
    {
        const auto dist = DistanceBetween(plane, p);
        if(Abs(dist) < 0.01f) continue;

        const auto pp = ClosestPoint(plane, p);
        d.AddArrow(Ray3f::FromTo(p, pp), Color::Black);
    }
}

int
main(int, char**)
{
    distance_to_plane();
    point_to_plane();
}
