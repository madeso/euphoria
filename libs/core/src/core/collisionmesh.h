#pragma once

#include <vector>
#include <optional>

#include "core/vec3.h"


namespace euphoria::core
{

struct Mesh;
struct UnitRay3f;

struct CollisionMesh
{
    std::vector<core::Vec3f> points;
    std::vector<core::Vec3i> faces;

    void add(const Mesh& mesh);
};

std::optional<float>
get_intersection(const UnitRay3f& ray, const CollisionMesh& mesh);


}
