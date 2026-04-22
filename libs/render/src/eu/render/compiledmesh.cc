#include "eu/render/compiledmesh.h"

#include "world.h"
#include "eu/core/mesh.h"

namespace eu::render
{
    CompiledMesh compile_mesh(DEBUG_LABEL_ARG_MANY const core::Mesh& mesh, const core::CompiledGeomVertexAttributes& geom_layout)
    {
        CompiledMesh ret;

        for (const auto& meshes : mesh.meshes)
        {
            CompiledTransformedMesh trans{
                .name = meshes.name,
                .transform = meshes.transform,
                .geoms = {}
            };
            for (const auto& geom : meshes.geoms)
            {
                auto compiled = compile_geom(USE_DEBUG_LABEL_MANY(debug_label) geom.geom, geom_layout);
                trans.geoms.emplace_back(compiled);
            }
            ret.meshes.emplace_back(trans);
        }

        return ret;
    }

    void MeshInWorld::add_to_world(CompiledMesh* m, World* world, std::shared_ptr<Material> mat)
    {
        ASSERT(compiled_mesh == nullptr);
        compiled_mesh = m;

        for (const auto& mesh : m->meshes)
        {
            TransformedMeshInWorld trans;
            for (const auto& geom: mesh.geoms)
            {
                auto instanced = make_mesh_instance(geom.geom, mat);
                world->meshes.emplace_back(instanced);
                trans.geoms.emplace_back(instanced);
            }
            meshes.emplace_back(trans);
        }
    }

    void MeshInWorld::set_transform(const m4& trans)
    {
        for (std::size_t mesh_index = 0; mesh_index < meshes.size(); mesh_index += 1)
        {
            for (std::size_t geom_index = 0; geom_index < meshes[mesh_index].geoms.size(); geom_index += 1)
            {
                 meshes[mesh_index].geoms[geom_index]->transform = trans * compiled_mesh->meshes[mesh_index].transform;
            }
        }
    }

}
