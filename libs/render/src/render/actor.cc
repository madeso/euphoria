#include "render/actor.h"

#include "assert/assert.h"

namespace eu::render
{
    Actor::Actor(const std::shared_ptr<CompiledMesh>& m)
        : mesh(m)
    {
        ASSERT(m);
    }


    std::shared_ptr<MaterialOverride>
    Actor::create_override() const
    {
        const auto s = mesh->materials.size();
        auto r = std::make_shared<MaterialOverride>();
        r->materials.resize(s);
        for(std::size_t index=0; index<s; index+=1)
        {
            r->materials[index] = mesh->materials[index];
        }
        return r;
    }


    void
    Actor::render
    (
        const core::mat4f& projection_matrix,
        const core::mat4f& view_matrix,
        const core::vec3f& camera,
        const Light& light
    )
    {
        mesh->render
        (
            calc_model_matrix(),
            projection_matrix,
            view_matrix,
            camera,
            light,
            overriden_materials
        );
    }

}
