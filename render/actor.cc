#include "render/actor.h"

#include "core/assert.h"

namespace euphoria::render
{
    Actor::Actor(const std::shared_ptr<CompiledMesh>& mesh)
        : mesh_(mesh)
        , overriden_materials()
    {
        ASSERT(mesh);
    }

    std::shared_ptr<MaterialOverride>
    Actor::CreateOverride()
    {
        const auto s = mesh_->materials.size();
        auto r = std::make_shared<MaterialOverride>();
        r->materials.resize(s);
        for(std::size_t index=0; index<s; index+=1)
        {
            r->materials[index] = mesh_->materials[index];
        }
        return r;
    }

    void
    Actor::Render(
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const Light&       light)
    {
        mesh_->Render(
                GetModelMatrix(),
                projection_matrix,
                view_matrix,
                camera,
                light,
                overriden_materials);
    }

}  // namespace euphoria::render
