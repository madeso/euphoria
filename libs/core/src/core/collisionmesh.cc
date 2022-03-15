#include "core/collisionmesh.h"

#include "core/mesh.h"
#include "core/intersection.h"


namespace euphoria::core
{


void
CollisionMesh::add(const Mesh& mesh)
{
    for(const auto& part: mesh.parts)
    {
        const auto point_base = points.size();

        points.reserve(points.size() + part.points.size());
        for(const auto& p: part.points)
        {
            points.emplace_back(p.vertex);
        }

        faces.reserve(faces.size() + part.faces.size());
        for(const auto& f: part.faces)
        {
            faces.emplace_back
            (
                point_base + f.a,
                point_base + f.b,
                point_base + f.c
            );
        }
    }
}


std::optional<float>
get_intersection(const UnitRay3f& ray, const CollisionMesh& mesh)
{
    std::optional<float> closest_intersection = std::nullopt;

    for(const auto& face: mesh.faces)
    {
        const auto face_intersection =  get_intersection_ray_triangle
        (
            ray,
            mesh.points[face.x], mesh.points[face.y], mesh.points[face.z]
        );

        if(closest_intersection.has_value() && face_intersection.has_value())
        {
            if(*face_intersection < *closest_intersection)
            {
                closest_intersection = face_intersection;
            }
        }
        else if (closest_intersection.has_value() == false)
        {
            closest_intersection = face_intersection;
        }
    }

    return closest_intersection;
}


}
