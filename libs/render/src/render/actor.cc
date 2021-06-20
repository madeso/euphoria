#include "render/actor.h"

#include "core/assert.h"

namespace euphoria::render
{
    actor::actor(const std::shared_ptr<compiled_mesh>& mesh)
        : mesh_(mesh)
    {
        ASSERT(mesh);
    }

    std::shared_ptr<material_override>
    actor::create_override() const
    {
        const auto s = mesh_->materials.size();
        auto r = std::make_shared<material_override>();
        r->materials.resize(s);
        for(std::size_t index=0; index<s; index+=1)
        {
            r->materials[index] = mesh_->materials[index];
        }
        return r;
    }

    void
    actor::render
    (
        const core::mat4f& projection_matrix,
        const core::mat4f& view_matrix,
        const core::vec3f& camera,
        const light& light
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
