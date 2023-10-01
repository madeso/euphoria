#pragma once


#include <optional>

#include "base/vec3.h"



namespace eu
{
    struct UnitRay3f;
}


namespace eu::core
{


struct Mesh;

struct CollisionFace
{
    int x;
    int y;
    int z;

    constexpr CollisionFace(int xx, int yy, int zz)
        : x(xx), y(yy), z(zz) {}
};

struct CollisionMesh
{
    std::vector<vec3f> points;
    std::vector<CollisionFace> faces;

    void add(const Mesh& mesh);
};

std::optional<float>
get_intersection(const UnitRay3f& ray, const CollisionMesh& mesh);


}
