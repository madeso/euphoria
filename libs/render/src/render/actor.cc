#include "render/actor.h"

#include "core/assert.h"

namespace euphoria::render
{
    Actor::Actor(const std::shared_ptr<CompiledMesh>& mesh)
        : mesh_(mesh)
    {
        ASSERT(mesh);
    }

    std::shared_ptr<MaterialOverride>
    Actor::create_override() const
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
    Actor::render
    (
        const core::Mat4f& projection_matrix,
        const core::Mat4f& view_matrix,
        const core::Vec3f& camera,
        const Light& light
    )
    {
        mesh_->render
        (
            calculate_model_matrix(),
            projection_matrix,
            view_matrix,
            camera,
            light,
            overriden_materials
        );
    }

}
