#pragma once

#include <memory>

#include "core/mat4.h"
#include "core/vec3.h"
#include "core/quat.h"
#include "core/rgb.h"

#include "render/instance.h"
#include "render/compiledmesh.h"

namespace euphoria::render
{
    struct MaterialOverride
    {
        std::vector<CompiledMeshMaterial> materials;
    };

    struct Actor : public Instance
    {
        Actor(const std::shared_ptr<CompiledMesh>& mesh);

        void
        render
        (
            const core::Mat4f& projection_matrix,
            const core::Mat4f& view_matrix,
            const core::Vec3f& camera,
            const Light& light
        ) override;


        [[nodiscard]] std::shared_ptr<MaterialOverride>
        create_override() const;


        std::shared_ptr<CompiledMesh> mesh;
        std::shared_ptr<MaterialOverride> overriden_materials;
    };

}

