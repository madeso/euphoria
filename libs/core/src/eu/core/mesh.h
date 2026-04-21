#pragma once

#include "eu/core/geom.h"


namespace eu::core
{

struct MeshGeom
{
    Geom geom;
    // std::size_t material;
};

struct TransformedMesh
{
    std::string name;
    m4 transform = m4_identity;
    std::vector<MeshGeom> geoms;
};

struct Mesh
{
    // todo(Gustav): change the single list to a hierachy?
    std::vector<TransformedMesh> meshes;
    // std::vector<Material> materials;
};

}
