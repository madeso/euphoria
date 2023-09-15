#pragma once

#include <memory>

#include "core/mat4.h"
#include "core/vec3.h"
#include "core/quat.h"
#include "core/rgb.h"

#include "render/instance.h"
#include "render/compiledmesh.h"

namespace eu::render
{
    struct MaterialOverride
    {
        std::vector<CompiledMeshMaterial> materials;
    };

    struct Actor : public Instance
    {
        std::shared_ptr<CompiledMesh> mesh;
        std::shared_ptr<MaterialOverride> overriden_materials;

        Actor(const std::shared_ptr<CompiledMesh>& mesh);

        [[nodiscard]] std::shared_ptr<MaterialOverride>
        create_override() const;

        void
        render
        (
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const Light& light
        ) override;
    };

}

