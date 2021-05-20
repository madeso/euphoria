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
    struct material_override
    {
        std::vector<compiled_mesh_material> materials;
    };

    struct actor : public instance
    {
        actor(const std::shared_ptr<compiled_mesh>& mesh);

        void
        render
        (
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const light&       light
        ) override;


        [[nodiscard]] std::shared_ptr<material_override>
        create_override() const;


        std::shared_ptr<compiled_mesh> mesh_;
        std::shared_ptr<material_override> overriden_materials;
    };

}

