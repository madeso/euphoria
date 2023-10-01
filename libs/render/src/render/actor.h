#pragma once

#include <memory>

#include "base/mat4.h"
#include "base/vec3.h"
#include "base/quat.h"
#include "base/rgb.h"

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
            const mat4f& projection_matrix,
            const mat4f& view_matrix,
            const vec3f& camera,
            const Light& light
        ) override;
    };

}

