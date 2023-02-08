#pragma once


#include <optional>

#include "core/vec3.h"


namespace euphoria::core
{

struct Mesh;
struct UnitRay3f;

struct CollisionFace
{
    constexpr CollisionFace(int xx, int yy, int zz) : x(xx), y(yy), z(zz) {}
    
    int x; int y; int z;
};

struct CollisionMesh
{
    std::vector<core::vec3f> points;
    std::vector<CollisionFace> faces;

    void add(const Mesh& mesh);
};

std::optional<float>
get_intersection(const UnitRay3f& ray, const CollisionMesh& mesh);


}
