#pragma once
#include "eu/core/mesh.h"
#include "eu/render/opengl_labels.h"

namespace eu::core
{
struct CompiledGeomVertexAttributes;
struct Mesh;
}

namespace eu::render
{
struct Material;
struct World;
struct MeshInstance;
struct CompiledGeom;

struct CompiledMeshGeom
{
    std::shared_ptr<CompiledGeom> geom;
    // std::size_t material;
};

struct CompiledTransformedMesh
{
    std::string name;
    m4 transform = m4_identity;
    std::vector<CompiledMeshGeom> geoms;
};

struct CompiledMesh
{
    // todo(Gustav): change the single list to a hierachy?
    std::vector<CompiledTransformedMesh> meshes;
    // std::vector<Material> materials;
};

CompiledMesh compile_mesh(DEBUG_LABEL_ARG_MANY const core::Mesh& mesh, const core::CompiledGeomVertexAttributes& geom_layout);


// ------------------------------------------------------------------------------------------------


struct TransformedMeshInWorld
{
    std::vector<std::shared_ptr<MeshInstance>> geoms;
};

struct MeshInWorld
{
    CompiledMesh* compiled_mesh = nullptr;
    std::vector<TransformedMeshInWorld> meshes;
    // std::vector<Material> materials;

    void add_to_world(CompiledMesh* m, World* world, std::shared_ptr<Material> mat);
    void set_transform(const m4& trans);
};

}
